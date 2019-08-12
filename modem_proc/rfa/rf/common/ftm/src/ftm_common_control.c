/*!
  @file
  ftm_common_control.c

  @brief
  This module contains FTM common Control code.
 
  @addtogroup RF_COMMON_FTM_CONTROL
  @{
*/

/*==============================================================================

  Copyright (c) 2010 - 2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_common_control.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
07/05/16   cv      Adding LTE B66 support
05/11/16   wxl     [Ported]Handle device voting in Tx disable
04/15/15   bp      Support bands differentiation during div enter_mode controlled by NV item
04/07/15   vkm     Increase WL1_START_STOP_WAIT_TIMEOUT to 200ms 
03/19/15   ck      Bypass ftm ccmgr cleanup during band/tech transition 
03/19/15   rmb/ck  Fix to include ftm concurrency manager cleanup in the ftm_wcdma_exit() API.
03/03/15   vv      Added ftm support for ADC VCM Cal feature
02/25/15   am      Fixed Compiler Warnings
02/10/15   dw      Fix L to G mode transition crash due to no cleanup of devices allocated
12/24/14   as      Forcibly clear SNUMs and AsDiv pending events related to LTE
                   during FTM LTE MODE EXIT
12/09/14   pl      Back out FTM MCPM request co-ordination between ftm APIs and 
                   FTM protocol activation\deactivation
15/05/14   JJ      Remove LM_BUFFER_NOT_ALLOCATED if used as handle type
12/08/14   yzw     Add FTM HORXD support
11/19/14   ck      Add tech protection to ftm ccmgr cleanup
11/18/14   yb      Updated self test API to use coupler driver to update settings
11/17/14   jps     Vote for max clk speed for TD-SCDMA FTM mode
11/17/14   kab     FTM RF Dynamic SCC Device allocation
11/13/14   ck      hook up FTM concurrency manager to ensure valid concurrency
11/11/14   ck      clean up ftm concurrency manager allocation status when exiting
11/12/14   cri     Added LM_NOT_ALLOCATED status check for deallocation of LM buffers
10/30/14   ck      Call ftm concurrency manager for gsm diversity + fix oft error
10/09/14   pl      Added FTM MCPM request co-ordination between ftm APIs and 
                   FTM protocol activation\deactivation
09/23/14   pk      Compiler warning fix
09/23/14   kai     Add WCDMA DRDS Cal support
09/17/14   dr      Adding Jolokia Featurization
09/15/14   cd      Reverting TQ changes to fix ACLR-based Pin char on Bolt
09/11/14   cri     Fix for RF_CMD_APP disabled after LTE enter mode on Dev1
09/02/14   dr      Including ftm_common.h outside any featurization.
08/06/14   fh      Fixed c2k thermistor read tq pair req/release and api
08/06/14   dw      Added API support to query different FTM states for WL1
07/28/14   vbh     Fix RSB Cal issues associated with W exit mode
07/23/14   dbz     Added support for selftest VSWR split capture
07/08/14   pk      Implementation of HDR FTM module
06/30/14   vbh     [DR-DS] [1] Clean-up device 0 hardcode 
                   [2] Use MC state variable to identify Primary Rx device
06/13/14   kab     Add 3xCA support
06/05/14   bsh     Clean up all LTE devices during device 0 exit
05/30/14   gvn     Fix Cleaning of LTE when moving to another Tech during cal
04/21/14   dej     Select correct cal command for DimePM
03/17/14   kg      Porting change from Dime
02/11/14   sr      Re-design asdiv_tuner to support multiple RF configurations
02/18/14   vbh     DBDC - allocate RxLM per chain, remove voting for CPU speed
02/14/14   ljl     Do not vote off MCPM RF REQ to ensure same MCPM setting before entering internal calibration
02/04/14   ljl     Call DAC mission mode enter function after DAC calibration is done
01/30/14   vbh     Featurization cleanup - removed fixed wait on WL1 command
01/09/14   ljl     DAC Cal NV write check
01/09/14   vbh     Reorder deallocation calls in WCDMA exit mode 
01/08/14   kai     Remove legacy mcpm function calling
01/07/14   sw      Move geran test mode api calls to rftech_gsm
12/26/13   sar/hdz Remove et_txlm_hanlde from Bolt c2k code, and use one single 
                   TxLm handle allocation.
12/20/13   pl      added timing information for RXAGC start/stop
12/18/13   ljl     Update self calibration function for Bolt
12/05/13   kr      Re-set RxD behaviour as per nv item value while leaving ftm mode
11/05/13   bn      voting for max clk speed in W non-signaling enter
10/31/13   shb     Switch to C++ TRX common interface
10/23/13   nrk     Fix compiler warnings
09/17/13   aro     Removed RFM sleep from WCDMA exit
09/12/13   aka     Add support for TDSCDMA in ftm_common_set_fb_path_state API
09/13/13   kai     Added WCDMA sleep and mc state machine reset in exit wcdma
09/09/13   aka     Added support for W and 1x in ftm_common_set_fb_path_state API
08/26/13   jr      Added FTM_GET_EFS_VERIFICATION_DATA
08/20/13   vbh     9x35 RUMI - Do not wait on L1 in enter mode, instead wait for 50 ms
08/19/13   spa     Added support for SVDO enter mode
08/15/13   zhw     Reset RF mode in ftm_1x_exit for a given device
08/09/13   rmb     Remove Abandoned Word. No SW change
08/07/13   nsh     Support Get ENH Thermal Value for TDS
08/05/13   aa/rmb  Fix cleanup for FTM RF when moving from RF to NS mode               
08/05/13   aa/rmb  Move ftm_wcdma_ber_set_state(FTM_WCDMA_BER_STATE_NULL) 
                   before ftm_wcdma_enter() returns.
07/29/13   aka     Added FTM_SET_FB_PATH_STATE cmd
07/26/12   tws     Get the GSM TX band via API call
07/19/13   ac      adding device support for DBDC 
07/24/13   aca     ftm_common_get_tuner_object update to use FTM state
07/23/13   Saul    Cal Time Reduction. Vote for MAX CPU SPEED in CDMA
07/23/13   aka     Added API for ACLR/FFT
07/15/13   aca     ftm_common_get_tuner_object update for multiple tuner objects
07/12/13   aca     Tuner obj accessor function update
07/10/13   JJ      Rename lagacy flag rumi_bup_cdma 
06/29/13   hm     Add new FTM command for tuner self cal: "FTM_DO_TUNER_SELF_CAL"
07/01/13   cc      Enable Vreg for c2k when reading unified therm if Vreg is not on
06/25/13   cri     Update therm read interface to use correct enum
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
06/20/13   JJ      Remove input param for fw msgs de-registration func
06/19/13   JJ      Remove register/de-register fw msgs funcs during 1x/hdr enter/exit 
06/18/13   JJ      Featurize register/de-register fw msgs funcs to 1x/hdr 
06/18/13   JJ      Add register/de-register fw msgs funcs during 1x/hdr enter/exit 
06/14/13   jl      Move ftm_wcdma_set_secondary_chain() to end of ftm_wcdma_exit()
06/13/13   ck      Add GSM tech case for FTM_GET_UNIFIED_ENH_THERM
06/12/13   aca     RL FTM API
06/10/13   aca     Tuner accessor function
06/04/13   aca     ET and EPT ET support for DAC cal
05/17/13   cc      Added 1x IQ capture support
05/16/13   aki     Intialize ftm_message struct in ftm_common_send_radio_command()
05/14/13   aca     DAC cal ET check- Triton support
05/13/13   jl      Port IQ capture code
05/13/13   aca     DAC cal error check update
05/09/13   kai     Moved unvoting max ftm clock rate to the end of wcdma_exit
05/07/13   ck      Stop GSM and WCDMA properly for non-signalling and FTM mode
05/03/13   aca     FTM K sensor read support
04/30/13   vb      During internal device cal, status should include results 
                   from ftm_rfmode_exit()
04/29/13   jj      change the default band in function ftm_wcdma_enter
04/29/13   cri     Band change without reset after DPD IQ capture
04/25/13   lp      Fixed compilation error with no FEATURE_LTE build variant
04/25/13   kab     Added LTE support for FTM_GET_UNIFIED_ENH_THERM
04/25/13   gvn     Support for Split bands for LTE  
04/11/13   ec	   Manually request 144 MHz Bus Clock speed in ftm_gsm_enter for TRITON, to prevent 
		   FW crash reported in GSM Tx Cal.
04/03/13   ec 	   Move allocation of ftm_vars out of ns_enter and remove cleanup from ns_exit
04/09/13   cri     Add init and deinit for cal data module
04/04/13   ka      WCDMA state machine updates
03/26/13   spa     Updated configure FW state API call
03/25/13   adk     Updated ftm_common_get_tuner_hdet_device()
03/21/13   yzw     Add FTM common command to query RFFE device information {mfg_id, prd_id, rev_id}
03/20/13   ms      No-TDSCDMA build compilation fix
03/13/13   aca     Tuner characterization
03/11/13   sar     Updated file for APQ, GNSS only target.
03/06/13   tws     Ensure that GSM non-signalling variables are available when 
                   going direct from FTM mode.
02/28/13   hdz     Added ftm_common_npa_vote_for_modem_bus_clk()
02/26/13   tsr     Remove the redundant rfgsm tx enable call from ftm_gsm_enter
02/22/13   dw      Support for FTM common Cal routines without set mode
02/22/13   sc      Fix incorrect clock voting for FTM mode
02/20/13   aca     DIME DAC cal-sequence update
02/11/13   aca     DIME DAC cal-disable DC cal as it is not required
02/06/13   sty     reverted previous change - rxlm_init is now called as part of 
                   rfm_init() - so do not need the change in this file
02/05/13   APU     Enable clocks before calling LM APIs since the LM APIs access
                   the MCPM Clocks.
02/04/13   aa      W FTM IRAT
02/04/13   tws     Correct typo.
02/04/13   tws     Update for geran_test_mode_api change.
01/29/13   ec      Add framework for Non Signalling on multiple RF Devices
01/2/13    aka     Changes to support multi chain GSM operation in FTM mode
01/17/13   aca     DIME DAC cal status update
01/14/13   aca     DIME DAC cal mission mode config
01/09/13   aca     DAC cal update
01/04/13   Saul    Common. Revert a KW fix.
01/04/13   pl      use MC State machine callflow to support CA CAL
01/03/13   Saul    Common. KW fixes.
12/20/12   sty     Added null pointer check in ftm_hdr_exit and ftm_1x_exit
12/19/12   cd      Allocate 1x ET TxLM buffer on enter and exit
12/19/12   bsh     Off target error fixed
12/19/12   bsh     KW errors fixed
12/18/12   pl      CA Serial Cal Support
12/11/12   cri     Move to common DPD buffer allocation
12/11/12   ac      fix for xo cal failures in wcdma after internal device cal
12/06/12   jmf     Removed xpt buffer allocation from ftm_1x_enter
12/05/12   tws     Use device2_as_id to get the correct ID for GL1.
12/03/12   aca     DIME Dac cal
11/30/12   tws     Add device id & as_id to FTM.
11/19/12   pl      Update ftm_mdsp_exit() for easier reuse
11/15/12   cri     Updates for LTE multiple sample capture   
11/13/12   ec      Update to use new geran api for DSDA   
11/09/12   gvn     B42 support (merge from Nikel)   
11/06/12   gh      Enable RC cal. DAC cal commented out until fixed
10/29/12   aro     Interface to perform FTM tech to RFM tech mapping
09/28/12   ac      device interface to pass band to rfc_wcdma_get_device
10/01/12   sb      Made changes to add rfm device to rf device interface and 
                   uppler layer APIs 
09/20/12   pl      Adding rflte_mc.h
09/10/12   Saul    Warning fixes.
08/27/12   pl/sbm  update to support LTE CA devices 
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
08/20/12   sty     Removed calls to enable and disable FW_APP_RFCMD   
08/14/12   Saul    COMMON. Enabling/disabling common FW on FTM enter/exit.
07/25/12   ndb     Fix for clean up of ftm_rfmode_current for Chain 1 in LTE
07/12/12   pl      Add FTM LTE CA support
07/06/12   zhw     Moved RUMI hack from common API to CDMA pecific call flow.
07/02/12   cvd     Add Null pointer checks for ftm_cdma_data_get_device_ptr()
06/29/12   sbo     GSM cleanup for NS->FTM transition
06/27/12   zhw     Stubb out vote_cpu_speed when RUMI debug flag is enabled
06/14/12   gh      Fix ftm_wcdma_enter/exit logic
06/08/12   cd      Fix for incorrect first HKADC reading in thermistor cal in CDMA mode (SVOC)
06/07/12   pl      Update RF LTE MC interfaces to support CA
05/16/12   vb      Moved the LTE DAC Cal above WCDMA DAC Cal
05/16/12   vb      Do DAC cal for a device path only if a tech is enabled for it 
05/15/12   APU     Added profiling for ftm_rfmode_enter() & ftm_rfmode_exit()                 
05/14/12   aa      Changes for Code Maintainability and data access        
05/13/12   kma     Fixed compilation warnings
05/04/12   kma     Fixed W to LTE FTM mode transition crash
04/26/12   ems     Ensure device 1 is unvoted upon exit 
04/13/12   sb      reset RF device to original state after GSM NS stop
04/06/12   aa      Added Feature to increase clock speed to max in FTM Mode
04/05/12   vb      Fix integration error
04/05/12   vb      Added function to perform DAC cal only     
04/01/12   vb      For 1x DAC cal, doing all the buffer memory allocation only
                   once for both DAC-0 and DAC-1 Cal
03/27/12   ems     Correct exit mode to use ftm_rfmode_current[device]
03/22/12   vb      LTE and TDSCDMA DAC cal disabled until it is fully verified
03/21/12   aca     LTE XO cal support
03/18/12   aro     Common function to create response packet
03/13/12   gh      Added code to clear W RRC cmd queue for enter/exit mode
04/30/12   ggs     Compiler warning fix
04/24/12   jfc     Use TASK_HANDLE instead of rex_self()
03/20/12   jfc     Replace TCB with RCINIT framework
03/06/12   aro     Added code to perform 1x Non-signalling FTM enter and exit
03/06/12   aro     Added code to perform HDR Non-signalling FTM enter and exit
03/02/12   ems     Place RF in deep sleep when exiting WCDMA
02/23/12   adk     Set dac_cal_status = FALSE if 1X DAC cal fails for any device
02/17/12   php     Added support for LTE B41
02/14/12   sar     Reinstated TDSCDMA features.
02/07/12   sar     Feature reduction changes.
02/07/12   aca     LTE Dac calibration support
01/30/12   dw      Fix compilation error
01/27/12   dw      Added ftm_wcdma_ns_enter and ftm_wcdma_ns_exit
01/26/12   gh      Update ftm_wcdma_deallocate_rxlm to take in chain as arguement
01/24/12   aro     Implemented run-time detection of techs supported by the 
                   modem to perform  DAC calibration
01/23/12   aro     KW/Compiler warning Fix
01/23/12   aro     KW Fix
01/23/12   aro     Added Vreg enable for THERM Read
01/17/12   aro     Added all device 1x FTM exit for the transitions which
                   end up in unsupported simultaneous FW app
01/17/12   aro     Added "New Mode" as a argument in FTM EXIT function
01/16/12   aro     Added interface to query THERM read
01/16/12   aro     Added common interface to get therm read from any device
                   for a given mode
01/16/12   aro     Documentation Update
01/03/12   aro     Renamed the TxLM and RxLM chain parameter to make it
                   explicit that it is used only for FTM
01/03/12   aro     Added TxLM DAC to logical device parameter
01/03/12   aro     Compiler error fix
01/03/12   aro     Added TxLM Chain specific to 1x and HDR in logical
                   device param
01/03/12   sar     Fixed compiler errors in non-LTE build.
12/29/11   adk     Query RF card to find out which DAC cals are needed
12/29/11   aro     Added RxLM ADC and 1x/HDR specific RxLM chain in logical
                   device param
12/28/11   adk     Temporarily disable DAC-1 cal, since it is RF card-dependent
12/28/11   adk     Implemented DAC_1 calibration in the 1x mode
12/27/11   dw      Offtarget compiler error fix
12/22/11   adk     Implemented DAC_0 calibration in the 1x mode
12/19/11   dw      Turn off Tx in WCDMA exit if it is still on
11/28/11   gh      Add check for ftm_wcdma_enter/exit
11/23/11   ems     Correct 1X exit transition for two devices
11/17/11   hy      Added TDSCDMA V2/V3 Cal    
11/17/11   aro     Enabled voting for CDMA FW State config and MCPM config
11/17/11   aro     Added new 1x MCPM configure function with voting mechanism
11/17/11   aro     Replaced the FW state config function with new FTM func
11/14/11   aro     Mainlined the debug flags
11/10/11   aro     Upadated FW state config interface to pass semaphore to 
                   wait on for blocking calls
10/27/11   gh      Use wait_sig for ftm_wcdma_enter/exit to let WL1 complete
10/20/11   adk     Added support for SV diversity.
10/18/11   dw      Fix WCDMA Composite Cal for Diversity Chain
09/29/11   cd      - Remove dependence on HDR L1 to enable FW App
                   - Configure MCPM correctly in FTM
09/28/11   sr      Fixed FTM to online mode transition crash for GSM
09/27/11   aro     Added RC Tuner calibration to Self cal node
09/27/11   aro     Documentation Update for Self Cal Node
09/25/11   vb      DAC cal framework for all WAN technologies   
09/25/11   vb      Support for FTM_DO_SELF_CAL  
09/08/11   vb      Support for DAC Cal
08/19/11   aro     Klocworks error fixes
08/18/11   vb      Removed MCPM Calls for WCDMA as WL1 configures it in FTM
08/18/11   aro     Moved MCPM config function to HAL
08/09/11   aro     Moved send FTM command from CDMA to FTM common
08/04/11   vb      Warning fix
08/04/11   vb      Added FTM common Internal device calibration support
07/29/11   aro     Fixed TDSCDMA enum typo
07/29/11   hy      Moved TD-SCDMA enter/exit to ftm_tdscdma_ctl.
07/28/11   sty     Disable FW app before a STOP command is sent to MCPM
07/28/11   aro     [1] Added iRAT support on FTM common control
                   [2] Moved combiner 1x combiner interrupts enable/disable to
                   1x enter and exit
07/22/11   sty     Added null pointer checks
07/19/11   Saul    Combined FW enable/disable APIs into one FW config state API.
07/11/11   hy      Including TD-SCDMA support.
07/11/11   dw      Remove rfm_exit_mode() in ftm_wcdma_exit() 
06/21/11   aro     Moved ftm_sys_control functionality to ftm_common_control
06/21/11   aro     Added function to map FTM Mode to RF Mode
06/02/11   aro     Removed debug flag
06/01/11   aro     Compiler warning removal
06/01/11   aro     Moved the CDMA Radio tear down to Set Mode
05/31/11   sr      Removed the previous set mode GSM changes, as it require GL1 changes as dependency.
05/31/11   sr      Removed MCPM calls from gsm set mode, which will be called from GL1. 
05/27/11   sty     Compiler warning fix 
05/27/11   sty     Added rfm_enter_mode calls to ftm_1x_enter and ftm_hdr_enter
05/26/11   sty     Set MCPM to a known state before doing any operations on MCPM
05/24/11   sty     Removed redundant MCPM calls while enabling HDR
05/24/11   aro     Fixed the buffer allocation failure for device 1
05/24/11   aro     Fixed TxLM Buffer allocation bug
05/21/11   aro     Added query to RF to get RxLM and TxLM Chain for buffer alloc
05/17/11   sty     Enable HDR FW app
05/16/11   aro     Added a check to block back to back MCPM configuration
05/16/11   aro     Added check to make sure that MCPM is configured once
05/14/11   dw/vb   Deallocate TxLM buffer when exiting WCDMA mode
05/13/11   aro     Added Flag to bypass MCPM in FTM
05/13/11   dw      Allocate WCDMA TxLM buffer when entering WCDMA mode
05/12/11   rsr     MCPM fix required to enable TX clocks during set mode GSM.  
05/09/11   sar     Included ftm_rf_cmd.h for rfa/api cleanup.
04/28/11   rsr     Fixes to move GSM to use the ftm_rfmode_enter/exit api. 
                   Support for FEATURE_GERAN_NIKEL_HW_INIT (has GL1 dependencies)
04/28/11   sar     CMI-4 Cleanup.
04/26/11   sty     Renamed ftm_state_enum_type to ftm_rfstate_enum_type
04/26/11   aro     Removed direct access to ftm_get_rf_db_state
                   variable (compiler error fix)
04/25/11   aro     Renamed FTM RFMODE to FTM State
04/26/11    dw     Added BER clean up in wcdma exit
03/25/11   aro     Added 1x and HDR Disable FW in FTM Exit
04/19/11   pl      Added LTE enter and exit functions
04/13/11   aro     Compiler Warning Fix
04/13/11   aro     Removed HDR Enable FW interface to replace with RFM
04/01/11    dw     Fix warnings
03/28/11   aro     Renamed function to ftm_cdma_send_command
03/28/11   aro     [1] Changed ftm_cdma_create_ftm_q_msg() signature to return
                   flag indicating success and failure
                   [2] Added functionality to send FTM message from this func
03/28/11   aro     Fixes to create FTM Command Queue Message
03/28/11   aro     Added function to create FTM Queue message
03/25/11    dw     WCDMA update: add MPCM calls and allocate RxLM buffer in ftm_wcdma_enter()
03/24/11   aro     Renamed FTM Enter Mode function
03/24/11   aro     Added support for Q6 clock Speed voting
03/24/11   aro     Removed Temporary Feature definition
03/23/11   sty     Deleted clkregim.h - mcpm_api.h used instead for clk 
                   functionality
03/18/11   aro     Removed old FTM clock config to be repalced with MCPM
03/17/11   aro     Added MCPM calls to configure modem calls
03/15/11   aro     Compiler Warning Removal
03/14/11   aro     Replaced FW enable with RFM interface
03/14/11   aro     Doxygen documentation update
03/08/11   aro     Compiler Warning Fix
03/02/11   aro     Renamed FTM and FTM-NS mode enums
03/02/11   aro     Include File cleanup
03/02/11   aro     Moved Set/Get Calibration state interface to RF
03/01/11   aro     Added WCDMA and SGM enter/exit functions
03/01/11   aro     Moved  FTM 1x/HDR enter and exit to system control file
03/01/11   aro     Moved Tech specific module to Top
03/01/11   aro     [1] Modularized the files for techs
                   [2] Doxygen update
02/16/11   aro     Cleaned up unused functions
02/15/11   aro     [1] Removed unused functions
                   [2] Changed mode enum type name
                   [3] Moved 1x enter and exit to 1x file
                   [4] Updated FTM Enter and Exit interface to take device
02/14/11   sty     Changed return types for ftm_enter and ftm_exit mode APIs
                   to boolean from void
02/13/11   aro     Added MC support for HDR Wakeup Rx Interface
02/10/11   sty     Added allocate and deallocate buffers for Rx and TxLM
02/02/11   aro     Cleaned up obselete code
02/02/11   aro     Added FTM CDMA Init
02/01/11   aro     Moved malloc function to FTM Common
01/15/11   aro     Initial Release
01/31/11    sr     Removed the HWIO_OUT which was NOT needed to fix linker err.
01/19/11    dw     Initial WCDMA RxLM support
01/19/11   aro     Updated the HDR API with link manaager parameters
01/19/11   aro     Renamed RF-MDSP filename
01/15/11   aro     Removed direct access to rfm_mode[] variable
12/20/10   aro     Removed hdr_enabled field
12/17/10   aro     Update RF Status Data Structure
12/14/10   aro     Removed rf_chain_status and expanded Status field
12/13/10   aro     Moved HDR State Variable to Common Data Structure
12/10/10   aro     Added back MDSP
12/09/10   aro     [1] Fixes for Zero-Compile Error SVDO Enabled Build
                   [2] Moved code to LNA specific file
12/07/10   aro     Added Initial SVDO Support

==============================================================================*/

#include "comdef.h"
#include "rf_hal_common.h"
#include "rfcommon_msg.h"
#include "stringl.h" /* for memscpy */
#include "ftm_common.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm_common_control.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "rfcommon_msm_cal.h"
#include "rfcommon_msm.h"
#endif /* FEATURE_GNSS_ONLY_NO_WWAN */
#include "ftm_rf_cmd.h"
#include "ftm.h"
#include "rfm_internal.h"
#include "ftm_msg.h"
#include "rxlm_intf.h"
#include "txlm_intf.h"
#include "rf_test_task_v.h"
#include "mcpm_api.h"
#include "npa.h"
#include "ftm_v.h"
#include "rfc_card.h"
#include "trm.h"
#include "ftm_common_data.h"
#include "fws.h"
#include "rfcommon_core_xpt_buffer.h"
#include "ftm_calibration_data_module.h"
#include "ftm_common_iq_processing.h"
#include "rfcommon_core_device_manager.h"
#include "ftm_common_concurrency_manager.h"

#ifdef FEATURE_CDMA1X
#include "ftm_cdma_data.h"
#include "ftm_1x_control.h"
#include "ftm_1x_msm.h"
#include "rfm_1x_ftm.h"
#include "rfm_cdma_ftm.h"
#include "rfcommon_mdsp.h"
#include "rf_cdma_data.h"
#include "rf_cdma_utils_freq.h"
#endif /* FEATURE_CDMA1X */

#ifdef FEATURE_HDR
#include "ftm_hdr_control.h"
#include "rfm_hdr_ftm.h"
#endif /* FEATURE_CDMA1X */

