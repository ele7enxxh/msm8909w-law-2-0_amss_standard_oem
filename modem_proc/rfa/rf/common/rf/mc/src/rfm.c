/*!
   @file rfm.c

   @brief
   This file implemnets the RF Driver's common multi-mode interface functions.

   @details
   Some of the RF Drivers interface functions are common across all technologies
   and some are technology dependent. The common interface definitions are
   implemented in this file. The technology dependent RF Drivers interface cam
   be found in rfm_<mode>.c file, whre mode can be 1x, wcdma, gsm, hdr or gps.

  @addtogroup RF_COMMON_RFM
  @{
*/

/*===========================================================================
Copyright (c) 1999 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/mc/src/rfm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/05/16   hm      Added a new API 
                   rfm_get_sys_lte_band_mask_from_rflte_band_mask()
07/05/16   cv      Adding LTE B66 and LTE-U support
05/26/16   esw     check for NON-FTM tech calls in rfm_set_antenna for GSM
10/21/15   as      Exposed API to ML1 for updating LTE 1Rx mode
04/06/15   ag      Adding support for enabling/disabling band capability from bc config
01/27/15   vc      Propogating immediate flag for FTM AsDiv support
11/13/14   qzh     Add check of if RFC TDS is created before calling TDS init in rfm_init
11/04/14   ndb     Fix the compiler error in W/T removed builds
11/03/14   ndb     Fix the compiler error in W/T removed builds
11/03/2014 aro     Added feature around W variable
10/22/14   zbz     Add support to reload NV for all chains for GSM
10/20/14   vbh     Add a debug routine that prints WTR registers
10/20/14   vb      ASDiv immediate script support
10/10/14   sw      Fix compiler warning
09/24/14   pl      Added get_rx_lo_freq support
09/18/14   vv      vreg_mgr API support to detect rf sleep status
09/10/14   vv      Added RFM therm API support
09/02/14   dr      Removed featurization for Non-W/TDS builds
08/21/14   sw      Add GSM case in rfm_set_antenna_imm()
08/14/14   aak     Move MGPI block config to Modem HAL module
08/13/14   aak     Disable MGPI CBCR clocks after the block is configured
08/11/14   ra      Support Rumi targets
08/08/14   aak     Configure MGPI block for TX_GTR_THRESH signal
08/07/14   nv      Enabling back ASDIV for WCDMA DPM as memory leak issue is fixed.   
07/31/14   ndb     Added support for extended EARFCN for LTE
08/01/14   vv      Added support to detect calibration state for use in device driver space.
07/31/14   sg      Update band support information to TRM based on RFC
                   as well as BC config NV mask
07/21/14   zhh    correct the implementation of rfm_is_sleeping
07/18/14   dr      [RUMI] Removing rf_hal_bus_enable calls for RUMI
06/30/14   vbh     [DR-DS] [1] Clean-up device 0 hardcode 
                   [2]Use MC state variable to identify Primary Rx device
06/25/14   nv      Temporarily disabling ASDIV for WCDMA DPM as it is causing memory leak.             
06/16/14   tks     Added more bands in the sys band to rf card band conversion
06/06/14   sw      Adding support for antenna switch in FTM mode for GSM tech
05/28/14   tks     Added trm interface api's for concurrency management
05/05/14   svi     Moved LTE TXPLL update to Core Task
02/25/14   zhh     change the input param in rfm_exit_mode()
02/18/14   sc      Remove unused code and dependencies 
02/08/14   zhh     Add param validation to check whether the device id is valid or not
02/05/14   st      DAC Vref Initialization - Mission Mode Setup
12/10/13   hdz     Added rflm_cmn_init()
1/26/13    nrk     Fixed compiler warnings
11/18/13   cri     Support for SW allocation of DPD sample capture buffers
10/31/13   svi     LTE Fed Shim Layer Checkin - 2
10/30/13   svi     Added support for RxPLL AFC
10/29/13   vrb     Gate TRM intf to get devices config based on rfc init
10/24/13   st      DAC Bringup added to Bootup
10/22/13   APU     Use enable_rumi_code flag to skip rumi_rfc_configure() to 
                   test WTR3925 instantiation.
10/07/13   ra      Add support to start RFLM
09/05/13   pl      Added Asdiv init default switch position API
08/14/13   ak      phone enter to "offline" with QRD RP4 RF card on TR1.1 CRM build 
07/29/13   ms      Enable TDSCDMA AsDiv
07/26/13   pl      Enable LTE AsDiv
07/24/13   ec      Pass rf_task into MC layer where necessary
07/23/13   aro     Added inteface to do immediate write
07/23/13   zhw     Update CDMA set_antenna handler interface
07/22/13   aro     Added immediate flag for rfm_set_antenna()
07/19/13   aro     Featureized ASDIV code
07/18/13   aro     CB Pointer fix
07/17/13   aro     Added rfm_set_antenna() implementation
07/10/13   JJ      Remove lagacy flag rumi_bup_cdma  
06/05/13   kb      [XPT] Support to update rfm_mode for TDS in FTM and online mode.
05/31/13   pl      Added API for Antenna Switch Diversity Feature
05/28/13   ak      Featurize/Fix G+W only build compiler warnings
05/23/13   rmb     Fix Compilation Error in No C2K and No TDS flavored builds by 
                   featurising the code for C2K and WCDMA
05/21/13   rmb     Added API to get multi synth lock.
05/01/13   jj      Add TTL markers to RFA_common
03/13/13   aro     Featurization not to perform TDSCDMA init for offtarget
                   to fix memleak issue
03/13/13   aro     Compiling GNSS Init for off-target to fix CDMA test
                   case crash
03/12/13   ms      Featurize TDSCDMA for linker issues
03/11/13   sar     Updated file for APQ, GNSS only target.
03/08/13   zhw     Remove outdated temporary fix
02/15/13   sc      Remove RF GSM exit mode from rfm_exit_mode
02/12/13   sr      changes to free the memory alloocated during rfm_init() 
01/21/13   nrk     Changed input param type path to device for rfm_get_synth_lock_status
01/18/13   fh      Triton compile error fixed
01/04/13   gvn     Move interface to query RF LTE CA bc config to rflte_mc
01/3/13    av      Changes to support chain2 for GSM
12/27/12   bsh     KW errors fixed
12/21/12   gvn     Adding functionality for Upper layers to query LTE CA bc config
11/29/12   av      Using proper featurization and removing featurization that is not needed
11/26/12   nrk     Added API to get multi synth lock 
11/21/12   dbz     Enable RFM GNSS init regardless of (irrelevant) WWAN RUMI flag
11/20/12   gvn     Featurize LTE for Triton 
11/15/12   adk     Backed out Qtuner code
11/07/12   jfc     Initialize rf hal bus vote handle to NULL
08/14/12   ndb     Check the current RF mode in rfm_set_ant_tuner_spi_wr_enable()
08/07/12   ndb      Change antenna tuner settings api to be customizable
11/02/12   jfc     Replace rf_hal_enable_rfcmd_app with rf_hal_bus_enable 
10/30/12   spa     Use RFM_DEVICE_3 as diversity device on SV path
10/25/12   gvn     Interface for LTE CA bc config
10/16/12   tsr     Cleanup GSM RF bringup flags 
10/09/12   pl      Featurize LTE enter_mode and quite_mode
10/02/12   ac      fix for the wcdma crash in online
09/25/12   tc      Mainline RF_HAS_RFA_GSM_TASK. 
09/20/12   pl      Moved RFLTE_MC() here. External entities to RFA are not to use RFLTE_MC() macro
09/19/12   jfc     Remove request for RFMCD resources at end of rfm_init() 
09/17/12   jfc     Enable voting for RFCMD 
08/30/12   sty     Deleted redundant msg_mask_tbl - was added during RUMI 
08/06/12   jfc     Call rf_hal_bus_init() during rfm_init
07/29/12   ac      Added wcdma debug flags for RUMI Bring Up   
07/24/12   zhw     CDMA remove F3 message hack since fix is available
07/18/12   jyu     Added TDSCDMA debug flags for RUMI Bring Up   
07/17/12   tsr     Add GSM debug flags for RUMI Bring Up
07/10/12   zhw     CDMA temporary fix for RF and FTM F3 message not being displayed.
07/06/12   php     Add LTE debug flags for RUMI Bring Up
06/27/12   zhw     CDMA RUMI support. Skipped init for other techs when debug flag is enabled.
06/19/12   sty     do not call rfgnss_init( ) for off-target 
06/07/12   sc      Add rfgsm_mode and functionality to monitor rfm_mode
06/05/12   pl      Update rflte_mc_enter_mode for CA support
06/04/12   vvr     Fix for Off Target compile issues
05/25/12  kb/npi   Updated the rfm_init() state using an enum
05/21/12   aca     AFC interface cleanup
05/11/12   spa     rfm_is_band_chan_supported_v2 queries all possible devices
04/17/12   kai     Move update_txlm_buffer from enable_tx to init_tx, remove txlm_buf_idx from disable_tx
04/13/12   kai     Revert the following update_txlm_buffer change since gsm layer1 dependence not ready
04/13/12   kai     Move update_txlm_buffer from enable_tx to init_tx, remove txlm_buf_idx from disable_tx
03/27/12   ems     Added device exit to RF NV Reload
03/20/12   sty     Reverted previous change 
03/26/12   jfc     Clean up pm_vreg_votes
02/23/12   aca     Removed incorrect error reporting
02/13/12   tks     Added band enum for WCDMA Band XIX & Band XI.   
02/09/12   aca     Tx PLL error correction
01/27/12   bmg     Updated rfm_get_band_mask() to support SV chains device 2
                   and device 4.
01/24/12   npi     Added support to return the WCDMA current PA state
01/03/12   sty     deleted RFM_DEBUG_EN_Q call from rfm_get_rx_warmup_time()
01/03/12   sty     Fixed RFM_DEBUG_EN_Q call
12/14/11   sbm     Added rfm_get_rx_freq_from_rx_earfcn for rfcn to freq conversion.
12/10/11   sty     Changes in rfm_exit_mode to invoke correct API based on 
                   1x / HDR tech
                   Update mode info only on success - this is applicable only
                   for 1x and HDR
                   Check for success of exit-mode before in rfm_enter_mode() for
                   1x and HDR
12/07/11   dbz     Added WWAN TX indicator for GNSS IMD feature
11/29/11   bmg     Added rfm_is_band_chan_supported_v2()
11/17/11   aak     Critical section locks for RFC 
11/17/11   aak     Changes to implement Quiet mode 
11/11/11   aak     Changes to implement Quiet mode 
11/10/11   Saul    SV Limiting. Initial support.
10/13/11   whc     Adding RFM layer function to return the current PA state.  
10/11/11   sty     Call rf_common_init_critical_section() unconditionally
                   Removed unused variables 
09/28/11   sb      Mainline FEATURE_RF_COMMON_LM_RFM_INTERFACE and FEATURE_RF_WCDMA_LM_RFM_INTERFACE.
09/19/11   sr      Enable GSM RF sleep/wakeup functionality.
09/16/11   aak     Implementation of rfm_power_clock_on() and rfm_power_clock_off() 
09/12/11   sty     Deprecated rfm_is_tunable_on_antenna
09/07/11   jhe     Include TD-SCDMA header file
09/02/11   bmg     Added rfm_get_supported_bw() support for 1x and HDR
08/17/11   av      Cleanup for WtoG
08/11/11   gy      Enabled GNSS init
08/12/11   jhe     Added support for TDS BC config
08/10/11   pl      Remove all references to legacy RFC structure and related API
08/08/11   dw      Obsolete rfwcdma_mc_tune_to_chan
08/08/11   hy      Changed FEATURE_RF_HAS_TDSCDMA to FEATURE_TDSCDMA 
07/20/11   jhe     Added TD-SCDMA support 
07/13/11   sty     Deleted FEATURE_RUMI3_BRINGUP code 
07/12/11   aro     Added check for success of rfm_init() execution
07/06/11   aro     Added RFM Lock Data and accessor function
06/30/11   cd      Moved TRM configuration functionality to RFC.
06/21/11   aro     Fixed TRM Config Bug: Changed Logical OR to Arithmetic OR
06/14/11   aro     Updated TRM interface to return data validity
06/08/11   aro     Corrected bit mask to indicate 1x support in device 2
06/07/11   aro     Added 1x as Tech supported for Device 2
06/07/11   aro     Enabled SVDO for TRM
06/07/11   aro     Mainlined FEATURE_RF_SVDO_API
05/10/11   sty     Added bringup workaround to skip GNSS init
05/10/11   sty     Work-around for 8960 bringup
05/06/11   aro     Filled in temporary TRM Configuration for nonSV platform.
04/22/11   aro     Doxygen Documentation Update
04/21/11   aro     Added Interface for TRM to get Device Configuration
04/20/11   pl      Fix Compiler Warning
04/18/11    xw     Deleted VCO coarse tune 
04/13/11   sty     Bypass rfm_init() for LTE/GNSS/GSM - since they end up 
                   doing RTR writes (which is not supported for RUMI)
04/13/11   sty     Changes for RUMI3 bringup
04/05/11   sar     Included rfm_helper_incs.h for debug code. 
04/04/11   aro     [1] Removed redundant function to get current RFM Mode
                   [2] Removed CDMA implementation from rfm_is_sleeping()
03/31/11   sty     Added API rfm_get_rfm_mode_for_device()
                   Made rfm_mode[] static
03/28/11   dw      Fix compilation error.
03/22/11   dw      RUMI update
03/21/11   sr      Added LM support for GSM
03/21/11   sty     Changed API name - rfm_is_rfm_init_success
03/21/11   sty     Made rfm_initialized static
                   Added accessor function rfm_get_rfm_initialized_state()
03/09/11   sty     Changed behavior of rfm_enter_mode on failure 
03/02/11   aro     Moved Set/Get Calibration state interface to RF
02/28/11   tnt     Merge to MDM9K PLF110
02/25/11   ka/dw   Merge support for thermal mitigation  
02/22/11   aro     Get RFM Mode Bug Fix
02/17/11    av     Reverting GP_CLK changes
02/17/11   aro     Removed RFM Device Check to replace with RFM Mode check
02/16/11   cri     Featurize code for compliance with GW removal
02/09/11   jyu     Updated rfm_mode variable when doing LTE enter_mode     
02/02/11   av      Disable GP_CLK
01/31/11   aro     Renamed CDMA get RF WU time function
01/31/11   dw      Fix compilation error
01/28/11   dw      Initial TxLM support for WCDMA
01/27/11    ka     Added support for thermal mitigation 
01/25/11   sty     Updated rfm_get_rx_warmup_time
01/21/11   bmg     Removed FEATURE_RFA_CMI_API from rfm_init()
01/05/11   dw      Initial Support for RxLM
01/15/11   aro     Removed direct access to rfm_mode[] variable
01/04/11   sty     Added check for successful rfm_init() in rfm_enter_mode 
                   Check for tech_specific_enter_mode success
12/17/10    ap     Fixed airplane mode power leakage 
12/17/10   aro     Removed Device Param from rf_cdma_mc_init()
12/16/10   aro     Removed unnecessary functions for SVDO flavor
12/09/10   aro     Added Support to Compile SVDO Build
12/07/10   aro     Added Initial SVDO Support
12/07/10   aro     Removed SVDO Init Functions
11/30/10   dbz     Added GSM fast scan notification API, changed GNSS GSM spur 
                   mitigation algo for GSM fast scan
10/14/10   whc     Added LTE Reload RFNV functionality
10/14/10   mkv/whc Integrating Reload RFNV from SCMM into MDM9x00 (CL 1315259)
10/05/10    av     Fix ARM compiler warning
09/22/10    dw     Remove rfwcdma_core_rxctl_init_cgagc_params() in GSM enter mode
08/31/10    pl     Adding LTE into rfm_enter_mode and rfm_exit_mode
08/20/10    zg     Removed duplicate 1x_enter_mode in HDR enter mode case.
08/04/10    av     Update to LTE B13 GPS algorithm 
07/23/10    ap     Reverted rfm_power_clock_on/off functions for wcdma
07/22/10    av     Fix compiler warnings
07/21/10    av     Fix compiler warnings
07/20/10    ap     Added power collapse debug variable & commented out power_clock_on for wcdma
06/30/10    ap     Added support for Power Collapse & Early Clock Enable 
06/28/10   can     Removing Power collapse changes that cause a crash.
06/24/10   can     Support for LTE BC Config.
06/22/10    ap     Added support for Power Collapse 
05/21/10   lcl     Back out clock changes from ver20.  Not working for mdm9k gsm
05/21/10   tws     Add API function to convert sys_band to rf_card_band.
04/20/10   tws     Initialise the W agc params dfuring G enter mode to prevent
                   timeline issues during G to W IRAT.
04/15/10   adk     Fixed linker errors
04/14/10   adk     Merged branch scmm_mdm9k_merge_final
03/16/10   wen     Fixed 9k compilation errors
03/01/10   kma     Added power collapse status API
02/18/10   aak     Eliminate compile error from UMTS-only build 
02/16/10   sar     KW fixes.
02/08/10   kma     Turned on VREGs in rfm_init()
02/03/10   kma     Fixed compilation issue
02/02/10   kma     Implemented technology specific power collapse functions
02/02/10   sar     Fixed lint errors.
02/01/10   aak     RF1 and RF2 power collapse for 1x mode 
01/31/10   sar     Fixed compiler warnings.
01/29/10   sty     Temp fix for compile errors
29/01/10   sr      Changed the <tech>_mc_init() signature to fix warnings in 
                   <tech> specific builds
01/28/10   kma     Implemented WCDMA specific power collapse functions
01/28/10   sr      made the power_clock_on/off functions technology specific
01/26/10   sr      Moved implementation code for HDR enter_mode to MC layer
01/25/10   ckl     Fixed returning incorrect WCDMA rf tune time
01/25/10   sar     Reverting the previous change due to compiler errors.
02/02/10   aro     Removed Target Specific PMIC voting
01/28/10   lcl     Merge MDM9K change
01/22/10   sar     Removed compiler warnings.
01/21/10   sr      Added rfm_get_rx_warmup_time()
01/21/10   bn      Lint Fixes
01/20/10   pl      Remove unused header file rfm_ext.h
12/17/09   dbz     Changed rfgnss_notch_cfg_type
12/08/09   sr      Removed debug flag 
11/30/09   sr      Fixed warnings
11/30/09   bmg     Added common NV initialization success check and parameter
                   to the technology-specific initialization calls.
11/18/09   sr      Moved rfm_is_band_chan_supported() to rfm_1x.c
11/17/09   sr      1x and EVDO cmd_dispatch functions now use RFCOMMON enums
11/17/09   sr      Added rfm_is_band_chan_supported()
11/16/09   gy      Fixed an Error Fatal message where rf mode is RFM_PARKED_MODE
                   and added gnss check at rfm_enter_mode.
11/16/09   aak     Redefine rf_vote again to resolve compile error. 
11/15/09   aak     Turn OFF RF vreg in rfm_init(). So the vreg will now be 
                   controlled only through the global PM config. 
11/06/09   av      Move RXFE intialization for IRAT from gsm_enter_mode to 
                   rfm_intersystem_setup.
10/30/09   bmg     Added SVDO option for rfm_init and rfm_is_sleeping
10/28/09   dyc     Mode specific build support.
10/28/09   sr      Added p4 file path
10/28/09   sr      Reverted previous change since it breaks 1x Tx
10/20/09   bmg     Removed a check in rfm_enter_mode that prevented 1x/DO
                   and GNSS from being handled in some cases.
10/16/09   jhe     Added rfm_power_clock_on/off
10/15/09   dbz     Added support for WCDMA/GSM spur mitigation algorithms
09/15/09   jhe     temp VREG change for initial w sleep
09/15/09   sr      Added few rfm functions when SVDO is defined
09/15/09   bmg     Added SVDO enter/exit mode support
09/01/09   bmg     Tweaked rfm_enter_mode prototype for better const safety.
08/21/09   bmg     Added RFCOM_1XEVDO_MODE handling
08/13/09   ckl     Changed WCDMA mode to go through enter mode.
08/12/09   xw      Added RTR Rx VCO coarse tune.  
07/20/09   sr      todo cleanup and replaced the MSG_FATAL to MSG_HIGH.
07/08/09   av      Added gobi3k featurization around vreg voting. Need to move this to RFC.
06/26/09   sr      Fixed warnings by changing rfcom_cb_handler to rfm_cb_handler
08/06/09   sr      Changed rfm_enter_mode() to goes through for GSM rgardless of prev_mode.
05/05/09   gy      Added GNSS Gen8 support
05/05/09   dyc     Update and clean up return values.
03/12/09   gy      Changed RFGPS_MC to RFGNSS_MC
03/12/09   sar     Added changes needed for 1x
10/24/08   rmd     QCS7X30 Code Clean up REV2. Mainlined: T_MSM7600.
10/13/08   dyc     Added MSG instrumentations for RFM layer
08/18/08   adk     Moved function rfm_get_tx_carrier_freq() from rf1x.c
08/13/08   ad      Fixed compiler warning: enumerated type mixed with another type
08/07/08   ad      Added support for rfnv_validate_tech_in_rf_card()
08/07/08   dw      Fixed issue that 76xx 1x only builds can not load QCN
08/05/08   ad      Removed FEATURE_HDR_REVB around rfm_rf_card_support[] array
07/31/08   Vish    Handle RFCOM_RECEIVER_1 for RFCOM_1X_MODE without affecting
                   RFCOM_TRANSCEIVER_0 in rfm_enter_mode().
07/30/08   Vish    Fixed rfm_enter_mode() for handling RFCOM_1XEVDO_MODE.
07/14/08   adk     Use rfapi_dev_ptr[RFCOM_RECEIVER_DUAL] only if feature
                   RF_HAS_FEATURE_CDMA1X is defined (to avoid NULL ptr access).
06/04/08   ra      move rfm_gps_pause() and rfm_gps_resume() to rfm file
05/24/08   ad      Enter rfm mode dynamically based on BC config
06/09/08   kma     Resolved compilation issue
05/23/08   adk     Ported DORB changes from the 7800 branch.
05/19/08   adk     Update rf_path_state management for GPS.
05/08/08   dw      Fixed two function prototypes.
05/08/08   jfc     Wrap 1x antenna calls with RF_HAS_FEATURE_CDMA1X
05/07/08   ad      Export generic ADC read functionality through RFM API
05/07/08   jfc     Add circular buffer for RFM_DEBUG
                   Update antenna APIs for multimode
04/24/08   dw      Added rfm_get_cgagc_settling_time_for_tune API to support both tune scenarios.
04/23/08   dw      Support both chains in function rfm_get_receive_agc_val
12/07/07   jfc     Add update_pavmode_burst_data() API for GL1
11/14/07   jfc     Export API for getting switchpoints
09/21/07   jfc     Compile rfm_get_rf_dbg_scr_param() for GSM and WCDMA
08/21/07   adk     Fixed compiler warning
08/13/07   jfc     Pass call back handler to rfapi layer when initializing
                    RFCOM_RECEIVER_DIV in rfm_enter_mode()
08/09/07   ad      Added support for reading HS LNA switch point NVs during HS calls.
08/06/07   jfc     Featurized code for EDGE-only builds
07/12/07   jfc/lcl Added lna_range param for set_rx_gain()
07/11/07   ra      update rf_path_state when entering GPS
04/12/07   ad      Added RFCOM_RECEIVER_DIV case to rfm_enter_mode
04/12/07   jfc     Changed api pointer to direct logical layer call
02/27/07   jfc     Removed RFM_DEBUG and cleaned up rfm_ioctl()
02/16/07   jfc     Fixed access to uninitialized rfapi_dev0_ptr
o2/15/07   go      Added stub function rfm_init_bbrx_sleep
02/12/07   jfc     Added channel type parameter to set_tx_general_ctl()
02/05/07   ycl     Merge to 7200 tip.
01/30/07   jfc     Added rfm_get_rf_warmup_time() and rfm_band_supports_rxd()
01/23/07   ra      Added rfm_trigger_gps_bbc_update()
01/17/07   jfc     Replaced T_MSM6280 with RF_HAS_FEATURE_WCDMA
01/11/06   ra      Added RFM_EXIT_GPS
01/09/07   jfc     Fixed compiler warnings
01/09/07   jfc     Changed RF_HAS_CDMA1X to RF_HAS_FEATURE_CDMA1X
01/08/07   jfc     Added RECEIVER_DIV case to rfm_is_sleeping
12/18/06   jfc     Moved rfm_is_sleeping to RFM layer
12/16/06   jfc     Added API for antenna for TRM layer
11/16/06   jfc     Changed rfm_get_band_mask() to take device type
11/15/06   jfc     Removed #ifdef RF_HAS_DM_SUPPORT from around
                    rfm_enable_data_mover_clk_for_cm()
11/07/06   ra      Added support for GPS entry in rfm_enter_mode
10/31/06   ycl     Added functions for rfm_get_mdsp_tx_power_limit() and
                   rfm_get_mdsp_tx_agc().
06/01/06   go      Initial revision.

============================================================================*/
/*============================================================================
                           INCLUDE FILES
===========================================================================*/
#include "ttl_map.h"
#include "comdef.h"
#include "rfm.h"
#include "rfm_internal.h"
#include "rfcom.h"
#include "msg.h"
#include "rfcommon_core.h"
#include "rfcommon_msg.h"
#include "rfcommon_locks.h"
#include "rfcommon_mc.h"  /* common to all technologies */
#include "pm.h" 
#include "rfc_pm.h"
#include "rfc_card.h"
#include "rfcommon_concurrency_manager.h"
#include "rflm_cmn_intf.h"
#ifdef FEATURE_CDMA1X
#include "rfm_cdma.h"
#include "rf_cdma_mc.h"
#include "rf_1x_mc.h"
#include "rf_hdr_mc.h"
#endif
#ifdef FEATURE_TDSCDMA
#include "rf_tdscdma_mc.h"
#include "rfc_card_tdscdma.h"
#endif /*FEATURE_TDSCDMA*/
#include "rfm_types.h"
#include "rfm_helper_incs.h"
#include "rfcommon_locks.h"
#ifdef FEATURE_LTE
#include "rfnv_sv.h"
#endif /*FEATURE_LTE*/
#include "rfc_common.h"
#include "rfcommon_nv_mm.h"
#include "ftm_common_control.h"
#ifdef FEATURE_CDMA1X
#include "rfm_cdma_band_types.h"
#include "rf_cdma_utils_freq.h"
#endif
#include "rf_hal_common.h"
#include "rxlm_intf.h"
#ifdef FEATURE_WCDMA
#include "rfwcdma_core_asdiv.h"
#include "rfwcdma_core_util.h"
#endif
#ifdef FEATURE_LTE
#include "rflte_mc.h"
#endif /*FEATURE_LTE*/
#ifdef FEATURE_GSM
#include "rfgsm_mc.h"
#include "rfgsm_core.h"
#include "rflm_api_gsm.h"
#include "rflm_gsm_dm.h"
#endif