#ifdef FEATURE_WCDMA
#include "ftm_wcdma_ctl.h"
#include "l1task.h"
#include "rxdiv.h"
#include "ftm_wcdma_ber.h"
#include "rfdevice_wcdma_intf.h"
#include "rfc_card_wcdma.h"
#include "rf_task_common_services.h"
#include "rfwcdma_mc.h" 
#include "rfwcdma_core.h" 
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_TDSCDMA
#include "ftm_tdscdma_ctl.h"
#include "rfc_card_tdscdma.h"
#include "rfdevice_tdscdma_intf.h"
#endif /* FEATURE_TDSCDMA */

#ifdef FEATURE_GSM
#include "ftm_gsm_ctl.h"
#include "ftm_gsm_ber.h"
#include "ftm_gsm_rfctl.h"
#include "ftm_gsm_rfprot.h"
#include "gl1_hw.h"
#include "rfdevice_gsm_intf.h"
#include "rfgsm_core.h"
#endif /* FEATURE_GSM */

#ifdef FEATURE_LTE
#include "rflte_ftm_mc.h"
#include "rflte_ftm_mdsp.h"
#include "rflte_core_util.h"
#include "rflte_ftm_calibration.h"
#include "rflte_mdsp.h"
#include "rfcommon_core.h"
#include "rflte_mc.h"
#include "rflte_state.h"
#include "rf_control_msg.h"
#include "rfdevice_lte_interface.h"
#include "rflm_cmn_asd.h"
#endif /* FEATURE_LTE */

#ifdef FEATURE_RF_HAS_QTUNER
#include "rfdevice_hdet_cmn_intf.h"
#endif

#include "rfcommon_nv_mm.h"


/*----------------------------------------------------------------------------*/
/*! Maximum Q6 Speed in KHz */
#define RF_Q6_MAX_CPU_SPEED_KHZ 576000
/*! WL1 Start/Stop cmd timeout of 200 ms */
#define WL1_START_STOP_WAIT_TIMEOUT 200000 

#ifdef FEATURE_WCDMA
extern rfcom_wcdma_band_type ftm_curr_wcdma_mode;
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
extern rf_dispatch_outcome_t rflte_dispatch_exit_mode_req(rfa_rf_lte_exit_mode_req_s* rf_lte_msg_ptr, boolean send_resp_flag);
extern rf_dispatch_outcome_t rflte_dispatch_fdd_tx_disable_req(rfa_rf_lte_fdd_tx_disable_req_s* rf_lte_msg_ptr, boolean send_resp_flag );
extern rf_dispatch_outcome_t rflte_dispatch_tdd_tx_disable_req(rfa_rf_lte_tdd_tx_disable_req_s* rf_lte_msg_ptr, boolean send_resp_flag );
#endif

#ifdef FEATURE_GSM
extern void ftm_gsm_rx_disable( rfm_device_enum_type rfm_dev );
#endif

extern int32 rfm_exit_mode (rfcom_device_enum_type device, rfcom_mode_enum_type to_rfmode);

boolean selftest_lowband = FALSE;
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Variable to store the current FTM RF Mode on all devices
*/
static ftm_rfstate_enum_type ftm_rfmode_current[RFM_MAX_DEVICES] =
{
  FTM_STATE_UNKNOWN,
  FTM_STATE_UNKNOWN,
  FTM_STATE_UNKNOWN,
  FTM_STATE_UNKNOWN,
  FTM_STATE_UNKNOWN,
  FTM_STATE_UNKNOWN,
  FTM_STATE_UNKNOWN
};

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Build a look up table here to easily get rf tech and band from a given rf mode 
  passed in from FTM side (rf mode will be the index of the array).
  This look up table needs to be in sync with enum ftm_mode_id_type in ftm.h and the following band type enum: 
  rfm_cdma_band_class_type
  rfcom_wcdma_band_type
  rfcom_gsm_band_type
  rfcom_lte_band_type
  rfcom_tdscdma_band_type
*/
rf_tech_band_info tech_band_lookup[FTM_PHONE_MODE_MAX] = 
{
	{FTM_RF_TECH_CDMA, RFM_CDMA_MAX_BAND},    /*FTM_PHONE_MODE_CDMA, not a real valid rf mode for a particular CDMA band*/
	{FTM_RF_TECH_UNKNOWN, RFM_CDMA_MAX_BAND}, /*Padding*/ 
	{FTM_RF_TECH_UNKNOWN, RFM_CDMA_MAX_BAND}, /*FTM_PHONE_MODE_SLEEP, not a real valid rf mode for a particular tech and band*/
	{FTM_RF_TECH_UNKNOWN, RFM_CDMA_MAX_BAND}, /*FTM_PHONE_MODE_GPS, not a real valid rf mode for a particular tech and band*/
	{FTM_RF_TECH_UNKNOWN, RFM_CDMA_MAX_BAND}, /*Padding*/ 
	{FTM_RF_TECH_CDMA, RFM_CDMA_BC0},         /*FTM_PHONE_MODE_CDMA_800, CDMA, BC0*/
	{FTM_RF_TECH_CDMA, RFM_CDMA_BC1},         /*FTM_PHONE_MODE_CDMA_1900, CDMA, BC1*/
	{FTM_RF_TECH_CDMA, RFM_CDMA_BC8},         /*FTM_PHONE_MODE_HDR, CDMA, BC8*/
	{FTM_RF_TECH_CDMA, RFM_CDMA_BC4},		  /*FTM_PHONE_MODE_CDMA_1800, CDMA, BC4*/	
	{FTM_RF_TECH_WCDMA, RFCOM_BAND_IMT}, 	  /*FTM_PHONE_MODE_WCDMA_IMT, WCDMA, RFCOM_BAND_IMT*/
	{FTM_RF_TECH_GSM, RFCOM_BAND_GSM900},     /*FTM_PHONE_MODE_GSM_900, GSM, GSM900*/
	{FTM_RF_TECH_GSM, RFCOM_BAND_GSM1800},    /*FTM_PHONE_MODE_GSM_1800, GSM, GSM1800*/
	{FTM_RF_TECH_GSM, RFCOM_BAND_GSM1900},    /*FTM_PHONE_MODE_GSM_1900, GSM, GSM1900*/
	{FTM_RF_TECH_UNKNOWN, RFM_CDMA_MAX_BAND}, /*FTM_PHONE_MODE_BLUETOOTH, Bluetooth*/
	{FTM_RF_TECH_CDMA, RFM_CDMA_BC3},         /*FTM_PHONE_MODE_JCDMA, CDMA, BC3*/	
	{FTM_RF_TECH_WCDMA, RFCOM_BAND_1900},     /*FTM_PHONE_MODE_WCDMA_1900A, WCDMA, RFCOM_BAND_1900*/
    {FTM_RF_TECH_WCDMA, RFCOM_BAND_1900},     /*FTM_PHONE_MODE_WCDMA_1900B, WCDMA, RFCOM_BAND_1900*/
    {FTM_RF_TECH_CDMA, RFM_CDMA_BC5},		  /*FTM_PHONE_MODE_CDMA_450, CDMA, BC5*/
    {FTM_RF_TECH_GSM, RFCOM_BAND_GSM850},     /*FTM_PHONE_MODE_GSM_850, GSM, GSM850*/
    {FTM_RF_TECH_CDMA, RFM_CDMA_BC6},         /*FTM_PHONE_MODE_IMT, CDMA, BC6*/
    {FTM_RF_TECH_CDMA, RFM_CDMA_BC10},		  /*FTM_PHONE_MODE_HDR_800, CDMA, BC10*/
    {FTM_RF_TECH_CDMA, RFM_CDMA_BC14},        /*FTM_PHONE_MODE_HDR_1900, CDMA, BC14*/
    {FTM_RF_TECH_WCDMA, RFCOM_BAND_800},	  /*FTM_PHONE_MODE_WCDMA_800, WCDMA, RFCOM_BAND_800*/
	{FTM_RF_TECH_UNKNOWN, RFM_CDMA_MAX_BAND}, /*Padding*/ 
	{FTM_RF_TECH_UNKNOWN, RFM_CDMA_MAX_BAND}, /*Padding*/ 	
    {FTM_RF_TECH_WCDMA, RFCOM_BAND_BC3},	  /*FTM_PHONE_MODE_WCDMA_BC3=25, WCDMA, BC3*/
    {FTM_RF_TECH_CDMA, RFM_CDMA_BC14},		  /*FTM_PHONE_MODE_CDMA_BC14 = 26, CDMA, BC14*/
    {FTM_RF_TECH_CDMA, RFM_CDMA_BC11},		  /*FTM_PHONE_MODE_CDMA_BC11 = 27, CDMA, BC11*/
    {FTM_RF_TECH_WCDMA, RFCOM_BAND_BC4},	  /*FTM_PHONE_MODE_WCDMA_BC4=28, WCDMA, BC4*/
    {FTM_RF_TECH_WCDMA, RFCOM_BAND_BC8},      /*FTM_PHONE_MODE_WCDMA_BC8=29, WCDMA, BC8*/
    {FTM_RF_TECH_CDMA, RFM_CDMA_BC18}, 		  /*FTM_PHONE_MODE_MF_700=30, CDMA, BC18, */
    {FTM_RF_TECH_WCDMA, RFCOM_BAND_BC9},	  /*FTM_PHONE_MODE_WCDMA_BC9=31, WCDMA, BC9*/
    {FTM_RF_TECH_CDMA, RFM_CDMA_BC15},		  /*FTM_PHONE_MODE_CDMA_BC15=32, CDMA, BC15*/
    {FTM_RF_TECH_CDMA, RFM_CDMA_BC10},		  /*FTM_PHONE_MODE_CDMA_BC10=33, CDMA, BC10*/

    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B1},	  /*FTM_PHONE_MODE_LTE_B1=34, LTE, B1*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B7},	  /*FTM_PHONE_MODE_LTE_B7=35, LTE, B7*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B13},	  /*FTM_PHONE_MODE_LTE_B13=36, LTE, B13*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B17},	  /*FTM_PHONE_MODE_LTE_B17=37, LTE, B17*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B38},    /*FTM_PHONE_MODE_LTE_B38=38, LTE, B38*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B40},	  /*FTM_PHONE_MODE_LTE_B40=39, LTE, B40*/
    {FTM_RF_TECH_WCDMA, RFCOM_BAND_BC11},	  /*FTM_PHONE_MODE_WCDMA_BC11=40, WCDMA, BC11*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B11},	  /*FTM_PHONE_MODE_LTE_B11=41, LTE, B11*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B4},     /*FTM_PHONE_MODE_LTE_B4=42, LTE, B4*/


    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B2},	 /*FTM_PHONE_MODE_LTE_B2=43, LTE, B2*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B3},    /*FTM_PHONE_MODE_LTE_B3=44, LTE, B3*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B5},    /*FTM_PHONE_MODE_LTE_B5=45, LTE, B5*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B6},    /*FTM_PHONE_MODE_LTE_B6=46, LTE, B6*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B8},    /*FTM_PHONE_MODE_LTE_B8=47, LTE, B8*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B9},	 /*FTM_PHONE_MODE_LTE_B9=48, LTE, B9*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B10},   /*FTM_PHONE_MODE_LTE_B10=49, LTE, B10*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B12},   /*FTM_PHONE_MODE_LTE_B12=50, LTE, B12*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B14},   /*FTM_PHONE_MODE_LTE_B14=51, LTE, B14*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B15},   /*FTM_PHONE_MODE_LTE_B15=52, LTE, B15*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B16},   /*FTM_PHONE_MODE_LTE_B16=53, LTE, B16*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B18},   /*FTM_PHONE_MODE_LTE_B18=54, LTE, B18*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B19},   /*FTM_PHONE_MODE_LTE_B19=55, LTE, B19*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B20},   /*FTM_PHONE_MODE_LTE_B20=56, LTE, B20*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B21},   /*FTM_PHONE_MODE_LTE_B21=57, LTE, B21*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B22},	 /*FTM_PHONE_MODE_LTE_B22=58, LTE, B22*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B23},	 /*FTM_PHONE_MODE_LTE_B23=59, LTE, B23*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B24},   /*FTM_PHONE_MODE_LTE_B24=60, LTE, B24*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B25},   /*FTM_PHONE_MODE_LTE_B25=61, LTE, B25*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B26},   /*FTM_PHONE_MODE_LTE_B26=62, LTE, B26*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B27},   /*FTM_PHONE_MODE_LTE_B27=63, LTE, B27*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B28},   /*FTM_PHONE_MODE_LTE_B28=64, LTE, B28*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B29},   /*FTM_PHONE_MODE_LTE_B29=65, LTE, B29*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B30},   /*FTM_PHONE_MODE_LTE_B30=66, LTE, B30*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B31},	 /*FTM_PHONE_MODE_LTE_B31=67, LTE, B31*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B32},   /*FTM_PHONE_MODE_LTE_B32=68, LTE, B32*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B33},   /*FTM_PHONE_MODE_LTE_B33=69, LTE, B33*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B34},   /*FTM_PHONE_MODE_LTE_B34=70, LTE, B34*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B35},   /*FTM_PHONE_MODE_LTE_B35=71, LTE, B35*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B36},   /*FTM_PHONE_MODE_LTE_B36=72, LTE, B36*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B37},   /*FTM_PHONE_MODE_LTE_B37=73, LTE, B37*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B39},   /*FTM_PHONE_MODE_LTE_B39=74, LTE, B39*/
    {FTM_RF_TECH_WCDMA, RFCOM_BAND_BC19},    /*FTM_PHONE_MODE_WCDMA_BC19=75, WCDMA, BC19*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B41},   /*FTM_PHONE_MODE_LTE_B41=76, LTE, B41*/
    {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B42},   /*FTM_PHONE_MODE_LTE_B42=77, LTE, B42*/
	{FTM_RF_TECH_UNKNOWN, RFM_CDMA_MAX_BAND}, /*Padding*/ 
	{FTM_RF_TECH_UNKNOWN, RFM_CDMA_MAX_BAND}, /*Padding*/ 
	{FTM_RF_TECH_UNKNOWN, RFM_CDMA_MAX_BAND}, /*Padding*/ 
	{FTM_RF_TECH_UNKNOWN, RFM_CDMA_MAX_BAND}, /*Padding*/
	{FTM_RF_TECH_UNKNOWN, RFM_CDMA_MAX_BAND}, /*Padding*/ 
        {FTM_RF_TECH_LTE, RFCOM_BAND_LTE_B66},    /*FTM_PHONE_MODE_LTE_B66=83, LTE, B66*/        
	{FTM_RF_TECH_UNKNOWN, RFM_CDMA_MAX_BAND}, /*Padding*/ 
	{FTM_RF_TECH_UNKNOWN, RFM_CDMA_MAX_BAND}, /*Padding*/ 	
	{FTM_RF_TECH_UNKNOWN, RFM_CDMA_MAX_BAND}, /*Padding*/ 
	{FTM_RF_TECH_UNKNOWN, RFM_CDMA_MAX_BAND}, /*Padding*/ 
	{FTM_RF_TECH_UNKNOWN, RFM_CDMA_MAX_BAND}, /*Padding*/ 
	{FTM_RF_TECH_UNKNOWN, RFM_CDMA_MAX_BAND}, /*Padding*/ 		
    {FTM_RF_TECH_TDSCDMA, RFCOM_BAND_TDSCDMA_B34},/*FTM_PHONE_MODE_TDSCDMA_B34=90, TDSCDMA, B34*/
    {FTM_RF_TECH_TDSCDMA, RFCOM_BAND_TDSCDMA_B39},/*FTM_PHONE_MODE_TDSCDMA_B39=91, TDSCDMA, B39*/
    {FTM_RF_TECH_TDSCDMA, RFCOM_BAND_TDSCDMA_B40},/*FTM_PHONE_MODE_TDSCDMA_B40=92, TDSCDMA, B40*/	
};

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FTM Common Init

  @details
  This function is used to initialize any common code for FTM
*/
void ftm_common_init(void)
{
  /* Initialize the calibration data module */
  ftm_cal_data_init_data_module();
}

#ifdef FEATURE_RF_HAS_QTUNER
/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
*/
void* ftm_common_get_tuner_object(rfm_device_enum_type device, rfdevice_type_enum_type device_type)
{
  void *tuner_obj = NULL;
  void **tuner_obj_pp = NULL;
  uint8 tuner_dev_instance = 0;

  ftm_rfstate_enum_type tech = ftm_get_current_state(RFM_DEVICE_0);

  #ifdef FEATURE_CDMA1X
  rfm_cdma_band_class_type band;
  const rf_cdma_data_status_type *dev_status_r;
  const rfc_cdma_logical_device_params_type *logical_dev;
  #endif

  #ifdef FEATURE_GSM
  rfgsm_core_handle_type *rfgsm_core_handle_ptr;
  #endif

  /* Currently only support device 0 or device 1*/
  if(!((device == RFM_DEVICE_0) || (device == RFM_DEVICE_1)) ||
    /* Currently only support TUNER AND TDET and TUNER MGR*/
    !((device_type == RFDEVICE_TUNER) || (device_type == RFDEVICE_HDET) || (device_type == RFDEVICE_TUNER_MANAGER)))
  {
    MSG_ERROR("ftm_common_get_tuner_object: Input params invalid Device %d, Type %d", device, device_type, 0);
    return NULL;
  }

  /* Get TUNER object from RFC via Tech*/
  switch(tech )
  {
  /* ----------------- LTE ---------------- */
  #ifdef FEATURE_LTE
    case FTM_STATE_LTE:
      tuner_obj = rflte_ftm_mc_get_tuner_object(device, device_type);
      break;
  #endif
  /* ----------------- GSM ---------------- */
  #ifdef FEATURE_GSM
      case FTM_STATE_GSM:        
        rfgsm_core_handle_ptr = rfgsm_core_handle_get(device);
        if(rfgsm_core_handle_ptr != NULL)
        {
          tuner_obj_pp = rfc_gsm_get_rf_device_list( device, 
                                                 RFC_CONFIG_TX, 
                                                 rfgsm_core_get_tx_band(device), 
                                                     device_type);
          if( tuner_obj_pp != NULL)
          {
            tuner_obj = (void *)(tuner_obj_pp[tuner_dev_instance]);
          }
        }
      break;
  #endif  
  /* ----------------- WCDMA ---------------- */
  #ifdef FEATURE_WCDMA
    case FTM_STATE_WCDMA:
      tuner_obj_pp = rfc_wcdma_get_device( device, 
                                 RFC_CONFIG_TX, 
                                 rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx, 
                                 device_type );
      if( tuner_obj_pp != NULL)
      {
        tuner_obj = (void *)(tuner_obj_pp[tuner_dev_instance]);
      }
      
      break;
  #endif
  /* ----------------- CDMA ---------------- */
  #ifdef FEATURE_CDMA1X
    case FTM_STATE_1X:
      dev_status_r = rf_cdma_get_device_status( device );
      logical_dev = rfc_cdma_get_logical_device_params( device );

      if( (dev_status_r != NULL) && (logical_dev != NULL) )
      {
        band = dev_status_r->curr_band;
        if(device_type == RFDEVICE_HDET)
        {
          tuner_obj = logical_dev->hdet_obj[band];
        }
        else
        {
          tuner_obj = logical_dev->qtuner_obj[band];
        }
      }
      
      break;
  #endif
  /* ----------------- TDSCDMA ---------------- */
  #ifdef FEATURE_TDSCDMA
      case FTM_STATE_TDSCDMA:
        {
          tuner_obj_pp = rfc_tdscdma_get_rf_device_obj(device, 
                                                     RFC_CONFIG_TX, 
                                                     rf_tdscdma_mc_state.curr_band_tx, 
                                                     device_type );                                   
          if( tuner_obj_pp != NULL)
          {
            tuner_obj = (void *)(tuner_obj_pp[tuner_dev_instance]);            
          }
        
        }
        break;  
   #endif

    default:
      RF_MSG( RF_ERROR, "ftm_common_get_tuner_object invalid tech. Tuning complete?");
      break;
  }
  
  if( tuner_obj == NULL )
  {
    RF_MSG_1( RF_ERROR, "ftm_common_get_tuner_object: tuner_obj = NULL for tech %d", tech);
  }
  return tuner_obj;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
*/
boolean ftm_common_set_tuner_hdet_gain(void* hdet_obj, uint8 hdet_pid, uint8 gain_state)
{
  if(hdet_pid > 7 || gain_state > 9 || hdet_obj == NULL)
  {
    RF_MSG_2( RF_LOW, "rfcommon_mc_set_hdet_tuner_gain invalid params: gain %d, pid %d",
                                                                  hdet_pid, gain_state );
    return FALSE;
  }
  else
  {
    RF_MSG_2( RF_LOW, "rfcommon_mc_set_hdet_tuner_gain: gain %d for pid %d", gain_state, hdet_pid );
    return rfdevice_hdet_cmn_set_gain_state(hdet_obj, 
                                            NULL, RFDEVICE_EXECUTE_IMMEDIATE,
                                            gain_state, hdet_pid); 
  }
}
/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
*/

uint16 ftm_common_get_tuner_hdet_power(void* hdet_obj, uint8 hdet_pid)
{
  uint16 power = 0;

  if(hdet_pid > 7 || hdet_obj == NULL)
  {
    RF_MSG_1( RF_LOW, "ftm_common_get_tuner_hdet_power invalid params: pid %d",hdet_pid);
    power = 0xFFFF;
  }
  else
  {
    
    switch(hdet_pid)
    {
      case 4:        
      case 5:
      case 6:
        if(rfdevice_hdet_cmn_get_power(hdet_obj, NULL, RFDEVICE_EXECUTE_IMMEDIATE, hdet_pid, &power))
        {
          RF_MSG_2( RF_LOW, "ftm_common_get_tuner_hdet_power: pid %d pwr %d", hdet_pid, power );
        }
        else
        {
          RF_MSG_2( RF_LOW, "ftm_common_get_tuner_hdet_power: failed for pid %d pwr %d", hdet_pid, power );
        }
        break;
      default:
        power = 0xFFFF;
        RF_MSG_1( RF_LOW, "ftm_common_get_tuner_hdet_power request not valid for pid %d",hdet_pid);
        break;
    }

    
    
  }
  return power;

}
#endif /* FEATURE_RF_HAS_QTUNER */

/*============================================================================*/
/*!
  @name Processor Clock Settings

  @brief
  This section contains interfaces to configure the clock for processors for FTM
  Mode.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Vote for Q6 CPU speed

  @details
  This function will be used to vote the CPU speed. This functionality can
  only be used for FTM mode.
 
  @param req_freq
  The requeste frequency for CPU
*/

static void
ftm_vote_cpu_speed
(
  npa_resource_state req_freq
)
{
  ftm_common_proc_clock_type* ftm_proc_clock_ptr; /* Pointer to proc clock data*/
  ftm_mode_type curr_ftm_mode; /* Current FTM Mode */

  /* Get the current mode */
  curr_ftm_mode = ftm_get_mode();

  /* Check if we are in FTM Mode. If not return. */
  if( curr_ftm_mode != FTM_MODE )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_vote_cpu_speed : Not in FTM Mode -  %d",
               curr_ftm_mode );
    return;
  }

  /* Get Pointer to the processor clock data structure */
  ftm_proc_clock_ptr = ftm_common_get_proc_clock_ptr();

  /* Get the handle only once. After the handle is received successfully
  for q6clk processor, then never request the handle again. */
  if( ftm_proc_clock_ptr->handle_initialised == FALSE )
  {
    /* Create NPA handle only once*/
    ftm_proc_clock_ptr->npa_handle_q6clk = npa_create_sync_client( "/clk/cpu", 
                                                "RF_FTM", NPA_CLIENT_REQUIRED );

    /* Check the Handle status */
    if( ftm_proc_clock_ptr->npa_handle_q6clk != NULL )
    {
      /* Flag the handle initialized as successful as q6clk handle is good */
      ftm_proc_clock_ptr->handle_initialised = TRUE;
    }
    else
    {
      FTM_MSG_1( FTM_ERROR, "ftm_vote_cpu_speed : Failed to acquire q6clk handle %d",req_freq);
    }
  }

  if( ftm_proc_clock_ptr->npa_handle_q6clk != NULL )
  {
    /* q6clk handle initialized as successful therefore vote for clk speed change */
    npa_issue_required_request( ftm_proc_clock_ptr->npa_handle_q6clk, req_freq );
    FTM_MSG_1( FTM_MED, "ftm_vote_cpu_speed : Voted Clock Speed %d", req_freq );
  }
} /* ftm_vote_cpu_speed */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Unvote for Q6 CPU speed

  @details
  This function will be used to  vote the CPU speed. This functionality can
  only be used for FTM mode.
*/
static void
ftm_unvote_cpu_speed
(
  void
)
{
  ftm_mode_type curr_ftm_mode; /* Current FTM Mode */
  ftm_common_proc_clock_type* ftm_proc_clock_ptr; /* Pointer to proc clock data*/

  /* Get the current mode */
  curr_ftm_mode = ftm_get_mode();

  /* Check if we are in FTM Mode. If not return. */
  if( curr_ftm_mode != FTM_MODE )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_unvote_cpu_speed : Not in FTM Mode - %d",
               curr_ftm_mode );
    return;
  }

  /* Get Pointer to the processor clock data structure */
  ftm_proc_clock_ptr = ftm_common_get_proc_clock_ptr();

  if( ftm_proc_clock_ptr->npa_handle_q6clk != NULL )
  {
    /* Cancel the request for CPU speed */
    npa_cancel_request( ftm_proc_clock_ptr->npa_handle_q6clk );
    FTM_MSG( FTM_MED, "ftm_unvote_cpu_speed : Success" );
  }
} /* ftm_unvote_cpu_speed */