#include "rfdevice_therm_intf.h"
#include "rfdevice_class.h" /* rfdevice_set_cal_state() */
#include "rfc_vreg_mgr_wtr1605_sv.h" 

#ifdef T_RUMI_EMULATION
#include "mcpm_api.h"
#endif 

#include "rfcommon_core_device_manager.h"
#include "ftm_gsm_rfctl.h"
/*----------------------------------------------------------------------------*/
/*! Indicates that enter_mode for a given tech has failed                     */
#define RFM_TECH_ENTER_MODE_FAILED                                   0xFFFFFFFF

/*----------------------------------------------------------------------------*/
/*! Indicates that initiailization routine has failed                         */
#define RFM_COMMON_INIT_FAILED                                       0xFFFFFFFE

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get critical section lock data
*/
rf_lock_data_type*
rfm_get_lock_data
(
  void
);

/*! @cond Doxygen_Suppress */

#ifdef FEATURE_LTE
extern rfcom_lte_band_type rflte_core_get_band_from_rx_uarfcn(rfcom_lte_earfcn_type rx_chan);
extern uint32 rflte_core_get_rx_freq_from_uarfcn(rfcom_lte_earfcn_type rx_chan, rfcom_lte_band_type band);
extern rfcom_lte_band_type rflte_core_get_band_from_tx_uarfcn(rfcom_lte_earfcn_type tx_chan);
extern uint32 rflte_core_get_tx_freq_from_uarfcn(rfcom_lte_earfcn_type tx_chan, rfcom_lte_band_type band);
extern uint8 rflte_mc_fed_get_current_pa_state(void);
#define RFLTE_MC(func, ret_val) ret_val = rflte_mc_##func
/* LTE 1RX mode flag, initialize with FALSE to have default 2RX mode */
boolean rflte_1rx_mode = FALSE;
void rfm_set_1rx_mode(boolean enable_1rx);
#else
#define RFLTE_MC(func, ret_val)
#endif
#ifdef FEATURE_WCDMA
extern uint8 rfwcdma_mdsp_get_pa_range(void);
#endif
#ifdef T_RUMI_EMULATION
extern void rumi_rfc_configure(void);
#endif /* T_RUMI_EMULATION */



/*  NOTE: ----------------------------------------------------------------------
 The MC macro calls like RF1X_MC(), RFWCDMA_MC() should be in one single line,
 eventhough it exceeds the 80 chars limit coding standard. Because if the
 high-level technlogy features like FEATURE_CDMA, FEATURE_WCDMA are not defined,
 then the macro is get replaced by c++ style single line comment (//). So, if
 the macro call is split into two or three lines then only the line with macro
 name is commented-out and rest of the lines will not and which leads to
 compiler errors.
 -----------------------------------------------------------------------------*/

/* GSM fast channel scan status */
boolean rfgsm_fast_ch_scan_status=FALSE;

/*! @endcond */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common initialization occurs in a single step, so start by assuming
  the common init failed.  This item is static to return the previous
  value after RFM has been initialized once. 
*/
static boolean common_init_success = FALSE;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Technology initialization starts assuming all will succeed.  The first
  failure will cause this to become false.  This item is static to return
  the previous value after RFM has been initialized once. 
*/
static boolean technology_init_success = TRUE;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Maintains the information about what transceivers are used by what
  technologies.
*/
static rfcom_mode_enum_type rfm_mode[RFCOM_MAX_DEVICES] = {RFCOM_PARKED_MODE};

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Maintains the information about what transceivers are used by what
  technologies for use in rfgsm_mc scope.
*/
rfcom_mode_enum_type  *rfgsm_mode = &rfm_mode[0];

/*----------------------------------------------------------------------------*/
/*!
  @brief
    RF Driver initialization flag to make sure that the rfm_init() is called
  before calling any other RF Driver's interface functions.
*/
rfm_init_state_enum_type rfm_initialized = RFM_INIT_NOT_RUN;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Maintains the information whether the calibration state is enabled or not.
  This variable will be referenced through a read interface by rest of the RF
  code to enable or disable various RF algorithms based on RF Calibration State.
*/
static boolean rfm_calibration_mode_en = FALSE;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Maintains the information on the Common RF Lock.
 
  @details
  This variable will be referenced through a read interface by RFM Layer
  code to acquire and release the locks. This lock can be used only to protect 
  the Top Level RFM APIs only. This cannot be reused to protect other chunks of 
  code fo critical section protection.
*/ 
static rf_lock_data_type rfm_lock_data;

/*! @cond Doxygen_Suppress */

/* Power Collapse Debug Variable */
boolean rf_disable_powercollapse = FALSE;

extern rfm_bands_bitmask rfc_bc_cfg_bands[RF_PATH_MAX];


/* function prototypes */
int32 rfm_exit_mode (rfcom_device_enum_type device,
                     rfcom_mode_enum_type to_rfmode);

/*!Exposing the rfm init flag to check rfm_init()*/
extern boolean rfm_init_complete;

/*----------------------------------------------------------------------------*/
/*
  @brief
  default rfm function for disabled features.

  @details
*/
void rfm_default_func(rfcom_device_enum_type device, ...)
{
  MSG_FATAL(">>> RFM_DEFAULT_FUNC(device: %d): executing for disabled FEATURE",
            device,0,0);
}

/* Remove this flag while removing T_RUMI_EMULATION. This is purely a runtime 
   flag to switch between that can be used to skip some code at runtime */
#ifdef T_RUMI_EMULATION
static volatile boolean enable_rumi_code = TRUE ;
#endif

/*----------------------------------------------------------------------------*/
/*
  @brief
  Initializes the all the available RF transceivers.

  @details
  Initializes all the radio transceiver devices.  Reads common and mode
  specific RF configuration parameters from NV and initializes common and mode
  specific configuration tables.  Initializes common hardware. RF driver is
  placed in parked/sleep state. It returns a state information of whether rf
  configuration is initialized or not. If initalized without any errors it
  returns non-zero.

  This function must be called before any other RF driver function.

  @param device             : Not used. It is here for backword compatibility.
  @param caller_tcb_ptr     : Calling task's TCB pointer
  @param task_nv_wait_sig   : Task signal to wait for when reading NV
  @param task_wait_func_ptr : Task's wait function to be called when reading NV

  @retval Returns TRUE if initialization is successful, otherwise returns FALSE.
*/
boolean 
rfm_init 
( 
                   rex_tcb_type *caller_tcb_ptr,
                   rex_sigs_type task_nv_wait_sig,
  void (*task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean ret_val;
  rf_hal_bus_client_handle* rf_rfm_hal_bus_client_handle = NULL;
#ifdef T_RUMI_EMULATION
  mcpm_request_parms_type rf_dummy = {0};
#endif 

  #ifdef T_RUMI_EMULATION
  if ( TRUE == enable_rumi_code )
  {
  rumi_rfc_configure();
  }
  /* Enable modem clocks and bus resources */
  MCPM_Config_Modem(MCPM_RF_START_REQ, &rf_dummy);
  #endif /* T_RUMI_EMULATION */

  RFM_DEBUG_EN_Q(&rfm_init, 0, 0, 0, 0);
  
  /*! rflm common init */
  rflm_cmn_init();

  if (rfm_initialized==RFM_INIT_NOT_RUN)
  {
    /* Initialize Critical Section for RFM APIs */
    rf_common_init_critical_section(rfm_get_lock_data());

    /* Initialize Critical Section for RFC*/
    rf_common_init_critical_section(rfc_get_lock_data());


    /* Initialize RFLM items */
    common_init_success = rf_hal_rfi_init();

    /* Initialize the RF HAL for SSBI, RFFE, GRFC, etc. */
    rf_hal_bus_init();

#ifndef T_RUMI_EMULATION
    /* Vote for the RFCMD app for the device init in this function */
    rf_hal_bus_enable(TRUE, &rf_rfm_hal_bus_client_handle, RF_HAL_RFCMD_APP_CLIENT(COMMON));
#endif

    common_init_success &= rfcommon_mc_init(RFCOM_TRANSCEIVER_0,
                                           caller_tcb_ptr, task_nv_wait_sig,
                                           task_wait_func_ptr);

#ifdef FEATURE_CDMA1X
    ret_val = TRUE; /* Needed if RF1X_MC macro is stubbed out */
    RFCDMA_MC(init, ret_val)(common_init_success, caller_tcb_ptr, task_nv_wait_sig,task_wait_func_ptr);
    technology_init_success = technology_init_success && (ret_val == TRUE);
#endif /*FEATURE_CDMA1X*/ 

#ifdef FEATURE_LTE
    if ( rf_lte_debug_flags.rumi_bup_lte == TRUE )
    {
      ret_val = TRUE; /* Needed if RFLTE_MC macro is stubbed out */
      RFLTE_MC(init, ret_val)(RFCOM_TRANSCEIVER_0, common_init_success, caller_tcb_ptr, task_nv_wait_sig,task_wait_func_ptr);
      technology_init_success = technology_init_success && (ret_val == TRUE);
    }
#endif /*FEATURE_LTE*/       

    #ifdef FEATURE_WCDMA
    if(rfm_is_tech_supported(RFCOM_TRANSCEIVER_0,RFCOM_WCDMA_MODE))
    {
    ret_val = TRUE; /* Needed if RFWCDMA_MC macro is stubbed out */
    RFWCDMA_MC(init, ret_val)(RFCOM_TRANSCEIVER_0,common_init_success, caller_tcb_ptr, task_nv_wait_sig,task_wait_func_ptr);
    technology_init_success = technology_init_success && (ret_val == TRUE);
    }    
    #endif 
    
    #ifdef FEATURE_GSM
    /* GSM Bring up*/
    if( rfgsm_debug_flags.rfgsm_init_only== TRUE )
    {
    ret_val = TRUE; /* Needed if RFGSM_MC macro is stubbed out */
       RFGSM_MC(init, ret_val)(common_init_success, caller_tcb_ptr, task_nv_wait_sig,task_wait_func_ptr);
    technology_init_success = technology_init_success && (ret_val == TRUE);
    }
    #endif
    #ifdef FEATURE_TDSCDMA
    if (rfc_tdscdma_is_rfc_tds_created() == TRUE)
    {
      ret_val = TRUE; /* Needed if RF1X_MC macro is stubbed out */
      #ifndef T_WINNT
      RFTDSCDMA_MC(init, ret_val)(common_init_success, caller_tcb_ptr, task_nv_wait_sig,task_wait_func_ptr);
      #endif
      technology_init_success = technology_init_success && (ret_val == TRUE);
    }
    #endif
    if (
#ifdef FEATURE_LTE
        ( rf_lte_debug_flags.rumi_bup_lte == FALSE ) && 
#endif /*FEATURE_LTE*/
        ( rfgsm_debug_flags.rfgsm_init_only == FALSE ) 

#ifdef FEATURE_TDSCDMA
        &&(rf_tdscdma_debug_flags.rumi_bup_tdscdma == FALSE ) 
#endif

#ifdef FEATURE_WCDMA
        &&(rf_wcdma_debug_flags.rumi_bup_wcdma == FALSE )
#endif
        )
    {
        
        #ifdef FEATURE_WCDMA
        if(rfm_is_tech_supported(RFCOM_TRANSCEIVER_0,RFCOM_WCDMA_MODE))
        {
        ret_val = TRUE; /* Needed if RFWCDMA_MC macro is stubbed out */
        RFWCDMA_MC(init, ret_val)(RFCOM_TRANSCEIVER_0,common_init_success, caller_tcb_ptr, task_nv_wait_sig,task_wait_func_ptr);
        technology_init_success = technology_init_success && (ret_val == TRUE);
        }
        #endif
        #ifdef FEATURE_GSM
        ret_val = TRUE; /* Needed if RFGSM_MC macro is stubbed out */
        RFGSM_MC(init, ret_val)(common_init_success, caller_tcb_ptr, task_nv_wait_sig,task_wait_func_ptr);
    technology_init_success = technology_init_success && (ret_val == TRUE);
        #endif 

#ifdef FEATURE_LTE
    ret_val = TRUE; /* Needed if RFLTE_MC macro is stubbed out */
    RFLTE_MC(init, ret_val)(RFCOM_TRANSCEIVER_0, common_init_success, caller_tcb_ptr, task_nv_wait_sig,task_wait_func_ptr);
    technology_init_success = technology_init_success && (ret_val == TRUE);
#endif /*FEATURE_LTE*/

        #ifdef FEATURE_CDMA1X
        ret_val = TRUE; /* Needed if RF1X_MC macro is stubbed out */
        RFCDMA_MC(init, ret_val)(common_init_success, caller_tcb_ptr, task_nv_wait_sig,task_wait_func_ptr);
        technology_init_success = technology_init_success && (ret_val == TRUE);
        #endif 
    #ifdef FEATURE_TDSCDMA
    if (rfc_tdscdma_is_rfc_tds_created() == TRUE)
    {
        ret_val = TRUE; /* Needed if RFTDSCDMA_MC macro is stubbed out */
        #ifndef T_WINNT
        RFTDSCDMA_MC(init, ret_val)(common_init_success, caller_tcb_ptr, task_nv_wait_sig, task_wait_func_ptr);
        #endif
        technology_init_success = technology_init_success && (ret_val == TRUE);
    }
    #endif /*FEATURE_TDSCDMA*/

    } /* if ( rf_cdma_debug_flags.rumi_bup_cdma == FALSE ) && ( rf_lte_debug_flags.rumi_bup_lte == FALSE )
       && ( rf_gsm_debug_flags.rumi_bup_gsm == FALSE ) && ( rf_tdscdma_debug_flags.rumi_bup_tdscdma == FALSE )*/

    ret_val = TRUE; /* Needed if RFGNSS_MC macro is stubbed out */
    RFGNSS_MC(init, ret_val)(RFCOM_TRANSCEIVER_0, common_init_success, caller_tcb_ptr, task_nv_wait_sig,task_wait_func_ptr);
    technology_init_success = technology_init_success && (ret_val == TRUE);

#ifndef T_RUMI_EMULATION
    rfcommon_msm_config_mgpi_block();
#endif 

#ifdef FEATURE_LTE
    /* Retrieve NV for SV module */
    common_init_success &= rfsv_retrieve_nv_items(caller_tcb_ptr, task_nv_wait_sig,
                                                  task_wait_func_ptr);
#endif/*FEATURE_LTE*/

    if ( common_init_success == TRUE && technology_init_success == TRUE )
    {
      rfm_initialized = RFM_INIT_SUCCEEDED;
      rfcommon_mc_config_trm_band_support();
    }
    else
    {
      rfm_initialized = RFM_INIT_FAILED;
    }

#ifndef T_RUMI_EMULATION
    /* Remove vote for RFCMD app */
    rf_hal_bus_enable(FALSE, &rf_rfm_hal_bus_client_handle, RF_HAL_RFCMD_APP_CLIENT(COMMON));
#endif
  } /* if (rfm_initialized==RFM_INIT_NOT_RUN) */

  /* This message will be printed any time rfm_init() is called, so there
     is a chance the user will see it beyond phone boot. */
  if ( (common_init_success == TRUE && technology_init_success == TRUE) == FALSE )
  {
    MSG_3( MSG_SSID_RF, MSG_LVL_ERROR,
           "Failed RF initialization.  Common: %d, Tech: %d, RFM_INIT_STATE: %d",
           common_init_success, technology_init_success, (int8)rfm_initialized );
  }

  TTLBOOTUP(STARTUP_RFMINIT_MS);

  #ifdef T_RUMI_EMULATION
  MCPM_Config_Modem(MCPM_RF_STOP_REQ, &rf_dummy);
  #endif
  return ( (rfm_initialized == RFM_INIT_SUCCEEDED)?TRUE:FALSE );
  }

/*----------------------------------------------------------------------------*/
/*
  @brief
  Terminates all the available RF resources which was initialized during rfm_init().

  @details
  Terminates all the available RF resources which was initialized during rfm_init().
. Free all the allocated memory during rfm_init().

  @retval Returns TRUE if termination is successful, otherwise returns FALSE.
*/
boolean rfm_deinit (void)
{
  boolean ret_val = FALSE;
  #ifdef FEATURE_GSM
  RFGSM_MC(deinit, ret_val)();
  #endif
  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  This API returns the time RF (in micro-secs) needs for the HW to warmup 
  and be ready for Rx

  @details
  The RF warmup time (RF WU) is a conservative estimate of time the RF SW 
  would need to configure the device in a given mode. This time estimate may 
  be used by L1 to decide when L1 needs to wakeup, in order to ensure that 
  the SW and HW is ready for demod.
  The device does not have to be configured to "rf_mode" before calling this
  API.
  
  @param device :  transceiver device to use for the requested mode

  @param rf_mode:  mode or technology for which the API is called

  @return
  RF WU time on success and -1 on failure

*/

uint32 
rfm_get_rx_warmup_time 
( 
  rfm_device_enum_type device, 
  rfm_mode_enum_type   rf_mode
)
{
  uint32 rx_warmup_time = 0;

  switch(rf_mode)
  {
  #ifdef FEATURE_CDMA1X
  case RFCOM_1X_MODE:
  case RFCOM_1XEVDO_MODE:
    rx_warmup_time = rf_cdma_get_warmup_time(device);
    break;
  #endif 
  default:
    RF_MSG_1(RF_ERROR, "Invalid mode = %d", rf_mode );
    rx_warmup_time = -1; 
    break;
  } /* switch */

  return rx_warmup_time;
} /* rfm_get_rx_wakeup_time */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Reports the largest bandwidth supported on a device in a given RF mode.

  @param device
  The RF device to be queried.

  @param rf_mode
  The technology mode for which the bandwidth should be reported.

  @return
  Maximum RF bandwidth in Hz.
*/
rfm_band_width_type
rfm_get_supported_bw
(
  const rfm_device_enum_type device,
  const rfm_mode_enum_type rf_mode
)
{
  rfm_band_width_type bandwidth;

  switch(rf_mode)
  {
  case RFM_1X_MODE:
    bandwidth = 1228800;
    break;

  case RFM_1XEVDO_MODE:
    bandwidth = 5*1250000;
    break;

  default:
    bandwidth = 0;
    break;
  }

  return bandwidth;
} /* rfm_get_supported_bw() */

/*----------------------------------------------------------------------------*/
/*
  @brief
  Initializes the RF Driver for the specified radio transceiver with the
  specified mode or technology.

  @details
  Initializes RF hardware for mode of operation specified by parameter
  rf_mode (WCDMA, GSM, CDMA, HDR, GPS, PARKED/SLEEP, etc.).  Before
  entering the requested rf mode, it checks whether the transceiver was
  used by any other technology, if it was used by other technology, it
  will call the rfm_exit_mode() for the previous user of the tranceiver to do
  clean-up if there is any.

  @param device Transceiver device to use for the requested mode.
  @param rf_mode mode or technology for which the tranceiver to be configured.
  @param user_data_ptr Pointer to user data passed into callback. (not used)
  @param cb_handler Call back handler (not used)

  @retval On success, returns total delay in microseconds this function will 
  wait for RF hardware to settle. A wait value of RFM_TECH_ENTER_MODE_FAILED
  or RFM_COMMON_INIT_FAILED is returned on failure (depending on failure 
  condition)

  @prev
  1. rfm_init() must have already been called.
  2. MDSP must be downloaded and initialized for mode of operation specified
     by mode before calling this function.
*/
uint32 rfm_enter_mode ( const rfcom_device_enum_type device,
                        const rfcom_mode_enum_type rf_mode,
                        void * const user_data_ptr,
                        rfm_cb_handler_type cb_handler
                        ,uint32 rxlm_buf_idx
                        )
{
  uint32 wait_delay_ms = RFM_TECH_ENTER_MODE_FAILED; /* delay to wait */
  boolean result = FALSE;
    
  RFM_DEBUG_EN_Q(&rfm_enter_mode, device, rf_mode, &cb_handler,rxlm_buf_idx);

  MSG_HIGH("rfm_enter_mode(): device=%d, mode=%d", device, rf_mode, 0 );

  if ( (device < RFM_DEVICE_0) || (device >= RFM_MAX_DEVICES) ) 
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "device out of bounds %d", device);
    return wait_delay_ms;
  }

  #ifndef FEATURE_WCDMA_RUMI_SUPPORT
  /* Bypass init success check for RUMI */
  if ( common_init_success == FALSE || technology_init_success == FALSE )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,
        "rfm_enter_mode failed due to unsuccessful rfm_init");
    wait_delay_ms = RFM_COMMON_INIT_FAILED;
    return wait_delay_ms;
  }
  #endif

  if (rfm_mode[device] != rf_mode)
  {
    /* exit the previous mode before entering the requested mode */
    (void)rfm_exit_mode(device, rf_mode);

    /* For 1x and HDR, check to make sure that rfm_exit_mode() succeeded - this
       can be done by looking at rfm_mode[device] ==> which is expected to be
       in PARKED_MODE
    
       This check will need to be done by other techs as well 
    
    */
    if ( (rfm_mode[device] != RFM_PARKED_MODE) &&
         ( (rf_mode == RFCOM_1X_MODE ) || (rf_mode == RFCOM_1XEVDO_MODE) ) )
    {
      MSG_2( MSG_SSID_RF, MSG_LEGACY_ERROR, 
             "rfm_enter_mode: device=%d NOT in parked mode, but in mode=%d", 
             device,  rfm_mode[device]);
    
      wait_delay_ms = RFM_COMMON_INIT_FAILED;
      return wait_delay_ms;
    }

  } /* if (rfm_mode[device] != rf_mode) */

  /* All checks passed - safe to enter the requested mode */
  switch(rf_mode)
  {
  #ifdef FEATURE_CDMA1X
  case RFCOM_1X_MODE:
    RF1X_MC(enter_mode, result)(device, user_data_ptr, (rfm_cb_handler_type) cb_handler );
    break;

  case RFCOM_1XEVDO_MODE:
    RFHDR_MC(enter_mode, result)(device, user_data_ptr, (rfm_cb_handler_type) cb_handler );
    break;
  #endif

  #ifdef FEATURE_WCDMA
  case RFCOM_WCDMA_MODE:
    RFWCDMA_MC(enter_mode, result)(device, user_data_ptr, (rfm_cb_handler_type) cb_handler, rxlm_buf_idx);
    break;
  #endif

  #ifdef FEATURE_GSM
  case RFCOM_GSM_MODE:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Failed rfgsm_mc_enter_mode - should not be called via rfm %d", rf_mode);
    break;
  #endif

  case RFCOM_GPS_MODE:
  case RFCOM_GNSS_MODE:
    RFGNSS_MC(enter_mode, result)(device, user_data_ptr, (rfm_cb_handler_type) cb_handler);
    break;

  case RFCOM_LTE_MODE:
    #ifdef FEATURE_LTE
    if(user_data_ptr != NULL)
    {   
      result = FALSE; /* important when RFLTE_MC macro is stubbed out */
      RFLTE_MC(enter_mode, result)(*((rflte_mc_carrier_info_type *)user_data_ptr));

    if (result == 0) /* for LTE, 0 represents a success */
    {
       result = TRUE;
         rfm_mode[device] = RFCOM_LTE_MODE;
      }
    }
    else
    {
      result = FALSE;
    }
    #endif  /* FEATURE_LTE */
    break;
    
  case RFCOM_TDSCDMA_MODE:      
    result = TRUE;
    break;

  default:
    MSG_FATAL("Invalid Mode for Device: (device= %d, mode = %d)", device, rfm_mode[device], NULL);
    break;
  }

  /* update rfm_mode[] only if the tech-specific enter_mode succeeded */
  if(result == TRUE)
  {
    wait_delay_ms = 0; /* success - no need to wait */
    rfm_mode[device] = rf_mode;
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Failed rfm_enter_mode to mode %d", 
          rf_mode);
  }

  return wait_delay_ms;

} /* rfm_enter_mode */


/*----------------------------------------------------------------------------*/
/*
  @brief
  Clean-up the transceiver settings so that next requesting mode can use it.

  @details
  Exit the current operating mode. Put the RF hardware into PARKED/SLEEP mode.

  @param device : tranceiver to exit.
  @param mode : Mode or technology which will be using the transceiver next.

  @prev
  rfm_init() must have already been called.

  @retval negative number if error.
*/