/*! \} */

/*============================================================================*/
/*!
  @name Internal Mode Transition Interface - 1x

  @brief
  This section contains interfaces to control the FTM mode transition for 1x.
*/
/*! \{ */
#ifdef FEATURE_CDMA1X

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enter 1x Mode for FTM

  @details
  Configure the RF to CDMA 1X mode. RF must be in PARK mode before calling
  this function.

  @param device RF Device to which FTM is to be entered

  @return
  TRUE on success and FALSE on failure
*/  
static boolean
ftm_1x_enter
(
  rfm_device_enum_type device
)
{
  boolean result = TRUE;  /* holds result for this function */
  ftm_cdma_data_status_type *device_status;
  lm_status_type rx_buffer_allocated = LM_INVALID_PARAMETER; /* Rx Buffer */
  lm_status_type tx_buffer_allocated = LM_INVALID_PARAMETER; /* Tx Buffer */
  ftm_rfstate_enum_type ftm_rfmode; /* Current FTM RF Mode */
  //ftm_cdma_mcpm_config_data_type *mcpm_ptr; /* Pointer to MCPM Data Structure */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Dev */

  ftm_rfmode = ftm_get_current_state(device);

  /* Do nothing, if the requested Mode is same as current mode in the system*/
  if ( ftm_rfmode == FTM_STATE_1X_NS ||
       ftm_rfmode == FTM_STATE_1X )
  {
    /* do nothing */
    return TRUE;
  }

  /* Get logical device parameter */
  logical_dev = rfm_cdma_ftm_get_logical_device_params(device);
  if ( logical_dev == NULL )
  {
    FTM_MSG_1(FTM_ERROR, "ftm_1x_enter: Failed to get valid logical device for"
                         " device %d", device); 
    return FALSE;
  }

  /* Get Device Status pointer */
  device_status = ftm_cdma_data_get_device_ptr(device);
  if ( device_status == NULL )
  {
    FTM_MSG_1(FTM_ERROR, "ftm_1x_enter: Failed to get valid device pointer for"
                        " device %d", device); 
    return FALSE;
  }

  /* Get Pointer to MCPM State Pointer */
  //mcpm_ptr = ftm_cdma_get_mcpm_state_ptr();

  /* Allocate RXLM buffers, only if Chain is valid */
  if ( logical_dev->ftm_rxlm_chain_onex < RXLM_CHAIN_MAX )
  {
    rx_buffer_allocated = rxlm_allocate_buffer( logical_dev->ftm_rxlm_chain_onex,
                                                LM_1X,
                                                &device_status->rxlm_handle);
  }

  /* Allocate TXLM buffers, only if Chain is valid */
  if ( logical_dev->ftm_txlm_chain_onex < TXLM_CHAIN_MAX )
  {
    tx_buffer_allocated = txlm_allocate_buffer( logical_dev->ftm_txlm_chain_onex,
                                                LM_1X,
                                                &device_status->txlm_handle );
  }

  /* Update the enter_falg to indicate whether the first of enter mode
  was done successfully or not. Based on this second half is executed
  or reverted. */
  if ( ( rx_buffer_allocated == LM_SUCCESS ) &&
       ( logical_dev->ftm_txlm_chain_onex == TXLM_CHAIN_MAX || 
         tx_buffer_allocated == LM_SUCCESS ) )
  {

     /* Register FW msgs response with FWRSP Task */
     ftm_1x_register_fw_msgs();

    /* Set the DB state to CDMA : For Legacy Support */
    ftm_set_rf_db_state(FTM_DB_RF_CDMA);

    /* MCPM Call to properly prepare the modem clock. These calls should be
      called in proper sequence to follow the MCPM STM. First vote MCPM before
      proceeding with MCPM State change */
    ftm_1x_vote_mcpm ( device ) ;
    ftm_1x_configure_mcpm_state ( TRUE ) ;

    /* Vote for Maximum CPU Speed */
    ftm_vote_cpu_speed( RF_Q6_MAX_CPU_SPEED_KHZ );

    /* Enable FW App. First Vote FW, before proceeding with FW State change */
    ftm_1x_vote_fw_state(device);
    ftm_1x_configure_fw_state( device, RFM_CDMA_FW_STATE_ACTIVE );
    
    FTM_MSG_1( FTM_HIGH, "ftm_1x_enter: Dev %d Success", device );

  } /* LM buffer allocation checks */
  
  /* check for : LM buffer allocation failures */
  if ( ( ( rx_buffer_allocated != LM_SUCCESS ) ||
         ( logical_dev->ftm_txlm_chain_onex != TXLM_CHAIN_MAX &&
           tx_buffer_allocated != LM_SUCCESS )
       )
     )
  {
    if ( rx_buffer_allocated == LM_SUCCESS )
    {
      /* deallocate RxLM buffer that may have been allocated */
      rxlm_deallocate_buffer( device_status->rxlm_handle );
    }

    if ( tx_buffer_allocated == LM_SUCCESS )
    {
      /* deallocate RxLM buffer that may have been allocated */
      txlm_deallocate_buffer( device_status->txlm_handle );
    }

    /* Update FTM Data Structrure to invalidate LM Buffer */
    ftm_cdma_data_invalidate_lm_buffers( device );

    FTM_MSG_1( FTM_ERROR, "ftm_1x_enter: Dev %d Failed", device );

    result = FALSE;
  }

  return result;

} /* ftm_1x_enter */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Exit 1x Mode for FTM

  @details
  This function is used to clean-up procedure for CDMA 1x.

  @param device RF Device for which FTM will exit 1x

  @return
  TRUE on success and FALSE on failure
*/  
static boolean
ftm_1x_exit
(
  rfm_device_enum_type device,
  ftm_rfstate_enum_type new_mode
)
{
  boolean result = TRUE;  /* holds result for this function */
  ftm_cdma_data_status_type *device_status;

  if ( ( device < RFM_DEVICE_0 ) || ( device >= RFM_MAX_DEVICES ) )
  {
    FTM_MSG_1 (FTM_ERROR, "ftm_1x_exit: Invalid Device %d", device);
    return FALSE;
  }

  /* Check if the device is in 1x Mode or not */
  if ( ftm_rfmode_current[device] == FTM_STATE_1X )
  {
    /* Get CDMA Device Pointer */
    device_status = ftm_cdma_data_get_device_ptr( device );
    if ( device_status != NULL )
    {
      lm_status_type rx_buffer_deallocated = LM_SUCCESS;
      lm_status_type tx_buffer_deallocated = LM_SUCCESS;
      //ftm_cdma_mcpm_config_data_type *mcpm_ptr; /* Pointer to MCPM Data Structure */

      /* Get Pointer to MCPM State Pointer */
      //mcpm_ptr = ftm_cdma_get_mcpm_state_ptr();
    
      /* Tear down 1x RF. Return type is insigificatnt for this case. */
      (void)ftm_1x_tear_down(device);
    
      /* DeAllocate RxLM Buffer */
      if ( device_status->rxlm_handle != FTM_INVALID_LM_BUFFER )
      {
        rx_buffer_deallocated =
          rxlm_deallocate_buffer( device_status->rxlm_handle );
      }
    
      /* DeAllocate TxLM Buffer */
      if ( device_status->txlm_handle != FTM_INVALID_LM_BUFFER )
      {
        tx_buffer_deallocated =
          txlm_deallocate_buffer( device_status->txlm_handle );

      }
    
      /* Update FTM Data Structrure to invalidate LM Buffer */
      ftm_cdma_data_invalidate_lm_buffers( device );
    
      if (( rx_buffer_deallocated != LM_SUCCESS ) ||
          ( tx_buffer_deallocated != LM_SUCCESS ))
      {
        result = FALSE;
        FTM_MSG(FTM_ERROR, "ftm_1x_exit: Buffer De-allocation failed");
      }
    
      /* Disable FW App. First unvote FW before proceeding with FW state change */
      ftm_1x_unvote_fw_state(device);
      ftm_1x_configure_fw_state( device, RFM_CDMA_FW_STATE_DISABLE );
    
      /* cancel vote for Maximum CPU Speed */
      ftm_unvote_cpu_speed();
    
      /* MCPM Call to properly turn off the modem clock. These calls should be
      called in proper sequence to follow the MCPM STM. First unvote MCPM before
      proceeding with MCM State change*/
      ftm_1x_unvote_mcpm(device);
      ftm_1x_configure_mcpm_state( FALSE );

      /* De-register FW msgs response with FWRSP Task */
      ftm_1x_deregister_fw_msgs();

      /* Reset RF mode after exiting from a given device */
      ftm_rfmode_current[device] = FTM_STATE_UNKNOWN;

      FTM_MSG_1( FTM_MED, "ftm_1x_exit : [ Done for Device %d ]", device );
    } /* if ( device_status != NULL ) */
    else
    {
      FTM_MSG_1( FTM_ERROR, "ftm_1x_exit: Null pointer for Device %d", device );
      result = FALSE;
    }

  } /* if ( ftm_rfmode_current[dev_to_use] == FTM_STATE_1X ) */
  else
  {
    FTM_MSG_1( FTM_HIGH, "ftm_1x_exit : [ Device %d not in 1x ]", device );
  } /* if !( ftm_rfmode_current[dev_to_use] == FTM_STATE_1X ) */

  /* Free dynamically allcoated memory (if not cleared till now or few structures which can be cleared
    only when exiting mode ) */
  ftm_cdma_free_memory();

  return result;

} /* ftm_1x_exit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enter 1x NS Mode for FTM

  @details
  Configure the RF to CDMA 1X mode. RF must be in PARK mode before calling
  this function.

  @param device RF Device to which FTM is to be entered

  @return
  TRUE on success and FALSE on failure
*/  
static boolean
ftm_1x_ns_enter
(
  rfm_device_enum_type device
)
{
  boolean result = TRUE;  /* holds result for this function */

  FTM_MSG_1( FTM_MED, "ftm_1x_ns_enter: Dev %d Done", device );

  return result;

} /* ftm_1x_ns_enter */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Exit 1x NS Mode for FTM

  @details
  This function is used to clean-up procedure for CDMA 1x.

  @param device RF Device for which FTM will exit 1x

  @return
  TRUE on success and FALSE on failure
*/  
static boolean
ftm_1x_ns_exit
(
  rfm_device_enum_type device,
  ftm_rfstate_enum_type new_mode
)
{
  boolean result = TRUE;  /* holds result for this function */

  /* Free dynamically allcoated memory (if not cleared till now or few structures which can be cleared
    only when exiting mode ) */
  ftm_cdma_free_memory();

  FTM_MSG_1( FTM_MED, "ftm_1x_ns_exit: Dev %d Done", device );

  return result;

} /* ftm_1x_ns_exit */

#endif /* FEATURE_CDMA1X */
/*! \} */

/*============================================================================*/
/*!
  @name Internal Mode Transition Interface - HDR

  @brief
  This section contains interfaces to control the FTM mode transition for HDR.
*/
/*! \{ */
#ifdef FEATURE_HDR

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enter HDR Mode for FTM

  @details
  Configure the RF to CDMA HDR mode. RF must be in PARK mode before calling
  this function.

  @param device RF Device to which FTM is to be entered

  @return
  TRUE on success and FALSE on failure
*/  
static boolean
ftm_hdr_enter
(
  rfm_device_enum_type device
)
{
  boolean result = TRUE;  /* holds result for this function */
  ftm_cdma_data_status_type *device_status;
  lm_status_type rx_buffer_allocated = LM_INVALID_PARAMETER; /* Rx Buffer */
  lm_status_type tx_buffer_allocated = LM_INVALID_PARAMETER; /* Tx Buffer */
  ftm_rfstate_enum_type ftm_rfmode; /* Current FTM RF Mode */
  //ftm_cdma_mcpm_config_data_type *mcpm_ptr; /* Pointer to MCPM Data Structure */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Dev */

  ftm_rfmode = ftm_get_current_state(device);

  /* Do nothing, if the requested Mode is same as current mode in the system*/
  if ( ftm_rfmode == FTM_STATE_HDR_NS ||
       ftm_rfmode == FTM_STATE_HDR )
  {
    /* do nothing */
    return TRUE;
  }

  /* Get logical device parameter */
  logical_dev = rfm_cdma_ftm_get_logical_device_params(device);
  if ( logical_dev == NULL )
  {
    FTM_MSG_1(FTM_ERROR, "ftm_hdr_enter: Failed to get valid logical device for"
                        " device %d", device); 
    return FALSE;
  }

  /* Get Pointer to Device State Pointer */
  device_status = ftm_cdma_data_get_device_ptr(device);
  if ( device_status == NULL )
  {
    FTM_MSG_1(FTM_ERROR, "ftm_hdr_enter: Failed to get valid device pointer for"
                         " device %d", device); 
    return FALSE;
  }

  /* Get Pointer to MCPM State Pointer */
  //mcpm_ptr = ftm_cdma_get_mcpm_state_ptr();

  /* Allocate RXLM buffers, only if Chain is valid */
  if ( logical_dev->ftm_rxlm_chain_hdr < RXLM_CHAIN_MAX )
  {
    rx_buffer_allocated = rxlm_allocate_buffer( logical_dev->ftm_rxlm_chain_hdr,
                                                LM_HDR, 
                                                 &device_status->rxlm_handle);

  }

  /* Allocate TXLM buffers, only if Chain is valid */
  if ( logical_dev->ftm_txlm_chain_hdr < TXLM_CHAIN_MAX )
  {
    tx_buffer_allocated = txlm_allocate_buffer( logical_dev->ftm_txlm_chain_hdr,
                                                LM_HDR,
                                                &device_status->txlm_handle);
  }


  /* Update the enter_flag to indicate whether the first of enter mode
  was done successfully or not. Based on this second half is executed
  or reverted. */
  if ( ( rx_buffer_allocated == LM_SUCCESS ) &&
       ( logical_dev->ftm_txlm_chain_hdr == TXLM_CHAIN_MAX || 
         tx_buffer_allocated == LM_SUCCESS ) )
  {
    
    /* Register FW msgs response with FWRSP Task */
    ftm_hdr_register_fw_msgs();

    /* Set the DB state to CDMA : For Legacy Support */
    ftm_set_rf_db_state(FTM_DB_RF_HDR);

    /* Need to put MCPM in DO_IDLE_RX and then in traffic state. First vote
    MCPM before proceeding with MCPM state change */
    ftm_hdr_vote_mcpm(device);
    ftm_hdr_configure_mcpm_state( TRUE );

    /* Enable HDR FW App. First Vote FW before proceeding with FW state change*/
    ftm_hdr_vote_fw_state(device);
    ftm_hdr_configure_fw_state( device, RFM_CDMA_FW_STATE_ACTIVE );

    FTM_MSG( FTM_HIGH, "ftm_hdr_enter: Success" );

    result = TRUE;
  } /* LM buffer allocation checks */ 

  
  /* check for  LM buffer allocation failures */
  if ( ( ( rx_buffer_allocated == LM_SUCCESS ) &&
         ( logical_dev->ftm_txlm_chain_hdr != TXLM_CHAIN_MAX &&
           tx_buffer_allocated != LM_SUCCESS )
        )
     )
  {
      if ( rx_buffer_allocated == LM_SUCCESS )
      {
        /* deallocate RxLM buffer that may have been allocated */
        rxlm_deallocate_buffer( device_status->rxlm_handle );
      }
  
      if ( tx_buffer_allocated == LM_SUCCESS )
      {
        /* deallocate RxLM buffer that may have been allocated */
        txlm_deallocate_buffer( device_status->txlm_handle );
      }
  
      /* Update FTM Data Structrure to invalidate LM Buffer */
      ftm_cdma_data_invalidate_lm_buffers( device );
  
      FTM_MSG( FTM_ERROR, "ftm_hdr_enter: Failed" );
  
      result = FALSE;
  }
  
  return result;

} /* ftm_hdr_enter */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Exit HDR Mode for FTM

  @details
  This function is used to clean-up procedure for CDMA HDR.

  @param device RF Device for which FTM will exit HDR

  @return
  TRUE on success and FALSE on failure
*/  
static boolean
ftm_hdr_exit
(
  rfm_device_enum_type device,
  ftm_rfstate_enum_type new_mode
)
{
  boolean result = TRUE;  /* holds result for this function */
  ftm_cdma_data_status_type *device_status;
  lm_status_type rx_buffer_deallocated = LM_INVALID_PARAMETER;
  lm_status_type tx_buffer_deallocated = LM_INVALID_PARAMETER;
  //ftm_cdma_mcpm_config_data_type *mcpm_ptr; /* Pointer to MCPM Data Structure */

  /* Get CDMA Device Pointer */
  device_status = ftm_cdma_data_get_device_ptr( device );
  if ( device_status == NULL )
  {
    FTM_MSG(FTM_ERROR, "ftm_hdr_exit: Device pointer is NULL !");
    return FALSE;
  }

  /* Get Pointer to MCPM State Pointer */
  //mcpm_ptr = ftm_cdma_get_mcpm_state_ptr();

  /* Tear down HDR RF. Return type is insigificatnt for this case. */
  (void)ftm_hdr_tear_down(device);

  /* DeAllocate RxLM Buffer */
  if ( device_status->rxlm_handle != FTM_INVALID_LM_BUFFER )
  {
    rx_buffer_deallocated =
      rxlm_deallocate_buffer( device_status->rxlm_handle );
  }

  /* DeAllocate TxLM Buffer */
  if ( device_status->txlm_handle != FTM_INVALID_LM_BUFFER )
  {
    tx_buffer_deallocated =
      txlm_deallocate_buffer( device_status->txlm_handle );
  }

  /* Update FTM Data Structrure to invalidate LM Buffer */
  ftm_cdma_data_invalidate_lm_buffers( device );

  if (( rx_buffer_deallocated == FALSE ) || ( tx_buffer_deallocated == FALSE ))
  {
    result = FALSE;
    FTM_MSG(FTM_ERROR, "ftm_hdr_exit: Buffer De-allocation failed");
  }

  /* Disable FW App. First unvote FW before proceeding with FW state change */
  ftm_hdr_unvote_fw_state(device);
  ftm_hdr_configure_fw_state( device, RFM_CDMA_FW_STATE_DISABLE );

  /* MCPM Call to properly turn off the modem clock. These calls should be
  called in proper sequence to follow the MCPM STM. First unvote MCPM before
  proceedign with MCPM state change */
  ftm_hdr_unvote_mcpm(device);
  ftm_hdr_configure_mcpm_state( FALSE );

  /* De-register FW msgs response with FWRSP Task */
  ftm_hdr_deregister_fw_msgs();

  return result;

} /* ftm_hdr_exit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enter HDR NS Mode for FTM

  @details
  Configure the RF to CDMA HDR mode. RF must be in PARK mode before calling
  this function.

  @param device RF Device to which FTM is to be entered

  @return
  TRUE on success and FALSE on failure
*/  
static boolean
ftm_hdr_ns_enter
(
  rfm_device_enum_type device
)
{
  boolean result = TRUE;  /* holds result for this function */

  FTM_MSG_1( FTM_MED, "ftm_hdr_ns_enter: Dev %d Done", device );

  return result;

} /* ftm_hdr_ns_enter */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Exit HDR NS Mode for FTM

  @details
  This function is used to clean-up procedure for CDMA HDR.

  @param device RF Device for which FTM will exit HDR

  @return
  TRUE on success and FALSE on failure
*/  
static boolean
ftm_hdr_ns_exit
(
  rfm_device_enum_type device,
  ftm_rfstate_enum_type new_mode
)
{
  boolean result = TRUE;  /* holds result for this function */

  FTM_MSG_1( FTM_MED, "ftm_hdr_ns_exit: Dev %d Done", device );

  return result;

} /* ftm_hdr_ns_exit */

#endif /* FEATURE_HDR */
/*! \} */

/*============================================================================*/
/*!
  @name Internal Mode Transition Interface - SVDO

  @brief
  This section contains interfaces to control the FTM mode transition for SVDO
*/
/*! \{ */
#if defined(FEATURE_HDR)&&defined(FEATURE_CDMA1X)
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enter SVDO Mode for FTM

  @details
  Configure the RF to CDMA SVDO mode. RF must be in PARK mode before calling
  this function.

  @param device RF Device to which FTM is to be entered

  @return
  TRUE on success and FALSE on failure
*/  
static boolean
ftm_svdo_enter
(
  rfm_device_enum_type device
)
{
  boolean result = TRUE;  /* holds result for this function */
  ftm_cdma_data_status_type *device_status;
  lm_status_type rx_buffer_allocated = LM_INVALID_PARAMETER; /* Rx Buffer */
  lm_status_type tx_buffer_allocated = LM_INVALID_PARAMETER; /* Tx Buffer */
  ftm_rfstate_enum_type ftm_rfmode; /* Current FTM RF Mode */
  /* ftm_cdma_mcpm_config_data_type *mcpm_ptr; Pointer to MCPM Data Structure */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Dev */
  
  ftm_rfmode = ftm_get_current_state(device);

  /* Do nothing, if the requested Mode is same as current mode in the system*/
  if ( ftm_rfmode == FTM_STATE_SVDO )
  {
    FTM_MSG_1(FTM_LOW, "ftm_svdo_enter: Already in SVDO state "
                        "for device %d", device); 
    return TRUE;
  }

  /* Get logical device parameter */
  logical_dev = rfm_cdma_ftm_get_logical_device_params(device);

  if ( logical_dev == NULL )
  {
    FTM_MSG_1(FTM_ERROR, "ftm_svdo_enter: Failed to get valid logical device "
                        "for device %d", device); 
    return FALSE;
  }

  /* Get Pointer to Device State Pointer */
  device_status = ftm_cdma_data_get_device_ptr(device);
  if ( device_status == NULL )
  {
    FTM_MSG_1(FTM_ERROR, "ftm_svdo_enter: Failed to get valid device pointer "
                         "for device %d", device); 
    return FALSE;
  }

  /* Get Pointer to MCPM State Pointer 
  mcpm_ptr = ftm_cdma_get_mcpm_state_ptr();*/

  /* For allocating LM buffers in FTM for SVDO, overload the HDR scenario
  since both 1x and HDR need to use the same LM buffers, this should suffice
  because rxlm_handle and txlm_handle are not tech specific in FTM mode */
  /* Allocate RXLM buffers, only if Chain is valid */
  if ( logical_dev->ftm_rxlm_chain_hdr < RXLM_CHAIN_MAX ) 
  {
    rx_buffer_allocated = rxlm_allocate_buffer( logical_dev->ftm_rxlm_chain_hdr, 
                                                LM_HDR, 
                                                &device_status->rxlm_handle);
  }

  /* Allocate TXLM buffers, only if Chain is valid */
  if ( logical_dev->ftm_txlm_chain_hdr < TXLM_CHAIN_MAX )
  {
    tx_buffer_allocated = txlm_allocate_buffer( logical_dev->ftm_txlm_chain_hdr,
                                                LM_HDR,
                                                &device_status->txlm_handle);
  }


  /* Update the enter_flag to indicate whether the first of enter mode
  was done successfully or not. Based on this second half is executed
  or reverted. */
  if ( ( rx_buffer_allocated == LM_SUCCESS ) &&
       ( (logical_dev->ftm_txlm_chain_hdr == TXLM_CHAIN_MAX )|| 
         tx_buffer_allocated == LM_SUCCESS ) )
  {
    /* Register 1x FW msgs response with FWRSP Task */
    ftm_1x_register_fw_msgs();

    /* Register HDR FW msgs response with FWRSP Task */
    ftm_hdr_register_fw_msgs();

    /* Set the DB state to CDMA : For Legacy Support */
    ftm_set_rf_db_state(FTM_DB_RF_HDR);

    /* Need to put MCPM in DO_IDLE_RX and then in traffic state. First vote
    MCPM before proceeding with MCPM state change*/
    ftm_hdr_vote_mcpm(device);
    ftm_hdr_configure_mcpm_state( TRUE ); 

    /* Enable HDR FW App. First Vote FW before proceeding with FW state change*/
    ftm_hdr_vote_fw_state(device);
    ftm_hdr_configure_fw_state( device, RFM_CDMA_FW_STATE_ACTIVE );

    /* Enable FW App. First Vote FW, before proceeding with FW State change */
    ftm_1x_vote_fw_state(device);
    ftm_1x_configure_fw_state( device, RFM_CDMA_FW_STATE_ACTIVE );

    FTM_MSG( FTM_HIGH, "ftm_svdo_enter: Success" );

    result = TRUE;
  } /* LM buffer allocation checks */ 

  
  /* check for  LM buffer allocation failures */
  if ( ( ( rx_buffer_allocated != LM_SUCCESS ) &&
         ( (logical_dev->ftm_txlm_chain_hdr != TXLM_CHAIN_MAX) &&
           tx_buffer_allocated != LM_SUCCESS )
        )
     )
  {
      if ( rx_buffer_allocated == LM_SUCCESS )
      {
        /* deallocate RxLM buffer that may have been allocated */
        rxlm_deallocate_buffer( device_status->rxlm_handle );
      }
  
      if ( tx_buffer_allocated == LM_SUCCESS )
      {
        /* deallocate RxLM buffer that may have been allocated */
        txlm_deallocate_buffer( device_status->txlm_handle );
      }
  
      /* Update FTM Data Structrure to invalidate LM Buffer */
      ftm_cdma_data_invalidate_lm_buffers( device );
  
      FTM_MSG( FTM_ERROR, "ftm_svdo_enter: Failed" );
  
      result = FALSE;
  }/* if ( ( ( rx_buffer_allocated == LM_SUCCESS ) &&
         ( logical_dev->ftm_txlm_chain_hdr != TXLM_CHAIN_MAX &&
           tx_buffer_allocated ! */
  
  return result;

} /* ftm_svdo_enter */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Exit SVDO Mode for FTM

  @details
  This function is used to clean-up procedure for CDMA SVDO.

  @param device RF Device for which FTM will exit SVDO

  @return
  TRUE on success and FALSE on failure