int32 rfm_exit_mode (rfcom_device_enum_type device,
                     rfcom_mode_enum_type to_rfmode)
{
  int32 ret_val = 0;

  RFM_DEBUG_EN_Q(&rfm_exit_mode, device, to_rfmode, 0, 0);

  if((device < RFM_DEVICE_0) || (device >= RFM_DEVICE_4))
  {
    RF_MSG_1(RF_HIGH, "rfm_exit_mode: Indalid device %d", device);
    return 0;
  }

  MSG_HIGH("rfm_exit_mode(): device=%d, mode=%d", device, rfm_mode[device],NULL);

  if (rfm_mode[device] == to_rfmode)
  {
    return ret_val; /* already in the requested mode */
  }

  /* exit the current operating mode */
  switch(rfm_mode[device])
  {
  #ifdef FEATURE_CDMA1X
  case RFCOM_1X_MODE:
    RF1X_MC(exit_mode, ret_val)(device, to_rfmode);
    break;

  case RFCOM_1XEVDO_MODE:
    RFHDR_MC(exit_mode, ret_val)(device, to_rfmode);
    break;
  #endif /* FEATURE_CDMA1X */
  #ifdef FEATURE_WCDMA
  case RFCOM_WCDMA_MODE:
    RFWCDMA_MC(exit_mode, ret_val)(device, to_rfmode);
    break;
  #endif
  #ifdef FEATURE_GSM
  case RFCOM_GSM_MODE:
    /* GSM exit mode must be called expliciting before calling rfm_enter_mode */
    break;
#endif

  case RFCOM_GNSS_MODE:
    RFGNSS_MC(exit_mode, ret_val)(device, to_rfmode);
    break;

  #ifdef FEATURE_LTE
  case RFCOM_LTE_MODE:
    /* LTE exit mode must be called expliciting before calling rfm_enter_mode */
    break;
#endif

  case RFCOM_PARKED_MODE:
    break;

  default:
    MSG_FATAL( "Invalid Mode for Device: (device= %d, mode = %d)", device, 
               rfm_mode[device], NULL);
    break;
  } /* switch(rfm_mode[device]) */

  
  /* check if exit_mode() succeeded - update rfm_mode[] only on success */  
  switch(rfm_mode[device])
  {
    #ifdef FEATURE_CDMA1X
    /* check for ret_val for 1x and HDR - update only on success */
    case RFCOM_1X_MODE:
    case RFCOM_1XEVDO_MODE:
      if (ret_val == 1) /* indicates success */
      {
        rfm_mode[device] = RFCOM_PARKED_MODE;
      }
      else
      {
        RF_MSG_2( RF_ERROR, "rfm_exit_mode: Failed for device %d, while "
                            "exiting mode %d", device, rfm_mode[device]);
  }
      break;
    #endif /* FEATURE_CDMA1X */
    /* other techs will need to have the above as well */
    default:
  rfm_mode[device] = RFCOM_PARKED_MODE;
       break;

  } /* switch(rfm_mode[device]) */

  return ret_val;

} /* rfm_exit_mode */

/*----------------------------------------------------------------------------*/
/*
  @brief
  Tunes the UHF Rx/Tx synthesizers to the channel specified.

  @details
  Tunes the UHF Rx/Tx synthesizers to the channel specified.  The channel
  is interpreted based on operating mode.  The input parameter synths
  specifies whether transmit, receive, or both synthesizers should be
  tuned to the specified channel.

  @param device : transceiver to be tuned.
  @param channel : Mode dependent channel
  @param synths : ynthesizor(s) to tune
  @param user_data_ptr : Pointer to user data passed into callback
  @param cb_handler : Call back handler

  @retval Returns total delay in microseconds this function will wait for
  synthesizers to lock.

  @pre
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rfm_tune_to_chan ( rfcom_device_enum_type device, uint16 channel,
                          rfm_synth_tune_enum_type synths,
                          const void *user_data_ptr,
                          rfm_cb_handler_type cb_handler)
{
  uint32 ret_val = 0;

  RFM_DEBUG_EN_Q(&rfm_tune_to_chan, device, channel, synths, &user_data_ptr);

  switch(rfm_mode[device])
  {
  case RFCOM_GSM_MODE:
    #ifdef FEATURE_GSM
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Failed rfgsm_mc_tune_to_chan - should not be called via rfm %d", rfm_mode[device]);
    #endif
   
    break;

  default:
    MSG_FATAL("Invalid Mode for Device: (device= %d, mode = %d)", device, rfm_mode[device], NULL);
    break;
  }

  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  Enables transmitter for the mode of operation RF driver is currently
  operating in.

  @details
  Enables transmitter for mode of operation RF driver is currently
  operating in, typically preparing transmitter for access attempts.
  Turns on LDOs for transmit hardware.  Turns on PA.  Places RF driver in
  Rx/Tx state of operation.

  @param device : transceiver to be turned on for tx.
  @param user_data_ptr : Pointer to user data passed into callback (not used)
  @param cb_handler : Call back handler (not used)

  @ret_val Returns total delay in microseconds this function will wait for RF
  hardware to settle.

  @pre
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.
  rfm_tune_to_chan() should have already been called.
*/