*/  
static boolean
ftm_svdo_exit
(
  rfm_device_enum_type device,
  ftm_rfstate_enum_type new_mode
)
{
  boolean result = TRUE;  /* holds result for this function */
  ftm_cdma_data_status_type *device_status;
  lm_status_type rx_buffer_deallocated = LM_INVALID_PARAMETER;
  lm_status_type tx_buffer_deallocated = LM_INVALID_PARAMETER;
  /*ftm_cdma_mcpm_config_data_type *mcpm_ptr;  Pointer to MCPM Data Structure */

  /* Get CDMA Device Pointer */
  device_status = ftm_cdma_data_get_device_ptr( device );

  if ( device_status == NULL )
  {
    FTM_MSG(FTM_ERROR, "ftm_svdo_exit: Device pointer is NULL !");
    return FALSE;
  }

  /* Get Pointer to MCPM State Pointer 
  mcpm_ptr = ftm_cdma_get_mcpm_state_ptr();*/

  /* Tear down HDR RF. Return type is insigificatnt for this case. 
  (void)ftm_hdr_tear_down(device);*/

  /* DeAllocate RxLM Buffer */
  if ( device_status->rxlm_handle != FTM_INVALID_LM_BUFFER )
  {
    rx_buffer_deallocated =
      rxlm_deallocate_buffer( device_status->rxlm_handle );
  }

  /* DeAllocate TxLM Buffer */
  if ( device_status->txlm_handle != FTM_INVALID_LM_BUFFER )
  {
    tx_buffer_deallocated =
      txlm_deallocate_buffer( device_status->txlm_handle );
  }

  /* Update FTM Data Structrure to invalidate LM Buffer */
  ftm_cdma_data_invalidate_lm_buffers( device );

  if (( rx_buffer_deallocated == FALSE ) || ( tx_buffer_deallocated == FALSE ))
  {
    result = FALSE;
    FTM_MSG(FTM_ERROR, "ftm_svdo_exit: Buffer De-allocation failed");
  }

  /* Disable FW App. First unvote FW before proceeding with FW state change */
  ftm_1x_unvote_fw_state(device);
  ftm_1x_configure_fw_state( device, RFM_CDMA_FW_STATE_DISABLE );

  /* Disable FW App. First unvote FW before proceeding with FW state change */
  ftm_hdr_unvote_fw_state(device);
  ftm_hdr_configure_fw_state( device, RFM_CDMA_FW_STATE_DISABLE );

  /* MCPM Call to properly turn off the modem clock. These calls should be
  called in proper sequence to follow the MCPM STM. First unvote MCPM before
  proceedign with MCPM state change */
  ftm_hdr_unvote_mcpm(device);
  ftm_hdr_configure_mcpm_state( FALSE );

  /* De-register HDR FW msgs response with FWRSP Task */
  ftm_hdr_deregister_fw_msgs();

  /* De-register 1x FW msgs response with FWRSP Task */
  ftm_1x_deregister_fw_msgs();

  return result;

} /* ftm_svdo_exit */

#endif /* defined(FEATURE_HDR)&&defined(FEATURE_CDMA1X)
 */

/*! \} */

/*============================================================================*/
/*!
  @name Internal Mode Transition Interface - GSM

  @brief
  This section contains interfaces to control the FTM mode transition for GSM.
*/
/*! \{ */
#ifdef FEATURE_GSM

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enter GSM Mode for FTM

  @details
  Configure the RF to GSM mode. RF must be in PARK mode before calling
  this function.

  @param device RF Device to which FTM is to be entered

  @return
  TRUE on success and FALSE on failure
*/  


rfm_device_enum_type device_in_ftm_mode = RFM_MAX_DEVICES;


void ftm_gsm_reset_device ( void )
{

  if( device_in_ftm_mode < RFM_MAX_DEVICES )
  {
    ftm_rfmode_current[device_in_ftm_mode] = FTM_STATE_UNKNOWN;

  }

}


static boolean ftm_gsm_ns_enter( rfm_device_enum_type device )
{
  /* Nothing done here at present */
  return TRUE;
}


static boolean ftm_gsm_ns_exit( rfm_device_enum_type device )
{
  boolean status = FALSE;

  ftm_gsm_iq_capture_info_free_memory();
  status = ftm_gsm_ber_cleanup(device);
  return status;
}

static boolean ftm_gsm_enter( rfm_device_enum_type device )
{
  ftm_rfstate_enum_type ftm_rf_state; /* Current FTM RF Mode */

  ftm_gsm_rfctl_vars_s * vars_ptr = ftm_gsm_create_and_init_vars( device );
  
  rfcom_band_type_u rfcom_band;
  ftm_concurrency_manager_params_type conmgr_params;
  uint64 rxd_dev_band_mask;
  rfm_device_enum_type rxd_dev = rfgsm_core_get_drx_dev(device);
  sys_band_mask_type bit_mask;
  
  
  if( vars_ptr == NULL) 
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "FTM GSM variables for device %d are NULL", device );
    return FALSE;
  }

  /* call concurrency manager to check valid concurrency */
  rfcom_band.gsm_band = ftm_gsm_convert_ftm_mode_id_type_to_rfcom((ftm_mode_id_type)vars_ptr->ftm_mode_id); 
  conmgr_params.band = rfcom_band;
  conmgr_params.carrier_index = 0;
  conmgr_params.prx_drx = RFC_PRX_PATH;
  conmgr_params.rfm_dev = device;
  conmgr_params.tech = RFM_EGSM_MODE;
  if(!ftm_concurrency_manager_validate_operation(&conmgr_params))
  {
    MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Invalid concurrency on dev %d", device );
    return FALSE;
  }

  /*check rxd is used or not*/
  
  /* get corresponding band bit_mask check if div is enabled */ 
  switch(rfcom_band.gsm_band)
  {
  case RFCOM_BAND_GSM850:
    bit_mask = SYS_BAND_MASK_GSM_850;
    break;
  case RFCOM_BAND_GSM900:
    bit_mask = SYS_BAND_MASK_GSM_EGSM_900;
    break;
  case RFCOM_BAND_GSM1800:
    bit_mask = SYS_BAND_MASK_GSM_DCS_1800;
    break;
  case RFCOM_BAND_GSM1900:
    bit_mask = SYS_BAND_MASK_GSM_PCS_1900;
    break;
  default : 
    bit_mask = RFCOM_NUM_GSM_BANDS;
    break;
   }

	
  rxd_dev_band_mask = rfcommon_nv_get_rf_bc_config( (rf_path_enum_type)(rxd_dev) );
  if (rxd_dev_band_mask & bit_mask )
  {
    if(vars_ptr->diversity_on)
    {
    conmgr_params.prx_drx = RFC_DRX_PATH;
    conmgr_params.rfm_dev = rxd_dev;
      if(!ftm_concurrency_manager_validate_operation(&conmgr_params))
      {        
        MSG_1( MSG_SSID_FTM, MSG_LEGACY_ERROR, "Invalid concurrency on dev %d", rxd_dev );
        return FALSE; 
      }
    }
  }
  

  ftm_rf_state = ftm_get_current_state(device);

  /* Do nothing, if the requested Mode is same as current mode in the system*/
  device_in_ftm_mode = device; 

  if( ftm_rf_state == FTM_STATE_GSM  || ftm_rf_state == FTM_STATE_GSM_NS )
  {
    /* do nothing */
     return TRUE;
  }

  /* Sanity check and helps with debugging */
  vars_ptr->rfm_dev = device;

  /* Start geran test mode */
  ftm_gsm_start_geran( device );
  
  /* Vote for Maximum CPU Speed */
  ftm_vote_cpu_speed( RF_Q6_MAX_CPU_SPEED_KHZ );

  return TRUE;

} /* ftm_gsm_enter */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Exit GSM Mode for FTM

  @details
  This function is used to clean-up procedure for GSM.

  @param device RF Device for which FTM will exit GSM

  @return
  TRUE on success and FALSE on failure
*/  
static boolean
ftm_gsm_exit
(
  rfm_device_enum_type device
)
{
  ftm_gsm_stop_geran( device );

  ftm_set_rf_db_state(FTM_DB_RF_IDLE);
  
  ftm_gsm_iq_capture_info_free_memory();

  ftm_gsm_free_vars( device );

  /* cancel vote for Maximum CPU Speed */
  ftm_unvote_cpu_speed();
  
  return TRUE;
} /* ftm_gsm_exit */

#endif /* FEATURE_GSM */
/*! \} */


/*============================================================================*/
/*!
  @name Internal Mode Transition Interface - LTE

  @brief
  This section contains interfaces to control the FTM mode transition for LTE.
*/
/*! \{ */
#ifdef FEATURE_LTE
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enter LTE Mode for FTM

  @details
  Configure the RF to LTE mode. RF must be in PARK mode before calling
  this function.

  @param device RF Device to which FTM is to be entered

  @return
  TRUE on success and FALSE on failure
*/  
static boolean
ftm_lte_enter
(
  rfm_device_enum_type device
)
{
  rflte_ftm_mc_sm_type     *lte_ftm_mc_sm_ptr      = NULL; /*holds pointer to LTE ftm State machine structure */
  lm_status_type           rx_buffer_prx_allocated = LM_BUFFER_NOT_ALLOCATED; /* Flag to indicate Rx Buffer Success */
  lm_status_type           rx_buffer_drx_allocated = LM_BUFFER_NOT_ALLOCATED; /* Flag to indicate Rx Buffer Success */
  lm_status_type           tx_buffer_allocated     = LM_BUFFER_NOT_ALLOCATED; /* Flag to indicate Tx Buffer Success */
  ftm_rfstate_enum_type    ftm_rfmode              = FTM_STATE_MAX; /* Current FTM RF Mode */
  lte_cphy_test_l1_mode_change_req_s mode_chg_req_msg;
  errno_enum_type          ret_status              = E_SUCCESS;
  boolean result = FALSE;  /* holds result for this function */
  uint32 carrier_idx = 0;
  uint8  loop_count  = 0 ;
  rfm_device_enum_type prx_device = RFM_INVALID_DEVICE;
  rfm_device_enum_type drx_device = RFM_INVALID_DEVICE;

  ftm_rfmode = ftm_get_current_state(device);

  /* Get LTE FTM state machine handle */
  lte_ftm_mc_sm_ptr = rflte_ftm_mc_get_sm();
  if(lte_ftm_mc_sm_ptr == NULL)
  {
    FTM_MSG( FTM_ERROR, "ftm_lte_enter: cannot get LTE FTM SM handle" );
    return FALSE;
  }

  if((device == RFM_DEVICE_0) || (device == RFM_DEVICE_1))
  {
    rflte_ftm_mc_sm_set_carrier_idx(RFLTE_MC_PCELL_IDX);
    rflte_ftm_mc_sm_set_prx_device(RFM_DEVICE_0);
    rflte_ftm_mc_sm_set_drx_device(RFM_DEVICE_1);
    rflte_ftm_mc_sm_set_tx_device(RFM_DEVICE_0);
    carrier_idx = 0;
  }
  else if((device >= RFM_DEVICE_2) && (device <= RFM_DEVICE_5))
  {
    /*check if it's in HORXD mode. if not, go through it to update carrier idx, prx/drx devices*/
    if(lte_ftm_mc_sm_ptr->current_carrier_idx != RFLTE_MC_PCELL_IDX)
    {
    /* Loop through all supported Secondary Carriers to check if "INPUT" Device is being used by one.  *
       If YES: re-set Device to same Carrier. If NO - Default to Hard-coded assignments.               */
    for(loop_count = RFLTE_MC_SCELL1_IDX; loop_count < RFA_RF_LTE_MAX_CELL_SUPPORTED ;loop_count++)
    {
      /* Set FTM State Carrier Index to current loop "Cell Idx". */
      rflte_ftm_mc_sm_set_carrier_idx(loop_count);
      if((rflte_ftm_mc_sm_get_prx_device() == device) ||
         (rflte_ftm_mc_sm_get_drx_device() == device))
      {
         rflte_ftm_mc_sm_set_carrier_idx(loop_count);

         /* Assign Even RFM Dev to PRx and Odd RFM Dev to DRx in RFM Pair. - To match with legacy implementation. */
         prx_device = (((device %2) == 0) ? device : (device-1));
         drx_device = (((device %2) == 0) ? (device+1) : device);

         rflte_ftm_mc_sm_set_prx_device(prx_device);
         rflte_ftm_mc_sm_set_drx_device(drx_device);
         rflte_ftm_mc_sm_set_tx_device(prx_device);
         carrier_idx = loop_count;
         break;
       }
    }

    /* If NO Match found - then Default to Hard-coded assignments. */ 
    if(RFA_RF_LTE_MAX_CELL_SUPPORTED == loop_count)
    {
      if ((device == RFM_DEVICE_2) || (device == RFM_DEVICE_3))
      {
        rflte_ftm_mc_sm_set_carrier_idx(RFLTE_MC_SCELL1_IDX);
        rflte_ftm_mc_sm_set_prx_device(RFM_DEVICE_2);
        rflte_ftm_mc_sm_set_drx_device(RFM_DEVICE_3);
        rflte_ftm_mc_sm_set_tx_device(RFM_DEVICE_2);
        carrier_idx = 1;
      }
      else if ((device == RFM_DEVICE_4) || (device == RFM_DEVICE_5))
      {
        rflte_ftm_mc_sm_set_carrier_idx(RFLTE_MC_SCELL2_IDX);
        rflte_ftm_mc_sm_set_prx_device(RFM_DEVICE_4);
        rflte_ftm_mc_sm_set_drx_device(RFM_DEVICE_5);
        rflte_ftm_mc_sm_set_tx_device(RFM_DEVICE_4);
        carrier_idx = 2;
      }
    }
    }
    else
    {
      FTM_MSG_1( FTM_MED, "ftm_lte_enter : Match found for SCELL Device in FTM State Machine, "
                          "Hence NOT Hard-Coding Devices for Carrier Idx: %d. ", carrier_idx );
    }
  }
  else
  {
    FTM_MSG_1( FTM_ERROR, "ftm_lte_enter: Invalid device %d",device );
    return FALSE;
  }

  /* Do nothing, if the requested Mode is same as current mode in the system*/
  if ( ftm_rfmode == FTM_STATE_LTE ||
       ftm_rfmode == FTM_STATE_LTE_NS )
  {
    /* do nothing */
    return TRUE;
  }

  /* First thing to do is to start clocks. If there was no clock Enter mode has never been called before
   * In this case, start clock set up FW, and Sent L1 message
   */
  if((rflte_ftm_mc_sm_is_clk_started() == FALSE) && 
     ((device == RFM_DEVICE_0) || (device == RFM_DEVICE_2) || (device == RFM_DEVICE_4))) 
  {
    /* Set up LTE clocks to default clock since BW is not known */
    rflte_ftm_mc_set_lte_clock(RF_LTE_FTM_START, RFCOM_BW_LTE_10MHz);

     /* Set a default rf mode, so the RF driver initializes in FTM test mode 
     * (Must not be FTM_DB_RF_IDLE). 
     * Actual mode and band will be set up in ftm_lte_set_mode()
     */
    ftm_set_rf_db_state(FTM_DB_RF_LTE_BC1);
    
    /* Init IQ capture */
    rflte_core_iq_capture_func_ptr_init();

    /* Notify ML1 that FTM mode has been entered */
    msgr_init_hdr( &mode_chg_req_msg.msgr_hdr, MSGR_RFA_RF_LTE_FTM, LTE_CPHY_TEST_L1_MODE_CHANGE_REQ);    
    mode_chg_req_msg.mode = LTE_L1_FTM_CAL_MODE;

    ret_status = msgr_send( &(mode_chg_req_msg.msgr_hdr), sizeof(lte_cphy_test_l1_mode_change_req_s) );

#ifdef TEST_FRAMEWORK
    #error code not present
#endif

    if (ret_status != E_SUCCESS)
    {
      ERR_FATAL("RF LTE FTM Command Processor cannot send LTE_CPHY_TEST_L1_MODE_CHANGE_REQ, cmd_cod=%d, err=%d",
                 mode_chg_req_msg.msgr_hdr.id, ret_status, 0);
    }
  }

  /* Previously running Tech might exit FW APP before we call LTE enter. For All devices we need 
  to make sure FW App is enabled in case Device1/3/5 was just exited by a previuos tech. */

  /* Enable FW App */
  rflte_ftm_mdsp_enter();

  if((device == RFM_DEVICE_0) || (device == RFM_DEVICE_2) || (device == RFM_DEVICE_4))
  {
    /* allocate Rx and Tx LM buffers to all devices */
    if((lte_ftm_mc_sm_ptr->current_carrier_idx == RFLTE_MC_PCELL_IDX) && ((device == RFM_DEVICE_2) || (device == RFM_DEVICE_4)))
    {
      /*HORXD DRX2 chain*/
      rx_buffer_prx_allocated = rxlm_allocate_buffer( RXLM_CHAIN_0,
                                                      LM_LTE,
	                                              &(lte_ftm_mc_sm_ptr->carrier[RFLTE_MC_PCELL_IDX].rxlm_drx2_buf_idx));
      if (rx_buffer_prx_allocated != LM_SUCCESS) 
      {
        lte_ftm_mc_sm_ptr->carrier[RFLTE_MC_PCELL_IDX].rxlm_drx2_buf_idx = FTM_INVALID_LM_BUFFER;
      }
      /*tx buffer has been allocated during PCELL tune*/
      tx_buffer_allocated = LM_SUCCESS;
    }
    else
    {
      /* allocate Rx and Tx LM buffers to all devices */
    rx_buffer_prx_allocated = rxlm_allocate_buffer( RXLM_CHAIN_0,
                                                    LM_LTE,
                                                    &(lte_ftm_mc_sm_ptr->carrier[carrier_idx].rxlm_prx_buf_idx));
      if (rx_buffer_prx_allocated != LM_SUCCESS) 
      {
        lte_ftm_mc_sm_ptr->carrier[carrier_idx].rxlm_prx_buf_idx = FTM_INVALID_LM_BUFFER;
      }

    tx_buffer_allocated = txlm_allocate_buffer( TXLM_CHAIN_0,
                                                LM_LTE,
                                                &(lte_ftm_mc_sm_ptr->carrier[carrier_idx].txlm_buf_idx));
      if (tx_buffer_allocated != LM_SUCCESS) 
      {
        lte_ftm_mc_sm_ptr->carrier[carrier_idx].txlm_buf_idx = FTM_INVALID_LM_BUFFER;
      }
    }

    /* Update the enter_falg to indicate whether the first of enter mode
    was done successfully or not. Based on this second half is executed
    or reverted. */
    if ( ( rx_buffer_prx_allocated == LM_SUCCESS ) &&( tx_buffer_allocated == LM_SUCCESS ) )
    {
      result = TRUE;
      {
        /*Set the min and max limits*/
        rflte_ftm_mc_sm_set_min_max_tx_gain_index(0,128);
        /* Vote for Maximum CPU Speed */
        ftm_vote_cpu_speed( RF_Q6_MAX_CPU_SPEED_KHZ );

        FTM_MSG_1( FTM_HIGH, "ftm_lte_enter: Success on Primary DEVICE %d", device );  
      }
    }
  }
  else if ((device == RFM_DEVICE_1) || (device == RFM_DEVICE_3) || (device == RFM_DEVICE_5))
  {
    /* allocate Rx and Tx LM buffers to all devices */
    if((lte_ftm_mc_sm_ptr->current_carrier_idx == RFLTE_MC_PCELL_IDX) && ((device == RFM_DEVICE_3) || (device == RFM_DEVICE_5)))
    {
      /*HORXD DRX3 chain*/
      rx_buffer_drx_allocated = rxlm_allocate_buffer( RXLM_CHAIN_1,
                                                      LM_LTE,
	                                              &(lte_ftm_mc_sm_ptr->carrier[RFLTE_MC_PCELL_IDX].rxlm_drx3_buf_idx));        
      if (rx_buffer_drx_allocated != LM_SUCCESS) 
      {
        lte_ftm_mc_sm_ptr->carrier[RFLTE_MC_PCELL_IDX].rxlm_drx3_buf_idx = FTM_INVALID_LM_BUFFER;
      }       
    }
    else
    {
      /* allocate Rx and Tx LM buffers to all devices */
    rx_buffer_drx_allocated = rxlm_allocate_buffer( RXLM_CHAIN_1,
                                                    LM_LTE,
                                                    &(lte_ftm_mc_sm_ptr->carrier[carrier_idx].rxlm_drx_buf_idx));
      if (rx_buffer_drx_allocated != LM_SUCCESS) 
      {
        lte_ftm_mc_sm_ptr->carrier[carrier_idx].rxlm_drx_buf_idx = FTM_INVALID_LM_BUFFER;
      }
    }
    if ( rx_buffer_drx_allocated == LM_SUCCESS)
    {
      /* Enter LTE mode */
      result = TRUE;
      {
        /* Vote for Maximum CPU Speed */
        ftm_vote_cpu_speed( RF_Q6_MAX_CPU_SPEED_KHZ );
      
        FTM_MSG_1( FTM_HIGH, "ftm_lte_enter: Success on Diversity DEVICE %d", device );  
      }
    }
  }
  else
  {
    FTM_MSG_1(FTM_ERROR, "ftm_lte_enter: device %d not supported",device);
  }

  if (result == FALSE )
  {
    if ( rx_buffer_prx_allocated == TRUE )
    {
      /* deallocate RxLM buffer that may have been allocated */
      rxlm_deallocate_buffer( lte_ftm_mc_sm_ptr->carrier[carrier_idx].rxlm_prx_buf_idx );
    }

    if ( rx_buffer_drx_allocated == TRUE )
    {
      /* deallocate RxLM buffer that may have been allocated */
      rxlm_deallocate_buffer( lte_ftm_mc_sm_ptr->carrier[carrier_idx].rxlm_drx_buf_idx  );
    }

    if ( tx_buffer_allocated == TRUE )
    {
      /* deallocate TxLM buffer that may have been allocated */
      txlm_deallocate_buffer( lte_ftm_mc_sm_ptr->carrier[carrier_idx].txlm_buf_idx );
    }

    /* Reset LTE FTM state machine */
    /* @TODO need to pass carrier index into this function to reset only the information for the said carrier
     *        if all carrier has invalid device, reset all.*/
    rflte_ftm_mc_sm_reset();

    MSG_1(MSG_SSID_FTM, MSG_LEGACY_ERROR, "ftm_lte_enter: Failed device %d", device );
  }
  return result;
} /* ftm_lte_enter */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Exit LTE Mode for FTM

  @details
  This function is used to clean-up procedure for LTE.

  @param device RF Device for which FTM will exit LTE

  @return
  TRUE on success and FALSE on failure
*/  
static boolean ftm_lte_exit(rfm_device_enum_type    device , boolean cleanup_dev_allocation)
{
  boolean       result          = TRUE;  /* holds result for this function */
  uint32        carrier_idx     = RFLTE_MC_PCELL_IDX;
  uint8         loop_count      = 0 ;

  rflte_ftm_mc_sm_type     *lte_ftm_mc_sm_ptr = rflte_ftm_mc_get_sm();

  rfa_rf_lte_exit_mode_req_s        exit_mode_req;
  rfa_rf_lte_tdd_tx_disable_req_s   tdd_tx_disable_req;
  rfa_rf_lte_fdd_tx_disable_req_s   fdd_tx_disable_req;

  lm_status_type        rx_buffer_prx_deallocated   = LM_SUCCESS;
  lm_status_type        rx_buffer_drx_deallocated   = LM_SUCCESS;
  lm_status_type        tx_buffer_deallocated       = LM_SUCCESS;
  lm_status_type        rx_buffer_fbrx_deallocated  = LM_SUCCESS;

  if (lte_ftm_mc_sm_ptr == NULL) 
  {
    FTM_MSG( FTM_ERROR, "ftm_lte_exit: cannot get LTE FTM SM handle" );
    return FALSE;
  }

  if((device == RFM_DEVICE_0) || (device == RFM_DEVICE_1))
  {
    rflte_ftm_mc_sm_set_carrier_idx(RFLTE_MC_PCELL_IDX);
  }
  else if((device >= RFM_DEVICE_2) && (device <= RFM_DEVICE_5))
  {
    /* Loop through all supported Secondary Carriers to check if "INPUT" Device is being used by one.              *
       If YES: Set FTM Carrier Index to identified Carrier IDx. If NO: Default to legacy Hardcoded assignemnt.     */
    for(loop_count = RFLTE_MC_SCELL1_IDX; loop_count < RFA_RF_LTE_MAX_CELL_SUPPORTED; loop_count++)
    {
      /* Set FTM State Carrier Index to current loop "Cell Idx". */
      rflte_ftm_mc_sm_set_carrier_idx(loop_count);
      if((rflte_ftm_mc_sm_get_prx_device() == device) ||
         (rflte_ftm_mc_sm_get_drx_device() == device))
      {
         rflte_ftm_mc_sm_set_carrier_idx(loop_count);
         break;
      }
    }

    /* If NO Match found - then Default to Hard-coded assignments. */ 
    if(RFA_RF_LTE_MAX_CELL_SUPPORTED == loop_count)
    { 
      if ((device == RFM_DEVICE_2) || (device == RFM_DEVICE_3))
      {
        rflte_ftm_mc_sm_set_carrier_idx(RFLTE_MC_SCELL1_IDX);
      }
      else if ((device == RFM_DEVICE_4) || (device == RFM_DEVICE_5))
      {
        rflte_ftm_mc_sm_set_carrier_idx(RFLTE_MC_SCELL2_IDX);
      }
    }
  }  
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_lte_exit: Unsupported device" );
    return FALSE;
  }

  carrier_idx = rflte_ftm_mc_sm_get_carrier_index();
  /* Unvote for Maximum CPU Speed */
  ftm_unvote_cpu_speed();


  /* If we are exiting PCELL, then all SCELL needs to be exited first */
  if (carrier_idx == RFLTE_MC_PCELL_IDX)
  {
    /* Clear all SCELL if not already done so */
    for (carrier_idx = RFLTE_MC_SCELL1_IDX; carrier_idx < RFA_RF_LTE_MAX_CELL_SUPPORTED; carrier_idx++) 
    {
      rflte_ftm_mc_sm_set_carrier_idx(carrier_idx);
      if( (rflte_mc_get_state(carrier_idx) != RF_STATE_LTE_IDLE) && 
          (rflte_mc_get_state(carrier_idx) != RF_STATE_INIT))
      {
        /* Stop FW from Doing RXAGC */
        rflte_ftm_mdsp_rxagc_start_stop(carrier_idx, 
                                        RXAGC_DISABLE, 
                                        rflte_ftm_mc_sm_get_band(),
                                        rflte_ftm_mc_sm_get_rx_chan(),
                                        rflte_ftm_mc_sm_get_tx_chan(),
                                        rflte_ftm_mc_sm_get_rx_bw(),0);

        /* EXIT SCELL */
        msgr_init_hdr(&exit_mode_req.hdr, MSGR_RFA_RF_LTE_FTM, RFA_RF_LTE_EXIT_MODE_REQ);
        exit_mode_req.carrier_idx_mask = (1 << carrier_idx);
        exit_mode_req.prx[carrier_idx].device = rflte_ftm_mc_sm_get_prx_device();
        exit_mode_req.prx[carrier_idx].lm_buffer_index = rflte_ftm_mc_sm_get_rxlm_buffer(RFLTE_FTM_MC_PRX);
        exit_mode_req.drx[carrier_idx].device = rflte_ftm_mc_sm_get_drx_device();
        exit_mode_req.drx[carrier_idx].lm_buffer_index = rflte_ftm_mc_sm_get_rxlm_buffer(RFLTE_FTM_MC_DRX);
        if (rflte_dispatch_exit_mode_req(&exit_mode_req, FALSE) != RF_DISPATCH_SUCESSFUL) 
        {
          FTM_MSG_ERROR("ftm_lte_exit: EXIT_MODE Error on carrier %d, mdevice %d, sdevice %d",
                        carrier_idx,
                        exit_mode_req.prx[carrier_idx].device,
                        exit_mode_req.drx[carrier_idx].device);
        }

        /* temporarily removed until rfcommon ccmgr fixes the bug for blind alt_path update */
        /*if( cleanup_dev_allocation)
        {
          if(!ftm_concurrency_manager_cleanup_device_allocation( exit_mode_req.prx[carrier_idx].device, RFM_LTE_MODE ))
          {
            FTM_MSG_ERROR("ftm cleanup device allocation failed for carrier index:%d, PRx device %d",
            carrier_idx,
            exit_mode_req.prx[carrier_idx].device,
            RFM_LTE_MODE);

          }
          if(!ftm_concurrency_manager_cleanup_device_allocation( exit_mode_req.drx[carrier_idx].device, RFM_LTE_MODE ))
          {
            FTM_MSG_ERROR("ftm cleanup device allocation failed for carrier index:%d, DRx device %d, Tech:%d",
                      carrier_idx,
                      exit_mode_req.drx[carrier_idx].device,
                      RFM_LTE_MODE);
          }
        }*/

      }
      /* DeAllocate RxLM Buffer */
      if ( rflte_ftm_mc_sm_get_rxlm_buffer(RFLTE_FTM_MC_PRX) != FTM_INVALID_LM_BUFFER )
      {
#ifdef FEATURE_RF_ASDIV
		  rflm_cmn_asd_abort_pending_event((uint32)rflte_ftm_mc_sm_get_rxlm_buffer(RFLTE_FTM_MC_PRX));

		  FTM_MSG_1 ( FTM_MED,
                      "FTM_COMMON_CONTROL: ftm_lte_exit: ASDIV DEBUG MESSAGE, CLEARED PENDING RxLM ASDIV EVENTS",
                      0 );
#endif
        rx_buffer_prx_deallocated =
          rxlm_deallocate_buffer( rflte_ftm_mc_sm_get_rxlm_buffer(RFLTE_FTM_MC_PRX) );
        lte_ftm_mc_sm_ptr->carrier[carrier_idx].rxlm_prx_buf_idx = FTM_INVALID_LM_BUFFER;
        if ( rx_buffer_prx_deallocated != LM_SUCCESS )
        {
          result = FALSE;
          FTM_MSG(FTM_ERROR, "ftm_lte_exit: SCELL PRx Buffer De-allocation failed");
        }
      }
      if ( rflte_ftm_mc_sm_get_rxlm_buffer(RFLTE_FTM_MC_DRX) != FTM_INVALID_LM_BUFFER )
      {
        rx_buffer_drx_deallocated =
          rxlm_deallocate_buffer( rflte_ftm_mc_sm_get_rxlm_buffer(RFLTE_FTM_MC_DRX) );
        lte_ftm_mc_sm_ptr->carrier[carrier_idx].rxlm_drx_buf_idx = FTM_INVALID_LM_BUFFER;
        if ( rx_buffer_drx_deallocated != LM_SUCCESS )
        {
          result = FALSE;
          FTM_MSG(FTM_ERROR, "ftm_lte_exit: SCELL DRx Buffer De-allocation failed");
        }
      }
      /* DeAllocate TxLM Buffer */
      if ( rflte_ftm_mc_sm_get_txlm_buffer(RFLTE_FTM_MC_PRX) != FTM_INVALID_LM_BUFFER )
      {
#ifdef FEATURE_RF_ASDIV
		  rflm_cmn_asd_abort_pending_event((uint32)rflte_ftm_mc_sm_get_txlm_buffer(RFLTE_FTM_MC_PRX));

		  FTM_MSG_1 ( FTM_MED,
                      "FTM_COMMON_CONTROL: ftm_lte_exit: ASDIV DEBUG MESSAGE, CLEARED PENDING TxLM ASDIV EVENTS",
                      0 ); 
#endif
        tx_buffer_deallocated =
          txlm_deallocate_buffer( rflte_ftm_mc_sm_get_txlm_buffer(RFLTE_FTM_MC_PRX) );
        lte_ftm_mc_sm_ptr->carrier[carrier_idx].txlm_buf_idx = FTM_INVALID_LM_BUFFER;
        if ( tx_buffer_deallocated != LM_SUCCESS )
        {
          result = FALSE;
          FTM_MSG(FTM_ERROR, "ftm_lte_exit: SCELL Tx Buffer De-allocation failed");
        }
      }
      /* DeAllocate FBRx Buffer */
      if ( lte_ftm_mc_sm_ptr->carrier[carrier_idx].rxlm_fbrx_buf_idx != FTM_INVALID_LM_BUFFER )
      {
        rx_buffer_fbrx_deallocated =
          rxlm_deallocate_buffer( lte_ftm_mc_sm_ptr->carrier[carrier_idx].rxlm_fbrx_buf_idx );
        lte_ftm_mc_sm_ptr->carrier[carrier_idx].rxlm_fbrx_buf_idx = FTM_INVALID_LM_BUFFER;
        if ( rx_buffer_fbrx_deallocated != LM_SUCCESS )
        {
          result = FALSE;
          FTM_MSG(FTM_ERROR, "ftm_lte_exit: SCELL FBRx Buffer De-allocation failed");
        }
      }
    }
    /* Now back to exiting PCELL */
    carrier_idx = RFLTE_MC_PCELL_IDX;
    rflte_ftm_mc_sm_set_carrier_idx(carrier_idx);

    /* If fw_txagc flag is set, send msg to FW to stop ul transmission */
    if(rflte_ftm_mc_sm_is_mdsp_txagc_started() == TRUE )
    {
      /* Stop FW from doing UL transmission */
      rflte_ftm_mdsp_stop_ul_transmission();
      /* Set the fw_txagc flag in ftm sm */
      rflte_ftm_mc_sm_set_fw_txagc_start(FALSE);
      /* After sending stop ul tx msg to fw, in the next UL tick, fw will schedule DTx 
         (might be upto 2 ms to schedule DTx if fw receives rf msg just after a tick), 
         in the following sf it will start to clear dtr registers
         and need a ms for completing clean-up. Budget 1ms extra, Hence we wait 5 ms*/
      
      DALSYS_BusyWait(5000);
    }
    else
    {
      FTM_MSG_1( FTM_HIGH, "ftm_lte_exit: MDSP TxAGC is not started, hence didn't send stop_ul trans request: fw_txagc flag: %d", rflte_ftm_mc_sm_is_mdsp_txagc_started() ); 
    }
      /* DISABLE TX */  
    if (rflte_mc_get_state(carrier_idx) == RF_STATE_LTE_FDD_RXTX) 
    {
      msgr_init_hdr(&fdd_tx_disable_req.hdr, MSGR_RFA_RF_LTE_FTM, RFA_RF_LTE_FDD_TX_DISABLE_REQ);
      fdd_tx_disable_req.carrier_idx_mask = (1<<carrier_idx);
      fdd_tx_disable_req.tx[carrier_idx].device = rflte_ftm_mc_sm_get_tx_device();
      fdd_tx_disable_req.tx[carrier_idx].lm_buffer_index = rflte_ftm_mc_sm_get_txlm_buffer(RFLTE_FTM_MC_PRX);
      if (rflte_dispatch_fdd_tx_disable_req(&fdd_tx_disable_req, FALSE) != RF_DISPATCH_SUCESSFUL)
      {
        FTM_MSG_ERROR("ftm_lte_exit: FDD_TX_DISABLE Error on carrier %d, tx_device %d",
                      carrier_idx,
                      fdd_tx_disable_req.tx[carrier_idx].device,0);
      }
      if ( FALSE == rfcommon_core_dev_power_vote( fdd_tx_disable_req.tx[carrier_idx].device,
                                                  RFCMN_CORE_DEV_OFF,
                                                  RFCMN_CORE_DEV_NO_VOTE) )
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
        "rflte_dispatch_fdd_tx_disable_req: sleep UL device: %d Pwr down failed.", 
        fdd_tx_disable_req.tx[carrier_idx].device);
		
      }
    }
    else if (rflte_mc_get_state(carrier_idx) == RF_STATE_LTE_TDD_RXTX) 
    {
      msgr_init_hdr(&tdd_tx_disable_req.hdr, MSGR_RFA_RF_LTE_FTM, RFA_RF_LTE_TDD_TX_DISABLE_REQ);
      tdd_tx_disable_req.carrier_idx_mask = (1<<carrier_idx);
      tdd_tx_disable_req.tx[carrier_idx].device = rflte_ftm_mc_sm_get_tx_device();
      tdd_tx_disable_req.tx[carrier_idx].lm_buffer_index = rflte_ftm_mc_sm_get_txlm_buffer(RFLTE_FTM_MC_PRX);
      if (rflte_dispatch_tdd_tx_disable_req(&tdd_tx_disable_req, FALSE) != RF_DISPATCH_SUCESSFUL)
      {
        FTM_MSG_ERROR("ftm_lte_exit: TDD_TX_DISABLE Error on carrier %d, tx_device %d",
                      carrier_idx,
                      tdd_tx_disable_req.tx[carrier_idx].device,0);
      }
      if ( FALSE == rfcommon_core_dev_power_vote( tdd_tx_disable_req.tx[carrier_idx].device,
                                                  RFCMN_CORE_DEV_OFF,
                                                  RFCMN_CORE_DEV_NO_VOTE) )
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
                          "rflte_dispatch_tdd_tx_disable_req: sleep UL device: %d Pwr down failed.", 
                          tdd_tx_disable_req.tx[carrier_idx].device);
      
	  }
    }
    else
    {
      // DO Nothing we are ready to EXIT_MODE
    }

    if(rflte_mc_get_state(carrier_idx) != RF_STATE_INIT)
    {
      /* Stop FW from Doing RXAGC */
      rflte_ftm_mdsp_rxagc_start_stop(carrier_idx, 
                                      RXAGC_DISABLE, 
                                      rflte_ftm_mc_sm_get_band(),
                                      rflte_ftm_mc_sm_get_rx_chan(),
                                      rflte_ftm_mc_sm_get_tx_chan(),
                                      rflte_ftm_mc_sm_get_rx_bw(),0);

      /*Send sys config app msg to fw with resource mode: LTE_LL1_UE_RES_MODE_NO_LTE*/
     if(rflte_ftm_mdsp_exit_lte_sys_config_app())
     {
        MSG(MSG_SSID_RF,MSG_LEGACY_HIGH, "ftm_lte_exit: Sys config app msg to stop LTE sent to FW : SUCCESS.");
     }
     else
     { 
       MSG(MSG_SSID_RF,MSG_LEGACY_ERROR, "ftm_lte_exit: Sys config app msg to stop LTE sent to FW : FAILURE.");
     }
     /* Wait 1ms to let fw process this msg */
     DALSYS_BusyWait(1000);


      /* EXIT PCELL */
      msgr_init_hdr(&exit_mode_req.hdr, MSGR_RFA_RF_LTE_FTM, RFA_RF_LTE_EXIT_MODE_REQ);
      exit_mode_req.carrier_idx_mask = (1 << carrier_idx);
      exit_mode_req.prx[carrier_idx].device = rflte_ftm_mc_sm_get_prx_device();
      exit_mode_req.prx[carrier_idx].lm_buffer_index = rflte_ftm_mc_sm_get_rxlm_buffer(RFLTE_FTM_MC_PRX);
      exit_mode_req.drx[carrier_idx].device = rflte_ftm_mc_sm_get_drx_device();
      exit_mode_req.drx[carrier_idx].lm_buffer_index = rflte_ftm_mc_sm_get_rxlm_buffer(RFLTE_FTM_MC_DRX);

      /* temporarily removed until rfcommon ccmgr fixes the bug for blind alt_path update */
      /*if( cleanup_dev_allocation)
      {
        if(!ftm_concurrency_manager_cleanup_device_allocation( exit_mode_req.prx[carrier_idx].device, RFM_LTE_MODE ))
        {
          FTM_MSG_ERROR("ftm cleanup device allocation failed for carrier index:%d, PRx device %d",
          carrier_idx,
          exit_mode_req.prx[carrier_idx].device,
          RFM_LTE_MODE);
          
        }

        if(!ftm_concurrency_manager_cleanup_device_allocation( exit_mode_req.drx[carrier_idx].device, RFM_LTE_MODE ))
        {
            FTM_MSG_ERROR("ftm cleanup device allocation failed for carrier index:%d, DRx device %d, Tech:%d",
            carrier_idx,
            exit_mode_req.drx[carrier_idx].device,
            RFM_LTE_MODE);
        }
      }*/

      if (rflte_dispatch_exit_mode_req(&exit_mode_req, FALSE) != RF_DISPATCH_SUCESSFUL) 
      {
        FTM_MSG_ERROR("ftm_lte_exit: EXIT_MODE Error on carrier %d, mdevice %d, sdevice %d",
                      carrier_idx,
                      exit_mode_req.prx[carrier_idx].device,
                      exit_mode_req.drx[carrier_idx].device);
      }
    }
    /* DeAllocate RxLM Buffer */
    if ( rflte_ftm_mc_sm_get_rxlm_buffer(RFLTE_FTM_MC_PRX) != FTM_INVALID_LM_BUFFER )
    {
#ifdef FEATURE_RF_ASDIV
		  rflm_cmn_asd_abort_pending_event((uint32)rflte_ftm_mc_sm_get_rxlm_buffer(RFLTE_FTM_MC_PRX));

 		  FTM_MSG_1 ( FTM_MED,
                      "FTM_COMMON_CONTROL: ftm_lte_exit: ASDIV DEBUG MESSAGE, CLEARED PENDING RxLM ASDIV EVENTS",
                      0 );
#endif      
      rx_buffer_prx_deallocated =
        rxlm_deallocate_buffer( rflte_ftm_mc_sm_get_rxlm_buffer(RFLTE_FTM_MC_PRX) );
      lte_ftm_mc_sm_ptr->carrier[carrier_idx].rxlm_prx_buf_idx = FTM_INVALID_LM_BUFFER;
      if ( rx_buffer_prx_deallocated != LM_SUCCESS )
      {
        result = FALSE;
        FTM_MSG(FTM_ERROR, "ftm_lte_exit: PCELL PRx Buffer De-allocation failed");
      }
    }
    if ( rflte_ftm_mc_sm_get_rxlm_buffer(RFLTE_FTM_MC_DRX) != FTM_INVALID_LM_BUFFER )
    {
      rx_buffer_drx_deallocated =
        rxlm_deallocate_buffer( rflte_ftm_mc_sm_get_rxlm_buffer(RFLTE_FTM_MC_DRX) );
      lte_ftm_mc_sm_ptr->carrier[carrier_idx].rxlm_drx_buf_idx = FTM_INVALID_LM_BUFFER;
      if ( rx_buffer_drx_deallocated != LM_SUCCESS )
      {
        result = FALSE;
        FTM_MSG(FTM_ERROR, "ftm_lte_exit: PCELL DRx Buffer De-allocation failed");
      }
    }
    /* DeAllocate TxLM Buffer */
    if ( rflte_ftm_mc_sm_get_txlm_buffer(RFLTE_FTM_MC_PRX) != FTM_INVALID_LM_BUFFER )
    {
#ifdef FEATURE_RF_ASDIV
		  rflm_cmn_asd_abort_pending_event((uint32)rflte_ftm_mc_sm_get_txlm_buffer(RFLTE_FTM_MC_PRX));

		  FTM_MSG_1 ( FTM_MED,
                      "FTM_COMMON_CONTROL: ftm_lte_exit: ASDIV DEBUG MESSAGE, CLEARED PENDING TxLM ASDIV EVENTS",
                      0 ); 
#endif      
      tx_buffer_deallocated =
        txlm_deallocate_buffer( rflte_ftm_mc_sm_get_txlm_buffer(RFLTE_FTM_MC_PRX) );
      lte_ftm_mc_sm_ptr->carrier[carrier_idx].txlm_buf_idx = FTM_INVALID_LM_BUFFER;
      if ( tx_buffer_deallocated != LM_SUCCESS )
      {
        result = FALSE;
        FTM_MSG(FTM_ERROR, "ftm_lte_exit: PCELL Tx Buffer De-allocation failed");
      }
    }
    /* DeAllocate FBRx Buffer */
    if ( lte_ftm_mc_sm_ptr->carrier[carrier_idx].rxlm_fbrx_buf_idx != FTM_INVALID_LM_BUFFER )
    {
      rx_buffer_fbrx_deallocated =
        rxlm_deallocate_buffer( lte_ftm_mc_sm_ptr->carrier[carrier_idx].rxlm_fbrx_buf_idx );
      lte_ftm_mc_sm_ptr->carrier[carrier_idx].rxlm_fbrx_buf_idx = FTM_INVALID_LM_BUFFER;
      if ( rx_buffer_fbrx_deallocated != LM_SUCCESS )
      {
        result = FALSE;
        FTM_MSG(FTM_ERROR, "ftm_lte_exit: PCELL FBRx Buffer De-allocation failed");
      }
    }
    rflte_ftm_mdsp_exit();

    /* Stop Clock */
    rflte_ftm_mc_set_lte_clock(RF_LTE_FTM_STOP, RFCOM_BW_LTE_INVALID);


    /* Update FTM Data Structrure to invalidate LM Buffer */
    rflte_ftm_mc_sm_reset();
  }
  /* EXIT SCELL only */
  else if ((carrier_idx > RFLTE_MC_PCELL_IDX) && (carrier_idx < RFA_RF_LTE_MAX_CELL_SUPPORTED))
  {

    /* Stop FW from Doing RXAGC */
    rflte_ftm_mdsp_rxagc_start_stop(carrier_idx, 
                                    RXAGC_DISABLE, 
                                    rflte_ftm_mc_sm_get_band(),
                                    rflte_ftm_mc_sm_get_rx_chan(),
                                    rflte_ftm_mc_sm_get_tx_chan(),
                                    rflte_ftm_mc_sm_get_rx_bw(),0);

    /* EXIT SCELL */
    msgr_init_hdr(&exit_mode_req.hdr, MSGR_RFA_RF_LTE_FTM, RFA_RF_LTE_EXIT_MODE_REQ);
    exit_mode_req.carrier_idx_mask = (1 << carrier_idx);
    exit_mode_req.prx[carrier_idx].device = rflte_ftm_mc_sm_get_prx_device();
    exit_mode_req.prx[carrier_idx].lm_buffer_index = rflte_ftm_mc_sm_get_rxlm_buffer(RFLTE_FTM_MC_PRX);
    exit_mode_req.drx[carrier_idx].device = rflte_ftm_mc_sm_get_drx_device();
    exit_mode_req.drx[carrier_idx].lm_buffer_index = rflte_ftm_mc_sm_get_rxlm_buffer(RFLTE_FTM_MC_DRX);
    if (rflte_dispatch_exit_mode_req(&exit_mode_req, FALSE) != RF_DISPATCH_SUCESSFUL) 
    {
      FTM_MSG_ERROR("ftm_lte_exit: EXIT_MODE Error on carrier %d, mdevice %d, sdevice %d",
                    carrier_idx,
                    exit_mode_req.prx[carrier_idx].device,
                    exit_mode_req.drx[carrier_idx].device);
    }

    /*If L is on PCELL, SCELL1 (dev 0,1,2,3) and then tech X enter mode called on dev 2,3*/
    /* temporarily removed until rfcommon ccmgr fixes the bug for blind alt_path update */
    /*if( cleanup_dev_allocation)
    {
      if(!ftm_concurrency_manager_cleanup_device_allocation( exit_mode_req.prx[carrier_idx].device, RFM_LTE_MODE ))
      {
        FTM_MSG_ERROR("ftm cleanup device allocation failed for carrier index:%d, PRx device %d",
        carrier_idx,
        exit_mode_req.prx[carrier_idx].device,
        RFM_LTE_MODE);
        
      }

      if(!ftm_concurrency_manager_cleanup_device_allocation( exit_mode_req.drx[carrier_idx].device, RFM_LTE_MODE ))
      {
          FTM_MSG_ERROR("ftm cleanup device allocation failed for carrier index:%d, DRx device %d, Tech:%d",
          carrier_idx,
          exit_mode_req.drx[carrier_idx].device,
          RFM_LTE_MODE);
      }
    }*/
        /* DeAllocate RxLM Buffer */
    if ( rflte_ftm_mc_sm_get_rxlm_buffer(RFLTE_FTM_MC_PRX) != FTM_INVALID_LM_BUFFER )
    {
      rx_buffer_prx_deallocated =
        rxlm_deallocate_buffer( rflte_ftm_mc_sm_get_rxlm_buffer(RFLTE_FTM_MC_PRX) );
      lte_ftm_mc_sm_ptr->carrier[carrier_idx].rxlm_prx_buf_idx = FTM_INVALID_LM_BUFFER;
      if ( rx_buffer_prx_deallocated != LM_SUCCESS )
      {
        result = FALSE;
        FTM_MSG(FTM_ERROR, "ftm_lte_exit: SCELL PRx Buffer De-allocation failed");
      }
    }
    if ( rflte_ftm_mc_sm_get_rxlm_buffer(RFLTE_FTM_MC_DRX) != FTM_INVALID_LM_BUFFER )
    {
      rx_buffer_drx_deallocated =
        rxlm_deallocate_buffer( rflte_ftm_mc_sm_get_rxlm_buffer(RFLTE_FTM_MC_DRX) );
      lte_ftm_mc_sm_ptr->carrier[carrier_idx].rxlm_drx_buf_idx = FTM_INVALID_LM_BUFFER;
      if ( rx_buffer_drx_deallocated != LM_SUCCESS )
      {
        result = FALSE;
        FTM_MSG(FTM_ERROR, "ftm_lte_exit: SCELL DRx Buffer De-allocation failed");
      }
    }
    /* DeAllocate TxLM Buffer */
    if ( rflte_ftm_mc_sm_get_txlm_buffer(RFLTE_FTM_MC_PRX) != FTM_INVALID_LM_BUFFER )
    {
      tx_buffer_deallocated =
        txlm_deallocate_buffer( rflte_ftm_mc_sm_get_txlm_buffer(RFLTE_FTM_MC_PRX) );
      lte_ftm_mc_sm_ptr->carrier[carrier_idx].txlm_buf_idx = FTM_INVALID_LM_BUFFER;
      if ( tx_buffer_deallocated != LM_SUCCESS )
      {
        result = FALSE;
        FTM_MSG(FTM_ERROR, "ftm_lte_exit: SCELL Tx Buffer De-allocation failed");
      }
    }
    /* DeAllocate FBRx Buffer */
    if ( lte_ftm_mc_sm_ptr->carrier[carrier_idx].rxlm_fbrx_buf_idx != FTM_INVALID_LM_BUFFER )
    {
      rx_buffer_fbrx_deallocated =
        rxlm_deallocate_buffer( lte_ftm_mc_sm_ptr->carrier[carrier_idx].rxlm_fbrx_buf_idx );
      lte_ftm_mc_sm_ptr->carrier[carrier_idx].rxlm_fbrx_buf_idx = FTM_INVALID_LM_BUFFER;
      if ( rx_buffer_fbrx_deallocated != LM_SUCCESS )
      {
        result = FALSE;
        FTM_MSG(FTM_ERROR, "ftm_lte_exit: SCELL FBRx Buffer De-allocation failed");
      }
    }
  }
  else
  {
    FTM_MSG_ERROR("ftm_lte_exit: Unsupported carrier index %d",carrier_idx,0,0 );
    result = FALSE;
  }
  return result;
} /* ftm_lte_exit */
#endif /* FEATURE_LTE */
/*! \} */