uint32 rfm_enable_tx ( rfcom_device_enum_type device,
                       const void *user_data_ptr,rfm_cb_handler_type cb_handler
                       ,uint32 txlm_buf_idx
                       )
{
  int32 ret_val = 0;

  RFM_DEBUG_EN_Q(&rfm_enable_tx, device, 0, 0, 0);

  if((device < RFM_DEVICE_0) || (device >= RFM_DEVICE_4))
  {
    RF_MSG_1(RF_HIGH, "rfm_enable_tx: Indalid device %d", device);
    return 0;
  }

  switch(rfm_mode[device])
  {
  case RFCOM_WCDMA_MODE:

    RFGNSS_MC(wwan_tx_status, ret_val)(device, TRUE);
    #ifdef FEATURE_WCDMA
    RFWCDMA_MC(enable_tx, ret_val)(device, user_data_ptr,(rfm_cb_handler_type) cb_handler);
    #endif
    break;
  case RFCOM_GSM_MODE:
    #ifdef FEATURE_GSM
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Failed rfgsm_mc_enable_tx - should not be called via rfm %d", rfm_mode[device]);
    #endif
    break;

  default:
    MSG_FATAL("Invalid Mode for Device: (device= %d, mode = %d)", device, rfm_mode[device], NULL);
    break;
  }

  return(ret_val);
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  Disables transmitter for the mode of operation RF driver is currently
  operating in.

  @details
  Disables transmitter for mode of operation RF driver is currently
  operating in.  Turns off LDOs for transmit hardware.  Turns off PA.
  Places RF driver in Rx state of operation.

  @param device : transceiver to be turned off for tx.

  @pre
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.
  rfm_tune_to_chan() should have already been called.
  rfm_enable_tx() should have already been called.
*/
void rfm_disable_tx (rfcom_device_enum_type device
                     ,uint32 txlm_buf_idx
                    )
{
  int32 ret_val = 0;

  RFM_DEBUG_EN_Q(&rfm_disable_tx, device, 0, 0, 0);

  if((device < RFM_DEVICE_0) || (device >= RFM_DEVICE_4))
  {
    RF_MSG_1(RF_HIGH, "rfm_disable_tx: Indalid device %d", device);
    return;
  }

  switch(rfm_mode[device])
  {
  case RFCOM_WCDMA_MODE:
    #ifdef FEATURE_WCDMA
    RFWCDMA_MC(disable_tx, ret_val)( device);
    #endif
    RFGNSS_MC(wwan_tx_status, ret_val)(device, FALSE); 
    break;
  case RFCOM_GSM_MODE:
    #ifdef FEATURE_GSM
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Failed rfgsm_mc_disable_tx - should not be called via rfm %d", rfm_mode[device]);
    #endif
    RFGNSS_MC(wwan_tx_status, ret_val)(device, FALSE); 
    break;

  default:
    MSG_FATAL("Invalid Mode for Device: (device= %d, mode = %d)", device, rfm_mode[device], NULL);
    break;
  }
  if (ret_val != 1)
  {
    MSG_ERROR("Failure in rfm_disable_tx: (device= %d, mode = %d)", device, rfm_mode[device], NULL);
  }
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  Enables/disabled Quiet mode when initiated from Layer 1
 
  @details
  Should call rfdevice level function that will enable or disable Quiet mode

  @param device : 

  @pre
*/
void rfm_set_quiet_mode (rfm_mode_enum_type mode, boolean enable)
{
   #if defined(FEATURE_LTE)||defined(FEATURE_WCDMA)
   int32 ret_val = 0;
   #endif
                                    
   switch(mode)
   {
     case RFM_LTE_MODE:
       #ifdef FEATURE_LTE
         RFLTE_MC(cmd_dispatch, ret_val)(RFM_DEVICE_0, RFLTE_MC_CMD_SET_QUIET_MODE, &enable);
       #endif /* FEATURE_LTE */
       break;

     case RFCOM_WCDMA_MODE:
       #ifdef FEATURE_WCDMA
         RFWCDMA_MC(cmd_dispatch, ret_val)(RFM_DEVICE_0, RFWCDMA_MC_SET_QUIET_MODE, &enable);
       #endif
       break;

     default:
       /* For all other techs, Quiet mode is initiated by RF SW*/
       MSG_ERROR(" Incorrect technology calling RFM Quiet mode function tech=%d !!",mode,0,0);
       break;
   }

   #if defined(FEATURE_LTE)||defined(FEATURE_WCDMA)
   if (ret_val == FALSE)
   {
     RF_MSG_1( RF_LOW, "rfm_set_quiet_mode: status = %d", ret_val);
   }   
   #endif
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  Queries whether the RF is sleeping right now.

  @details
  Queries whether the RF is in sleeping for the specified transceiver.

  @param device : transceiver to check for sleeting.

  @pre
  rfm_init() must have already been called.
*/
boolean rfm_is_sleeping( rfcom_device_enum_type device )
{
  int32 is_sleeping = TRUE;

  switch(rfm_mode[device])
  {
  case RFCOM_WCDMA_MODE:
    #ifdef FEATURE_WCDMA
    RFWCDMA_MC(cmd_dispatch, is_sleeping)(device, RFWCDMA_MC_IS_SLEEPING, NULL);
    #endif
    break;

  case RFCOM_GSM_MODE:
    #ifdef FEATURE_GSM
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Failed rfgsm_mc_cmd_dispatch is sleeping - should not be called via rfm %d", rfm_mode[device]);
    #endif
    break;

  default:
    RF_MSG_2( RF_ERROR, "rfm_is_sleeping: Invalid Mode %d on Device %d)",
              rfm_mode[device], device );
    break;
  }

  return (boolean)is_sleeping;
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  Places the majority of RF hardware in sleep mode for the currently
  enabled mode of operation.

  @details
  Places the majority of RF hardware in sleep mode for the currently
  enabled mode of operation and begins transition of RF driver to sleep
  state.  Actual state of hardware after calling this function is
  operating mode dependent and may require a call to rfm_stay_asleep()
  later in the sleep timeline to complete the shutdown of all RF hardware
  for sleep.  This is the first stage of the transition to sleep state.

  @param device : transceiver to put into sleep mode.
  @param user_data_ptr : Pointer to user data passed into callback (not used)
  @param cb_handler : Call back handler (not used)

  @ret_val Returns total delay in microseconds this function will wait for RF
  hardware to settle.

  @pre
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.
  rfm_tune_to_chan() should have already been called.
*/
uint32 rfm_sleep( rfcom_device_enum_type device, const void *user_data_ptr,
                  rfm_cb_handler_type cb_handler
                  ,uint32 rxlm_buf_idx
                 )
{
  int32 ret_val = 0;

  RFM_DEBUG_EN_Q(&rfm_sleep, device, 0, 0, 0);

  if((device < RFM_DEVICE_0) || (device >= RFM_DEVICE_4))
  {
    RF_MSG_1(RF_HIGH, "rfm_sleep: Indalid device %d", device);
    return 0;
  }

  switch(rfm_mode[device])
  {
  case RFCOM_WCDMA_MODE:
    #ifdef FEATURE_WCDMA
    RF_MSG_1(RF_ERROR, "Failed rfwcdma_mc_sleep: should not be called via rfm %d", rfm_mode[device]);
    #endif
    break;

  case RFCOM_GSM_MODE:
    #ifdef FEATURE_GSM
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Failed rfgsm_mc_sleep - should not be called via rfm %d", rfm_mode[device]);
    #endif
    break;

  default:
    MSG_FATAL("Invalid Mode for Device: (device= %d, mode = %d)", device, rfm_mode[device], NULL);
    break;
  }

  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  Wakes up RF hardware from sleep mode.

  @details
  Wakes up RF hardware from sleep mode to the mode of operation which was in
  operation before going into sleep mode. Places RF driver in Rx state.

  @param device : transceiver to wakeup from sleep mode.
  @param mode : mode in which to wakeup. (not used)
  @param user_data_ptr : Pointer to user data passed into callback (not used)
  @param cb_handler : Call back handler (not used)

  @ret_val Returns total delay in microseconds this function will wait for RF
  hardware to settle.

  @pre
  rfm_sleep() must have already been called and rfm_stay_asleep()
  may have been called.
*/
uint32 rfm_wakeup ( rfcom_device_enum_type device, rfm_mode_enum_type rf_mode,
                    const void *user_data_ptr, rfm_cb_handler_type cb_handler
                    ,uint32 rxlm_buf_idx
                  )
{
  int32 ret_val = 0;

  RFM_DEBUG_EN_Q(&rfm_wakeup, device, rf_mode, 0, 0);
  
  if((device < RFM_DEVICE_0) || (device >= RFM_DEVICE_4))
  {
    RF_MSG_1(RF_HIGH, "rfm_wakeup: Indalid device %d", device);
    return 0;
  }

  switch(rfm_mode[device])
  {
  case RFCOM_WCDMA_MODE:
    #ifdef FEATURE_WCDMA
    RF_MSG_1(RF_ERROR, "Failed rfwcdma_mc_wakeup: should not be called via rfm %d", rfm_mode[device]);
    #endif
    break;

  case RFCOM_GSM_MODE:
    #ifdef FEATURE_GSM
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Failed rfgsm_mc_wakeup - should not be called via rfm %d", rfm_mode[device]);
    #endif
    break;

  default:
    MSG_FATAL("Invalid Mode for Device: (device= %d, mode = %d)", device, rfm_mode[device], NULL);
    break;
  }

  return(ret_val);
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  Enables a receiver chain for the mode of operation.

  @details
  Enables a receiver chain for the mode of operation, which was specified by
  calling the rfm_enter_mode(). Turns on LDOs for transmit hardware.  Turns on
  PA. Places receiver chain in Rx state of operation.

  Calling module must wait for the delay msec before assuming
  RF enabled. This operation will not tune the RF chain.

  @param device : receiver chain to be enabled.
  @param user_data_ptr : Pointer to user data passed into callback (not used)
  @param cb_handler : Call back handler (not used)

  @ret_val Returns total delay in microseconds this function will wait for RF
  hardware to settle.

  @pre
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.

*/
uint32 rfm_enable_rx ( rfcom_device_enum_type device, const void *user_data_ptr,
                       rfm_cb_handler_type cb_handler )
{
  int32 ret_val = 0;

  RFM_DEBUG_EN_Q(&rfm_enable_rx, device, 0, 0, 0);

  switch(rfm_mode[device])
  {
  case RFCOM_WCDMA_MODE:
    #ifdef FEATURE_WCDMA
    RFWCDMA_MC(enable_rx, ret_val)(  device, user_data_ptr,(rfm_cb_handler_type) cb_handler );
    #endif
    break;

  case RFCOM_GSM_MODE:
    #ifdef FEATURE_GSM
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Failed rfgsm_mc_enable_rx - should not be called via rfm %d", rfm_mode[device]);
    #endif
    break;

  default:
      MSG_FATAL("Invalid Mode for Device: (device= %d, mode = %d)", device, rfm_mode[device], NULL);
    break;
  }

    return ret_val;
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  Disables a receiver chain.

  @details
  Disables a receiver chain. Performs all the necessary activities to minimize
  current drawn by the RF chain which has been disabled.

  @param device :  receiver to be disabled.

  @pre
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.
  rfm_tune_to_chan() should have already been called.
*/
void rfm_disable_rx (rfcom_device_enum_type device)
{
  int32 ret_val = 0;

  RFM_DEBUG_EN_Q(&rfm_disable_rx, device, 0, 0, 0);
  switch(rfm_mode[device])
  {
  case RFCOM_WCDMA_MODE:
    #ifdef FEATURE_WCDMA
    RFWCDMA_MC(disable_rx, ret_val)( device );
    #endif
    break;

  case RFCOM_GSM_MODE:
    #ifdef FEATURE_GSM
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Failed rfgsm_mc_disable_rx - should not be called via rfm %d", rfm_mode[device]);
    #endif
    break;

  default:
    MSG_FATAL("Invalid Mode for Device: (device= %d, mode = %d)", device, rfm_mode[device], NULL);
    break;
  }
  if (ret_val != 1)
  {
    MSG_ERROR("Failure in rfm_disable_rx: (device= %d, mode = %d)", device, rfm_mode[device], NULL);
  }
}
/*----------------------------------------------------------------------------*/
/*
  @brief
  Returns a bitmask of the WWAN bands that are supported on the RF
  logical device.

  @details
  This interface is only valid for 1x, HDR, WCDMA, and GSM WWAN technologies.
  The LTE band mask exists in a different structure.

  @param device
  The RF logical device 

  @return
  A bitmask with the bands that are supported for this RFM logical device.

  @pre
  rfm_init() must have already been called.
*/
uint64 rfm_get_band_mask ( rfm_device_enum_type device )
{
  RFM_DEBUG_EN_Q(&rfm_get_band_mask, device, 0, 0, 0);

  switch (device)
{
  case RFM_DEVICE_0 :
    return rfcommon_nv_get_rf_bc_config(RF_PATH_0);

  case RFM_DEVICE_1 :
    return rfcommon_nv_get_rf_bc_config(RF_PATH_1);

  case RFM_DEVICE_2 :
    return rfcommon_nv_get_rf_bc_config(RF_PATH_2);

  case RFM_DEVICE_3 :
    return rfcommon_nv_get_rf_bc_config(RF_PATH_3);

  case RFM_DEVICE_4 :
    /* Internal WWAN NV path 3 is mapped to RFM device 4 */
    return rfcommon_nv_get_rf_bc_config(RF_PATH_3);

  default :
    /* Any other device request, return an empty mask */
    return 0;
  } /* switch (device) */
} /* rfm_get_band_mask() */

/*----------------------------------------------------------------------------*/
/*
  @brief
  Returns enabled/disabled state of bands set in NV item, NV_LTE_BC_CONFIG_I.

  @details
  Returns enabled/disabled state of bands set in NV item, NV_LTE_BC_CONFIG_I.

  @param device :  transceiver for which the band mask to be returned..

  @retval band mask value.

  @pre
  rfm_init() must have already been called.
*/
#ifdef LTE_BAND_NUM 
sys_lte_band_mask_e_type rfm_get_lte_band_mask ( void )
{
  uint64 band_mask = 0;
  sys_lte_band_mask_e_type rfm_lte_band_mask;

  RFM_DEBUG_EN_Q(&rfm_get_lte_band_mask, 0, 0, 0, 0);

  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&rfm_lte_band_mask);

  /* Retrieve lte bc config mask from nv */ 
  band_mask = rfcommon_mc_get_lte_band_mask();

  rfm_lte_band_mask = rfm_get_sys_lte_band_mask_from_rflte_band_mask(band_mask);

  return (rfm_lte_band_mask); 

} /* end of rfm_lte_get_band_mask for 256 band support */
#else
uint64 rfm_get_lte_band_mask ( void )
{
  RFM_DEBUG_EN_Q(&rfm_get_lte_band_mask, 0, 0, 0, 0);

  return (rfcommon_mc_get_lte_band_mask()); 

} /* end of rfm_get_lte_band_mask - legacy design */
#endif 

/*----------------------------------------------------------------------------*/
/*
  @brief
  Returns enabled/disabled mask of bands set in input param.

  @details
  Returns enabled/disabled state of bands set in input param.

  @input uint64 rfm_mask
   A band mask that conforms to the logics used in NV item, NV_LTE_BC_CONFIG_I

  @retval band mask value.

  @pre
  rfm_init() must have already been called.
*/
#ifdef LTE_BAND_NUM 
sys_lte_band_mask_e_type rfm_get_sys_lte_band_mask_from_rflte_band_mask ( uint64 rflte_band_mask )
{
  uint8  loop_idx  = 0;
  sys_lte_band_mask_e_type rfm_lte_band_mask; 
  uint8 offset, b66_offset;

  if (256 == LTE_BAND_NUM)
  {
    offset = 191; 
    b66_offset = 14;
  }
  else
  {
    offset = 0; 
    b66_offset = 0;
  }

  RFM_DEBUG_EN_Q(&rfm_get_sys_lte_band_mask_from_rflte_band_mask, 0, 0, 0, 0);

  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&rfm_lte_band_mask); 

  if ( rflte_band_mask > 0 )
  {

  /* Loop to set supported LTE bands 1 to 60 */ 
  for (loop_idx = 0; loop_idx < 50; loop_idx++)
  {
      if ((rflte_band_mask >> loop_idx) & 1)
    {
      SYS_LTE_BAND_MASK_ADD_BAND(&rfm_lte_band_mask, loop_idx); 
    }
  } /* end of loop for idx 0 to 59 */

  for (loop_idx = 50; loop_idx < 60; loop_idx++)
  {
      if ((rflte_band_mask >> loop_idx) & 1)
    {
      SYS_LTE_BAND_MASK_ADD_BAND(&rfm_lte_band_mask, loop_idx + b66_offset); 
    }
  } /* end of loop for idx 0 to 59 */

  /* Loop to set supported LTE-U bands (252, 253, 254, 255) */ 
  for (loop_idx = 60; loop_idx < 64; loop_idx++)
  {
      if ((rflte_band_mask >> loop_idx) & 1)
    {
      SYS_LTE_BAND_MASK_ADD_BAND(&rfm_lte_band_mask, loop_idx + offset); 
    }
  } /* end of loop for idx 60 to 63 */
  }

  return (rfm_lte_band_mask); 

} /* end of rfm_lte_get_band_mask for 256 band support */
#endif 

/*----------------------------------------------------------------------------*/
/*
  @brief
  Returns enabled/disabled state of bands set in TDS BC CONFIG NV item.

  @details
  
  @param device:  transceiver for which the band mask to be returned.

  @retval band mask value.

  @pre
  rfm_init() must have already been called.
*/
uint64 rfm_get_tds_band_mask ( rfcom_device_enum_type device )
{
  uint64 band_mask = 0;

  RFM_DEBUG_EN_Q(&rfm_get_tds_band_mask, device, 0, 0, 0);

  band_mask = rfcommon_mc_get_tds_band_mask(device);
  return (band_mask);
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  Generic IOCTL operation for the specific mode of operation.

  @details
  Generic IOCTL operation for the specific mode of operation.

  @param device :  trasceiver device for which IOCTL operation to be performed.
  @param request :  IOCTL operation to be performed.
  @param *pBuf :  pointer to a buffer which is required by the IOCTL operation.
  @param length :  if the pBuf is an array, specifies the array length.

  @retval IOCTL operation error code if there is an error, otherwise
  RFCOM_IOCTL_NO_ERROR.

  @pre
  rfm_init() must have already been called.
*/
rfcom_ioctl_return_type rfm_ioctl (rfcom_device_enum_type device,
                                   rfcom_ioctl_param_type request,
                                   void *pbuf, int32 length)
{
  int32 ret_val = 0;

  switch(rfm_mode[device])
  {
  case RFCOM_WCDMA_MODE:
    #ifdef FEATURE_WCDMA
    RFWCDMA_MC(ioctl, ret_val)( device, request, pbuf, length);
    #endif
    break;

  default:
    MSG_FATAL("Invalid Mode for Device: (device= %d, mode = %d)", device, rfm_mode[device], NULL);
    break;
  }

  return((rfcom_ioctl_return_type) ret_val);
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  Returns the number of rf antennas supported by the specified device

  @details
  Returns the number of rf antennas supported by the specified device

  @param device :  trasceiver device to get the number of supported
  antennas value.

  @retval number of antennas supported by the device.

  @pre
  rfm_init() must have already been called.
*/
uint8
rfm_get_num_antenna
(
  void
)
{
  uint8 ret_val = 0;
  RFCOMMON_MC(cmd_dispatch, ret_val)((rfcom_device_enum_type) 0, RFCOMMON_MC_GET_NUM_ANTENNA, 0);

  return(ret_val);
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  Compares the two specified antennas sensitivity.

  @details
  Compares the two specified antennas sensitivity information and returns
  whether any one of antenna senstivity is better or same.

  @param device :  trasceiver device
  @param band : band for which sensityvity to be measured and compared.
  @param chan : channel for which sensityvity to be measured and compared.
  @param ant_ref : reference antenna to be compared against.
  @param ant_x : antenna for which sensitivity to be measured and compared.

  @retval
  RF_ANT_QLTY_UNKNOWN : if there is problem in measuring sensitivity.
  RF_ANT_QLTY_IDENTICAL : if the sensitivity is same between two antennas.
  RF_ANT_QLTY_BETTER : if the sensitivity is better than the reference antenna.
  RF_ANT_QLTY_WORSE : if the sensitivity is worst than the reference antenna.

  @pre
  rfm_init() must have already been called.
*/
rfm_antenna_diff_type
rfm_get_antenna_diff
(
  const rfm_device_enum_type device,
  const rfm_antenna_enum_type ant1,
  const rfm_antenna_enum_type ant2,
  const sys_channel_type band_chan
)
{
    rf_antenna_diff_type ret_val = RF_ANT_QLTY_UNKNOWN;
    rfcommon_antenna_diff_type antenna_diff;

    #if defined(FEATURE_CDMA1X) && defined(FEATURE_HDR)
    antenna_diff.chan = band_chan.chan_num;
    antenna_diff.band = band_chan.band;
    antenna_diff.ant_x = ant1;
    antenna_diff.ant_ref = ant2;

    #endif
    RFCOMMON_MC(cmd_dispatch, ret_val)(device, RFCOMMON_MC_GET_ANTENNA_DIFF,
                                       &antenna_diff);

    return((rfm_antenna_diff_type)ret_val);

}

/*----------------------------------------------------------------------------*/
/*
  @brief
  Reads the latest ADC value being used by HKADC channels maintained by RF
  driver.

  @details
  Reads the latest ADC value being used by HKADC channels maintained by RF
  driver.

  @param adc_addr : logical channel id of the adc.
  @param *adc_val : buffer in which read adc value to be returned.

  @retval Read error type rfcom_adc_err_enum_type
*/
rfcom_adc_err_enum_type rfm_adc_read(rfcom_adc_logical_channel_type adc_addr,
                                     uint16 *adc_val)
{
  int32 ret_val = RFCOM_ADC_ERR_FLAG_DEV_UNSUPPORTED;
  #ifdef FEATURE_CDMA1X
  adc_read_type adc_read;
  adc_read.adc_addr = adc_addr;
  adc_read.adc_val = adc_val;

  RFCOMMON_MC(cmd_dispatch, ret_val)(RFCOM_TRANSCEIVER_0, RFCOMMON_MC_ADC_READ,
                                     &adc_read);
  #endif /* FEATURE_CDMA1X */
  return((rfcom_adc_err_enum_type)ret_val);
}

/*----------------------------------------------------------------------------*/
/*
  @brief

  @details

*/
void rfm_adjust_tx_power_limit(int16 limit_adj_db12)
{

}


/*----------------------------------------------------------------------------*/
/*
  @brief
  Power on RTR and enable gp_clk.

  @details
  Powers on VREGs for RTR, waits for it to settle and do a RX reset, and 
  then enable gp_clk.

  @param

  @pre
  NOTE: This function is deprecated in 2.2 and above PLs

*/
void rfm_power_clock_on(void)
{
  switch(rfm_mode[RFCOM_TRANSCEIVER_0])
  {
    case RFCOM_1X_MODE:
    case RFCOM_1XEVDO_MODE:
      //1x has their own code to manage power rail
      break;

    case RFCOM_WCDMA_MODE:
      #ifdef FEATURE_WCDMA
      RFWCDMA_MC_V(power_clock_on)(RFCOM_TRANSCEIVER_0);
      #endif
      break;

    case RFCOM_GSM_MODE:
      #ifdef FEATURE_GSM
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Failed rfgsm_mc_power_clock_on - should not be called via rfm %d", rfm_mode[RFCOM_TRANSCEIVER_0]);
      #endif
      break;

    default:
      MSG_FATAL("Invalid Mode for Device: (device= %d, mode = %d)", RFCOM_TRANSCEIVER_0, rfm_mode[RFCOM_TRANSCEIVER_0], NULL); 
      break;
  }
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  Disable tcxo input buffer and power down RTR.

  @details
  Disables TCXO input buffer and then votes off RTR VREGs.

  @param

  @pre
  NOTE: This function is deprecated in 2.2 and above PLs

*/
void rfm_power_clock_off(void)
{
  switch(rfm_mode[rfwcdma_mc_state.primary_rx_device])
  {
    case RFCOM_1X_MODE:
    case RFCOM_1XEVDO_MODE:
      //1x has their own code to manage power rail
      break;

    case RFCOM_WCDMA_MODE:
      #ifdef FEATURE_WCDMA
      RFWCDMA_MC_V(power_clock_off)(rfwcdma_mc_state.primary_rx_device);
      #endif
      break;

    case RFCOM_GSM_MODE:
      #ifdef FEATURE_GSM
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Failed rfgsm_mc_power_clock_off - should not be called via rfm %d", rfm_mode[rfwcdma_mc_state.primary_rx_device]);
      #endif
      break;

    default:
      MSG_FATAL("Invalid Mode for Device: (device= %d, mode = %d)", rfwcdma_mc_state.primary_rx_device, rfm_mode[rfwcdma_mc_state.primary_rx_device], NULL);
      break;
  }
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  Returns RF power collapse status

  @details
  Returns RF power collapse status. Function only returns true if both RF chains
  have voted for power collapse

  @param

  @pre
*/
boolean rfm_get_power_collapse_statue(void)
{
//    return rfcommon_get_power_collapse_status();
  return FALSE;
}

#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
/*----------------------------------------------------------------------------*/
/*
  @brief
  Power on RTR and enable gp_clk.

  @details
  Powers on VREGs for RTR, waits for it to settle and vote TCXO on, and 
  then enable gp_clk.

  @param
  Mode of current technology

  @pre
*/
void rfm_early_power_clock_on(rfm_mode_enum_type rf_mode)
{
//    rfcommon_mc_early_power_clock_enable(rf_mode);
}
#endif

/*----------------------------------------------------------------------------*/
/*
  @brief
  Converys the sys_band enum to the rf_card_band enum

  @param
  sys_band_class_e_type sys_band to convert to rf_card_band_type

  @return
  rf_card_band_type coverted band
*/
rf_card_band_type rfm_convert_sys_band_to_rf_card_band
(
  sys_band_class_e_type sys_band
)
{
  rf_card_band_type rf_card_band = RF_MAX_BAND;

  switch( sys_band)
  {
  /* GSM bands */
  case SYS_BAND_GSM_850:
    rf_card_band = RF_GSM_850_BAND;
    break;

  case SYS_BAND_GSM_EGSM_900:
  case SYS_BAND_GSM_PGSM_900:
  case SYS_BAND_GSM_RGSM_900:
    rf_card_band = RF_EGSM_BAND;
    break;

  case SYS_BAND_GSM_DCS_1800:
    rf_card_band = RF_DCS_BAND;
    break;

  case SYS_BAND_GSM_PCS_1900:
    rf_card_band = RF_GSM_1900_BAND;
    break;

  /* WCDMA bands */
  case SYS_BAND_WCDMA_I_IMT_2000:
    rf_card_band = RF_BCI_BAND;
    break;

  case SYS_BAND_WCDMA_II_PCS_1900:
    rf_card_band = RF_BCII_BAND;
    break;

  case SYS_BAND_WCDMA_III_1700:
    rf_card_band = RF_BCIII_BAND;
    break;

  case SYS_BAND_WCDMA_IV_1700:
    rf_card_band = RF_BCIV_BAND;
    break;

  case SYS_BAND_WCDMA_V_850:
    rf_card_band = RF_BCV_BAND;
    break;

  case SYS_BAND_WCDMA_VI_800:
    rf_card_band = RF_BCVI_BAND;
    break;

  case SYS_BAND_WCDMA_VIII_900:
    rf_card_band = RF_BCVIII_BAND;
    break;

  case SYS_BAND_WCDMA_IX_1700:
    rf_card_band = RF_BCIX_BAND;
    break;

  case SYS_BAND_WCDMA_XI_1500:
    rf_card_band = RF_BCXI_BAND;
    break; 

  case SYS_BAND_WCDMA_XIX_850:
    rf_card_band = RF_BCXIX_BAND; 
    break; 

  /* LTE bands */
  case SYS_BAND_LTE_EUTRAN_BAND1: 
    rf_card_band = RF_LTE_B1_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND2: 
    rf_card_band = RF_LTE_B2_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND3: 
    rf_card_band = RF_LTE_B3_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND4: 
    rf_card_band = RF_LTE_B4_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND5: 
    rf_card_band = RF_LTE_B5_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND6: 
    rf_card_band = RF_LTE_B6_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND7: 
    rf_card_band = RF_LTE_B7_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND8: 
    rf_card_band = RF_LTE_B8_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND9: 
    rf_card_band = RF_LTE_B9_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND10:
    rf_card_band = RF_LTE_B10_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND11:
    rf_card_band = RF_LTE_B11_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND12:
    rf_card_band = RF_LTE_B12_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND13:
    rf_card_band = RF_LTE_B13_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND14:
    rf_card_band = RF_LTE_B14_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND17:
    rf_card_band = RF_LTE_B17_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND18:
    rf_card_band = RF_LTE_B18_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND19:
    rf_card_band = RF_LTE_B19_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND20:
    rf_card_band = RF_LTE_B20_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND21:
    rf_card_band = RF_LTE_B21_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND23:
    rf_card_band = RF_LTE_B23_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND25:
    rf_card_band = RF_LTE_B25_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND26:
    rf_card_band = RF_LTE_B26_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND27:
    rf_card_band = RF_LTE_B27_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND28:
    rf_card_band = RF_LTE_B28_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND29:
    rf_card_band = RF_LTE_B29_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND30:
    rf_card_band = RF_LTE_B30_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND33:
    rf_card_band = RF_LTE_B33_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND34:
    rf_card_band = RF_LTE_B34_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND35:
    rf_card_band = RF_LTE_B35_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND36:
    rf_card_band = RF_LTE_B36_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND37:
    rf_card_band = RF_LTE_B37_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND38:
    rf_card_band = RF_LTE_B38_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND39:
    rf_card_band = RF_LTE_B39_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND40:
    rf_card_band = RF_LTE_B40_BAND;
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND41:
    rf_card_band = RF_LTE_B41_BAND;
    break; 

  #ifdef LTE_BAND_NUM 
  case SYS_BAND_LTE_EUTRAN_BAND252:
    rf_card_band = RF_LTE_B125_BAND;
    break; 
  #endif

  #ifdef LTE_BAND_NUM 
  case SYS_BAND_LTE_EUTRAN_BAND255:
    rf_card_band = RF_LTE_B127_BAND;
    break; 
  #endif

  case SYS_BAND_TDS_BANDA:
    rf_card_band = RF_TDSCDMA_B34_BAND;
    break; 

  case SYS_BAND_TDS_BANDF:
    rf_card_band = RF_TDSCDMA_B39_BAND; 
    break; 

  case SYS_BAND_TDS_BANDE: 
    rf_card_band = RF_TDSCDMA_B40_BAND; 
    break; 
  
  case SYS_BAND_LTE_EUTRAN_BAND125:
    rf_card_band = RF_LTE_B125_BAND; 
    break; 

  case SYS_BAND_LTE_EUTRAN_BAND126:
    rf_card_band = RF_LTE_B126_BAND; 
    break;

  case SYS_BAND_LTE_EUTRAN_BAND127:
    rf_card_band = RF_LTE_B127_BAND; 
    break;  
  default:
    break;
  }

  return( rf_card_band );
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  Conveys the LTE Tx band to the GPS RF

  @param
  rfcom_lte_band_type

  @return
  None
*/ 
void rfm_gnss_tx_band(rfcom_lte_band_type rf_tx_band) 
{ 
  RFGNSS_MC_V(tx_band)(rf_tx_band); 
}
 
/*----------------------------------------------------------------------------*/
/*
  @brief
  Update the Tx flag whenever LTE Tx turns ON/OFF

  @param
  boolean

  @return
  None
*/ 
 
void rfm_gnss_tx_indicator( boolean enable )
{
  RFGNSS_MC_V(tx_indicator)(enable);
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  Reading NV from all the available RF technologies.

  @details
  Retrieves all the NV from all available technologies. Reads common and mode
  specific RF configuration parameters from NV. This is called as part of the 
  LOAD_RF_NV function case where we need to read the calibration NV and write 
  them without having to reset.

  @param caller_tcb_ptr     : Calling task's TCB pointer
  @param task_nv_wait_sig   : Task signal to wait for when reading NV
  @param task_wait_func_ptr : Task's wait function to be called when reading NV

  @retval Returns TRUE if NV retrieve is successful, otherwise returns FALSE.
*/
boolean 
rfm_reload_nv 
( 
  rex_tcb_type *caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (*task_wait_func_ptr)( rex_sigs_type )
)
{

  boolean ret_val;
  uint8 dev_idx;
  
  for ( dev_idx = RFM_DEVICE_0; dev_idx< RFM_MAX_DEVICES; dev_idx++ )
  {
    ftm_rfmode_exit ( dev_idx, FTM_STATE_PARK );
  }

    rfcommon_mc_reload_nv(RFCOM_TRANSCEIVER_0,
                        caller_tcb_ptr, task_nv_wait_sig,
                        task_wait_func_ptr);

  #ifdef FEATURE_CDMA1X
    ret_val = TRUE; /* Needed if RF1X_MC macro is stubbed out */
    RF1X_MC(reload_nv, ret_val)(RFCOM_TRANSCEIVER_0, caller_tcb_ptr, task_nv_wait_sig, task_wait_func_ptr);
  #endif

  #ifdef FEATURE_WCDMA
    ret_val = TRUE; /* Needed if RFWCDMA_MC macro is stubbed out */
    RFWCDMA_MC(reload_nv, ret_val)(RFCOM_TRANSCEIVER_0, caller_tcb_ptr, task_nv_wait_sig, task_wait_func_ptr);
  #endif

  #ifdef FEATURE_GSM
    ret_val = FALSE; /* Needed if RFGSM_MC macro is stubbed out */
    for (dev_idx = 0; dev_idx < RFM_MAX_WAN_DEVICES; dev_idx ++)
    {
      ret_val |= rfgsm_mc_reload_nv(dev_idx, caller_tcb_ptr, task_nv_wait_sig, task_wait_func_ptr);
    }
  #endif

#ifdef FEATURE_LTE
    ret_val = TRUE; /* Needed if RFLTE_MC macro is stubbed out */
    RFLTE_MC(reload_nv, ret_val)(RFCOM_TRANSCEIVER_0, caller_tcb_ptr, task_nv_wait_sig, task_wait_func_ptr);
#endif /*FEATURE_LTE*/

    #ifdef FEATURE_TDSCDMA
    ret_val = TRUE; /* Needed if RFTDSCDMA_MC macro is stubbed out */
    RFTDSCDMA_MC(reload_nv, ret_val)(caller_tcb_ptr, task_nv_wait_sig, task_wait_func_ptr);
    #endif /*FEATURE_TDSCDMA*/

  rfm_set_calibration_state(FALSE);

    return ( ret_val );
}



/*----------------------------------------------------------------------------*/
/*
  @brief
  Reduces MTPL for thermal mitigation

  @details
  Reduces max power limit based on thermal mitigation decisions coming from
  the DEM module

  @param backoff : amount to reduce the tx power limit in dB

*/
void rfm_reduce_mptl_db_for_therm( uint8 backoff )
{
  int32 ret_val = 0;
  switch(rfm_mode[RFCOM_TRANSCEIVER_0])
  {

  case RFCOM_WCDMA_MODE:
    #ifdef FEATURE_WCDMA
    RFWCDMA_MC(cmd_dispatch, ret_val)(RFCOM_TRANSCEIVER_0, RFWCDMA_REDUCE_MTPL_FOR_THERM, (void*)(&backoff));
    #endif
    break;

  default:
    MSG_FATAL( "Invalid Mode for Device: (device= %d, mode = %d)", 
                   RFCOM_TRANSCEIVER_0, rfm_mode[RFCOM_TRANSCEIVER_0], NULL);
    break;
  }

  RF_MSG_1( RF_LOW, "rfm_reduce_mptl_db_for_therm() return status = %d", ret_val);
}

/*! @endcond */


/*============================================================================*/
/*!
  @name Calibration

  @brief
  This section holds Interfaces to control the RF Calibration Mode.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Flag Calibration State
 
  @details
  This function will be used by FTM to indicate RF that the target is entering
  calibration mode. This function will update the static variable blindly
  without cross checking. It will be the responsibility of FTM module to take
  necessary steps to move Calibration State.

  The calibration state variable which is touched by this function is referenced
  by the RF code to enable or disable various RF algorithms such as 
  intelliceiver, HDET, temp comp etc.
 
  @param cal_enable Boolean flag indicating if the calibration state is
  enabled or not. TRUE means calibration state is enabled, whereas FALSE means
  calibration state is disabled
*/
void
rfm_set_calibration_state
(
  boolean cal_enable
)
{

  /* Throw F3 msg, if calibration state is already in the state as requested */
  if ( rfm_calibration_mode_en == cal_enable )
  {
    RF_MSG_1( RF_HIGH, "rfm_set_calibration_mode : Cal State is already %d",\
              cal_enable );
  }
  else
  {
    RF_MSG_1( RF_LOW, "rfm_set_calibration_mode : Cal State set to %d",\
              cal_enable );
    rfm_calibration_mode_en = cal_enable;

    //Updating the calibration info for using in device drive space
    rfdevice_set_cal_state(rfm_calibration_mode_en);

    if (FALSE == cal_enable)
    {
      rfcommon_mdsp_free_dpd_buffer();
    }
  }

} /* rfm_set_calibration_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Calibration State
 
  @details
  This function will used by RF module to query the current calibration state.
  This information is used by RF code to enable or disable various RF
  algorithms such as  intelliceiver, HDET, temp comp etc.
 
  @return
  Boolean flag indicating if the calibration state is
  enabled or not. TRUE means calibration state is enabled, whereas FALSE means
  calibration state is disabled
*/
boolean
rfm_get_calibration_state
(
  void
)
{

  return rfm_calibration_mode_en;

} /* rfm_get_calibration_state */

/*! @} */

/*============================================================================*/
/*!
  @name Transceiver Resource Manager

  @brief
  This section holds Interfaces exported to Transceiver Resource Manager to
  Query/Configure RF Data.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Devices Configuration
 
  @details
  This function will return all the device configuration that is supported for
  the target. The configuration includes the techs and antenna supported in each 
  RF device. Along with this it will include flag indicating whether features
  like SV is supported.
 
  @param dev_cfg
  The container to populate the Device configuration configuraion which includes
  SV flag, tech supported, and antenna supported.
 
  @return
  Flag indicating if the RF Device configuration is valid or not
*/
boolean
rfm_get_devices_configuration
(
  rfm_devices_configuration_type *dev_cfg
)
{
  boolean dev_cfg_valid = TRUE;
  rf_path_enum_type rf_path = RF_PATH_0;
  uint8 mask_cntr = 0;
  
  dev_cfg_valid = rfc_get_devices_configuration( dev_cfg );
  /* Check if dev_cfg is NULL */
  if ( dev_cfg == NULL )
  {
    RF_MSG( RF_ERROR, "rfm_get_devices_configuration: Invalid Container");
    return FALSE;
  }

  for(rf_path = RF_PATH_0; rf_path < RF_PATH_MAX; rf_path++)
  {
   for(mask_cntr = 0; mask_cntr <3; mask_cntr++)
   {
    /* AND the band support from the RFC with the ones from the BC config NV */
    dev_cfg->device_support[rf_path].bands_supported.mask[mask_cntr] &= rfc_bc_cfg_bands[rf_path].mask[mask_cntr];
   }
  }
  return dev_cfg_valid;
} /* rfm_get_devices_configuration */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Logical device allocation for a tech and band combo
 
  @details
  This function will update the rf concurrency manager with the current
  tech and band allocation details provided by TRM. 
 
  @param dev_allocation
  contains details about which band and tech is used on a specific logical
  device 
  
  @return 
  Flag to indicate if the device allocation has been successful or not 
*/
boolean
rfm_inform_device_allocation
(
  rfm_device_allocation_type *dev_allocation
)
{
  if (dev_allocation == NULL)
  {
    RF_MSG(RF_ERROR, "rfm_inform_device_allocation: Invalid container"); 
    return FALSE; 
  }

  return (rfcmn_concurrency_mgr_rx_update_device_alloc(dev_allocation)); 

} /* end of rfm_inform_device_allocation */


/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Check concurrency possibilities for a particular tech and band combo
 
  @details
  The function which check with the rf concurrency manager if it is possible
  to allocate a particular logical device to a specific tech and band combo
 
  @param dev_client
  contains details about which band and tech is used on a specific logical
  device
 
  @return 
  Flag to indicate if the dev client info is valid or not
*/
boolean
rfm_validate_device_concurrency
(
  rfm_device_client_info_type *dev_client
)
{
  /* Check if dev client is NULL*/
  if (dev_client == NULL)
  {
    RF_MSG(RF_ERROR, "rfm_validate_device_concurrency: Invalid container"); 
    return FALSE; 
  }

  return TRUE; 
  
} /* end of rfm_validate_device_concurrency */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Check concurrency possibilities for a particular tech and band combo
 
  @details
  The function which check with the rf concurrency manager if it is possible
  to allocate a particular logical device to a specific tech and band combo
 
  @param dev_client
  contains details about which band and tech is used on a specific logical
  device
 
  @return 
  structure which captures the status per logical device 
*/
rfm_device_allocation_status_type* rfm_verify_rf_concurrency
(
  rfm_device_client_info_type *dev_client
)
{
  /* Check if dev client is NULL*/
  if (dev_client == NULL)
  {
    RF_MSG(RF_ERROR, "rfm_validate_device_concurrency: Invalid container"); 
    return FALSE; 
  }

  return (rfcmn_concurrency_mgr_rx_validate_scenario(dev_client)); 

} /* end of rfm_validate_device_concurrency */


/*! @} */

/*============================================================================*/
/*!
  @name State Query

  @brief
  This section holds Interfaces to Query RFM State.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns status of rfm_init()
 
  @details
  This function returns the value of the flag rfm_initialized. This flag  
  indicates if rfm_init() procedure has succeeded. This function should NOT be 
  exported to non-RF modules. It may be used by MC-level APIs to check if 
  rfm_init() was successful.
    
  @return
  TRUE means rfm_init() was successful
*/
boolean
rfm_is_rfm_init_success
(
  void
)
{

  return rfm_initialized;

} /* rfm_get_rfm_initialized_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Current Mode
 
  @details
  This is an accessor function which will return the current active Mode on the
  given device.
 
  @param device Device whose cuurent mode is queried
 
  @return
  Current active mode
*/
rfm_mode_enum_type
rfm_get_current_mode
(
  rfm_device_enum_type device
)
{

  rfm_mode_enum_type current_mode; /* Current RFM Mode */

  /* Perform Device validity Check. Return Invalid Mode, if the device is
  not supported */
  if ( device < RFM_MAX_DEVICES )
  {
    current_mode = rfm_mode[device];
  }
  else
  {
    current_mode = RFM_INVALID_MODE;
  }
  return current_mode;

} /* rfm_get_current_mode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Lock Data
 
  @details
  This is an accessor function which will return the pointer to the RF
  Lock Data.
 
  @return
  Pointer to RFA Lock Data
*/
rf_lock_data_type*
rfm_get_lock_data
(
  void
)
{
  return &rfm_lock_data;
} /* rfm_get_current_mode */

/*! @} */

/*! @} */



/*----------------------------------------------------------------------------*/
/*
  @brief
  Returns the current PA state.

*/
uint8 rfm_get_current_pa_state(void)
{
  uint8 cur_pa_state = 0;

  switch(rfm_mode[RFCOM_TRANSCEIVER_0])
  {
  #ifdef FEATURE_LTE
  case RFCOM_LTE_MODE:
      cur_pa_state = rflte_mc_fed_get_current_pa_state();
      break;
  #endif
  #ifdef FEATURE_WCDMA
  case RFCOM_WCDMA_MODE:
      cur_pa_state = rfwcdma_core_util_get_rflm_pa_range();
      break;
  #endif

  default:
      cur_pa_state = 0;
      break;
  }

    return cur_pa_state;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF driver query function to determine if a technolgy can tune a channel,
  without requiring rfm_enter_mode() to be called first.
 
  @details
  This function returns a boolean indicating whether the specified channel
  is supported by the chipset driver in the specified band.  If the band 
  is not supported FALSE is returned; If the band is supported but channel 
  is not valid as per spec 3GPP2 C.S0057-C, FALSE is returned. 
  If the band has subclass support and the channel falls under an invalid
  subclass, FALSE is returned
  otherwise, TRUE is returned
  
  @param[in] rf_mode current RF mode of operation
  @param[in] band Specifies the band class for which spec/chipset support is to
                  be validated
  @param[in] chan Specifies the RF channel for which spec/chipset support is to
                  be validated 

  @retval is_band_chan_supported
                  TRUE if band/channel is supported by the current devices
                  configured in the library
                  FALSE  if band/channel is not supported by the current 
                  devices configured in the library
*/
boolean
rfm_is_band_chan_supported_v2
(
  rfm_mode_enum_type rf_mode, 
  sys_band_class_e_type band, 
  word chan
)
{

  boolean is_band_chan_supported = FALSE;
#if defined(FEATURE_CDMA1X) && defined(FEATURE_HDR)
  rfm_device_enum_type dev_idx;
#endif 
  /* Device counter */

  switch(rf_mode)
  {

  #if defined(FEATURE_CDMA1X) && defined(FEATURE_HDR)
  case RFM_1X_MODE:    
  case RFM_1XEVDO_MODE:
    for( dev_idx = 0 ; dev_idx < RFM_MAX_DEVICES; dev_idx++ )
    {
      is_band_chan_supported |= rf_cdma_mc_is_band_chan_supported( dev_idx,
                                   rf_cdma_sys_band_type_to_rf_band_type(band), 
                                   (rfm_cdma_chan_type)chan);

      if( is_band_chan_supported == TRUE )
      {
        break;
      }
    }
    break;
  #endif /* defined(FEATURE_CDMA1X) && defined(FEATURE_HDR) */

  default:
    is_band_chan_supported = FALSE;
    RF_MSG_1( RF_ERROR, "Invalid Mode (mode = %d)", rf_mode );
    break;
  } /* switch(rf_mode) */

  return is_band_chan_supported;
}  /* rfm_is_band_chan_supported_v2 */

/*----------------------------------------------------------------------------*/ 
/*! 
  
 @brief 
  This function returns the DL frequency in Khz based on the  
  downlink channel number and rf mode passed in as an argument.  
  
 @details 
  Provided for LTE. Each tech needs to fill out case and return the frequecny value 
  for the corresponding rx_chan and rf mode type.
   
  @param  
   rx_chan: Downlink channel number. 
   mode = rf mode type
   returns frequency in Khz (eg: for 2110Mhz, it returns 2110000).
*/ 

uint32 rfm_get_rx_freq_from_rx_earfcn(uint16 rx_chan, rfcom_band_type_u band, rfm_mode_enum_type mode)
{
  uint32 freq = 0;  
#ifdef FEATURE_LTE // Currently only LTE uses this variable. Need to remove if other Techs will use it
  rfcom_band_type_u rf_band;
#endif
  switch (mode) 
  { 
#ifdef FEATURE_LTE
  case RFM_LTE_MODE: 
  case RFM_LTE_FTM_MODE: 
       rf_band.lte_band = rflte_core_get_band_from_rx_uarfcn((rfcom_lte_earfcn_type)rx_chan);
       if(RFCOM_BAND_LTE_INVALID != rf_band.lte_band)
       {
         freq = rflte_core_get_rx_freq_from_uarfcn((rfcom_lte_earfcn_type)rx_chan,rf_band.lte_band);
       }
       break; 
#endif /*FEATURE_LTE*/
  case RFM_PARKED_MODE:
  case RFM_IMT_MODE: 
  case RFM_EGSM_MODE: 
  case RFM_1X_MODE: 
  case RFM_GPS_MODE: 
  case RFM_TDSCDMA_MODE: 
  case RFM_1XEVDO_MODE: 
  case RFM_SVDO_MODE: 
  case RFM_GNSS_MODE: 
  default: 
     MSG_ERROR("Invalid RF mode = %d", mode, 0, 0); 
     break; 
  } 
   
  return (freq); 
   
} /*rfm_get_rx_freq_from_rx_earfcn*/    

/*----------------------------------------------------------------------------*/
/*!
 
  @brief
  This function returns the UL frequency in Khz based on the  
  uplink channel number and rf mode passed in as an argument.  
 
  @details
  Provided for LTE. Each tech needs to fill out case and return the frequecny value 
  for the corresponding tx_chan and rf mode type.

  @param  
   tx_chan: Uplink channel number. 
   mode = rf mode type
   returns frequency in Khz (eg: for 2110Mhz, it returns 2110000).
*/

uint32 rfm_get_tx_freq_from_tx_earfcn(uint16 tx_chan, rfcom_band_type_u band, rfm_mode_enum_type mode)
{
  uint32 freq = 0;  
#ifdef FEATURE_LTE // Currently only LTE uses this variable. Need to remove if other Techs will use it
  rfcom_band_type_u rf_band;
#endif
   switch(mode)
   {
#ifdef FEATURE_LTE
     case RFM_LTE_MODE:
  case RFM_LTE_FTM_MODE: 
       rf_band.lte_band = rflte_core_get_band_from_tx_uarfcn(tx_chan);
       if(RFCOM_BAND_LTE_INVALID != rf_band.lte_band)
       {
         freq = rflte_core_get_tx_freq_from_uarfcn(tx_chan,rf_band.lte_band);
       }
       break;
#endif /*FEATURE_LTE*/
  case RFM_PARKED_MODE:
  case RFM_IMT_MODE: 
  case RFM_EGSM_MODE: 
  case RFM_1X_MODE: 
  case RFM_GPS_MODE: 
  case RFM_TDSCDMA_MODE: 
  case RFM_1XEVDO_MODE: 
  case RFM_SVDO_MODE: 
  case RFM_GNSS_MODE: 
     default:
     MSG_ERROR("Invalid RF mode = %d", mode, 0, 0); 
       break;
   }

  return (freq); 

} /*rfm_get_tx_freq_from_tx_earfcn*/  

/*----------------------------------------------------------------------------*/ 
/*!
  @brief
  Configure Rx PLL script for correcting the rx error
 
  @details
  This is an accessor function which will return the script 
  required to retune the PLL

  @param mode 
  Technology for which rx error needs to be corrected

  @param rxpll_script_buf_ptr 
  SSBI script which retunes the PLL to correct for the error
  
*/
void 
rfm_get_rxpll_script
( 
rfm_mode_enum_type         mode,
void*                      rxpll_script_buf_ptr
)
{
  #ifdef FEATURE_LTE 

  boolean result = FALSE;

  #endif /* FEATURE_LTE */

  switch(mode)
  {
    /* Currently only implemented for LTE */
    #ifdef FEATURE_LTE

    case RFM_LTE_MODE:

    /* Call API to get RxPLL Script */
    RFLTE_MC
      ( get_rxpll_script, result )
      ( rxpll_script_buf_ptr );       

    break;
    
    #endif /*FEATURE_LTE*/

    default:
    
    MSG_ERROR("Invalid RF mode = %d for get_rxpll_script", mode, 0, 0);     

    break;
    
  }
#ifdef FEATURE_LTE 
  if (result == FALSE)
  {
    RF_MSG_1( RF_LOW, "get_rxpll_script status = %d", result);
  }  
#endif /*FEATURE_LTE*/

} /* rfm_get_rxpll_script */

/*----------------------------------------------------------------------------*/ 
/*!
  @brief
  Configure Tx PLL script for correcting the tx error
 
  @details
  This is an accessor function which will return the 
  script required to retune the PLL

  @param mode 
  Technology for which tx error needs to be corrected

  @param rx_freq_error_in_hz   
  rx_freq_error_in_hz Rx error being reported by 
  FW where +ve error means current freq > nominal freq

  @param txpll_script_buf_ptr     
  txpll_script_buf_ptr SSBI script which retunes the ppl 
  to correct for the error
  
*/
void 
rfm_get_txpll_script
( 
rfm_mode_enum_type            mode,
                           int32 rx_freq_error_in_hz, 
void*                         txpll_script_buf_ptr
)
{
  #ifdef FEATURE_LTE
  
  boolean result = FALSE;
  
  #endif /* FEATURE_LTE */
  
    switch(mode)
    {
    /* Currently only implemented for LTE */  
    #ifdef FEATURE_LTE
    
      case RFM_LTE_MODE:
        
      result = rflte_mc_tx_update_pll_scripts ( rx_freq_error_in_hz, 
                                                txpll_script_buf_ptr );       

      break;
    
#endif /*FEATURE_LTE*/
    
     default:

     MSG_ERROR("Invalid RF mode = %d for get_txpll_script", mode, 0, 0);     

    break;
}
  
#ifdef FEATURE_LTE 
  if (result == FALSE)
  {
    RF_MSG_1( RF_LOW, "get_txpll_script status = %d", result);
  }
 #endif /* FEATURE_LTE */
 
} /* rfm_get_txpll_script */

/*----------------------------------------------------------------------------*/ 
/*
  @brief
This API will return the synth status for a given Tech.Path and synthesizer 
 
  @details

  @param 
  mode : RF path
  rf_mode : RF mode
  synth_type: where TX PLL is rqequired or RX PLL 
  is_synth_locked: PLL status.
*/
void 
rfm_get_synth_lock_status
(
  rfm_device_enum_type device,
  rfcom_mode_enum_type rf_mode,
  rfcom_synth_type synth_type,
  void *is_synth_locked
)
{   
   switch(rf_mode)
   {
     #ifdef FEATURE_CDMA1X
     case RFCOM_1X_MODE:
       RF1X_MC_V(get_synth_lock_status)(device,rf_mode,synth_type,is_synth_locked);
       break;
     #endif

     #ifdef FEATURE_WCDMA
     case RFCOM_WCDMA_MODE:
       RFWCDMA_MC_V(get_synth_lock_status)(device,rf_mode,synth_type,is_synth_locked);
       break;
     #endif

     default:   
       MSG_ERROR("rfm_get_synth_lock_status: Invalid RF mode=%d",rf_mode,0,0);
   }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initiate Antenna Switching
 
  @details
  This function will be called by L1 to perform antenna switching
  using the tech active on given deivce. Based on the action flag, this
  function will either create switch script for the given device with the
  given antenna switch position; or perform immdiate hardware writes to
  perform antenna switch. Immediate hardware write would be primarily done
  for FTM mode testing.

  @param device
  RFM Device on which antenna position is set
 
  @param ant_sw_pos
  New Antenna Switch Position
 
  @param cb_func
  Callback function pointer passed by L1 to be called when switch is completed
 
  @param cb_data
  Callback function data
 
  @param immediate
  Flag indicating if a script is to be created or immediate action is to
  be done

  @return
  Status indicating the success of the function execution
*/
boolean
rfm_set_antenna
(
  rfm_device_enum_type device, 
  uint32 ant_sw_pos,
  rfm_l1_cb_type cb_func,
  void *cb_data
)
{  
  #ifdef FEATURE_RF_ASDIV
  #ifdef FEATURE_GSM
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_data_type          rfgsm_core_data;
  #endif
  #endif
  boolean ret_val = TRUE;

  #ifdef FEATURE_RF_ASDIV
  rfm_mode_enum_type rf_mode =  RFM_INVALID_MODE;

  /* RFM call Logging for debug*/
  RFM_DEBUG_EN_Q(&rfm_set_antenna,device,ant_sw_pos,0,0);

  /* Param Validation */
  if ( (device < RFM_DEVICE_0) || (device >= RFM_MAX_WAN_DEVICES) )
  {
    RF_MSG_1( RF_ERROR, "rfm_set_antenna - Invalid Device %d",
              device);
    return FALSE;
  } /* if ((device < RFM_DEVICE_0) || (device >= RFM_MAX_WAN_DEVICES)) */

  /* Get RFM device */
  rf_mode = rfm_get_current_mode(device);

  /* Call function pertaining to active tech on given device */
  switch (rf_mode) 
  {
  case RFM_EGSM_MODE:
  case RFM_EGSM_MODE_2:
  case RFM_EGSM_MODE_3:
      #ifdef FEATURE_GSM
      if(ftm_gsm_cal_mode_enabled(device))
      {
      rfgsm_core_handle_ptr = rfgsm_core_handle_get( device );
      if( rfgsm_core_handle_ptr != NULL )
      {
         /* These will be stored in event hdr and returned by RFLM so store them away */
         rfgsm_core_handle_ptr->asd_l1_cb_func = (rfgsm_l1_cb_type) cb_func; 
         rfgsm_core_handle_ptr->asd_sub_id = RFLM_GSM_DEFAULT_SUBSCRIPTION; /* Use default */

         /* Set the core data */
         rfgsm_core_data.rf_task = RF_TASK_1;
         rfgsm_core_data.rfm_dev = device;
         rfgsm_core_data.sub_id = RFLM_GSM_DEFAULT_SUBSCRIPTION;
         rfgsm_core_data.as_id = SYS_MODEM_AS_ID_1; 
         rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
         rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;

         rfgsm_mc_set_antenna_req(&rfgsm_core_data, ant_sw_pos);
         RF_MSG_2( RF_HIGH, "rfgsm_mc_set_antenna_req device %d, ant_sw_pos = %d", device, ant_sw_pos);
      }
      else
      {
        RF_MSG_1( MSG_LEGACY_ERROR, "rfgsm_core_handle_ptr is NULL for device %d!", device);
      }
      #endif
      }
      else
      {
       RF_MSG_2( RF_FATAL, "Invalid Non FTM rfm_set_antenna request on device %d, ant_sw_pos = %d", device, ant_sw_pos);
       return FALSE;
      }
    break;
  case RFM_1X_MODE:
  case RFM_1XEVDO_MODE:
  case RFM_SVDO_MODE:
    {
      #ifdef FEATURE_CDMA1X

      /* NOT immediate writes. The device driver returns script. */
      boolean immediate = FALSE ;
      
      ret_val &= rf_cdma_mc_set_antenna_to_position( device, 
                                                     ant_sw_pos,
                                                     immediate ,
                                                     cb_func, 
                                                     cb_data );
      #endif
      break;
    }

  case RFM_IMT_MODE:
    #ifdef FEATURE_WCDMA
    ret_val &= rfwcdma_asdiv_handle_set_antenna( device, 
                                                 ant_sw_pos,
                                                 cb_func,
                                                 cb_data,
                                                 FALSE );
    #endif
    break;

  case RFM_LTE_MODE:
    #ifdef FEATURE_LTE
    rflte_mc_send_switch_ant_msgr( device,
                                   ant_sw_pos,
                                   cb_func,
                                   cb_data,
                                   FALSE );
    #endif
    break;
  case RFM_TDSCDMA_MODE:
    #ifdef FEATURE_TDSCDMA
    rf_tdscdma_mc_send_switch_ant_msgr( device,
                                        ant_sw_pos,
                                        cb_func,
                                        cb_data,
                                        FALSE );
    #endif
    break;
  default:
    RF_MSG_2( RF_ERROR, "rfm_set_antenna - Invalid Mode %d on device %d",
              rf_mode, device );
    break;
  }

  RF_MSG_4( RF_HIGH, "rfm_set_antenna : [Status %d] Device %d, Tech %d, "
            "SwitchPos %d", ret_val, device, rf_mode, ant_sw_pos );

  #endif /* FEATURE_RF_ASDIV */

  return ret_val;

} /* rfm_set_antenna */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initiate Antenna Switching (Immediate)
 
  @details
  This function will be called by L1 to perform antenna switching
  using the tech active on given deivce. Based on the action flag, this
  function will either create switch script for the given device with the
  given antenna switch position; or perform immdiate hardware writes to
  perform antenna switch. Immediate hardware write would be primarily done
  for FTM mode testing.

  @param device
  RFM Device on which antenna position is set
 
  @param ant_sw_pos
  New Antenna Switch Position
 
  @param cb_func
  Callback function pointer passed by L1 to be called when switch is completed
 
  @param cb_data
  Callback function data
 
  @return
  Status indicating the success of the function execution
*/
boolean
rfm_set_antenna_imm
(
  rfm_device_enum_type device, 
  uint32 ant_sw_pos,
  rfm_l1_cb_type cb_func,
  void *cb_data
)
{
  #ifdef FEATURE_RF_ASDIV
  #ifdef FEATURE_GSM
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_data_type          rfgsm_core_data;
  #endif
  #endif
  boolean ret_val = TRUE;

  #ifdef FEATURE_RF_ASDIV
  rfm_mode_enum_type rf_mode =  RFM_INVALID_MODE;

  /* RFM call Logging for debug */
  RFM_DEBUG_EN_Q(&rfm_set_antenna_imm,device,ant_sw_pos,0,0);

  /* Param Validation */
  if ( (device < RFM_DEVICE_0) || (device >= RFM_MAX_WAN_DEVICES) )
  {
    RF_MSG_1( RF_ERROR, "rfm_set_antenna_imm - Invalid Device %d",
              device);
    return FALSE;
  } /* if ((device < RFM_DEVICE_0) || (device >= RFM_MAX_WAN_DEVICES)) */

  /* Get RFM device */
  rf_mode = rfm_get_current_mode(device);

  /* Call function pertaining to active tech on given device */
  switch (rf_mode) 
  {
  case RFM_1X_MODE:
  case RFM_1XEVDO_MODE:
  case RFM_SVDO_MODE:
    {
      #ifdef FEATURE_CDMA1X
      /* Immediate writes. The device driver does NOT return script.*/
      boolean immediate = TRUE ;
      ret_val &= rf_cdma_mc_set_antenna_to_position( device, 
                                                     ant_sw_pos,
                                                     immediate ,
                                                     cb_func, 
                                                     cb_data );
      #endif
      break;
    }

  case RFM_IMT_MODE:
    #ifdef FEATURE_WCDMA

    ret_val &= rfwcdma_asdiv_handle_set_antenna( device, 
                                                 ant_sw_pos,
                                                 cb_func,
                                                 cb_data,
                                                 TRUE );
    
    #endif
    break;

  case RFM_LTE_MODE:
    #ifdef FEATURE_LTE
    rflte_mc_send_switch_ant_msgr( device,
                                   ant_sw_pos,
                                   cb_func,
                                   cb_data,
                                   TRUE );
    #endif
    break;

  case RFM_TDSCDMA_MODE:
    #ifdef FEATURE_TDSCDMA
    rf_tdscdma_mc_send_switch_ant_msgr( device,
                                        ant_sw_pos,
                                        cb_func,
                                        cb_data,
                                        TRUE );
    #endif
    break;

  case RFM_EGSM_MODE:
  case RFM_EGSM_MODE_2:
  case RFM_EGSM_MODE_3:
    #ifdef FEATURE_GSM
      rfgsm_core_handle_ptr = rfgsm_core_handle_get( device );
      if( rfgsm_core_handle_ptr != NULL )
      {
         /* These will be stored in event hdr and returned by RFLM so store them away */
         rfgsm_core_handle_ptr->asd_l1_cb_func = (rfgsm_l1_cb_type) cb_func; 
         rfgsm_core_handle_ptr->asd_sub_id = RFLM_GSM_DEFAULT_SUBSCRIPTION; /* Use default */

         /* Set the core data */
         rfgsm_core_data.rf_task = RF_TASK_1;
         rfgsm_core_data.rfm_dev = device;
         rfgsm_core_data.sub_id = RFLM_GSM_DEFAULT_SUBSCRIPTION;
         rfgsm_core_data.as_id = SYS_MODEM_AS_ID_1; 
         rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
         rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;

         rfgsm_mc_set_antenna_req(&rfgsm_core_data, ant_sw_pos);
         RF_MSG_2( RF_HIGH, "rfgsm_mc_set_antenna_req device %d, ant_sw_pos = %d", device, ant_sw_pos);
      }
      else
      {
        RF_MSG_1( MSG_LEGACY_ERROR, "rfgsm_core_handle_ptr is NULL for device %d!", device);
      }
      #endif
    break;

  default:
    RF_MSG_2( RF_ERROR, "rfm_set_antenna_imm - Invalid Mode %d on device %d",
              rf_mode, device );
    break;
  }

  RF_MSG_4( RF_HIGH, "rfm_set_antenna_imm : [Status %d] Device %d, Tech %d, "
            "SwitchPos %d", ret_val, device, rf_mode, ant_sw_pos );

  #endif /* FEATURE_RF_ASDIV */

  return ret_val;

} /* rfm_set_antenna_imm */

/*----------------------------------------------------------------------------*/
/*
  @brief
This API will abort switch antenna diversity request sent by rfm_set_antenna, 
as long as the switching script has not been executed yet. 
 
  @details

  @param 
  device : RFM device of the controlling tech
  ant_sw_pos : Antenna Switch Position desired after aborting
  cb_func: Callback function pointer, called when switch is completed
  cb_data: callback function data
*/
boolean
rfm_set_antenna_abort
(
  rfm_device_enum_type device, 
  uint32 ant_sw_pos,
  rfm_l1_cb_type cb_func,
  void *cb_data
)
{
  boolean ret_val = TRUE;

  #ifdef FEATURE_RF_ASDIV

  rfm_mode_enum_type rf_mode =  RFM_INVALID_MODE;
  
  /* Param Validation */
  if ( (device < RFM_DEVICE_0) || (device >= RFM_MAX_WAN_DEVICES) )
  {
    RF_MSG_1( RF_ERROR, "rfm_set_antenna_abort - Invalid Device %d",
              device);
    return FALSE;
  } /* if ((device < RFM_DEVICE_0) || (device >= RFM_MAX_WAN_DEVICES)) */

  /* Get RFM device */
  rf_mode = rfm_get_current_mode(device);

  /* RFM call Logging for debug */
  RFM_DEBUG_EN_Q(&rfm_set_antenna_abort,device,ant_sw_pos,0,0);

  /* Call function pertaining to active tech on given device */
  switch (rf_mode) 
  {
  case RFM_EGSM_MODE:
  case RFM_EGSM_MODE_2:
  case RFM_EGSM_MODE_3:
    #ifdef FEATURE_GSM
    RF_MSG( RF_ERROR, "rfm_set_antenna_abort - GSM implementation needed");
    #endif
    break;
  case RFM_1X_MODE:
  case RFM_1XEVDO_MODE:
  case RFM_SVDO_MODE:
    #ifdef FEATURE_CDMA1X
    RF_MSG( RF_ERROR, "rfm_set_antenna_abort - CDMA implementation needed");
    #endif
    break;

  case RFM_IMT_MODE:
    #ifdef FEATURE_WCDMA
    RF_MSG( RF_ERROR, "rfm_set_antenna_abort - WCDMA implementation needed");
    #endif
    break;

  case RFM_LTE_MODE:
    #ifdef FEATURE_LTE
    rflte_mc_send_switch_ant_abort_msgr (device, ant_sw_pos, cb_func, cb_data);
    #endif
    break;
  case RFM_TDSCDMA_MODE:
    RF_MSG( RF_ERROR, "rfm_set_antenna_abort - TDS implementation needed");
    break;
  default:
    RF_MSG_2( RF_ERROR, "rfm_set_antenna_abort - Invalid Mode %d on device %d",
              rf_mode, device );
    break;
  }

  RF_MSG_4( RF_HIGH, "rfm_set_antenna_abort : [Status %d] Device %d, Tech %d, "
            "SwitchPos %d", ret_val, device, rf_mode, ant_sw_pos );

  #endif /* FEATURE_RF_ASDIV */

  return ret_val;

} /* rfm_set_antenna_abort */

/*----------------------------------------------------------------------------*/
/*
  @brief
  Update the RFM mode variable. Current API is primarily to be used by TDSCDMA tech which
  does not update the mode variable.
 
  @details
  Update the RFM mode variable. Current API is primarily to be used by TDSCDMA tech which
  does not update the mode variable.

  @param 
  mode : RF mode
  data: Device for which the mode setting update needs to be done
  
  @pre
*/

void rfm_mode_update(const rfcom_device_enum_type device,
                     const rfcom_mode_enum_type rf_mode)
{
    if ((device != RFCOM_TRANSCEIVER_0) || (rf_mode >= RFM_NUM_MODES))
    {
        MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, 
            "rfm_mode_update failed due to invalid parameters");
        return;      
    }

    rfm_mode[device]= rf_mode;
}

/*
  @brief
This API is used to check if a particular tech is supported on a particular config. 
 
  @details

  @param 
  
*/


boolean rfm_is_tech_supported(rfcom_device_enum_type device, rfm_mode_enum_type tech)
{

  boolean is_supported = FALSE;
  
  switch (tech) 
  {
  case RFM_EGSM_MODE:
  case RFM_EGSM_MODE_2:
  case RFM_EGSM_MODE_3:
    #ifdef FEATURE_GSM
    RF_MSG( RF_ERROR, "rfm_is_tech_supported - GSM implementation needed");
    #endif
    break;
  case RFM_1X_MODE:
  case RFM_1XEVDO_MODE:
  case RFM_SVDO_MODE:
    #ifdef FEATURE_CDMA1X
    RF_MSG( RF_ERROR, "rfm_is_tech_supported- CDMA implementation needed");
    #endif
    break;

  case RFM_IMT_MODE:
    #ifdef FEATURE_WCDMA
   // RF_MSG( RF_ERROR, "rfm_is_tech_supported - WCDMA implementation needed");
     is_supported = rfm_is_wcdma_supported();   
    #endif
    break;

  case RFM_LTE_MODE:
    #ifdef FEATURE_LTE
    RF_MSG( RF_ERROR, "rfm_is_tech_supported - LTE implementation needed");
    #endif
    break;
  case RFM_TDSCDMA_MODE:
    RF_MSG( RF_ERROR, "rfm_is_tech_supported - TDS implementation needed");
    break;
  default:
    RF_MSG_2( RF_ERROR, "rfm_is_tech_supported - Invalid Mode %d on device %d",
              tech, device );
    break;
 }

return(is_supported);


}


/*----------------------------------------------------------------------------*/
boolean
rfm_init_antenna
(
  uint32 ant_sw_pos
)
{
  boolean ret_val = TRUE;
#ifdef FEATURE_RF_ASDIV
  rfcommon_asdiv_init_switch_state(ant_sw_pos);
#endif
  return ret_val;
}


/*----------------------------------------------------------------------------*/
boolean rfm_is_coex_backoff_req (rfm_device_enum_type rfm_dev, rfm_mode_enum_type tech)
{
  boolean is_req = FALSE;

  switch (tech)
  {
    case RFM_TDSCDMA_MODE:
    #ifdef FEATURE_TDSCDMA

      is_req = rf_tdscdma_mc_is_coex_backoff_req(rfm_dev);
      RF_MSG_3( RF_HIGH, "rfm_is_coex_backoff_req rfm_dev = %d on tech %d. is_req=%d",
                rfm_dev, tech ,is_req );

    #endif
      break; 
  
    default:
      break;
  }
  return (is_req);
}


/*!--------rfm therm APIs for external use----------*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfm API to get the active number of thermistors
 
  @details
  rfm API to get the active number of thermistors

  @param   size : out parameter. Returns the number of active thermistors

*/
boolean rfm_get_thermistor_count(uint8* size_p)
{

  /*Check if rfm_init is complete*/
  if (!rfm_init_complete)
  {
    RF_MSG_1( RF_ERROR, "rfm_get_thermistor_count()  rfm_init_complete flag is %d. Returning FALSE",rfm_init_complete);
    return FALSE;
  }
  /*NULL pointer check*/
  if (size_p == NULL)
  {
    RF_MSG( RF_MED, "rfm_get_active_thermistor_size()  null pointer detected. Returning FALSE");
    return FALSE;
  }

  /*call the device interface api to get the number of active thermistors*/
  if(!rfdevice_get_thermistor_count(size_p))
  {
    RF_MSG( RF_MED, "rfdevice_get_active_thermistor_size() failed. Returning FALSE");
    return FALSE;
  }

  return TRUE;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfm API to enumerate and return the active thermistors for external use
 
  @details
  rfm API to enumerate and return the active thermistors for external use

  @param   size : in+out parameter. It specifies the number of entries provide in the array by the caller, 
                  and is updated to the number of thermistors that have been filled in after the call
  @param   therm_device_list : out parameter.Array of structures containing handles and any other associated data 
                      that may be used in the future by the thermal algorithm to improve their performance

*/
boolean rfm_enumerate_thermistors(uint8* size_p, rfm_thermistor_type* therm_device_list_p)
{

  /*Check if rfm_init is complete*/
  if (!rfm_init_complete)
  {
    RF_MSG_1( RF_ERROR, "rfm_enumerate_thermistors()  rfm_init_complete flag is %d. Returning FALSE",rfm_init_complete);
    return FALSE;
  }

  /*NULL pointer check*/
  if (size_p == NULL || therm_device_list_p == NULL)
  {
    RF_MSG( RF_MED, "rfm_enumerate_thermistors()  null pointer detected. Returning FALSE");
    return FALSE;
  }

  /*call the device interface api to enumerate the thermistors*/
  if(!rfdevice_enumerate_thermistors(size_p, therm_device_list_p))
  {
    RF_MSG( RF_MED, "rfdevice_enumerate_thermistors() failed. Returning FALSE");
    return FALSE;
  }

  return TRUE;

}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfm API to read and return the temperature for the requested thermistor 
 
  @details
  rfm API to read and return the temperature for the requested thermistor 

  @param  handle : in parameter to specify the thermistor to read temperature
  @param  result : out parameter which will be updated with the therm read value if the read was successfull

*/
boolean rfm_query_thermistor(rfm_thermistor_handle_type therm_handle, rfm_thermistor_result_type* therm_result_p)
{

  /*Check if rfm_init is complete*/
  if (!rfm_init_complete)
  {
    RF_MSG_1( RF_ERROR, "rfm_query_thermistor()  rfm_init_complete flag is %d. Returning FALSE",rfm_init_complete);
    return FALSE;
  }

   /*NULL pointer check*/
  if (therm_result_p == NULL)
  {
    RF_MSG( RF_ERROR, "rfm_query_thermistor()  null pointer detected. Returning FALSE");
    return FALSE;
  }

  if(rf_is_asleep())
  {
    /*returning default/invalid temperature value equivalent to -273 degree celcius (0 kelvin)*/
    therm_result_p->therm_read_deg_C = RFM_INVALID_TEMPERATURE;

    RF_MSG( RF_MED, "rfm_query_thermistor()  rf is in sleep state.");
    return TRUE;
  }

  /*call the device interface api to query temperature of the requested thermistor*/
  if(!rfdevice_query_thermistor(therm_handle, therm_result_p))
  {
    RF_MSG( RF_MED, "rfdevice_query_thermistor() failed. Returning FALSE");
    return FALSE;
  }

  return TRUE;

}

/*----------------------------------------------------------------------------*/
uint32 rfm_get_rx_lo_frequency(rfm_mode_enum_type tech, rfm_frequency_list_type freq_list)
{
  uint64 lo_freq = 0;
  uint32 i = 0;

  if (freq_list.num_valid_freq == 0)
  {
    RF_MSG_1( RF_HIGH, "Frequency List is Empty for Tech %d", tech);
    return lo_freq;
  }

  switch (tech)
  {
  #ifdef FEATURE_WCDMA
  case RFM_IMT_MODE:
    if (freq_list.num_valid_freq == 1)
    {
      lo_freq = freq_list.frequency_list[0];
    }
    else if (freq_list.num_valid_freq <= 2)
    {
      for (i=0;i<freq_list.num_valid_freq;i++)
      {
        lo_freq += freq_list.frequency_list[i];
      }
      lo_freq = lo_freq/freq_list.num_valid_freq;
    }
    else if (freq_list.num_valid_freq > 2)
    {
      RF_MSG( RF_HIGH, "num of Carrier >2 not supported in WCDMA");
    }
    else
    { 
      // Do nothing
    }
    break;
  #endif

  #ifdef FEATURE_CDMA1X
  case RFM_1X_MODE:
  case RFM_1XEVDO_MODE:
    if (freq_list.num_valid_freq == 1)
    {
      lo_freq = freq_list.frequency_list[0];
    }
    else if (freq_list.num_valid_freq <=3)
    {
      for (i=0;i<freq_list.num_valid_freq;i++)
      {
        lo_freq += freq_list.frequency_list[i];
      }
      lo_freq = lo_freq/freq_list.num_valid_freq;
    }
    else
    { 
      // Do nothing
    }
    break;
  #endif

  #ifdef FEATURE_LTE
  case RFM_LTE_MODE:
    if (freq_list.num_valid_freq == 1)
    {
      lo_freq = freq_list.frequency_list[0];
    }
    else if (freq_list.num_valid_freq > 1)
    {
      RF_MSG( RF_HIGH, "Multi-carrier is not supported for LTE");
    }
    else
    { 
      // Do nothing
    }
    break;
  #endif

  #ifdef FEATURE_TDSCDMA
  case RFM_TDSCDMA_MODE:
    if (freq_list.num_valid_freq == 1)
    {
      lo_freq = freq_list.frequency_list[0];
    }
    else if (freq_list.num_valid_freq > 1)
    {
      RF_MSG( RF_HIGH, "Multi-carrier is not supported for TDS");
    }
    else
    { 
      // Do nothing
    }
    break;
  #endif

  #ifdef FEATURE_GSM
  case RFM_EGSM_MODE_2:
  case RFM_EGSM_MODE:
  case RFM_EGSM_MODE_3:
    if (freq_list.num_valid_freq == 1)
    {
      lo_freq = freq_list.frequency_list[0];
    }
    else if (freq_list.num_valid_freq > 1)
    {
      RF_MSG( RF_HIGH, "Multi-carrier is not supported for GSM");
    }
    else
    { 
      // Do nothing
    }
    break;
  #endif

  default:
    RF_MSG_1( RF_HIGH, "Mode not supported %d", tech);
    break;
  }

  return ((uint32)(lo_freq & 0xFFFFFFFF));
}

/*!-------------------------------------------------*/

/* This routine should be called at the point where a WTR/ WXE dump is needed*/

void rfm_read_wtr_regs(uint8 register_set)
{
  uint8 dev_id =0;
  rfdevice_rxtx_common_class *temp_cmn_dev_ptr = NULL;

  RF_MSG_1(RF_HIGH, "rfm_read_wtr_regs: Dumping set %d (0 - cric, 1 - all)", register_set);

  for (dev_id = RFCMN_CORE_PHY_DEVICE_0; dev_id < RFCMN_CORE_MAX_PHY_DEVICES; dev_id++)
  {
    temp_cmn_dev_ptr = rfc_common_get_cmn_device_param(RFDEVICE_TRANSCEIVER, dev_id );
    if(temp_cmn_dev_ptr != NULL)
    {
      rfdevice_cmn_reg_dump(temp_cmn_dev_ptr,register_set);
    }
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update LTE 1Rx mode
 
  @details
  This function will be called by LTE L1 to update 1Rx mode, having this mode set
  LTE RFSW will not build/execute script for Drx path. 

  @param enable_1rx
  Boolean to set mode to true or false  
  
*/
#ifdef FEATURE_LTE
void rfm_set_1rx_mode(boolean enable_1rx)
{
  rflte_1rx_mode = enable_1rx;  
  RF_MSG_1(RF_HIGH, "rfm_set_1rx_mode: LTE 1Rx mode set to %d", rflte_1rx_mode);
}
#endif