/*============================================================================*/
/*!
  @name Internal Mode Transition Interface - TDS-CDMA

  @brief
  This section contains interfaces to control the FTM mode transition for
  TDS-CDMA.
*/
/*! \{ */
#ifdef FEATURE_TDSCDMA 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enter TDSCDMA Mode

  @details
  Configure the RF to TDSCDMA mode. RF must be in PARK mode before calling
  this function.

  @param device RF Device to which FTM is to be entered

  @return
  TRUE on success and FALSE on failure
*/  
static boolean
ftm_tdscdma_enter
(
  rfm_device_enum_type device
)
{  
  /* Vote for Maximum CPU Speed */
  ftm_vote_cpu_speed( RF_Q6_MAX_CPU_SPEED_KHZ );

  return ftm_tdscdma_mode_enter(device);

} /* ftm_tdscdma_enter */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Exit TDSCDMA Mode for FTM

  @details
  This function is used to clean-up procedure for TDSCDMA.

  @param device RF Device for which FTM will exit TDSCDMA

  @return
  TRUE on success and FALSE on failure
*/  
static boolean
ftm_tdscdma_exit
(
  rfm_device_enum_type device
)
{
  boolean ret_val = FALSE;  
  /*directly call tds specific function.*/
  ret_val = ftm_tdscdma_mode_exit(device);

  /* cancel vote for Maximum CPU Speed */
  ftm_unvote_cpu_speed();

  return ret_val;

} /* ftm_tdscdma_exit */

#endif /* FEATURE_TDSCDMA */

/*! \} */

/*============================================================================*/
/*!
  @name Internal Mode Transition Interface - WCDMA

  @brief
  This section contains interfaces to control the FTM mode transition for
  WCDMA.
*/
/*! \{ */
#ifdef FEATURE_WCDMA 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enter Mode for WCDMA

  @details
  Configure the RF to WCDMA mode. RF must be in PARK mode before calling
  this function.

  @param device RF Device to which FTM is to be entered

  @return
  TRUE on success and FALSE on failure
*/  
static boolean
ftm_wcdma_enter
(
  rfm_device_enum_type device
)
{
  l1_ext_cmd_type  *l1_cmd_ptr;
  timetick_type start_time = 0;
  rf_time_tick_type profiler_var;
  rf_time_type profiler_var_val;

  /* Set to FTM RF state by default.*/
  rfm_wcdma_set_ftm_state(RFM_WCDMA_FTM_RF);

  if (((device == RFM_DEVICE_0)&&(rfwcdma_mc_state.primary_rx_device != RFM_DEVICE_2)) ||
      ((device == RFM_DEVICE_2)&&(rfwcdma_mc_state.primary_rx_device != RFM_DEVICE_0)))
  {
     /* Vote for Maximum CPU Speed */
     ftm_vote_cpu_speed( RF_Q6_MAX_CPU_SPEED_KHZ );
	
     /* Set a default rf mode, so the RF driver initializes in FTM test mode 
     * (Must not be FTM_DB_RF_IDLE). 
     * Actual mode and band will be set up in ftm_wcdma_set_mode()
     */
     ftm_set_rf_db_state(ftm_wcdma_convert_rfcom_band_type_to_ftm_band(rfwcdma_mc_get_default_rfcom_band()));
  
    /* Send cmd to WL1 to start WCDMA mode */
    l1_cmd_ptr = l1_get_cmd_buf();
    ASSERT(l1_cmd_ptr != NULL);
  
    l1_cmd_ptr->cmd_hdr.cmd_id = CPHY_START_WCDMA_MODE_REQ;
  
    l1_put_cmd(l1_cmd_ptr);
    profiler_var = rf_time_get_tick() ; 
  
    /* wait for WL1 to complete */
    start_time = timetick_get();
    do
    { 
      rex_wait(FTM_WCDMA_BER_CMD_Q_SIG);
    }while((!ftm_wcdma_ber_is_rrc_cmd_start_wcdma_mode()) &&
           (timetick_get_elapsed(start_time, T_USEC) < WL1_START_STOP_WAIT_TIMEOUT)
          );

    /* check if timeout occured */
    if(timetick_get_elapsed(start_time, T_USEC) >= WL1_START_STOP_WAIT_TIMEOUT)
    {
      ERR_FATAL("ftm_wcdma_enter: Timeout occured while waiting for WL1!",
                0, 0, 0);
    }
   
    profiler_var_val = rf_time_get_elapsed ( profiler_var , RF_USEC ) ; 
    FTM_MSG_1( FTM_MED, "ftm_wcdma_enter: time spent in waiting for L1: %d",profiler_var_val) ;

    /* Clear RRC cmd Q after sending WL1 start cmd */
    ftm_wcdma_ber_clear_rrc_cmd_q();

    fws_app_enable(FW_APP_RFCMD);

    /* allocate rxlm buffer for chain 0*/ 
      ftm_wcdma_allocate_rxlm(device); 
   
    /* allocate txlm buffer */
    ftm_wcdma_allocate_txlm();
    /* override rxd nv setting in L1 state machine for FTM */
    #ifdef FEATURE_WCDMA_RX_DIVERSITY
    rxd_overide_nv_settings( 0 );
    #endif
    
    ftm_wcdma_ber_set_state(FTM_WCDMA_BER_STATE_NULL);
  }
  else
  {
    ftm_wcdma_allocate_rxlm(device);  
  }
  
  /* Cleanup concurrency manager database for this particular device at the end of Enter 
   as enabling L1 before would inform TRM and TRM would update concurrency manager 
   Do not move the location of this call */
  ftm_concurrency_manager_cleanup_device_allocation(device,RFM_IMT_MODE);
   
  return TRUE;
} /* ftm_wcdma_enter */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enter NonSignaling Mode for WCDMA

  @details

  @param 

  @return
  TRUE on success and FALSE on failure
*/  
static boolean
ftm_wcdma_ns_enter
(
  rfm_device_enum_type device
)
{

  /* Set FTM NS state */
  rfm_wcdma_set_ftm_state(RFM_WCDMA_FTM_NONSIG);

  /* Vote for Maximum CPU Speed */
  ftm_vote_cpu_speed( RF_Q6_MAX_CPU_SPEED_KHZ );

  /* Nothing needs to be done here for now */
  return TRUE;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Exit WCDMA Mode for FTM

  @details
  This function is used to clean-up procedure for WCDMA.

  @param device RF Device for which FTM will exit WCDMA

  @return
  TRUE on success and FALSE on failure
*/  
static boolean
ftm_wcdma_exit
(
  rfm_device_enum_type device
)
{
  timetick_type start_time = 0;
  rex_tcb_type* current_tcb_ptr = rex_self();

  /* Disable diversity chain if it's still ON*/
  if (device == RFM_DEVICE_0) 
  {
  ftm_wcdma_set_secondary_chain(RFM_DEVICE_1,FALSE);
  }

  else if (device == RFM_DEVICE_2)
  {
    ftm_wcdma_set_secondary_chain(RFM_DEVICE_3,FALSE);
  }

  if ( device == rfwcdma_mc_state.primary_rx_device )
  {
    /* Disable Tx first if it is still on*/
    ftm_wcdma_set_tx(device,FALSE);
    
    fws_app_disable(FW_APP_RFCMD);
    
    ftm_wcdma_ber_send_CPHY_STOP_WCDMA_MODE_REQ();
  
    /* deallocate all rxlm buffers */
    ftm_wcdma_deallocate_rxlm((ftm_receive_chain_handle_type)device);
  
    /* deallocate all txlm buffers */
    ftm_wcdma_deallocate_txlm();
  
    ftm_set_rf_db_state(FTM_DB_RF_IDLE);

    /* Wait for WL1 to complete if currently in rf or ftm task */
    if( (current_tcb_ptr == TASK_HANDLE(ftm)) || (current_tcb_ptr == TASK_HANDLE(rf)) )
    {
      start_time = timetick_get();
      do
      {
        /* If in FTM task then wait for FTM_WCDMA_BER_CMD_Q_SIG */
        if(current_tcb_ptr == TASK_HANDLE(ftm))
        {
          rex_wait(FTM_WCDMA_BER_CMD_Q_SIG);
        }
        /* If in RF task then wait for FTM_WCDMA_ENTER_EXIT_COMPLETE_SIG */
        else if (current_tcb_ptr == TASK_HANDLE(rf))
        {
          rex_wait(FTM_WCDMA_ENTER_EXIT_COMPLETE_SIG);
          (void) rex_clr_sigs(TASK_HANDLE(rf), FTM_WCDMA_ENTER_EXIT_COMPLETE_SIG);
        }
      }while((!ftm_wcdma_ber_is_rrc_cmd_stop_wcdma_mode()) &&
             (timetick_get_elapsed(start_time, T_USEC) < WL1_START_STOP_WAIT_TIMEOUT)
            );
    
      
      /* check if timeout occured */
      if(timetick_get_elapsed(start_time, T_USEC) >= WL1_START_STOP_WAIT_TIMEOUT)
      {
        ERR_FATAL("ftm_wcdma_exit: Timeout occured while waiting for WL1!",
                  0, 0, 0);
      }
    
      /* Clear RRC cmd Q for any pending cmds */
      ftm_wcdma_ber_clear_rrc_cmd_q();
    }
    /* cancel vote for Maximum CPU Speed */
    ftm_unvote_cpu_speed();

  ftm_wcdma_ber_set_state(FTM_WCDMA_BER_STATE_NULL);

  rxd_overide_nv_settings(RXD_NV_VALUE_MAX);

  // Free up the allocated memory. 
  ftm_wcdma_free_memory();

  /* Reset FTM RF state */
  rfm_wcdma_set_ftm_state(RFM_WCDMA_FTM_NONE);
  }

  else
  {
    ftm_wcdma_deallocate_rxlm((ftm_receive_chain_handle_type)device);
  }

  /* Cleanup concurrency manager database for this particular device at the end of Exit*/
  /* temporarily removed until rfcommon ccmgr fixes the bug for blind alt_path update */
  //ftm_concurrency_manager_cleanup_device_allocation(device,RFM_IMT_MODE);

  /* Call RFM Exit mode */
  rfm_exit_mode(device, RFCOM_PARKED_MODE);

  return TRUE;
} /* ftm_wcdma_exit */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Exit WCDMA Mode for FTM

  @details
  This function is used to clean-up procedure for WCDMA.

  @param device RF Device for which FTM will exit WCDMA

  @return
  TRUE on success and FALSE on failure
*/  
static boolean ftm_wcdma_ns_exit
(
  rfm_device_enum_type device
)
{
  ftm_unvote_cpu_speed();

  /* Nothing needs to be done here for now */
  ftm_wcdma_ber_cleanup();
	
  // Free up the allocated memory. 
  ftm_wcdma_free_memory();

  /* Reset FTM RF variable in exit */
  rfm_wcdma_set_ftm_state(RFM_WCDMA_FTM_NONE);

  return TRUE;
}
#endif /* FEATURE_WCDMA */
/*! \} */


/*============================================================================*/
/*!
  @name Mode Transition Interface

  @brief
  This section contains interfaces to control the FTM mode transition.
*/
/*! \{ */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Exit FTM Mode

  @details
  This function will exit the particular device to a given mode. This will
  clean up FTM current Mode. All FTM mode transition should go through 
  interface.

  @param device RF Device to exit FTM

  @return
  TRUE on success and FALSE on failure
*/  
boolean
ftm_rfmode_exit
(
  rfm_device_enum_type device,
  ftm_rfstate_enum_type new_mode
)
{
  boolean result = TRUE ;
  rf_time_type profiler_var_val;
  rfm_device_enum_type dev_idx = RFM_MAX_DEVICES;
  /* Get starting time tick to profile the function */
  rf_time_tick_type profiler_var = rf_time_get_tick() ; 
  rfm_mode_enum_type tech = RFM_PARKED_MODE;
  boolean cleanup_flag = FALSE;

  /* Perform cleanup for cuurent Mode */
  switch( ftm_rfmode_current[device] )
  {
 
  #ifdef FEATURE_CDMA1X
  case FTM_STATE_1X_NS:
    result &= ftm_1x_ns_exit( device, new_mode );
    tech = RFM_1X_MODE;
    break;
 
  case FTM_STATE_1X:
    result &= ftm_1x_exit( device, new_mode );
    tech = RFM_1X_MODE;
    break;
  #endif /* FEATURE_CDMA1X */ 
  
  #ifdef FEATURE_GSM
  case FTM_STATE_GSM_NS:
    result &= ftm_gsm_ns_exit( device );
    tech = RFM_EGSM_MODE;
    break;
 
  case FTM_STATE_GSM:
    result &= ftm_gsm_exit( device );
    tech = RFM_EGSM_MODE;
    break;
  #endif /* FEATURE_GSM */ 
 
  #ifdef FEATURE_WCDMA
  case FTM_STATE_WCDMA_NS:
    result &= ftm_wcdma_ns_exit( device );
    tech = RFM_IMT_MODE;
    break;
 
  case FTM_STATE_WCDMA:
    result &= ftm_wcdma_exit( device );
    tech = RFM_IMT_MODE;
    break;
  #endif /* FEATURE_WCDMA */ 
 
  #ifdef FEATURE_TDSCDMA
  case FTM_STATE_TDSCDMA_NS:
    tech = RFM_TDSCDMA_MODE;    
    break;
  
  case FTM_STATE_TDSCDMA:
    result &= ftm_tdscdma_exit( device );
    tech = RFM_TDSCDMA_MODE;    
    break;
  #endif /* FEATURE_TDSCDMA */ 
 
  #ifdef FEATURE_HDR
  case FTM_STATE_HDR_NS:
    result &= ftm_hdr_ns_exit( device, new_mode );
    tech = RFM_1XEVDO_MODE;
    break;
 
  case FTM_STATE_HDR:
    result &= ftm_hdr_exit( device, new_mode );
    tech = RFM_1XEVDO_MODE;
    break;


  #endif /* FEATURE_HDR */ 

  #if defined(FEATURE_HDR)&&defined(FEATURE_CDMA1X)

  case FTM_STATE_SVDO:
    result &= ftm_svdo_exit( device, new_mode );  
    tech = RFM_SVDO_MODE;
    break;

  #endif /* FEATURE_HDR && FEATURE_CDMA1X */

  #ifdef FEATURE_LTE
  case FTM_STATE_LTE_NS:
    tech = RFM_LTE_MODE;
    break;
 
  case FTM_STATE_LTE:
    /*For L to X mode transitions*/
    if( new_mode != FTM_STATE_LTE)
    {
      cleanup_flag = TRUE;
    }
    result &= ftm_lte_exit( device, cleanup_flag);
    /* LTE exit is path agnostic, so clear path_1 state also when LTE exit is done */
    if ( (device == RFM_DEVICE_0) && (result == TRUE ) )
    {
        /*If PCELL and device 0, clear all the other devices also*/
        for ( dev_idx = RFM_DEVICE_0; dev_idx < RFM_MAX_DEVICES; dev_idx++ )
        {
            if (FTM_STATE_LTE == ftm_rfmode_current[dev_idx])
            {
                 ftm_rfmode_current[dev_idx] = FTM_STATE_UNKNOWN;
                 FTM_MSG_1( FTM_MED, "ftm_rfmode_exit: LTE mode cleared on Dev %d during lte exit", dev_idx );
            }
        
        } /* for ( dev_idx = RFM_DEVICE_0; dev_idx < RFM_MAX_DEVICES; dev_idx++ ) */

    }
    tech = RFM_LTE_MODE;
    break;
  #endif /* FEATURE_LTE */ 
 
  default:
    break;
 
  }

  FTM_MSG_2( FTM_MED, "ftm_rfmode_exit : Mode %d on Dev %d exit",
             ftm_rfmode_current[device], device );


  ftm_rfmode_current[device] = FTM_STATE_UNKNOWN;

  /* Get execution time for this function  */
  profiler_var_val = rf_time_get_elapsed ( profiler_var , RF_USEC ) ; 
  FTM_MSG_1 ( FTM_HIGH, "ftm_rfmode_exit() Execution Time: %d",
              profiler_var_val ) ;

  return result;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enter FTM Mode

  @details
  This function will enter the particular device to a given mode. All FTM
  mode transition should go through interface.

  @param device RF Device for which FTM is to be entered

  @param mode New FTM mode for the device

  @return
  TRUE on success and FALSE on failure
*/  
boolean
ftm_rfmode_enter
(
  rfm_device_enum_type device,
  ftm_rfstate_enum_type mode
)
{
  /* holds the result for this function */
  boolean result = TRUE;

  boolean new_mode_is_irat = FALSE; /* Flag indicating, if the new mode is
  iRAT mode */

  rfm_device_enum_type dev_idx; /* Device for Looping */
  rf_time_type profiler_var_val;

  /* Get starting time tick to profile the function */
  rf_time_tick_type profiler_var = rf_time_get_tick() ; 

  if( (device < RFM_DEVICE_0) || (device >= RFM_MAX_DEVICES) )
  {
    FTM_MSG_1( FTM_ERROR,"RFC LTE device not valid, device %d ", device);
    result = FALSE;
    return result;
  }
  
  /* Check if we are in FTM Mode. If not return. */
  if( ftm_mode != FTM_MODE )
  {
    FTM_MSG_2( FTM_ERROR, "ftm_rfmode_set : Not in FTM Mode -  Dev %d, "
               "New Mode %d", device, mode);
    result = FALSE;
    return result;
  }

  /* Do Nothing, if the requested Mode is same as current mode in the system */
  if ( ftm_rfmode_current[device] == mode )
  {
    /* do nothing */
    FTM_MSG_2( FTM_HIGH, "ftm_rfmode_set : No Transition Required -  Dev %d, "
               "Mode %d", device, mode);
    return result;
  }

  /* decide if the new mode is iRAT mode */
  switch( mode )
  {
  case FTM_STATE_1X_IRAT:
  case FTM_STATE_HDR_IRAT:
  case FTM_STATE_GSM_IRAT:
  case FTM_STATE_LTE_IRAT:
  case FTM_STATE_TDSCDMA_IRAT:
  case FTM_STATE_WCDMA_IRAT:
    new_mode_is_irat = TRUE;
    break;

  default:
    new_mode_is_irat = FALSE;
    break;
  }

  /* If new Mode is irat, then the existing mode on the device should not
  be torn down. However, if it is not IRAT mode, then current mode on
  the device should be exited. */
  if ( new_mode_is_irat == FALSE )
  {
    /* Exit the previous Mode in FTM */
    result &= ftm_rfmode_exit( device, mode);
  }
  else
  {
    FTM_MSG_3( FTM_MED, "ftm_rfmode_enter: New Mode on Dev %d is iRAT %d - No "
               "exiting of current mode %d", device, mode, 
               ftm_rfmode_current[device] );
  }

  /* Due to lack of capability of FW to have simultaneous App enabled for few
  combination of techs, all devices will be exit'ed. This is required to
  have power-cycle less RF calibration.

  One used case where such issue is seen is as follows
  [1] 1x calibration on first RTR (RFM_DEVICE_0)
  [2] 1x Calibration on SV RTR (RFM_DEVICE_2)
  [3] GSM Calibration on first RTR (RFM_DEVICE_0)

  In this case, step 1 will vote 1x on RFM_DEVICE_0 to enable CDMA FW. Step 2
  will vote 1x on RFM_DEVICE_2 to enable CDMA FW. When
  Step 3 comes, 1x on RFM_DEVICE_0 will unvote CDMA FW; but since 1x FW
  is still voted by RFM_DEVICE_2, 1xFw will not be disabled. Thus when GSM
  FW is enabled, there is crash because 1x and GSM FW cannot be enabled
  simultaneously enabled.

  Following are the incompatible simultaneous FW techs. As these incompatibility
  are removed, exiting of all devices should be removed.

  [1] 1x and GSM
  [2] 1x and WCDMA
  [3] 1x and TDSCDMA */
  if ( ( 
         ( ftm_rfmode_current[device] ==  FTM_STATE_1X ) ||
         ( ftm_rfmode_current[device] ==  FTM_STATE_PARK ) ||
         ( ftm_rfmode_current[device] ==  FTM_STATE_UNKNOWN ) 
       )
       &&
       ( 
         ( mode == FTM_STATE_WCDMA ) ||
         ( mode == FTM_STATE_TDSCDMA ) 
       )
     )
  {
    FTM_MSG( FTM_MED, "ftm_rfmode_enter: Exiting 1x in all devices" );

    for ( dev_idx = RFM_DEVICE_0; dev_idx < RFM_MAX_DEVICES; dev_idx++ )
    {
      #ifdef FEATURE_CDMA1X
      ftm_1x_exit( dev_idx, mode );
      #endif
    } /* for ( dev_idx = RFM_DEVICE_0; dev_idx < RFM_MAX_DEVICES; dev_idx++ ) */
  } /* if ( ( 
         ( ftm_rfmode_current[device] ==  FTM_STATE_1X ) ||
         ( ftm_rfmode_current[device] ==  FTM_STATE_PARK ) ||
         ( ftm_rfmode_current[device] ==  FTM_STATE_UNKNOWN ) 
       )
       &&
       ( 
         ( mode == FTM_STATE_GSM ) ||
         ( mode == FTM_STATE_WCDMA ) ||
         ( mode == FTM_STATE_TDSCDMA ) 
       )
     ) */

  /* Enter mode */    
  switch(mode)
  {

  #ifdef FEATURE_CDMA1X
  case FTM_STATE_1X_NS:
    result &= ftm_1x_ns_enter( device );
    break;

  case FTM_STATE_1X:
    result &= ftm_1x_enter( device );
    break;

  case FTM_STATE_1X_IRAT:
    break;
  #endif /* FEATURE_CDMA1X */
  
  #ifdef FEATURE_GSM
  case FTM_STATE_GSM_NS:
    result &= ftm_gsm_ns_enter(device);
    break;

  case FTM_STATE_GSM:
    result &= ftm_gsm_enter( device );
    break;

  case FTM_STATE_GSM_IRAT:
    break;
  #endif /* FEATURE_GSM */

  #ifdef FEATURE_WCDMA
  case FTM_STATE_WCDMA_NS:
    result &= ftm_wcdma_ns_enter( device );
    break;

  case FTM_STATE_WCDMA:
    result &= ftm_wcdma_enter( device );
    break;

  case FTM_STATE_WCDMA_IRAT:
    result &= ftm_wcdma_enter( device );
    break;
  #endif /* FEATURE_WCDMA */
  
  #ifdef FEATURE_TDSCDMA
  case FTM_STATE_TDSCDMA_NS:
    break;
  
  case FTM_STATE_TDSCDMA:
    result &= ftm_tdscdma_enter( device );
    break;

  case FTM_STATE_TDSCDMA_IRAT:
    break;
  #endif /* FEATURE_TDSCDMA */    

  #ifdef FEATURE_HDR
  case FTM_STATE_HDR_NS:
    result &= ftm_hdr_ns_enter( device );
    break;

  case FTM_STATE_HDR:
    result &= ftm_hdr_enter( device );
    break;

  case FTM_STATE_HDR_IRAT:
    break;

  #endif /* FEATURE_HDR */

  #if defined(FEATURE_HDR)&&defined(FEATURE_CDMA1X)

  case FTM_STATE_SVDO:
    result &= ftm_svdo_enter( device);
    break;

  #endif 

  #ifdef FEATURE_LTE
  case FTM_STATE_LTE_NS:
    break;

  case FTM_STATE_LTE:
    result &= ftm_lte_enter( device );
    break;

  case FTM_STATE_LTE_IRAT:
    break;
  #endif /* FEATURE_LTE */

  default:
    FTM_MSG_2( FTM_ERROR, "ftm_rfmode_set : Invalid Mode %d on Dev %d", mode,
               device );
    mode = FTM_STATE_PARK;
    break;

  }
  
  /* Get execution time for this function  */
  profiler_var_val = rf_time_get_elapsed ( profiler_var , RF_USEC ) ; 
  
  if (result == TRUE)
  {  
    ftm_rfmode_current[device] = mode;
    FTM_MSG_4( FTM_MED, "ftm_rfmode_set : New FTM Mode %d on Dev %d, "
               "Execution Time: %d result: %d", ftm_rfmode_current[device], device , 
               profiler_var_val,result) ;
  }

  FTM_MSG_4( FTM_MED, "ftm_rfmode_set : New FTM Mode %d on Dev %d, "
             "Execution Time: %d result: %d", ftm_rfmode_current[device], device , 
             profiler_var_val,result) ;

  return result;

} /* ftm_rfmode_enter */

/*! \} */

/*============================================================================*/
/*!
  @name Query

  @brief
  This section contains interfaces to query Common FTM.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return current FTM State

  @param device RF Device whose FTM State is queried

  @return
  Current FTM RF Mode
*/  
ftm_rfstate_enum_type
ftm_get_current_state
(
  rfm_device_enum_type device
)
{
  return ftm_rfmode_current[device];
} /* ftm_get_current_state */

/*! \} */

/*============================================================================*/
/*!
  @name Utility Functions

  @brief
  Utility Functions for FTM usage
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Resolve RF Mode for given FTM Mode ID

  @details
  When RF mode is requested to be set, PC will send the mode enum which
  does not directly correspond to a RF Mode. To make RF calibration
  compatible to older routines, this API will be used by to resolve the RF Mode
  for the given FTM mode,

  DISCLAIMER : This API is used to facilitate the legacy RF Cal API. This API
  should not be called for other purpose. 

  @param mode
  FTM mode sent from the PC side

  @return
  Resolved RF Mode for given FTM Mode
*/
rfm_mode_enum_type
ftm_common_resolve_rf_mode
(
  ftm_mode_id_type mode
)
{
  rfm_mode_enum_type rf_mode = RFM_INVALID_MODE;

  /* Assign Band Class based on mode requested and override the band info
  on band_chan structure */
  switch (mode)
  {
  case FTM_PHONE_MODE_CDMA:
  case FTM_PHONE_MODE_CDMA_800:
  case FTM_PHONE_MODE_CDMA_1900:
  case FTM_PHONE_MODE_JCDMA:
  case FTM_PHONE_MODE_CDMA_1800:
  case FTM_PHONE_MODE_CDMA_450:
  case FTM_PHONE_MODE_IMT:
  case FTM_PHONE_MODE_CDMA_BC10:
  case FTM_PHONE_MODE_CDMA_BC11:
  case FTM_PHONE_MODE_CDMA_BC14:
  case FTM_PHONE_MODE_CDMA_BC15:
    rf_mode = RFM_1X_MODE; /* 1x Mode */
    break;

  case FTM_PHONE_MODE_HDR:
  case FTM_PHONE_MODE_HDR_800:
  case FTM_PHONE_MODE_HDR_1900:
    rf_mode = RFM_1XEVDO_MODE; /* HDR Mode */
    break;

  case FTM_PHONE_MODE_WCDMA_IMT:
  case FTM_PHONE_MODE_WCDMA_1900A:
  case FTM_PHONE_MODE_WCDMA_1900B:
  case FTM_PHONE_MODE_WCDMA_800:
  case FTM_PHONE_MODE_WCDMA_BC3:
  case FTM_PHONE_MODE_WCDMA_BC4:
  case FTM_PHONE_MODE_WCDMA_BC8:
  case FTM_PHONE_MODE_WCDMA_BC9:
  case FTM_PHONE_MODE_WCDMA_BC11:
  case FTM_PHONE_MODE_WCDMA_BC19:
    rf_mode = RFM_IMT_MODE; /* WCDMA Mode */
    break;

  case FTM_PHONE_MODE_LTE_B1:
  case FTM_PHONE_MODE_LTE_B7:
  case FTM_PHONE_MODE_LTE_B13:
  case FTM_PHONE_MODE_LTE_B17:
  case FTM_PHONE_MODE_LTE_B38:
  case FTM_PHONE_MODE_LTE_B40:
  case FTM_PHONE_MODE_LTE_B11:
  case FTM_PHONE_MODE_LTE_B4:
  case FTM_PHONE_MODE_LTE_B2:
  case FTM_PHONE_MODE_LTE_B3:
  case FTM_PHONE_MODE_LTE_B5:
  case FTM_PHONE_MODE_LTE_B6:
  case FTM_PHONE_MODE_LTE_B8:
  case FTM_PHONE_MODE_LTE_B9:
  case FTM_PHONE_MODE_LTE_B10:
  case FTM_PHONE_MODE_LTE_B12:
  case FTM_PHONE_MODE_LTE_B14:
  case FTM_PHONE_MODE_LTE_B15:
  case FTM_PHONE_MODE_LTE_B16:
  case FTM_PHONE_MODE_LTE_B18:
  case FTM_PHONE_MODE_LTE_B19:
  case FTM_PHONE_MODE_LTE_B20:
  case FTM_PHONE_MODE_LTE_B21:
  case FTM_PHONE_MODE_LTE_B22:
  case FTM_PHONE_MODE_LTE_B23:
  case FTM_PHONE_MODE_LTE_B24:
  case FTM_PHONE_MODE_LTE_B25:
  case FTM_PHONE_MODE_LTE_B26:
  case FTM_PHONE_MODE_LTE_B27:
  case FTM_PHONE_MODE_LTE_B28:
  case FTM_PHONE_MODE_LTE_B29:
  case FTM_PHONE_MODE_LTE_B30:
  case FTM_PHONE_MODE_LTE_B31:
  case FTM_PHONE_MODE_LTE_B32:
  case FTM_PHONE_MODE_LTE_B33:
  case FTM_PHONE_MODE_LTE_B34:
  case FTM_PHONE_MODE_LTE_B35:
  case FTM_PHONE_MODE_LTE_B36:
  case FTM_PHONE_MODE_LTE_B37:
  case FTM_PHONE_MODE_LTE_B39:
  case FTM_PHONE_MODE_LTE_B41:
  case FTM_PHONE_MODE_LTE_B42:
  case FTM_PHONE_MODE_LTE_B66:
    rf_mode = RFM_LTE_MODE; /* LTE Mode */
    break;   
  case FTM_PHONE_MODE_TDSCDMA_B34:
  case FTM_PHONE_MODE_TDSCDMA_B39:
  case FTM_PHONE_MODE_TDSCDMA_B40:
    rf_mode = RFM_TDSCDMA_MODE;
    break;
    
  case FTM_PHONE_MODE_GSM_900:
  case FTM_PHONE_MODE_GSM_1800:
  case FTM_PHONE_MODE_GSM_1900:
  case FTM_PHONE_MODE_GSM_850:
    rf_mode = RFM_EGSM_MODE;
    break;
  
  default:
    FTM_MSG_1( FTM_ERROR,
               "ftm_common_resolve_rf_mode: Invalid FTM Mode Switch %d",
               mode );   
    break;
  }

  return rf_mode;
} /* ftm_common_resolve_rf_mode */




/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get band number and tech for given FTM rf mode

  @details
  This function will return the rf tech and band based on the FTM rf mode passed in

  @param rf_mode

  @param band
   passing in parameter to store the band got from FTM rf mode

  @param rf_tech
  passing in parameter to store the rf tech from FTM rf mode

  @return
  True on success and FALSE on failures
*/
boolean
ftm_common_get_band_tech_from_rf_mode
(
  ftm_mode_id_type rf_mode,
  uint8 * band,
  ftm_rf_technology_type *rf_tech
)
{
  /* holds the result for this function */
  boolean result = TRUE;

  /*validate parameters*/	
  if((band == NULL) || (rf_tech == NULL))
  {
	FTM_MSG( FTM_ERROR,"NULL parameters passed in");
	result = FALSE;
	return result;	
  }

  if((rf_mode < FTM_PHONE_MODE_CDMA ) || (rf_mode >= FTM_PHONE_MODE_MAX))
  {
	  FTM_MSG( FTM_ERROR,"wrong rf id mode");
	  result = FALSE;
	  return result;  
  }

  /*use rf_mode as index to get the corresponding rf_tech and band from the tech_band_lookup table*/ 
  *rf_tech = tech_band_lookup[rf_mode].rf_tech;
  *band = tech_band_lookup[rf_mode].band;
  
  FTM_MSG_3( FTM_HIGH, "get band:%d, tech %d from rf mode: %d", *band, *rf_tech, rf_mode);

  return result;
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Resolve RFM Mode for given FTM Tech

  @details
  Tools will communicate with embedded side in terms of 
  enumeration listed in ftm_rf_technology_type. Thus these enums needs to be
  remapped to RFM_MODE to perform necessary action for the correct tech.
 
  @param ftm_tech
  FTM tech sent from the PC side

  @return
  Resolved RFM Mode for given FTM tech
*/
rfm_mode_enum_type
ftm_common_ftm_tech_to_rfm_mode
(
  ftm_rf_technology_type ftm_tech
)
{
  rfm_mode_enum_type rfm_mode = RFM_INVALID_MODE;

  /* Assign Band Class based on mode requested and override the band info
  on band_chan structure */
  switch (ftm_tech)
  {
  case FTM_RF_TECH_CDMA:
    rfm_mode = RFM_1X_MODE; /* 1x Mode */
    break;

  case FTM_RF_TECH_WCDMA:
    rfm_mode = RFM_IMT_MODE; /* WCDMA Mode */
    break;

  case FTM_RF_TECH_GSM:
    rfm_mode = RFM_EGSM_MODE; /* GSM Mode */
    break;

  case FTM_RF_TECH_LTE:
    rfm_mode = RFM_LTE_MODE; /* LTE Mode */
    break; 
      
  case FTM_RF_TECH_TDSCDMA:
    rfm_mode = RFM_TDSCDMA_MODE; /* TDS-CDMA Mode */
    break;
  default:
    FTM_MSG_1( FTM_FATAL, "ftm_common_ftm_tech_to_rfm_mode: Invalid FTM Mode "
                          "Switch %d", ftm_tech );   
    break;
  }

  return rfm_mode;
} /* ftm_common_ftm_tech_to_rfm_mode */



/*! \} */

/*============================================================================*/
/*!
  @name FTM Request/Response Packet Creation Functions

  @brief
  Functions to create Request and/or Response Packets.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Create Common Response Packet for Diag.

  @details
  This function creates good or bad response packet depending on the status
  passed as argument. In case of bad response packet, the packet includes
  FTM_RSP_BAD_PARM as response command. On the other hand, good packet uses
  FTM_RSP_DO_LEGACY asresponse command.

  @param status
  Flag to indicate whether to create good packet or bad packet.

  @return
  CDMA Response Packet
*/
ftm_rsp_pkt_type
ftm_common_create_rsp_pkt
(
  ftm_common_rsp_type status
)
{
  /* Good/Bad Response Packet */
  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_MAX, 0, NULL, FALSE};

  switch( status )
  {
  case FTM_COMMON_BAD_RSP: /* Create Bad Response Packet */
    rsp_pkt.cmd = FTM_RSP_BAD_PARM;
    break;
  case FTM_COMMON_GOOD_RSP: /* Create Good Response Packet */
    rsp_pkt.cmd = FTM_RSP_DO_LEGACY;
    break;
  case FTM_COMMON_NO_RSP: /* Create No Response Packet */
    rsp_pkt.cmd = FTM_RSP_NO_RSP;
    #ifndef FEATURE_GNSS_ONLY_NO_WWAN
    #ifdef FEATURE_CDMA1X
    ftm_pkt_data.data_rsp_ptr = NULL; /* This is required as this prevents
    ftmdiag_diag_free() from being called */
    #endif /* FEATURE_CDMA1X */
    #endif
    break;
  default:
    FTM_MSG_1( FTM_ERROR, "Common Control : Bad FTM Command %d", status );
    break;
  }

  return rsp_pkt;
} /* ftm_common_create_rsp_pkt */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Malloc FTM Common Response Packet for Diag.

  @details
  This function Mallocs common Response Packet for Diag.

  @param ftm_req_data
  FTM Request Data

  @param rsp_pkt_size
  Size of Response Packet to allocate sufficient memory

  @return
  Response Packet
*/
ftm_rsp_pkt_type
ftm_common_malloc_rsp_pkt
(
  void * ftm_req_data,
  uint16 rsp_pkt_size
)
{
  /* Request Packet : Used extract the FTM Command only */
  ftm_composite_cmd_header_type* header = ftm_req_data;

  ftm_rsp_pkt_type rsp_pkt;
  ftm_composite_cmd_header_type *common_rsp;/* Packet Payload for each command*/

  /* Create a new reponse packet in the ftm heap */
  rsp_pkt = ftmdiag_create_new_pkt( rsp_pkt_size );  

  if ( rsp_pkt.pkt_payload != NULL )
  {
    common_rsp = rsp_pkt.pkt_payload;
    common_rsp->diag_hdr.cmd_code = header->diag_hdr.cmd_code;
    common_rsp->diag_hdr.subsys_id = header->diag_hdr.subsys_id;
    common_rsp->diag_hdr.subsys_cmd_code = header->diag_hdr.subsys_cmd_code;
    common_rsp->ftm_hdr.cmd_id = header->ftm_hdr.cmd_id;
    common_rsp->ftm_hdr.cmd_data_len = header->ftm_hdr.cmd_data_len;
    common_rsp->ftm_hdr.cmd_rsp_pkt_size = rsp_pkt_size;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_common_malloc_rsp_pkt: Malloc Failed" );
  }

  return rsp_pkt;

} /* ftm_common_malloc_rsp_pkt */

/*! \} */

/*============================================================================*/
/*!
  @name FTM Command

  @brief
  Functions to send Dispatch Command to FTM Task.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Create and Push common Radio Command to FTM Queue

  @details
  This function will create and push the FTM Command message to be placed on the 
  FTM queue. FTM Data Pointer and FTM Request Packet pointers which are
  dynamically allocated in this function will be freed at the end of
  ftm_req() execution.

  @param cmd_param
  Pointer to the command parameter to be sent to FTM Q. This parameter should 
  include both Diag Sub-system header and the payload for the FTM command.

  @param param_size
  Size of the command parameter (cmd_param)
   
  @retval TRUE 
  TRUE return indicates that FTM command is successfully created and send to FTM 
  queue 
   
  @retval FALSE 
  FALSE return indicates that FTM command could not be successfully created, 
  thus the command is not sent to FTM queue. 
   
  @return
  Flag indicating the success or failure of FTM Queue message creation  
  corresponding to the given FTM command parameter. 
*/
boolean
ftm_common_send_radio_command
(
  void *cmd_param,
  uint32 param_size
)
{
  boolean ret_val = FALSE; /* return value */

  /* Allocate a instance of memory for command to be linked on the queue.
  This will be freed when the command is executed by FTM Task. Freeing is done
  at the end of ftm_req() */
  ftm_msg_type *ftm_message = NULL;

  /* Allocate a instance of memory for command to be linked on the queue.
  This will be freed when the command is executed by FTM Task. Freeing is done
  at the end of ftm_req() */
  ftm_message = ftm_malloc(sizeof(ftm_msg_type));

  /* Continue only if the Malloc did not fail */
  if ( ftm_message != NULL )
  {
    // Reset all fields
    ftm_message->data_rsp_ptr = NULL;
    ftm_message->ftm_data_ptr = NULL;
    ftm_message->req_pkt_len = 0;
    ftm_message->rsp_pkt_len = 0;

    /* Allocate memory. This will de-allocated when ftm_req() is executed */
    ftm_message->ftm_data_ptr = (void *)ftm_malloc( param_size );

    /* Continue only if the Malloc did not fail */
    if ( ftm_message->ftm_data_ptr )
    {
      /* Length of Packet */
      ftm_message->req_pkt_len = (uint16) param_size;
           
      /* Fill the ftm_data_ptr with the ftm data (ftm_data) */
      memscpy( ftm_message->ftm_data_ptr, param_size, 
               cmd_param, param_size );
      
      /* No-done queue. This will be used in ftm_req() */
      ftm_message->hdr.done_q_ptr = NULL;
      
      /* Put it on the FTM Command queue */
      ftm_cmd( ftm_message );

      ret_val = TRUE;
    }
    else
    {
      /* Since Malloc for FTM Data failed which resulted in not sending
      Command to FTM Queue, Free the memory allocated for FTM Request Packet */
      ftm_free( ftm_message );

      ret_val = FALSE;
    }
  }
  else
  {
    ret_val = FALSE;
  }

  /* Return success */ 
  return ret_val;

} /* ftm_common_send_radio_command */

/*! \} */

/*============================================================================*/
/*!
  @name FTM Common Resource Config Request

  @brief
  This function configures the resources required for FTM common Cal.

  @details
  This function will vote to enable/disable all resources needed for FTM
  common Cal routine includes internal device Cal, DAC Cal, Therm Cal without
  a sepcific tech being enabled. Resource currently includes:
  - PMIC VREGs
  - MCPM
  - CCS APP
 
  @param enable
  Enable or Disable resources
 
  @return 
  Flag indicating the status of config request
*/
boolean
ftm_common_resource_config_request
(
  boolean enable
)
{

  boolean status = TRUE;

  /*MCPM params */
  mcpm_request_parms_type mcpm_param = {0};

  /* RF bus client params */
  static rf_hal_bus_client_handle* rf_rfm_hal_bus_client_handle = NULL;

  static boolean resource_enabled = FALSE;
  
  if (enable == resource_enabled)
  {
    FTM_MSG_1( FTM_MED, "FTM resource request same as last:%d, skip", enable );
    return TRUE;
  }

  if (enable == TRUE)
  {
    /* Setup MCPM */
    MCPM_Config_Modem( MCPM_RF_START_REQ, &mcpm_param ); /* Tech independent State *

    /* Set common VREGS */	
    rfc_common_enable_autocal_vregs(enable);

    /* Initialize the RF HAL for SSBI, RFFE, GRFC, etc. */
    (void)rf_hal_bus_init();

    /* Vote for the RFCMD app*/
    status &= rf_hal_bus_enable(enable, &rf_rfm_hal_bus_client_handle, RF_HAL_RFCMD_APP_CLIENT(FTM));
  }
  else
  /* In disable case, config sequence shall be reversed */
  {
    /* Skip CCS App vote disable if client handle is NULL */
    if (rf_rfm_hal_bus_client_handle == NULL)
  {
      FTM_MSG_1( FTM_ERROR, "rf_rfm_hal_bus_client_handle is NULL when voting to disable CCS APP! skip CCS App vote", 0 );
    status = FALSE;
  }
  else
  {
      status &= rf_hal_bus_enable(enable, &rf_rfm_hal_bus_client_handle, RF_HAL_RFCMD_APP_CLIENT(FTM));
  }
    /* Vote off common VREGS */	
    rfc_common_enable_autocal_vregs(enable);

    /* Vote off MCPM RF config*/
    /* Do not vote off MCPM RF config to ensure same MCPM status before entering Internal Calibration */
    //MCPM_Config_Modem( MCPM_RF_STOP_REQ, &mcpm_param ); /* Tech independent State */
  }

  /* Set internal state */
  resource_enabled = enable;

  return status;
} /* ftm_common_resource_config_request */

/*============================================================================*/
/*!
  @name FTM Common internal device calibration

  @brief
  Functions related to FTM Common internal device calibration
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function performs any required internal calibration routines for all
  MSM/ RF devices on the current target (e.g. vco coarse tuning, baseband filter 
  cal, RC Tuner cal, Tx DAC cal etc.). 

  @details
  This function will perform the calibration regardless of the state of the NV
  items. Also it is expected that the UE would not need any external input
  signals during calibration.
  -# <b> Some of the calibration routines might commit the results to NV </b>
  -# <b> Return will be TRUE only when all the internal device calibration
         routines are successfull</b>
   
  @return 
  Flag indicating the status of self calibration 
*/
boolean
ftm_common_self_calibration
(
  void
)
{
  boolean cal_result = TRUE;

  /*==========================================================================*/
  /* Enable common resources for FTM self calibration  */
  /*==========================================================================*/
  cal_result &= ftm_common_resource_config_request(TRUE);

  if (cal_result == FALSE )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_common_self_calibration:common resource enable failed", 0 );
    return FALSE;
  }

  /*==========================================================================*/
  /* RFIC internal Calibration */
  /*==========================================================================*/

  cal_result &= rfc_common_do_internal_device_calibration();

  /*==========================================================================*/
  /* Tx DAC Calibration */
  /*==========================================================================*/
  
  FTM_MSG_1( FTM_LOW, "ftm_common_self_calibration: DAC Cal starting with status %d", cal_result);

  cal_result &= ftm_common_dac_calibration();
 
  FTM_MSG_1( FTM_LOW, "ftm_common_self_calibration: DAC Cal is done with status %d", cal_result);

   /*==========================================================================*/
  /* Rx ADC Calibration */
  /*==========================================================================*/

  FTM_MSG_1( FTM_LOW, "ftm_common_self_calibration: ADC Cal starting with status %d", cal_result);

  fws_app_enable(FW_APP_RFCMD);

  cal_result &= ftm_common_adc_calibration();

  fws_app_disable(FW_APP_RFCMD);

  FTM_MSG_1( FTM_LOW, "ftm_common_self_calibration: ADC Cal is done with status %d", cal_result);

  
  /*==========================================================================*/
  /* Disable common resources for FTM self calibration  */
  /*==========================================================================*/
  (void)ftm_common_resource_config_request(FALSE);

  /* Return calibration results */
  return cal_result;

} /* ftm_common_self_calibration */

/*! \} */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function get complete RFC data for RF card configuration
*/
boolean
ftm_common_get_efs_verifcation_data
(
  void
)
{
  boolean rfc_card_result = TRUE;

  /*==========================================================================*/
  /* Get RF CARD data */
  /*==========================================================================*/

  rfc_card_result &= rfc_common_get_rfc_data();

  /* Return rf card result */
  return rfc_card_result;

} /* ftm_common_get_efs_verifcation_data */

/*============================================================================*/
/*!
  @name FTM Common tuner self calibration

  @brief
  Tuner Self Cal only for NOK
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function performs tuner self cal only 

  @details
  This function will perform the calibration regardless of the state of the NV
  items. Also it is expected that the UE would not need any external input
  signals during calibration.
  -# <b> Some of the calibration routines might commit the results to NV </b>
  -# <b> Return will be TRUE only when all the internal device calibration
         routines are successfull</b>
   
  @return 
  Flag indicating the status of self calibration 
*/
boolean
ftm_common_tuner_self_calibration
(
  void
)
{
  boolean cal_result = TRUE;

  /*==========================================================================*/
  /* Enable common resources for FTM self calibration  */
  /*==========================================================================*/
  cal_result &= ftm_common_resource_config_request(TRUE);

  if (cal_result == FALSE )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_common_tuner_self_calibration:common resource enable failed", 0 );
    return FALSE;
  }

  /*==========================================================================*/
  /* Tuner Device internal Calibration */
  /*==========================================================================*/

  cal_result &= rfc_common_do_tuner_device_calibration();

  if (cal_result == FALSE )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_common_tuner_self_calibration:common resource enable failed", 0 );
    return FALSE;
  }

  /*==========================================================================*/
  /* Disable common resources for FTM self calibration  */
  /*==========================================================================*/
  (void)ftm_common_resource_config_request(FALSE);

  /* Return calibration results */
  return cal_result;

} /* ftm_common_tuner_self_calibration */

/*! \} */

/*============================================================================*/
/*!
  @name FTM Common DAC calibration

  @brief
  This function does the DAC cal in FTM mode.

  @details
  This function will perform the DAC calibration regardless of the state of the NV
  items. Also the UE would not need any external input signals during calibration.
     
  @return 
  Flag indicating the status of dac calibration 
*/
/*----------------------------------------------------------------------------*/
boolean
ftm_common_dac_calibration
(
  void
)
{
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
  /*==========================================================================*/
  /* INITALIZATION  */
  /*==========================================================================*/  
#if defined(FEATURE_DIMEPM_MODEM) || defined(FEATURE_JOLOKIA_MODEM)
  uint8 cal_cmd = rfcommon_msm_get_dac_cal_command();
#else
  uint8 cal_cmd = 0x2D; /* Disable DC cal */
#endif

  /* DAC cal status */
  //boolean dac_cal_status = FALSE; 
  boolean dac_cal_status = TRUE; 

  /* Modem config */
  xpt_cfg_type supported_cfg = XPT_CFG_TYPE_MAX;

  /*==========================================================================*/
  /* Set up */
  /*==========================================================================*/    

  FTM_MSG( FTM_HIGH, "ftm_common_dac_calibration: DAC Cal starting setup");
    

  /*==========================================================================*/
  /* ET cal */
  /*==========================================================================*/
  /* NO IREF required for ET */
  /* Check if the modem supports ET */
  supported_cfg = rfcommon_msm_get_supported_xpt_mode();
  if(supported_cfg == ET_CFG || supported_cfg == EPT_ET_CFG)
  {
    if(cal_cmd & DAC_1_ET_B_MSK)
    {
      FTM_MSG( FTM_HIGH, "ftm_common_dac_calibration: RFCOMMON_DAC_CAL_DAC1_ET");
      dac_cal_status &= rfcommon_msm_cal_execute_dac_cal(RFCOMMON_DAC_CAL_DAC1_ET);
    }
    if(cal_cmd & DAC_0_ET_B_MSK)
    {
      FTM_MSG( FTM_HIGH, "ftm_common_dac_calibration: RFCOMMON_DAC_CAL_DAC0_ET");
      dac_cal_status &= rfcommon_msm_cal_execute_dac_cal(RFCOMMON_DAC_CAL_DAC0_ET);
    }
  }

  /*==========================================================================*/
  /* No need to set up WTR */
  /*==========================================================================*/

  FTM_MSG( FTM_HIGH, "ftm_common_dac_calibration: DAC Cal setup complete");
  

  /*==========================================================================*/
  /* Execute */
  /*==========================================================================*/  
  /* Trigger dac cal using the HAL layer*/
  /* The HAL layer will call into the LM layer to interface with HW, return cal codes */

  if(cal_cmd & DAC_1_MSB_B_MSK)
  {
    FTM_MSG( FTM_HIGH, "ftm_common_dac_calibration: RFCOMMON_DAC_CAL_DAC1_IQ_MSB");
    dac_cal_status &= rfcommon_msm_cal_execute_dac_cal(RFCOMMON_DAC_CAL_DAC1_IQ_MSB);
  }
  if(cal_cmd & DAC_1_DC_B_MSK)
  {
    FTM_MSG( FTM_HIGH, "ftm_common_dac_calibration: RFCOMMON_DAC_CAL_DAC1_IQ_DC");
    dac_cal_status &= rfcommon_msm_cal_execute_dac_cal(RFCOMMON_DAC_CAL_DAC1_IQ_DC);
  } 
  if(cal_cmd & DAC_0_MSB_B_MSK)
  {
    FTM_MSG( FTM_HIGH, "ftm_common_dac_calibration: RFCOMMON_DAC_CAL_DAC0_IQ_MSB");
    dac_cal_status &= rfcommon_msm_cal_execute_dac_cal(RFCOMMON_DAC_CAL_DAC0_IQ_MSB);
  }
  if(cal_cmd & DAC_0_DC_B_MSK)
  {
    FTM_MSG( FTM_HIGH, "ftm_common_dac_calibration: RFCOMMON_DAC_CAL_DAC0_IQ_DC");
    dac_cal_status &= rfcommon_msm_cal_execute_dac_cal(RFCOMMON_DAC_CAL_DAC0_IQ_DC);
  }

    
  /*==========================================================================*/
  /* No need to clean up WTR */
  /*==========================================================================*/

  FTM_MSG( FTM_HIGH, "ftm_common_dac_calibration: DAC Cal starting cleanup");
  
    
  /*==========================================================================*/
  /* Clean up */
  /*==========================================================================*/

  /* save the NV item */
  dac_cal_status &= rfcommon_msm_dac_cal_nv_cfg(RFCOMMON_DAC_CAL_SAVE_NV, RFCOMMON_DAC_CAL_RD_DATA_ALL, NULL);

  FTM_MSG( FTM_HIGH, "ftm_common_dac_calibration: DAC Cal cleaning complete");

  // DAC initialization for mission mode entering
  rfcommon_msm_dac_mission_mode_enter();
  
  return dac_cal_status;
#else
  return FALSE;
#endif /* FEATURE_GNSS_ONLY_NO_WWAN */

} /* ftm_common_dac_calibration */

/*! \} */

/*============================================================================*/

/*!
  @name FTM Common ADC calibration

  @brief
  This function does the ADC cal in FTM mode.

  @details
  This function will perform the ADC calibration regardless of the state of the NV
  items. Also the UE would not need any external input signals during calibration.
     
  @return 
  Flag indicating the status of adc calibration 
*/
/*----------------------------------------------------------------------------*/
boolean
ftm_common_adc_calibration
(
  void
)
{

  /*perfrom ADC DC calibration for all ADCs*/
  if(!rfcommon_msm_adc_cal())
  {
    FTM_MSG( FTM_HIGH, "ftm_common_adc_calibration: ADC Cal failed");
    return FALSE;
  }

  FTM_MSG( FTM_HIGH, "ftm_common_adc_calibration: ADC Cal completed succesfully");

  return TRUE;

} /* ftm_common_adc_calibration */


/*============================================================================*/

/*!
  @name Thermistor

  @brief
  Functions related to Therminstor read
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Therm Read

  @details
  This function calls RFM API to query the current Therm on given Device.
 
  @param *ftm_req_data
  FTM Request Data

  @return
  FTM response packet structure indicating whether the command succeeded,
  and if so the response packet data to be returned through Diag.
*/

#ifndef FEATURE_GNSS_ONLY_NO_WWAN
ftm_rsp_pkt_type
ftm_common_get_thermistor_read
(
  void *ftm_req_data
)
{
#if defined(FEATURE_CDMA1X) || defined(FEATURE_HDR)
  ftm_cdma_data_status_type dev_state; /*holds current state of device*/
#endif
  /* Request Packet */
  ftm_common_get_therm_req_type* header = 
    (ftm_common_get_therm_req_type*)ftm_req_data;
  
  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data;          /* Packet returned to Diag */
  ftm_common_get_therm_rsp_type *cmn_rsp; /* Packet Payload for each cmd */

#ifdef FEATURE_LTE
  rfdevice_lte_script_data_type script_data = {TRUE, NULL};
  uint8 band;
  ftm_rf_technology_type rf_tech;
#endif /* #ifdef FEATURE_LTE */

  /* Define : To Extract Useful Data */  
  rfm_device_enum_type device;
  ftm_mode_id_type ftm_rf_mode;

  rfm_mode_enum_type rfm_mode;
  uint32 therm_read_val = 0;
  boolean ret_val = FALSE;
  
  boolean tcxo_voting_flag=FALSE;

  rfm_device_enum_type rfm_dev = RFM_DEVICE_0;

  /* Create a new reponse packet in the ftm heap. This will also update first 
  10-bytes field in Packet Payload in the FTM response packet. These fields 
  are command(1), subsystem ID (1), Command Mode (2), FTM command (2), 
  Request Packet Length (2), and Response packet Lenth (2). */
  ftm_rsp_data = ftm_common_malloc_rsp_pkt( ftm_req_data,
                                    sizeof(ftm_common_get_therm_rsp_type) ); 

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  ftm_rf_mode = (ftm_mode_id_type)header->rf_mode;

  rfm_mode = ftm_common_resolve_rf_mode(ftm_rf_mode);
  
    /*Only enable vreg & power voting if doing therm_read in stand-alone mode 
  and CDMA mode if Set channel is not done */
  if(rfm_mode == RFM_PARKED_MODE  )
  {
     if(!(rfcommon_get_dev_power_voting_status(device)))
     {
        rfcommon_core_dev_power_vote(device,RFCMN_CORE_DEV_ON,RFCMN_CORE_DEV_NORMAL_VOTE);
	    tcxo_voting_flag = TRUE;
     }
  }

  switch ( rfm_mode )
  {

  #ifdef FEATURE_CDMA1X
  case RFM_1X_MODE:
  #endif /* #ifdef FEATURE_CDMA1X */
  #ifdef FEATURE_HDR
  case RFM_1XEVDO_MODE:
  #endif /* ifdef FEATURE_HDR */
  #if defined(FEATURE_CDMA1X) || defined(FEATURE_HDR)
  /* Query the current RF State */
     (void)ftm_cdma_data_get_device_status( device, &dev_state );

     /* Not in correct state for CDMA mode */
     if (dev_state.rf_state == FTM_CDMA_STATE_INVALID) 
     {

        ret_val = FALSE;

        FTM_MSG_1( FTM_ERROR, "ftm_common_get_thermistor_read: Therm read for "
                              "tech %d RF device not in correct state", rfm_mode );
     }

     /* Turn on VREG if not already enabled */
     if (dev_state.rf_state == FTM_CDMA_STATE_SLEEP) 
     {

        (void)rfc_cdma_manage_vregs( device, rfm_mode, dev_state.curr_band,
                                                              RF_PATH_RX_STATE);

        if(!(rfcommon_get_dev_power_voting_status(device)))
        {
               rfcommon_core_dev_power_vote(device,RFCMN_CORE_DEV_ON,RFCMN_CORE_DEV_NORMAL_VOTE);
               tcxo_voting_flag = TRUE;
        }
     }
    ret_val = rfm_cdma_ftm_get_therm_read( device, &therm_read_val, dev_state.curr_band);
    break;
  #endif /* #ifdef FEATURE_CDMA1X || FEATURE_HDR */

  #ifdef FEATURE_WCDMA
  case RFM_IMT_MODE:
    rfdevice_wcdma_tx_cmd_dispatch( RFM_DEVICE_0,ftm_curr_wcdma_mode,RFDEVICE_GET_THERMISTER_VALUE,
                                    &therm_read_val);
    ret_val = TRUE;
    break;
  #endif /* #ifdef FEATURE_WCDMA */

  #ifdef FEATURE_GSM
  case RFM_EGSM_MODE:
    rfdevice_gsm_cmd_dispatch( rfm_dev, RF_PATH_0, RFDEVICE_GET_THERMISTER_VALUE,
                               &therm_read_val);
    ret_val = TRUE;
    break;
  #endif /* #ifdef FEATURE_GSM */

  #ifdef FEATURE_TDSCDMA
  case RFM_TDSCDMA_MODE:
  	rfdevice_tdscdma_tx_cmd_dispatch(RFM_DEVICE_0,rfm_mode,RFCOM_BAND_TDSCDMA_ANY_BAND, RFDEVICE_GET_THERMISTER_VALUE,&therm_read_val);
	ret_val = TRUE;
	FTM_MSG_1( FTM_HIGH, "ftm_common_get_thermistor_read for TD is done, ", 0 );
	break;

  #endif /* #ifdef FEATURE_TDSCDMA */
  #ifdef FEATURE_LTE
  case RFM_LTE_MODE:
     if (ftm_common_get_band_tech_from_rf_mode(ftm_rf_mode,&band,&rf_tech))
     {
        rfdevice_lte_tx_cmd_dispatch(rfm_dev,
                                       band,
                                       RFDEVICE_LTE_GET_THERMISTER_VALUE, 
                                       &therm_read_val, &script_data);                                       
        ret_val = TRUE;
      }
     break;
  #endif /* #ifdef FEATURE_LTE */
  default:
    ret_val = FALSE;
    FTM_MSG_1( FTM_ERROR, "ftm_common_get_thermistor_read: Therm read for "
                          "tech %d no supported", rfm_mode );
    break;

  }
  
  /*Disable resources including TQ*/
  (void)ftm_common_resource_config_request(FALSE);

  /*Only disable vreg & power voting if doing therm_read in stand-alone mode 
  and CDMA mode if Set channel is not done */
  if(rfm_mode == RFM_PARKED_MODE)
  {
     if(tcxo_voting_flag == TRUE)
     {
        rfcommon_core_dev_power_vote(device,RFCMN_CORE_DEV_OFF,RFCMN_CORE_DEV_NORMAL_VOTE);
        tcxo_voting_flag = FALSE;
     }
  }

switch (rfm_mode)
{
  #ifdef FEATURE_CDMA1X
  case RFM_1X_MODE:
  #endif /* #ifdef FEATURE_CDMA1X */
  #ifdef FEATURE_HDR
  case RFM_1XEVDO_MODE:
  #endif /* ifdef FEATURE_HDR */
  #if defined(FEATURE_CDMA1X) || defined(FEATURE_HDR)
     if(tcxo_voting_flag == TRUE)
     {
        rfcommon_core_dev_power_vote(device,RFCMN_CORE_DEV_OFF,RFCMN_CORE_DEV_NORMAL_VOTE);
        tcxo_voting_flag = FALSE;
     }

     /* Turn off VREG if enabled before */
     if (dev_state.rf_state == FTM_CDMA_STATE_SLEEP) 
     {
           rfc_cdma_manage_vregs(device, rfm_mode, dev_state.curr_band, RF_PATH_SLEEP_STATE);
     }
  break;
  #endif /* #ifdef FEATURE_CDMA1X || FEATURE_HDR */
  default:
  break;
}

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cmn_rsp = (ftm_common_get_therm_rsp_type*)ftm_rsp_data.pkt_payload;

  if ( cmn_rsp != NULL )
  {
    /* Update the Response Packet Field */
    cmn_rsp->therm_read = therm_read_val;
    cmn_rsp->therm_status = ret_val;
  }
  else
  {
    ret_val = FALSE;
  }

  /* Check if the eveything in the function executed or not. If not send
  back a BAD response packet */
  if ( ret_val == FALSE )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG_1( FTM_ERROR, "ftm_common_get_thermistor_read: Dev %d, "
                          "Therm Read failed", device );
  }
  else
  {
    FTM_MSG_3( FTM_LOW, "ftm_common_get_thermistor_read: Dev %d, "
                        "Therm Read %d, Valid %d", device, therm_read_val, 
                        ret_val );
  } 
  
  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_common_get_thermistor_read */
#endif /* FEATURE_GNSS_ONLY_NO_WWAN */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Vote/Unvote modem bus clk

  @details
  This function calls NPA API to vote and unvote modem bus clk.
 
  @param bus_clk
  When bus_clk is non-zero, then will vote modem bus clk to bus_clk(HHz);
  When bus_clk = 0, will unvote modem bus clk;
*/
void ftm_common_npa_vote_for_modem_bus_clk
(
   uint32 bus_clk
)
{
  npa_client_handle        mss_config_bus;
  mss_config_bus = npa_create_sync_client("/clk/mss/config_bus",
                                          "rf_mss_config_bus",
                                           NPA_CLIENT_REQUIRED);

  if(mss_config_bus != NULL)
  {
    if ( bus_clk >0 )
    {
      npa_issue_required_request(mss_config_bus, bus_clk);
    }
    else
    {
      npa_complete_request(mss_config_bus);
    }
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_common_npa_vote_for_modem_bus_clk: NULL ptr");
  }

  return;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable/disable FB path and set gain state

  @details
  This function will call tech specific API to enable/disable FB path and set gain state

  @param *ftm_req_data
  FTM Request Data

  @return
  None
 */
void
ftm_common_set_fb_path_state
(
  void* ftm_req_data,
  ftm_rf_technology_type ftm_rf_tech
)
{
  boolean api_status = TRUE;
  ftm_common_set_fb_path_state_req_type *set_fb_path_state_param;
  rfm_device_enum_type rfm_dev;

  #if defined(FEATURE_CDMA) || defined(FEATURE_TDSCDMA)
  const rfc_common_logical_device_params_type *device_params_ptr = NULL;
  #endif
  
  /* Extract parameters */
  set_fb_path_state_param = (ftm_common_set_fb_path_state_req_type*)ftm_req_data;

  rfm_dev = set_fb_path_state_param->tx_device;

  /* Call FB path cmd according to tech*/
  switch( ftm_rf_tech )
   {
  case FTM_RF_TECH_LTE:
      #ifdef FEATURE_LTE
       rflte_ftm_mc_sm_set_use_fb_path(set_fb_path_state_param->fb_path_enable);

       rflte_ftm_mc_set_et_path(set_fb_path_state_param->fb_gain_state, set_fb_path_state_param->fb_path_enable);
      #endif
      break;

  case FTM_RF_TECH_WCDMA:
      #ifdef FEATURE_WCDMA
      if(set_fb_path_state_param->fb_path_enable == TRUE)
      {
        ftm_wcdma_feedback_setup(rfm_dev,
                                RFM_DEVICE_1,
                                FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_WTR| FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_RXLM_FOR_FBAGC,
                                set_fb_path_state_param->fb_gain_state,
                                TRUE,
                                FALSE,
                                TRUE);
      }
      #endif
      break;

  case FTM_RF_TECH_CDMA:
      #ifdef FEATURE_CDMA
      device_params_ptr = rfc_common_get_logical_device_params(rfm_dev);

      if(device_params_ptr == NULL)
      {
        api_status = FALSE;
      }
      else
      {
        rfdevice_cmn_set_et_path(device_params_ptr->cmn_device,RFM_1X_MODE,set_fb_path_state_param->fb_gain_state,set_fb_path_state_param->fb_path_enable);
      }
      #endif
      break;

  case FTM_RF_TECH_TDSCDMA:
      #ifdef FEATURE_TDSCDMA
      device_params_ptr = rfc_common_get_logical_device_params(rfm_dev);

      if(device_params_ptr == NULL)
      {
        api_status = FALSE;
      }
      else
      {
        rfdevice_cmn_set_et_path(device_params_ptr->cmn_device,RFM_TDSCDMA_MODE,set_fb_path_state_param->fb_gain_state,set_fb_path_state_param->fb_path_enable);
      }
      #endif
      break;

  default:
      break;
   }   
   
  if ( api_status == FALSE)
  {
    FTM_MSG( FTM_ERROR, "ftm_common_set_fb_path_state: api status = FAIL" );
  }
  return;
} /* ftm_common_set_fb_path_state */

void
ftm_common_set_coupler_gain
(uint8 gain)
{

  //rf_hal_bus_result_type err;
  rf_hal_bus_resource_script_settings_type settings;
  rf_hal_bus_rffe_type script;
  uint8 reg_value;
  //uint8 reg_mask=0x09; //0x30; //0x08;

  /* Set the extra settings */
  memset(&settings, 0, sizeof(rf_hal_bus_resource_script_settings_type));
  settings.half_rate = 1; //halfspeed
  settings.rffe_force_ext = 0; //non-extended;


  /* set the LB ASM */

  /* Set the RFFE transaction settings */
  script.channel = 1; // RFFE1
  script.extended_cmd = 1;
  script.half_rate = 1; //halfspeed;
  script.num_bytes = 1;
  script.rd_delay = 0;
  script.slave_id = 0x0A; /* LB ASM slave ID = 0xA*/
  script.start_delta = 0;
#if 0
  /* read the coupler register*/
  script.addr = 0x03; //0x04; //0x03;  //COUPLER_CTRL register
  script.rd_wr = 1;    //read
  err = rfhal_rffe_read_bytes_ext(&script, script.channel, 1, &settings);
  if(err == RF_HAL_BUS_SUCCESS)
  {
    reg_value = script.data[0];
  }
  else
  {
    FTM_MSG( FTM_ERROR, "RFFE read failed" );
    return;
  }

  FTM_MSG_1( FTM_HIGH, "ftm_common_set_coupler_direction: read_value %X", reg_value);
#endif



    reg_value = gain; //(reg_value & ~reg_mask) | (0x09 & reg_mask); // Set bit:3 to 1



  /*write the new register value in coupler register*/
  script.addr = 0x04;  //COUPLER_CTRL register
  script.rd_wr = 0;    //write
  script.data[0] = reg_value;
  rfhal_rffe_write_bytes_ext(&script, script.channel, 1, &settings);

  /* Write the trigger register*/
  script.addr = 0x1C;  //PM_TRIG register
  script.rd_wr = 0;    //write
  script.data[0] = 0x01;
  rfhal_rffe_write_bytes_ext(&script, script.channel, 1, &settings);


  script.slave_id = 0x0B; /* LB ASM slave ID = 0xA*/

    /*write the new register value in coupler register*/
  script.addr = 0x04;  //COUPLER_CTRL register
  script.rd_wr = 0;    //write
  script.data[0] = reg_value;
  rfhal_rffe_write_bytes_ext(&script, script.channel, 1, &settings);

  /* Write the trigger register*/
  script.addr = 0x1C;  //PM_TRIG register
  script.rd_wr = 0;    //write
  script.data[0] = 0x01;
  rfhal_rffe_write_bytes_ext(&script, script.channel, 1, &settings);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Change the coupling direction of a coupler

  @details
  
  @param
  
  FTM Request Data

  @return
  None
 */

void
ftm_common_set_coupler_direction
(
  uint8 coupler_dir
)
{

  //rf_hal_bus_result_type err;
  rf_hal_bus_resource_script_settings_type settings;
  rf_hal_bus_rffe_type script;
  uint8 reg_value;
  //uint8 reg_mask=0x09; //0x30; //0x08;

     rf_time_tick_type prof_tick;
   rf_time_type prof_time = -1;

  prof_tick = rf_time_get_tick();

  /* For all the cases of LTE tech deal with the device API.
     No hard coding */
  /*! @note 
      Currently this API supports the operation only on RFM_DEVICE_0
      Hence this API would break for UL CA!*/
  if(  ( ftm_rfmode_current[RFM_DEVICE_0] ==   FTM_STATE_LTE) ||
       ( ftm_rfmode_current[RFM_DEVICE_0] ==   FTM_STATE_LTE_NS) ||
       ( ftm_rfmode_current[RFM_DEVICE_0] ==   FTM_STATE_LTE_IRAT ) )
  {
#ifdef FEATURE_LTE

    if(coupler_dir == 0)
    {
      rflte_ftm_tx_hw_ctl_config_coupler_device_imm ( RFM_DEVICE_0, 
                                                      RFDEVICE_COUPLER_DIRECTION_FWD );
    }
    else if(coupler_dir == 1)
    {
      rflte_ftm_tx_hw_ctl_config_coupler_device_imm ( RFM_DEVICE_0, 
                                                      RFDEVICE_COUPLER_DIRECTION_REV );
    }
    else
    {
      RF_MSG_1( RF_ERROR, "Unsupported coupler direction provided!  %d", coupler_dir);
    }

#endif
  }
  else
  {
  /* Set the extra settings */
  memset(&settings, 0, sizeof(rf_hal_bus_resource_script_settings_type));
  settings.half_rate = 1; //halfspeed
  settings.rffe_force_ext = 0; //non-extended;

  if (selftest_lowband) {
    /* set the LB ASM */

    /* Set the RFFE transaction settings */
    script.channel = 1; // RFFE1
    script.extended_cmd = 1;
    script.half_rate = 1; //halfspeed;
    script.num_bytes = 1;
    script.rd_delay = 0;
    script.slave_id = 0x0A; /* LB ASM slave ID = 0xA*/
    script.start_delta = 0;
  #if 0
    /* read the coupler register*/
    script.addr = 0x03; //0x04; //0x03;  //COUPLER_CTRL register
    script.rd_wr = 1;    //read
    err = rfhal_rffe_read_bytes_ext(&script, script.channel, 1, &settings);
    if(err == RF_HAL_BUS_SUCCESS)
    {
      reg_value = script.data[0];
    }
    else
    {
      FTM_MSG( FTM_ERROR, "RFFE read failed" );
      return;
    }

    FTM_MSG_1( FTM_HIGH, "ftm_common_set_coupler_direction: read_value %X", reg_value);
  #endif

    if(coupler_dir == 1)
    {
      //reg_value = (reg_value & ~reg_mask) | (0x30 & reg_mask); // Set bit:3 to 1

      reg_value = 0x89; //(reg_value & ~reg_mask) | (0x09 & reg_mask); // Set bit:3 to 1
    }
    else
    {
      //reg_value = (reg_value & ~reg_mask) | (0x00 & reg_mask); // Set bit:3 to 0

      reg_value = 0x81; //(reg_value & ~reg_mask) | (0x01 & reg_mask); // Set bit:3 to 0
    }



    /*write the new register value in coupler register*/
    script.addr = 0x03;  //COUPLER_CTRL register
    script.rd_wr = 0;    //write
    script.data[0] = reg_value;
    rfhal_rffe_write_bytes_ext(&script, script.channel, 1, &settings);

    /* Write the trigger register*/
    script.addr = 0x1C;  //PM_TRIG register
    script.rd_wr = 0;    //write
    script.data[0] = 0x01;
    rfhal_rffe_write_bytes_ext(&script, script.channel, 1, &settings);

  } else{
  #if 0
    /* read the coupler register again and print new value*/
    script.addr = 0x03;  //COUPLER_CTRL register
    script.rd_wr = 1;    //read
    err = rfhal_rffe_read_bytes_ext(&script, script.channel, 1, &settings);
    if(err == RF_HAL_BUS_SUCCESS)
    {
      reg_value = script.data[0];
    }
    else
    {
      FTM_MSG( FTM_ERROR, "RFFE read failed" );
      return;
    }


    FTM_MSG_1( FTM_HIGH, "ftm_common_set_coupler_direction: after write and change gain read_value LB ASM %X", reg_value);


  #endif

    /* set the HB ASM */

   
    /* Set the RFFE transaction settings */
    script.channel = 1; // RFFE1
    script.extended_cmd = 1;
    script.half_rate = 1; //halfspeed;
    script.num_bytes = 1;
    script.rd_delay = 0;
    script.slave_id = 0x0B; /* HB ASM slave ID = 0xA*/
    script.start_delta = 0;
  #if 0
    /* read the coupler register*/
    script.addr = 0x03; //0x04; //0x03;  //COUPLER_CTRL register
    script.rd_wr = 1;    //read
    err = rfhal_rffe_read_bytes_ext(&script, script.channel, 1, &settings);
    if(err == RF_HAL_BUS_SUCCESS)
    {
      reg_value = script.data[0];
    }
    else
    {
      FTM_MSG( FTM_ERROR, "RFFE read failed" );
      return;
    }

    FTM_MSG_1( FTM_HIGH, "ftm_common_set_coupler_direction: read_value %X", reg_value);
  #endif
    /*update the register value with direction change*/
    if(coupler_dir == 1)
    {
      //reg_value = (reg_value & ~reg_mask) | (0x30 & reg_mask); // Set bit:3 to 1

      reg_value = 0x89;//(reg_value & ~reg_mask) | (0x09 & reg_mask); // Set bit:3 to 1
    }
    else
    {
      //reg_value = (reg_value & ~reg_mask) | (0x00 & reg_mask); // Set bit:3 to 0

      reg_value = 0x81; //(reg_value & ~reg_mask) | (0x01 & reg_mask); // Set bit:3 to 0
    }


    /*write the new register value in coupler register*/
    script.addr = 0x03;  //COUPLER_CTRL register
    script.rd_wr = 0;    //write
    script.data[0] = reg_value;
    rfhal_rffe_write_bytes_ext(&script, script.channel, 1, &settings);

    /* Write the trigger register*/
    script.addr = 0x1C;  //PM_TRIG register
    script.rd_wr = 0;    //write
    script.data[0] = 0x01;
    rfhal_rffe_write_bytes_ext(&script, script.channel, 1, &settings);
  #if 0
    /* read the coupler register again and print new value*/
    script.addr = 0x03;  //COUPLER_CTRL register
    script.rd_wr = 1;    //read
    err = rfhal_rffe_read_bytes_ext(&script, script.channel, 1, &settings);
    if(err == RF_HAL_BUS_SUCCESS)
    {
      reg_value = script.data[0];
    }
    else
    {
      FTM_MSG( FTM_ERROR, "RFFE read failed" );
      return;
    }


    FTM_MSG_1( FTM_HIGH, "ftm_common_set_coupler_direction: after write and change gain read_value HB ASM %X", reg_value);
  #endif
  }

  } /* End of else*/

  prof_time = rf_time_get_elapsed( prof_tick, RF_USEC );
  FTM_MSG_1( FTM_HIGH, "Self_test coupler Time profiling "
                         "ExecTime %d us", prof_time );

}

/*! \} */

#endif /*FEATURE_FACTORY_TESTMODE */

/*! @} */
