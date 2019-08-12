/*!
  @file
  rf_tdscdma_mc.c

  @brief
  This file contains the RF TDSCDMA main control call flows.

  @details
*/

/*===========================================================================
Copyright (c) 2011 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

						   EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/rf/mc/src/rf_tdscdma_mc.c#1 $
$DateTime: 2016/12/13 07:59:50 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/08/16   jr      Reduce F3 messages
04/12/16   jz      Store correct LNA offset for HL
03/02/16   nsh     Make sure the 1st Thermal used is read from WTR instead default value
02/01/16   whz     Save autopin WTR result
12/01/15   jr      Fix XPT IQ capture in Pin CHAR
01/14/16   nsh     Fix power jump issue during max power
11/20/15   jr      Added WTR LPM feature changes
11/18/15   whz     Push AutoPin config to DM
                   Use global variable to remember cmn_device_ptr
11/05/15   whz     AutoPin on TA: online support
11/04/15   kma     Updated WTR interface
10/06/15   whz     Add support to AutoPin script for TA
09/27/15   jak     Fix rf_tds_bc_config mask for B39 in enter mode
09/12/15   jr      Abort pending AsDiv Event
04/08/15   lwu     Fixed BHO issue due to IMM write the script while update_bho   
04/08/15   jz      Disable ASM rx before sleep
03/17/15   whz     Fix TempComp on Jolokia
03/04/15   jz      Enable XO cal on TDS node
01/31/15   jak     Fix asdiv tune code update called in sleep state
01/30/15   jyu     Fixed the bug for band port update after device and/or alt_path change
                   (need to update for all bands and for PRx and DRx 
                    regardless if RxD is on or not)
01/28/15   whz     Keep the original sequence for HDET in enable_tx
01/26/15   jz      Fix Jolokia HDET/MTPL functionality
01/16/15   nsh     Move IP2 Cal script to enter mode
01/13/15   jyu     Fix LNA gain stuck issue in N-FREQ
12/30/14   jak     Added RFNV_TDSCDMA_<band>_MPR_VAL_V2_I support
12/29/14   whz     Fix FW standalone build error due to is_sawless_enabled
12/26/14   jz      Enable jdetInfo when it's sawless card
12/22/14   whz     Skip Rx burst script in RF Cal
12/16/14   jyu     Fixed T2X/X2T/T2T tuner issue
12/12/14   nsh     Pass immediate flag in tds asdiv api
12/10/14   jyu     Fixed bug introduced by separation of RF WAKEUP_STEP0 into 0p1 and 0p2
12/05/14   jyu     Separate RF WAKEUP_STEP0 into 0p1 and 0p2 for wakeup timeline optimization
11/14/14   jyu     Assert the case when ftm_concurrency_validate() return FALSE
11/13/14   whz     Enable HDET on Jolokia
11/12/14   jyu     MDSP/SMEM cleanup
11/10/14   jyu     Add message field to indicate if use_sw_tune_in_acq and to support 
                   CMAPI type 4 (for RxD override)
11/05/14   jyu     Call rfm_enter_mode() in mc_enter_mode()
11/03/14   jyu     RF calibration time reduction
10/29/14   jyu     Fixed the bug when updating rfm_mode[] in wakeup and enter_mode
10/22/14   ck      adapt the hooking to ftm conmgr due to interface change
10/22/14   kr      Fix compiler warning
10/15/14   jyu     Added support for ftm_concurrency_manager_validate_operation()
10/15/14   jyu     Fixed bug for IFREQ script re-building
10/15/14   jyu     Changed to execute txWakeup script during enable_tx()
10/07/14   ych     Matched ant_pos from both L1 and setting to ASM script at tuning channel
10/03/14   ych     Executed xsw immediate writes in retuning channel to avoid ASM overwriting
10/02/14   ych     Save ant position in enter mode for the use in RF sleep/wakeup
10/01/14   kb      [JOLOKIA] RUMI hack for RF sleep/wakeup
09/30/14   ndb     Pass "rflm_handle" in tuner_cb to help with debug
09/28/14   ms      Fix bug for populating HL/LL NVs using new RX NV structure
09/26/14   jyu     Added support to calibrate different alt_path
09/25/14   jyu     Added support for pre-processing issue_seq (for FED TS0 timeline reduction)
09/23/14   jyu     Added support for DR-DSDS X2T IRAT to change RxLM mode inside the gap
09/20/14   jyu     Integrated Tx WXE into MC/MDSP
09/18/14   ych     Enabled QTUNER Tx in tuning to some chan
09/17/14   kb      Clean up FEATURE_DIMEPM_MODEM from mainline
09/16/14   jyu     Need to re-build those scripts that are not double buffered
09/16/14   jyu     Support to record WB ADC for primary and secondary logic devices
09/10/14   ych     Fix Tuner callflow issues
09/10/14   ych     Disabled rfnv_tdscdma_b40.c and made corresponding compilation/cw/kw changes
09/01/14   jyu     Need to re-build Rx LNA script when logic_device changes
08/28/14   kg      Pass tech_id in tuner_cb to help with debug
08/20/14   kb      Set FED RUMI flag in enter mode if the MC RUMI flag is set
08/19/14   jyu     Need to release and request TQs when logic device change on wakeup
08/13/14   nsh     rf_tdscdma_mc_switch_ant need to return certain value for TL1
08/12/14   nsh     Avoid building ASDIV script when TL1 request ASDIV in wake up/enter mode  for TDSCDMA
08/06/14   jyu     Put ASM and WTR into deep sleep mode in mc_exit_mode
08/05/14   jhe     Separate mtpl for coex 
08/02/14   jyu     Enforce to abort AOL script when RxLM handle changing
07/26/14   lcl     Fix OFT compilation error
07/25/14   ms      Not perform tuner_disable in sleep->exit transition
07/23/14   kb      Jolokia RUMI brung up.
07/17/14   ms      Sawless RX support
07/16/14   kb      Add support for HDET trigger API
07/09/14   ms      Move mdsp_init and LNA script building to enable_rx to avoid crash in DSDA on DPM
06/28/14   jyu     Added support to build Tx Non-TRX one time init script for BHO and 
                   single-Tx DSDA
07/01/14   jyu     Can't put phone into park mode when no TRM lock
06/30/14   jyu     Return rebuild_ifreq script info to L1
06/23/14   jyu     Fixed Rx ASM wakeup sequence issue
06/20/14   jyu     Added support to query alt_path from RFC for DR-DSDS
06/20/14   ych	   Add tuner_disable for both sleep/exit and Tx mode in IRAT
06/17/14   jyu     Added fix when calling RF exit_mode() without TRM lock
06/12/14   jyu     Updated support for dual-sim dsds sleep/wakeup
06/12/14   jz      Fix false alarm in exit_mode
06/09/14   sty     Set the use_events field to FALSE
06/06/14   jyu     Fixed Tx ASM wakeup sequence issue
06/05/14   jyu     Added support to separate non-trigger and trigger writes
06/05/14   yb      Tuner force_detune() for supporting FTM_PROGRAM_QFE_SCENARIO
05/29/14   ych     Updated for 8 PA gain states
05/28/14   whz     TQ release/request in sleep/wakeup for DPM
05/27/14   jhe     Update txlim handling
05/19/14   jhe     BHO FBrx update
05/13/14   jyu     Fixed memory leakage when HW doesn't support tuner
05/13/14   ych     Assigned NULL to tuner_event_handle_ptr
05/06/14   jps     TDS port switching fixes
05/05/14   jyu     Added support to abort AOL when in mc_exit_mode()
05/05/14   sn      Added ASM sleep wakeup callflow
05/05/14   jyu     Added support for DSDS sleep/wakeup
05/03/14   ych     Added a msg print-out for ifreq_meas_freq
05/02/14   jps     TDS port switching fixes
04/30/14   jhe     Call fbrx api in cal mode too
04/28/14   jps     Support for TDS port switching
04/24/14   ych     Added dynamic tuner script for ifreq
04/23/14   jyu     Added support to build Tx wakeup script and execute through FED
04/15/14   jyu     Added support to configure DC cal
04/12/14   jyu     Added single_valid_band into mc_state variable
04/10/14   jyu     Added support for Tuner Close Loop (CL)
04/09/14   jyu     Added support for Tuner Advanced Open Loop (AOL)
04/06/14   jyu     KW fix
04/03/14   jyu     Added support for port A/B feature
03/26/14   as      Added callback function as part of antenna switch
                   (porting per CR629532)
03/26/14   as      Deprecated asdiv_obj from config_switch API
                   (porting per CR629532)
03/26/14   ms      Disable tuner in X2T iRAT case (porting per CR600367)
03/24/14   jyu     Added support for asdiv
03/20/14   jhe     temp comp update
03/10/14   jyu     Added support for therm read
03/07/14   hm      FW Decoupling of rfdevice_interface and WTR3925
03/03/14   aa      Fix off-target error
03/01/14   jyu     Fixed the bug that PRxDRx script might not get updated
                   after cell reselection
02/28/14   jyu     Added support to allow RF wakeup in parallel with MCPM
02/28/14   jyu     Added tx timeline profiling
02/27/14   aa      Update rfcommon_fbrx_mc_tune_to_chan
01/30/14   aa      Fixed bug in rfcommon_fbrx_mc_init_tx
02/02/14   jyu     Updated X2T IRAT RxAGC support
01/28/14   jps     Use one API to setup TX data
01/24/14   jps     FBRX warning fix
01/25/14   jyu     Added support for RxAGC timeline optimization
01/24/14   jps     FBRX warning fix
01/23/14   jyu     Rx wakeup timeline optimization
01/16/14   jps     Intial FBRX support
01/16/14   jyu     Added sleep/wakeup timeline detailed profiling
01/15/14   jyu     Fixed RxAGC bug for IFREQ
01/10/14   jyu     Disabled SW tune by default
01/06/14   jyu     Fixed bug for power collapse (don't need to call mdsp_init/exit
                   during the sleep/wakeup)
12/15/13   jps     Fixes for RFLM based TXAGC override
12/10/13   jyu     Updated set_lna_range()
12/10/13   jyu     Updated RxAGC
12/04/13   qzh     Fix KW errors
11/19/13   nsh     FED RxAGC Feature
11/15/13   jyu     Force to use SW tune (before fully verify FED slot update)
11/15/13   jyu     Added support to set either Tx or Rx ASM with SW tune
10/30/13   jps     Updates and fixes for common TXAGC hookup
10/26/13   nsh     Fix calibration timing due to fix AGC in config_tx_chain
10/24/13   ych/jyu Bypass TxAGC on rumi
10/24/13   zg      Fixed Tx unlock issue on FTM mode.
10/23/13   jyu     Added the check and not to build scripts when
                   in calibration mode
10/21/13   jyu     Added support to release TQs when in sleep and
                   request TQs in wakeup
10/20/13   jyu     Re-defined the feature RF_TDSCDMA_USE_SW_TUNE
10/14/13   jyu     Changed MC device call flow
10/14/13   jyu     Added work-around for Rumi bring-up
10/08/13   nshen   Avoid NULL script sent for ASDIV in IMM mode
10/08/13   jyu     Added device init call in wakeup and exit in sleep
10/04/13   jps     Fixes for IQ capture in FTM calibration mode
10/02/13   jyu     Added support to bypass loading TxLIN on Rumi
                   (enable/disable by a flag and by dafault will disable)
10/02/13   jyu     Added support for SW tune (by default will disable)
10/01/13   jyu     Added PA_sleep/wakeup
09/23/13   ms      Reset WTR on exit_mode
09/12/13   jyu     Changed to use set_lna_gain_state data type to set LNA
09/09/13   ms      Add AsDiv abort and skip AOL update if switch is in progress
09/09/13   ms      Fixed bug in update_tuner (check for diversity and pass correct rf_state)
09/04/13   ms      Added hdet init and program_tx_mode in update_bho()
08/30/13   ych     Change Macro to number to avoid compling error in BO
08/28/13   ych     Report warmup time to L1
08/27/13   whz     Make sure thermal mitigation is executed only during online call
08/22/13   jr      Removed API rf_tdscdma_core_util_calc_tx_cal_chan_freq()
08/22/13   jyu     Re-organized the code to allow better sharing between
                   configure prx and drx and between enable_rx() and wakeup()
08/21/13   jhe     Update to support DPD
08/20/13   jyu     Changed the call sequence in enter_mode() and exit_mode()
08/16/13   ndb     Clean-up the SPI based antenna tuner support
08/12/13   ych     kw fix
08/08/13   ych     Changed the sequence in ACQ to program LNA before PLL
08/05/13   jyu     Moved mdsp_init() into enable_rx()
08/01/13   st      Remove Un-used LM Includes
07/26/13   ms      AsDiv support
07/23/13   jz      Check RxD_state as well as rx_channel when wake up
07/21/13   jr      Remove rf_tdscdma_core_util_calc_tx_cal_chan_freq() in rf_tdscdma_mc_select_band_tx
07/17/13   ms      Support for update tune code
07/08/13   ms      Support for program_qfe_scenario
06/30/13   qzh     Replace memcpy() with safer memscpy().
06/26/13   jyu     Added support to remove the requirement to specify band info
                   when it is not ready
06/18/13   ms      Change default band to B34
06/10/13   ms      API for channel to band conversion for L1
06/05/13   kb      [XPT] Support to update rfm_mode for TDS in online mode
06/03/13   nsh     Support Get Multi Synth for Dime
05/25/13   qzh     Porting FTM RF code from NIKEL to DIME
05/23/13   jps     Fix FTM RF TX mutex implementation
05/20/13   al      Added support for split band
05/16/13   jyu     Added pa_config in enable_tx()
05/14/13   zg      Featured out RSB temp comp for TRITON modem.
05/14/13   zg      Enabled RSB temp comp
05/14/13   ms      Added program_tx_mode for tuner
05/13/13   jyu     Fixed a bug that could enable debug messages accidently
05/09/13   whz     Cleanup the legacy codes not needed by both Dime and Triton
05/06/13   qzh     Comment out rf_tdscdma_rf_mode_tx_lock about reverting its init in enater mode
05/06/13   ms      Revert critical section init in enater mode to avoid boot up crash
05/04/13   whz     Featurize the definition of tune_rx_pll for Triton to remove a compilation warning
05/03/13   ms      Fixed call flow bug for tuner init
05/03/13   ms      Fixed bug in enable_tx for using tx_channel
05/02/13   whz     Move Rx SBI programming from enable_tx to enable_rx/wakeup for Triton
05/01/13   qzh     Porting FTM RF code from NIKEL to DIME
04/30/13   jyu     Enabled debug messages
04/24/13   jyu     Added support to allow pass multiple script buffers for script building
04/23/13   jyu     Changed to configure TUNER only if in ACQ or PWR_SCAN mode
04/23/13   jyu     Removed hard-coded swpt mode for diversity chain
04/23/13   jyu     Updated API for tuner programing
04/15/13   jyu     Implemented the change to clean up GRFCs when in TRK mode
04/13/13   jyu     Added support to enable debug flags by NV setting
04/02/13   sn      Cleanup ASM interface
03/28/13   gy      Notify GNSS of TDSCDMA status for concurrency
03/27/13   ms      Edit HDET/Therm debug message
03/26/13   ms      Fix bug for tx_freq in hdet_init
03/25/13   jyu     Added more message for HDET/Therm debug
                    (enable/disable with a flag)
03/22/13   sr      changes to PAPM device interface.
03/21/13   ms      Tuner TDET support
03/20/13   jyu     Added support to query HDET info from Device
03/15/13   jyu     RF wakeup timeline optimization
03/13/13   jyu     Not enabled Tx_ON when enable Tx as it might have an
                   side effect on Rx (especially for WTR2605)
03/11/13   jyu     Added support to allow L1 update RxLM for L2T test case
03/11/13   jyu     Change to write primary and secondary chain common table
                   from devices
03/07/13   jyu     Added support for online support for WTR2605
03/07/13   sr      changed the "pwr_tracker" name to "papm"
02/22/13   jyu     Added more debug message to check if L1 passes correct info
02/22/13   jyu     Added support for online HDET (temporarily disabled MTPL
                   and wait for temp comp become ready and then enable MTPL)
02/13/13   jyu     Added more debug messages for diversity
02/08/13   jyu     Removed hard-coded device info
01/08/13   jyu     Removed hard-coded band info
12/21/12   jyu     Integrated with rfdevice TDSCDMA ASM APIs
12/10/12   jyu     Set RUMI bringup flag = FALSE
12/03/12   jyu     Enabled time profiling to check timing critical APIs
12/04/12   ms      APT support
12/03/12   jyu     Added support to pass correct primary/diversity device
                  for script building
11/19/12   jyu     Added initial Tuner support for TDSCDMA to enable HDET
11/15/12   jyu     Enable TxAGC
11/15/12   ms      Removed rf_tdscdma_debug_flags.rumi_bup_tdscdma flag for tx lin
11/12/12   jyu     Updated the event script build cmd id
11/02/12   jfc     Replace rf_hal_enable_rfcmd_app with rf_hal_bus_enable
10/29/12   sty     Renamed interface and type definitions for timer APIs
10/10/12   ms      Moved IRAT APIs to rf_tdscdma_mc_meas.c
10/08/12   kb      Moved rf_timetick_get_difference() inside tdscdma_display_time_profile flag
10/04/12   sr      Added IRAT APIs.
10/03/12   kb      Fix Off-target compiler errors.
10/02/12   kb      Add Time Profiling to Enter mode, Rx config and Tx config
                   with a global flag defaulted to false, which can be
                   turned on if time profile is desired in F3 msgs
09/19/12   ms      Diversity support for wakeup and BHO
09/19/12   kb      Enabled vote for RFCMD app
09/17/12   jyu     Added a temp flag to switch between SW tune and dynamic switch
09/11/12   jyu     MC code cleanup
09/11/12   jyu     Move all ant_tuner code under !FEATURE_DIME_MODEM
09/10/12   ms      Diversity support for FTM
08/24/12   jyu     Added a flag to enable/disable Rf script building during bring-up
08/20/12   jyu     Added support to set/clean specific GRFC (immediate execution)
08/20/12   jyu     Added support to cleanup GRFCs
08/19/12   jyu     Changed to use B40 as default due to TDS card support B40 only
08/16/12   jyu     Integrated QPOET driver
08/13/12   jyu     Updated interface to take rfm_device_enum_type and for both
                   primary and secondary device
08/13/12   jyu     Integrated with PA/ASM interface
08/02/12   nsh     Interface change from "PATH" to "DEVICE"
08/01/12   jyu     Added support for RF script buffer
07/31/12   qzh     Rfdevice interface change to use rfm_device instead of path
07/18/12   jps     TD-SCDMA RF Cal should use FTM_SET_CALIBRATION_STATE
07/16/12   jps     Initial FTM burst TX/RX framework
07/11/12   jps     TD-SCDMA Antenna Tuner Feature
07/18/12   jyu     Added support for RUMI bring-up
07/02/12   kb      updated API functions to support script buffers
06/28/12   zg      KW fix.
06/06/12   jyu     Remove MODEM specific setting from RF TDSCDMA MC/MEAS
06/13/12   jhe     Multi-lin NV update.
06/07/12   ycl     Fix erroneous error message.
05/17/12   jyu     Fix link error by removing HWIO_OUT
05/04/12   ycl     Fix to match max power NV changed from 230 to 930
05/04/12   ycl     Fix to match max power NV changed from 230 to 930.
05/02/12   zg      Added fatal message in rf_tdscdma_mc_update_rx_swpt.
05/02/12   ycl     BHO change.
04/24/12   jfc     Remove unused headers and hack out HWIO
04/18/12   ycl     Fix compiler warnings.
04/18/12   ycl     Added one more threshould to RxAGC ACQ mode.
04/16/12   jhe     Fix the FTM PDET cal bug
04/12/12   ycl     RFC GRFC/GPIO sleep.
04/10/12   hy      Remove reference to rf_tdscdma_msm_set_rx_on
04/07/12   ycl     Added DAC0_UPDATE GRFC.
03/26/12   jyu     Added support for PDET cal
03/26/12   zg      Put device in LPM before TXCO voting in sleep function.
03/23/12   zg/jhe  Added Rx channel infor into rf_wakeup.
06/05/12   jyu     Fix compilation for RxLM/TxLM
03/09/12   zg      Enabled power collapse in rf sleep and removed obsolete
                   TCXO shutdown API.
03/06/12   jyu     Updated rfdevice cmd ID
03/02/12   ycl     Update RxLNA switch points.
02/13/12   jyu/pv  RF Common Core Device Manager Implementation.
02/13/12   jyu     Added RF support for HDET online operation
02/10/12   zg/jhe  Enabled sleep and wakeup
12/09/11   jhe     BHO update
12/07/11   jyu     Updated device cmd id to build TDS standalone scripts
11/29/11   jyu     Removed RX_ON toggling
10/21/11   jhe     Added basic ssbi script
10/03/11   jyu     Updated rfdevice callflow for standalone TDSCDMA
09/12/11   jhe     Minor cleanup.
09/06/11   ycl     Change to start TDSCDMA manually.
08/31/11   jyu     Updated GRFC and SSBI sequence per systems recommendation
08/01/11   jyu     Added initial support from rfdevice driver
07/01/11   jhe     Initial version.

============================================================================*/
#include <stringl/stringl.h>

#include "rfcom.h"
#include "msg.h"
#include "rf_tdscdma_mc.h"
#include "rfcommon_core.h"
#include "rfcommon_core_cb.h"
#include "rfcommon_nv_mm.h"
#include "rfcommon_nv.h"
#include "rfmeas_mc.h"
#include "rfnv_tdscdma.h"
#include "rf_tdscdma_core_rxctl.h"
#include "rf_tdscdma_core_temp_comp.h"
#include "rf_tdscdma_core_xpt.h"
#include "rf_tdscdma_core_txctl.h"
#include "rf_tdscdma_core_txlin.h"
#include "rf_tdscdma_core_txplim.h"
#include "rf_tdscdma_core_util.h"
#include "rfdevice_tdscdma_intf.h"
#include "rfdevice_hdet_tdscdma_intf.h"
#include "rfc_card_tdscdma.h"
#include "rf_tdscdma.h"
#include "ftm.h"
#include "rf_tdscdma_mdsp.h"
#include "rf_tdscdma_msm.h"
#include "pm.h"
#include "DALSys.h"
#include "rfm_types.h"
#include "msm.h"
#include "timetick.h"
#include "rfcommon_core_device_manager.h"
#include "rfdevice_tdscdma_type_defs.h"
#include "rfm_internal.h" // For RUMI Bring up
#include "rfdevice_tdscdma_asm_intf.h"
#include "rfdevice_tdscdma_pa_intf.h"
#include "rfdevice_tdscdma_papm_intf.h"
#include "rf_hal_common.h"
#include "rfcommon_time_profile.h"
#include "rfmeas_mc.h"
#include "rf_tdscdma_mc_meas.h"
#include "rfcommon_locks.h"
#include "rfm.h"
#include "rfcommon_mdsp_types.h"
#include "rfcommon_mc.h"
#include "rfcommon_mdsp.h"
#include "rfcommon_mdsp_event_intf.h"
#include "rfcommon_atuner_intf.h"
#include "rfdevice_cmn_type_defs.h"
#include "modem_mem.h"
#include "rf_tdscdma_mdsp_rf_event.h"
#include "rf_tdscdma_mdsp_smem_intf.h"
#include "rf_tdscdma_core_txctl.h"
#include "rfcommon_concurrency_manager.h"
#include "ftm_common_concurrency_manager.h"
#include "rfcommon_core_utils.h"

#include "assert.h"


#ifdef FEATURE_RF_HAS_QTUNER
#include "rf_tdscdma_core_antenna_tuner.h"
#include "rf_tdscdma_core_tuner_temp_comp.h"
#include "rfcommon_mdsp_tuner.h"
#include "rfcommon_tuner.h"
#endif

#ifdef FEATURE_RF_ASDIV
#include "rfcommon_asdiv_manager.h"
#include "rflm_cmn_asd.h"
#endif

#include "rflm_dm_api.h"
#include "rflm_rxagc_api.h"
#include "rflm_api_tds.h"
#include "rfcommon_fbrx_api.h"
#include "rflm_tds_mc.h"
#include "ftm_tdscdma_ctl.h"

#include "rfcommon_autopin_api.h"
#include "rfdevice_cmn_intf.h"


#define RF_TDSCDMA_FUNCTION_OVERWRITE_ALLOWED                  7

/* RF_TDSCDMA_MC_DEBUG: RESERVED 16 BITS */
#define RF_TDSCDMA_MC_DISPLAY_TIME_PROFILE                     8
#define RF_TDSCDMA_MC_WAKEUP_TIME_DEBUG                        9
#define RF_TDSCDMA_MC_HDET_DEBUG                              10
#define RF_TDSCDMA_MC_THERM_DEBUG                             11

#define RF_TDSCDMA_MC_ENABLE_MTPL                             16
#define RF_TDSCDMA_MC_ENABLE_THERM                            17
#define RF_TDSCDMA_MC_ENABLE_HDET_CLASS                       18

/* RF_TDSCDMA_MDSP_DEBUG: RESERVED 16 BITS */
#define RF_TDSCDMA_MDSP_RF_EVENT_SCRIPT_DISPLAY_TIME_PROFILE  24
#define RF_TDSCDMA_MDSP_DISPLAY_SUCCESSFUL_SCRIPT             25
#define RF_TDSCDMA_MDSP_TASKNUM_UPDATE_DEBUG                  26
#define RF_TDSCDMA_MDSP_RFC_TIMING_DEBUG                      27
#define RF_TDSCDMA_MDSP_BUILD_DYNAMIC_UPDATE_DEBUG            28

#define RF_TDSCDMA_MDSP_DISABLE_APT_UPDATE                    32
#define RF_TDSCDMA_MDSP_DISABLE_PA_UPDATE                     33
#define RF_TDSCDMA_MDSP_DISABLE_LNA_UPDATE                    34
#define RF_TDSCDMA_MDSP_DISABLE_TUNER_UPDATE                  35
#define RF_TDSCDMA_MDSP_DISABLE_ASM_UPDATE                    36

/* RF_TDSCDMA_DEVICE_DEBUG: RESERVED 8 BITS */
#define RF_TDSCDMA_DEVICE_SSBI_DEBUG                          40
#define RF_TDSCDMA_DEVICE_HDET_DEBUG                          41
#define RF_TDSCDMA_DEVICE_THERM_DEBUG                         42
#define RF_TDSCDMA_DEVICE_DISABLE_CONFIG_DC_CAL               43

/* RF_TDSCDMA_FED_DEBUG: RESERVED 8 BITS */
#define RF_TDSCDMA_FED_DISABLE_SLOT_UPDATE                    49
#define RF_TDSCDMA_FED_DISABLE_TXAGC                          50
#define RF_TDSCDMA_FED_DISABLE_RXAGC                          51
#define RF_TDSCDMA_FED_DISABLE_PDET                           52
#define RF_TDSCDMA_FED_DISABLE_THERM                          53
#define RF_TDSCDMA_FED_DISABLE_AOL                            54
#define RF_TDSCDMA_FED_DISABLE_ASDIV                          55
#define RF_TDSCDMA_FED_DISABLE_CL                             56
#define RF_TDSCDMA_FED_DISABLE_HEADSTART                      57
#define RF_TDSCDMA_FED_DEBUG_PEDT                             58
#define RF_TDSCDMA_FED_DEBUG_THERM                            59
#define RF_TDSCDMA_FED_DISABLE_FBRX                           60
#define RF_TDSCDMA_FED_PRINT_DIAG_MESSAGES                    61
#define RF_TDSCDMA_FED_DISABLE_DIAG_ERROR_MESSAGE             62






/* *****************************************************************
                                              DEBUG FLAGS
********************************************************************/

/* Time profiling flag */
boolean rf_tdscdma_mc_display_time_profile = FALSE;

/* Wakeup timeline debug flag */
boolean rf_tdscdma_mc_wakeup_time_debug = FALSE;

/* HDET MC debug flag */
boolean rf_tdscdma_mc_hdet_debug = FALSE;

/* Therm MC debug flag */
boolean rf_tdscdma_mc_therm_debug = FALSE;

/* RxAGC debug flag */
boolean rf_tdscdma_mc_rxagc_debug = FALSE;

/* Tx timeline debug flag */
boolean rf_tdscdma_mc_tx_time_debug = FALSE;

extern boolean rflm_tds_use_on_rumi;

/* *****************************************************************
                                           FUCNTIONAL FLAGS
********************************************************************/

/* Added a flag to enable/disable RF SW tune during bring-up */
#if RF_TDSCDMA_USE_SW_TUNE
boolean rf_tdscdma_enable_sw_tune = TRUE;
#else
boolean rf_tdscdma_enable_sw_tune = FALSE;
#endif /* #ifdef  */

/* Choose Tx or Rx ASM with SW tune */
boolean rf_tdscdma_use_tx_asm_in_sw_tune = TRUE;

/* PDM value for SW tune */
uint32 rf_tdscdma_pdm = 80;

/* Whether to enable bursty mode in FTM  */
boolean rf_tdscdma_enable_ftm_bursty_mode = FALSE;

/* Whether use WTR HDET (hard-coded) or HDET adaptor class */
boolean is_wtr_hdet=FALSE;

/* HDET-based max power limiting Debug Variable */
boolean rf_tdscdma_mc_enable_hdet_max_pwr_limiting = TRUE;

/* Power Collapse Debug Variable - set to TRUE for bring up */
boolean rf_tdscdma_disable_powercollapse = FALSE;

/* Debug flag bitmask */
uint64 tdscdma_debug_flag_bitmask = 0x0;

/* RF TDSCDMA flag on Rumi*/
boolean rf_tdscdma_use_on_rumi = FALSE;

/* RxAGC Freq Comp Info*/
extern rf_tdscdma_core_rxctl_data_type rf_tdscdma_rxfreq_comp;

/* FBRX LM Handle */
/*! @todo Needs to be moved */
static lm_handle_type rf_tdscdma_fbrx_lm_handle;

/* *****************************************************************
                                       LOCAL DEFINITIONS
********************************************************************/

rf_tdscdma_mc_lm_buf_type rf_tdscdma_mc_lm_bufs =
{
  {{FALSE,0,RFA_TDSCDMA_RXLM_MODE_MAX},{FALSE,0,RFA_TDSCDMA_RXLM_MODE_MAX}},
  {{FALSE,0,RFA_TDSCDMA_RXLM_MODE_MAX},{FALSE,0,RFA_TDSCDMA_RXLM_MODE_MAX}},
  {FALSE,0},
  {FALSE,0}
};


/* *****************************************************************
                                                  LOCAL VARIABLES
********************************************************************/
rf_tdscdma_mc_dbl_buf_type rf_tdscdma_mc_dbl_bufs;

rf_tdscdma_rfmode_cmd_rsp_status_type
rf_tdscdma_rfmode_multi_synth_status = TDSCDMA_RFMODE_CMD_RSP_IDLE;
uint8 rf_tdscdma_rx_pll_status = 0;
uint8 rf_tdscdma_tx_pll_status = 0;

// TO-DO: Need to clean this flag
rf_tdscdma_bup_flags_t rf_tdscdma_debug_flags = {0};

/* MC State variable to hold the state on each path.
   Also stores rx and tx band and frequency, etc */
rf_tdscdma_mc_state_type rf_tdscdma_mc_state =
{
  {RF_TDSCDMA_STATE_DEFAULT, RF_TDSCDMA_STATE_DEFAULT, RF_TDSCDMA_STATE_DEFAULT, RF_TDSCDMA_STATE_DEFAULT},
  RFCOM_BAND_TDSCDMA_INVALID,
  RFCOM_BAND_TDSCDMA_INVALID,
  0,
  0,
  RF_PATH_0,
  TRUE,
  FALSE,
  FALSE,
  FALSE,
  FALSE,
  RFM_INVALID_DEVICE,
  RFM_INVALID_DEVICE,
  RFA_TDSCDMA_RX_PATH_PRIM,
  0,
  0xFF,
  FALSE,
  FALSE,
  RFCOM_BAND_TDSCDMA_INVALID, //rfcom_tdscdma_band_type single_valid_band
  RF_TDSCDMA_PORT_STATE_A,
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},  
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  RF_TDSCDMA_RX_LIN_STATE_LOW,
  0xFF, // last_rxlm_handle_for_AOL_update
  RFM_INVALID_DEVICE, // last_pri_device_used_to_build_rx_lna_scpt
  {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE}, // rx_lna_script_built_on_bands[RFCOM_NUM_TDSCDMA_BANDS]
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},  // scell_wb_adc_index_on_devices
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},  // ncell_wb_adc_index_on_devices
  0,                                     // curr_ant_sw_pos ("0" is default position)
  0                                      // next_ant_sw_pos ("0" is default position)
};

/* RFCMD app client handle */
rf_hal_bus_client_handle* rf_tdscdma_hal_bus_client_handle;

static rex_crit_sect_type rf_tdscdma_rf_mode_tx_lock;
boolean rf_tdscdma_rf_mode_tx_lock_init_once = FALSE;

/* Status of RF Mode TX & RX command */
rf_tdscdma_rfmode_tx_cmd_rsp_status_type rf_tdscdma_rfmode_tx_cmd_rsp_status = TDSCDMA_RFMODE_TX_CMD_RSP_IDLE;
rf_tdscdma_rfmode_rx_cmd_rsp_status_type rf_tdscdma_rfmode_rx_cmd_rsp_status = TDSCDMA_RFMODE_RX_CMD_RSP_IDLE;

rf_tdscdma_rfmode_cmd_rsp_status_type rf_tdscdma_rfmode_iq_capture_status = TDSCDMA_RFMODE_CMD_RSP_IDLE;

//rf_tdscdma_port_state_enum_type rf_tdscdma_mc_port_state = RF_TDSCDMA_PORT_STATE_A; //initialize to A

int16 rf_tdscdma_coex_tx_pwr_limit = 300; //need to use MACRO from MCS

rfdevice_rxtx_common_class *rf_tdscdma_mc_cmn_device_ptr = NULL;



/* *****************************************************************
                                                 LOCAL FUNCTIONS
********************************************************************/
void rf_tdscdma_mc_update_rx_mdsp
(
  rfm_device_enum_type device,
  rf_path_enum_type path,
  rfcom_tdscdma_band_type band,
  uint16 rx_channel,
  lm_handle_type rxlm_buffer_idx,
  rfa_tdscdma_rxlm_mode_t rxlm_mode,
  boolean call_in_wakeup,
  uint8 buf_id,
  boolean call_for_bho_update,
  rfa_tdscdma_wakeup_indicator_t wakeup_step_ind
);

void rf_tdscdma_mc_config_rx_chain
(
  rfm_device_enum_type device,
  rf_path_enum_type path,
  rfcom_tdscdma_band_type band,
  uint16 rx_channel,
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  rfa_tdscdma_rx_path_t   rx_path,
#endif
  lm_handle_type rxlm_buffer_idx,
  rfa_tdscdma_rxlm_mode_t rxlm_mode,
  boolean call_in_wakeup,
  uint8 buf_id,
  uint8 rx_lin_state,
  boolean use_sw_tune_in_acq  
);

void rf_tdscdma_mc_update_tx_mdsp
(
  rfm_device_enum_type device,
  rf_path_enum_type path,
  rfcom_tdscdma_band_type band,
  uint16 tx_channel,
  lm_handle_type txlm_buffer_idx,
  uint8 buf_id,
  boolean is_bho_update
);

void rf_tdscdma_mc_config_tx_chain
(
  rfm_device_enum_type device,
  rf_path_enum_type path,
  rfcom_tdscdma_band_type band,
  uint16 tx_channel,
  lm_handle_type rxlm_buffer_idx,
  uint8 buf_id
);


rflm_tds_rx_mode_e rf_tdscdna_mc_rxagc_convert_rxlm_mode
(
  rfa_tdscdma_rxlm_mode_t tds_rxlm_mode,
  boolean is_ifreq
);

extern rf_tdscdma_ftm_burst_tx_params_type ftm_tdscdma_get_rf_mode_tx_params(void);

extern uint32 ftm_tdscdma_get_lm_idx(void);

extern boolean rfnv_tdscdma_b34_tx_get_mixed_nv_flag(void);

extern boolean rfnv_tdscdma_b39_tx_get_mixed_nv_flag(void);

void rf_tdscdma_mc_set_blocking_cmd_rsp_status
(
  rf_tdscdma_mdsp_msg_t ftm_rf_cmd_msg,
  rf_tdscdma_rfmode_cmd_rsp_status_type status
);

rf_tdscdma_rfmode_cmd_rsp_status_type  rf_tdscdma_mc_get_blocking_rsp_status(rf_tdscdma_mdsp_msg_t ftm_rf_cmd_msg);

rfdevice_rxtx_common_class *rf_tdscdma_mc_get_curr_common_device(rfm_device_enum_type device,
                                                                 uint16 band);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Called to update TDSCDMA RX switch points according to mode (ACQ, TRK, IDLE, IFREQ, POWERSCAN).

  @details
  This function should be called to update Rx LNA switch points


  @return

*/
void rf_tdscdma_mc_init_debug_and_func_flags(void)
{
  rfdevice_tdscdma_init_device_debug_flag_data_type init_device_debug_flag_data = {0};
  uint32 tdscdma_debug_flag_bitmask_msb_32_bit;

  /* Temporarily hack RF_TDSCDMA_BC_CONFIG_DIV */
  memscpy(&(tdscdma_debug_flag_bitmask),sizeof(tdscdma_debug_flag_bitmask),&(rfcommon_nv_tbl.rf_tds_bc_config[RF_PATH_1]),sizeof(rfcommon_nv_tbl.rf_tds_bc_config[RF_PATH_1]));

  MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "RF_TDSCDMA: HW_ID=%d, Debug_Flags=0x%x", rfcommon_nv_tbl.rf_hw_config, tdscdma_debug_flag_bitmask);

  /* ******************************
  **		RF_TDSCDMA_MC                         **
  ********************************/
  if (tdscdma_debug_flag_bitmask & (1 << RF_TDSCDMA_MC_DISPLAY_TIME_PROFILE))
  {
    rf_tdscdma_mc_display_time_profile = TRUE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mc_display_time_profile has been set", 0);
  }

  if (tdscdma_debug_flag_bitmask &  (1 << RF_TDSCDMA_MC_WAKEUP_TIME_DEBUG))
  {
    rf_tdscdma_mc_wakeup_time_debug = TRUE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mc_wakeup_time_debug has been set", 0);
  }

  if (tdscdma_debug_flag_bitmask & (1 << RF_TDSCDMA_MC_HDET_DEBUG))
  {
    rf_tdscdma_mc_hdet_debug = TRUE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mc_hdet_debug has been set", 0);
  }

  if (tdscdma_debug_flag_bitmask & (1 << RF_TDSCDMA_MC_THERM_DEBUG))
  {
    rf_tdscdma_mc_therm_debug = TRUE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mc_therm_debug has been set", 0);
  }


  /* ******************************
   **		RF_TDSCDMA_MDSP 	                **
   ********************************/
  if (tdscdma_debug_flag_bitmask & (1 << RF_TDSCDMA_MDSP_RF_EVENT_SCRIPT_DISPLAY_TIME_PROFILE))
  {
    rf_tdscdma_mdsp_rf_event_script_display_time_profile = TRUE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mdsp_rf_event_script_display_time_profile has been set", 0);
  }

  if (tdscdma_debug_flag_bitmask & ( 1<< RF_TDSCDMA_MDSP_DISPLAY_SUCCESSFUL_SCRIPT))
  {
    rf_tdscdma_mdsp_display_successful_script = TRUE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mdsp_display_successful_script has been set", 0);
  }

  if (tdscdma_debug_flag_bitmask & (1 << RF_TDSCDMA_MDSP_TASKNUM_UPDATE_DEBUG))
  {
    rf_tdscdma_mdsp_tasknum_update_debug_flag = TRUE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mdsp_tasknum_update_debug_flag has been set", 0);
  }

  if (tdscdma_debug_flag_bitmask & (1<< RF_TDSCDMA_MDSP_RFC_TIMING_DEBUG))
  {
    rf_tdscdma_mdsp_rfc_timing_debug_flag = TRUE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mdsp_rfc_timing_debug_flag has been set", 0);
  }

  if (tdscdma_debug_flag_bitmask & (1 << RF_TDSCDMA_MDSP_RFC_TIMING_DEBUG))
  {
    rf_tdscdma_mdsp_build_dynamic_update_debug_flag = TRUE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mdsp_build_dynamic_update_debug_flag has been set", 0);
  }

  /* ******************************
  **	  RF_TDSCDMA_DEVICE 	 **
  ********************************/
  tdscdma_debug_flag_bitmask_msb_32_bit = tdscdma_debug_flag_bitmask >> 32;
  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "tdscdma_debug_flag_bitmask_msb_32_bit = %d", tdscdma_debug_flag_bitmask_msb_32_bit);

  if (tdscdma_debug_flag_bitmask_msb_32_bit & (1<< (RF_TDSCDMA_DEVICE_SSBI_DEBUG - 32)))
  {
    init_device_debug_flag_data.rfdevice_tdscdma_enable_ssbi_debug_flag = TRUE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfdevice_tdscdma_enable_ssbi_debug_flag has been set", 0);
  }

  if (tdscdma_debug_flag_bitmask_msb_32_bit & (1<< (RF_TDSCDMA_DEVICE_HDET_DEBUG - 32)))
  {
    init_device_debug_flag_data.rfdevice_tdscdma_enable_hdet_debug_flag = TRUE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfdevice_tdscdma_enable_hdet_debug_flag has been set", 0);
  }

  if (tdscdma_debug_flag_bitmask_msb_32_bit & (1<< (RF_TDSCDMA_DEVICE_DISABLE_CONFIG_DC_CAL - 32)))
  {
    init_device_debug_flag_data.rfdevice_tdscdma_disable_config_dc_cal_flag = TRUE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfdevice_tdscdma_disable_config_dc_cal_flag has been set TRUE", 0);
  }

  rfdevice_tdscdma_rx_cmd_dispatch(RFM_DEVICE_0,
                                   RFCOM_TDSCDMA_MODE,
                                   rf_tdscdma_mc_state.single_valid_band,
                                   RFDEVICE_TDSCDMA_INIT_DEVICE_DEBUG_FLAG,
                                   &init_device_debug_flag_data);


  /* *****************************
   **		FUNCTION FLAGS  	                  **
   *******************************/

  if (tdscdma_debug_flag_bitmask & (1 << RF_TDSCDMA_FUNCTION_OVERWRITE_ALLOWED))
  {
    if (tdscdma_debug_flag_bitmask & (1 << RF_TDSCDMA_MC_ENABLE_MTPL))
    {
      rf_tdscdma_mc_enable_hdet_max_pwr_limiting = TRUE;
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mc_enable_hdet_max_pwr_limiting has been enabled", 0);
    }

    if (tdscdma_debug_flag_bitmask & (1 << RF_TDSCDMA_MC_ENABLE_THERM))
    {
      //rf_tdscdma_mc_therm_debug = TRUE;
      //MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mc_therm_debug has been enabled", 0);
    }

    if (tdscdma_debug_flag_bitmask & (1 << RF_TDSCDMA_MC_ENABLE_HDET_CLASS))
    {
      is_wtr_hdet = TRUE;
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "is_wtr_hdet has been enabled", 0);
    }
  }


  /* ******************************
  **	  RF_TDSCDMA_FED 	 **
  ********************************/
  if (tdscdma_debug_flag_bitmask_msb_32_bit & (1<< (RF_TDSCDMA_FED_DISABLE_SLOT_UPDATE - 32)))
  {
    rflm_tds_disable_slot_update = TRUE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rflm_tds_disable_slot_update has been set to TRUE", 0);
  }

  if (tdscdma_debug_flag_bitmask_msb_32_bit & (1<< (RF_TDSCDMA_FED_DISABLE_TXAGC - 32)))
  {
    rflm_tds_disable_txagc = TRUE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rflm_tds_disable_txagc has been set to TRUE", 0);
  }

  if (tdscdma_debug_flag_bitmask_msb_32_bit & (1<< (RF_TDSCDMA_FED_DISABLE_RXAGC - 32)))
  {
    rflm_tds_disable_rxagc = TRUE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rflm_tds_disable_rxagc has been set to TRUE", 0);
  }

  if (tdscdma_debug_flag_bitmask_msb_32_bit & (1<< (RF_TDSCDMA_FED_DISABLE_PDET - 32)))
  {
    rflmTdsRfModeCtlState.pdetInfo.enabled = FALSE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rflmTdsRfModeCtlState.pdetInfo.enabled has been set to FALSE", 0);
  }

  if (tdscdma_debug_flag_bitmask_msb_32_bit & (1<< (RF_TDSCDMA_FED_DISABLE_THERM - 32)))
  {
    rflmTdsRfModeCtlState.thermInfo.enabled = FALSE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rflmTdsRfModeCtlState.thermInfo.enabled has been set to FALSE", 0);
  }

  if (tdscdma_debug_flag_bitmask_msb_32_bit & (1<< (RF_TDSCDMA_FED_DISABLE_AOL - 32)))
  {
    rflmTdsRfModeCtlState.aolInfo.enabled = FALSE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rflmTdsRfModeCtlState.aolInfo.enabled has been set to FALSE", 0);
  }

  if (tdscdma_debug_flag_bitmask_msb_32_bit & (1<< (RF_TDSCDMA_FED_DISABLE_ASDIV - 32)))
  {
    rflmTdsRfModeCtlState.asdInfo.enabled = FALSE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rflmTdsRfModeCtlState.asdInfo.enabled has been set to FALSE", 0);
  }

  if (tdscdma_debug_flag_bitmask_msb_32_bit & (1<< (RF_TDSCDMA_FED_DISABLE_CL  - 32)))
  {
    rflmTdsRfModeCtlState.clInfo.enabled = FALSE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rflmTdsRfModeCtlState.clInfo.enabled has been set to FALSE", 0);
  }

  if (tdscdma_debug_flag_bitmask_msb_32_bit & (1<< (RF_TDSCDMA_FED_DISABLE_HEADSTART  - 32)))
  {
    rflmTdsRfModeCtlState.headStartInfo.enabled = FALSE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rflmTdsRfModeCtlState.headStartInfo.enabled has been set to FALSE", 0);
  }

  if (tdscdma_debug_flag_bitmask_msb_32_bit & (1<< (RF_TDSCDMA_FED_DEBUG_PEDT  - 32)))
  {
    rflm_tds_debug_pdet = TRUE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rflm_tds_debug_pdet  has been set to TRUE", 0);
  }

  if (tdscdma_debug_flag_bitmask_msb_32_bit & (1<< (RF_TDSCDMA_FED_DEBUG_THERM  - 32)))
  {
    rflm_tds_debug_therm = TRUE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rflm_tds_debug_therm has been set to TRUE", 0);
  }

  if (tdscdma_debug_flag_bitmask_msb_32_bit & (1<< (RF_TDSCDMA_FED_DISABLE_FBRX   - 32)))
  {
    rflm_tds_enable_fbrx = FALSE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rflm_tds_enable_fbrx has been set to FALSE", 0);
  }

  if (tdscdma_debug_flag_bitmask_msb_32_bit & (1<< (RF_TDSCDMA_FED_PRINT_DIAG_MESSAGES - 32)))
  {
    //rflm_tds_print_qxdm_message = TRUE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "Enable Log 0x186A to view RFLM F3s!", 0);
  }

  if (tdscdma_debug_flag_bitmask_msb_32_bit & (1<< (RF_TDSCDMA_FED_DISABLE_DIAG_ERROR_MESSAGE  - 32)))
  {
    //rflm_tds_disable_qxdm_error_message = FALSE;
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "Enable Log 0x186A to view RFLM F3s!", 0);
  }


}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Called to update TDSCDMA RX switch points according to mode (ACQ, TRK, IDLE, IFREQ, POWERSCAN).

  @details
  This function should be called to update Rx LNA switch points


  @return

*/
void rf_tdscdma_mc_update_rx_swpt
(
  rfm_device_enum_type device,
  rfcom_tdscdma_band_type band,
  rfa_tdscdma_rxlm_mode_t rxlm_mode,
  rf_tdscdma_mdsp_rxtx_buf_type rxtx_buf_id,
  uint8 buf_idx
)
{
  rf_time_tick_type update_rx_swpt_start_t = {0};
  rf_time_type update_rx_swpt_time;
  rfnv_tdscdma_rx_tbl_type* tds_rxnv_ptr;
  rf_tdscdma_mdsp_rx_swpt_pair_type tds_rx_swpt[2] = {{NULL}, {NULL}};
  rf_path_enum_type path;
  rfcom_tdscdma_band_type full_band = band;

  if(rf_tdscdma_mc_display_time_profile)
  {
    update_rx_swpt_start_t = rf_time_get_tick();
  }

  path = rfcommon_core_device_to_path(device);

  if (path == RF_PATH_0 || path == RF_PATH_1)
  {
    /* Initialize nv table pointer */

    // If input band is a split band, convert it to full band
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "Assigning nv pointer for band %d", band);
    /* Get the full band from split band. If not a split band, this will not change */
    rf_tdscdma_core_get_fullband_from_subband(&full_band);
    if (full_band != band)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "Full band is %d, assigning NVs from full band", full_band);
    }
    /* Assign nv_tbl_ptr for the fullband */
    tds_rxnv_ptr = rf_tdscdma_core_util_init_rx_nv_table(full_band);

    switch (rxlm_mode)
    {
    case RFA_TDSCDMA_RXLM_MODE_ACQ:
      tds_rx_swpt[0].nb_ptr = tds_rxnv_ptr->lna_range_rise_fall_nb_mode_acq;
      tds_rx_swpt[0].wb_ptr = tds_rxnv_ptr->lna_range_rise_fall_wb_mode_acq;
      tds_rx_swpt[1].nb_ptr = tds_rxnv_ptr->lna_range_rise_fall_nb_mode_acq;
      tds_rx_swpt[1].wb_ptr = tds_rxnv_ptr->lna_range_rise_fall_wb_mode_acq;
      break;

    case RFA_TDSCDMA_RXLM_MODE_TRK:
      tds_rx_swpt[0].nb_ptr = tds_rxnv_ptr->lna_range_rise_fall_nb_mode_tracking;
      tds_rx_swpt[0].wb_ptr = tds_rxnv_ptr->lna_range_rise_fall_wb_mode_tracking;
      break;

    case RFA_TDSCDMA_RXLM_MODE_TRK_IDLE:
      tds_rx_swpt[0].nb_ptr = tds_rxnv_ptr->lna_range_rise_fall_nb_mode_idle;
      tds_rx_swpt[0].wb_ptr = tds_rxnv_ptr->lna_range_rise_fall_wb_mode_idle;
      break;

    case RFA_TDSCDMA_RXLM_MODE_TRK_IRAT:
      tds_rx_swpt[0].nb_ptr = tds_rxnv_ptr->lna_range_rise_fall_nb_mode_ifreq;
      tds_rx_swpt[0].wb_ptr = tds_rxnv_ptr->lna_range_rise_fall_wb_mode_ifreq;
      break;

    case RFA_TDSCDMA_RXLM_MODE_PWR_SCAN:
      tds_rx_swpt[0].nb_ptr = tds_rxnv_ptr->lna_range_rise_fall_nb_mode_power_scan;
      tds_rx_swpt[0].wb_ptr = tds_rxnv_ptr->lna_range_rise_fall_wb_mode_power_scan;
      break;

    default:
      MSG_1(MSG_SSID_RF, MSG_LVL_FATAL, "wrong rxlm_mode %d in rf_tdscdma_mc_update_rx_swpt", rxlm_mode);
      break;
    }
  }
  
  if(rf_tdscdma_mc_display_time_profile)
  {
    update_rx_swpt_time = rf_time_get_elapsed(update_rx_swpt_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Update Rx Switch Point time profile: %d usec", update_rx_swpt_time);
  }

}


#ifdef FEATURE_RF_ASDIV
rf_tdscdma_mc_status_type rf_tdscdma_mc_switch_ant
(
  rfm_device_enum_type device,
  uint8 to_pos,
  void *cb_func,
  void *cb_data,
  boolean is_immediate
)
{
  rf_buffer_intf *ant_sw_script;
  rfcommon_asdiv_asm_conifg_type get_script_cb;
  rfcommon_asdiv_send_script_type    send_script_cb;
  rfcommon_asdiv_switch_start_notify_type switch_start_cb;
  rfcommon_asdiv_update_tuner_type update_tuner_cb;
  rftdscdma_mdsp_common_event_data_type rf_tds_build_event_data;

  rf_tdscdma_asdiv_script_data asm_script_data;

  rf_tdscdma_mc_status_type ret_val = RF_TDSCDMA_MC_SUCCESS;
  rfcommon_asdiv_position_type position = 0;

  /*Only update the current position when in wake up/enter mode*/
  if ( (rf_tdscdma_mc_state.rf_state[device] == RF_TDSCDMA_STATE_INIT) || 
       (rf_tdscdma_mc_state.rf_state[device] == RF_TDSCDMA_STATE_SLEEP) )
  {
  	 rfcommon_asdiv_set_current_position((rfcommon_asdiv_position_type)to_pos);     
     position = rfcommon_asdiv_get_current_position(device);
     rfcommon_asdiv_init_switch_state(position);
     
     MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH,
           "rf_tdscdma_mc_ant_switch: only update the pos during enter_mode and sleep (no switch), device: %d, rf_state: %d, current position:%d", 
           device, rf_tdscdma_mc_state.rf_state[device], position);
     return RF_TDSCDMA_MC_SUCCESS;
  }

  /* Allocate mem for tuner script */
  ant_sw_script = rf_buffer_create(RFTDSCDMA_STANDALONE_MAX_SSBI_SCRIPT_SIZE,
                                   RFTDSCDMA_STANDALONE_MAX_RFFE_SCRIPT_SIZE,
                                   RFTDSCDMA_STANDALONE_MAX_GRFC_SCRIPT_SIZE ) ;
  
  if(ant_sw_script == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,
        "RF_TDSCDMA_MC: rf_tdscdma_mc_ant_switch: ERROR, Cannot create script buffer");
    return RF_TDSCDMA_MC_INVALID_PARAM;
  }

  if ((rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RX)    ||
      (rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RXTX)   )
  {
    (void)rf_buffer_clear(ant_sw_script);

    asm_script_data.device = device;
    asm_script_data.band = rf_tdscdma_mc_state.curr_band_rx;
    asm_script_data.immediate = is_immediate;

    get_script_cb.cb_func = rf_tdscdma_mdsp_build_asdiv_script;
    get_script_cb.cb_data = &asm_script_data;

    if (FALSE == is_immediate)
    {
      send_script_cb.cb_func = rfcommon_asdiv_manager_send_script;
      #ifdef FEATURE_RF_HAS_QTUNER
      update_tuner_cb.cb_func = rf_tdscdma_mc_update_tuner_code;
      #endif
      update_tuner_cb.cb_data = (uint8)is_immediate;
    }
    else
    {
      send_script_cb.cb_func = NULL;
      update_tuner_cb.cb_func = NULL;
      update_tuner_cb.cb_data = FALSE;
    }

    send_script_cb.cb_data = NULL;
    switch_start_cb.cb_func = NULL;
    switch_start_cb.cb_data = NULL;

    //debug
    //MSG(MSG_SSID_RF, MSG_LEGACY_HIGH,
    //"Debug asdiv: reading GRFC BEFORE rfcommon_asdiv_manager_config_switch");
    //   rfm_meas_read_grfc();

    if (rfcommon_asdiv_manager_config_switch(device,            // rfm_device_enum_type device
                                             RFM_TDSCDMA_MODE,  // rfm_mode_enum_type rfm_mode
                                             to_pos,            // rfcommon_asdiv_position_type antenna_position
                                             ant_sw_script,     // rf_buffer_intf *switch_script_ptr
                                             ant_sw_script,     // rf_buffer_intf *tuner_script_ptr
                                             get_script_cb,     // rfcommon_asdiv_asm_conifg_type asm_script_cfg
                                             send_script_cb,    // rfcommon_asdiv_send_script_type send_script_cfg
                                             switch_start_cb,   // rfcommon_asdiv_switch_start_notify_type switch_start_cfg
                                             update_tuner_cb,   // rfcommon_asdiv_update_tuner_type update_tuner_cb
                                             cb_func,           // rfm_l1_cb_type l1_cb_func
                                             cb_data,           // void *l1_cb_data
                                             FALSE,             // boolean gen_script_only
                                             is_immediate,             // boolean immediate
                                             0,                 // int16 tuner_start_delta
                                             rf_tdscdma_mc_lm_bufs.rx_buf_wrk[0].buf_idx // uint32 rxlm_handle
                                             ) == FALSE)
    {
      MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,
            "RF_TDSCDMA_MC: rf_tdscdma_mc_ant_switch: ERROR, gen switch script error for Device %d, Band %d, IMM %d",
            device, 
            rf_tdscdma_mc_state.curr_band_rx,
            is_immediate);
      ret_val = RF_TDSCDMA_MC_INVALID_PARAM;
    } 
    else
    {
      //debug
      #if 0
      MSG(MSG_SSID_RF, MSG_LEGACY_HIGH,
           "Debug asdiv: reading GRFC AFTER rfcommon_asdiv_manager_config_switch");
         rfm_meas_read_grfc();
      #endif
      
      /* Re-build home restore script */

      // Firstly, update the ant state variable
      rf_tdscdma_mc_state.curr_ant_sw_pos = rfcommon_asdiv_get_current_position(rf_tdscdma_mc_state.curr_pri_device);
      rf_tdscdma_mc_state.next_ant_sw_pos = to_pos;

      rf_tds_build_event_data.pri_device = rf_tdscdma_mc_state.curr_pri_device;
      rf_tds_build_event_data.sec_device = rf_tdscdma_mc_state.curr_sec_device;
      rf_tds_build_event_data.tx_device  = rf_tdscdma_mc_state.curr_pri_device;
      rf_tds_build_event_data.band       = rf_tdscdma_mc_state.curr_band_rx;
      rf_tds_build_event_data.channel    = rf_tdscdma_mc_state.curr_chan_rx;
      rf_tds_build_event_data.drx_enable = rf_tdscdma_mc_state.div_enable;
      rf_tds_build_event_data.ant_sw_pos = to_pos;

      rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_BUILD_HOME_RESTORE,
                                            &rf_tds_build_event_data);
      
      MSG_5(MSG_SSID_RF, MSG_LEGACY_HIGH,
            "rf_tdscdma_mc_ant_switch: Build script for Device %d, Band %d, Chan %d. From Pos %d -> %d",
            rf_tdscdma_mc_state.curr_pri_device,
            rf_tdscdma_mc_state.curr_band_rx,
            rf_tdscdma_mc_state.curr_chan_rx,
            rf_tdscdma_mc_state.curr_ant_sw_pos,
            rf_tdscdma_mc_state.next_ant_sw_pos);
    }
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "RF_TDSCDMA_MC: rf_tdscdma_mc_ant_switch: ERROR, Invalid state %d for Antenna switch",
          rf_tdscdma_mc_state.rf_state[RF_PATH_0]);
    ret_val = RF_TDSCDMA_MC_SM_ERROR;
  }

  /* Destroy script buffer for Ant switch */
  if( ant_sw_script != NULL )
  {
    rf_buffer_destroy(ant_sw_script);
  }

  return ret_val;
}


void rf_tdscdma_mc_switch_ant_abort
(
  rfm_device_enum_type device,
  uint8 to_pos,
  void *cb_func,
  void *cb_data
)
{
#ifdef FEATURE_RF_ASDIV
  void** tuner_mgr;
  rftdscdma_mdsp_common_event_data_type rf_tds_build_event_data;

  if (rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RX)
  {
    /* Get tuner object from RFC */
    tuner_mgr = rfc_tdscdma_get_rf_device_obj(device,
                                              RFC_CONFIG_RX,
                                              rf_tdscdma_mc_state.curr_band_rx,
                                              RFDEVICE_TUNER_MANAGER);
  }
  else if (rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RXTX)
  {
    /* Get tuner object from RFC */
    tuner_mgr = rfc_tdscdma_get_rf_device_obj(device,
                                              RFC_CONFIG_TX,
                                              rf_tdscdma_mc_state.curr_band_tx,
                                              RFDEVICE_TUNER_MANAGER);
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "RF_TDSCDMA_MC: rf_tdscdma_mc_ant_switch: ERROR, Invalid state %d for Antenna switch ",
          rf_tdscdma_mc_state.rf_state[RF_PATH_0]);
    return;
  }

  if(tuner_mgr != NULL)
  {
    if(rfcommon_asdiv_manager_abort_switch(device, RFCOM_TDSCDMA_MODE, *tuner_mgr) == FALSE)
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "RF_TDSCDMA_MC: rf_tdscdma_mc_switch_ant_abort: ERROR aborting AsDiv scripts ");
    }
    else
    {
      /* Re-build home restore script */

      // Firstly, update the ant state variable
      rf_tdscdma_mc_state.curr_ant_sw_pos = rfcommon_asdiv_get_current_position(rf_tdscdma_mc_state.curr_pri_device);
      rf_tdscdma_mc_state.next_ant_sw_pos = rfcommon_asdiv_get_current_position(rf_tdscdma_mc_state.curr_pri_device);

      rf_tds_build_event_data.pri_device = rf_tdscdma_mc_state.curr_pri_device;
      rf_tds_build_event_data.sec_device = rf_tdscdma_mc_state.curr_sec_device;
      rf_tds_build_event_data.tx_device  = rf_tdscdma_mc_state.curr_pri_device;
      rf_tds_build_event_data.band       = rf_tdscdma_mc_state.curr_band_rx;
      rf_tds_build_event_data.channel    = rf_tdscdma_mc_state.curr_chan_rx;
      rf_tds_build_event_data.drx_enable = rf_tdscdma_mc_state.div_enable;
      rf_tds_build_event_data.ant_sw_pos = rf_tdscdma_mc_state.next_ant_sw_pos;

      rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_BUILD_HOME_RESTORE,
                                            &rf_tds_build_event_data);
      
      MSG_4(MSG_SSID_RF_DEBUG, MSG_LEGACY_HIGH,
            "rf_tdscdma_mc_ant_switch: Build script for Device %d, Band %d, Chan %d. Restore Ant Pos to %d",
            rf_tdscdma_mc_state.curr_pri_device,
            rf_tdscdma_mc_state.curr_band_rx,
            rf_tdscdma_mc_state.curr_chan_rx,
            rf_tdscdma_mc_state.next_ant_sw_pos);
    }
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "rf_tdscdma_mc_switch_ant_abort: ERROR, Tuner device failed for Device %d, Band %d",
          device,
          rf_tdscdma_mc_state.curr_band_rx);
  }
#endif/* FEATURE_RF_ASDIV */
}
#endif /* FEATURE_RF_ASDIV */


void rf_tdscdma_mc_asdiv_script
(
  uint32 ant_position,
  rfm_device_enum_type pri_device,
  rfcom_tdscdma_band_type band
)
{
  boolean script_status;
  rf_buffer_intf *ant_sw_script;
  rf_tdscdma_asdiv_script_data asm_script_data;

  ant_sw_script = rf_buffer_create (RFTDSCDMA_STANDALONE_MAX_SSBI_SCRIPT_SIZE,
  RFTDSCDMA_STANDALONE_MAX_RFFE_SCRIPT_SIZE,
  RFTDSCDMA_STANDALONE_MAX_GRFC_SCRIPT_SIZE ) ;
  (void)rf_buffer_clear(ant_sw_script);

  asm_script_data.device = pri_device;
  asm_script_data.band = band;
  asm_script_data.immediate = TRUE;

  script_status = rf_tdscdma_mdsp_build_asdiv_script(ant_position, /*!< New Antenna Position */
  ant_sw_script, /*!< pointer to buffer to store Script */
  (void*)(&asm_script_data) /*!< Callback data to be passed to the function */);

  if (!script_status)
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,
    "RF_TDSCDMA_MC: rf_tdscdma_mdsp_build_asdiv_script(ant_pos %d, device %d, band %d) ERROR", ant_position, pri_device, band);
  }

  if( ant_sw_script != NULL )
  {
    rf_buffer_destroy(ant_sw_script);
  }
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Called to enter TDSCDMA mode.

  @details
  This function should be called after MDSP image is downloaded and when
  entering TDSCDMA mode.

  The agreement between L1 and RFSW is that mc_enter_mode() will be called
  only once regardless if diversity is enabled. pri_device must be valid but
  sec_device can be garbage.

  Based on this agreement, mc_enter_mode() will do following (not strictly sequence):
	*  State machine checking ONLY on primary chain
	*  Vreg voting ONLY happen on primary chain
	*  TCXO buffer voting ONLY happen on primary chain
	*  Initialize state variables on both primary chain and secondary chain
	*  Enforce to check if enter_mode is called two times in a row without
	   calling exit_mode in between
	   -  Basically mc_enter_mode() and mc_exit_mode() must be called in pairs
	*  Enforce to check if a valid pri_device is passed in

  RxLM update has been removed from mc_enter_mode()

  Init once flag handles programming that needs to be done
  only once for the power cycle of the phone.

  @param pri_device
  Primary device (logical device)

  @param sec_device
  Secondary device (logical device)

  @param rxlm_buffer_idx
  RxLM buffer index (this parameter has been obsoloted; see above comment)

  @param rxlm_mode
  RxLM mode (this parameter has been obsoloted; see above comment)

  @return
  rf_tdscdma_mc_status_type
*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_enter_mode
(
  rfm_device_enum_type pri_device,
  rfm_device_enum_type sec_device,
  lm_handle_type rxlm_buffer_idx,
  rfa_tdscdma_rxlm_mode_t rxlm_mode,
  boolean is_dsds
)
{
  rf_path_enum_type pri_path, loop_path;
  boolean dev_voting_status;
  rftdscdma_mdsp_common_event_data_type rf_tds_build_event_data;
  rf_time_tick_type enter_mode_start_t = {0};
  rf_time_type enter_mode_time;
  uint32 ant_position;
  boolean is_sawless;

  if(rf_tdscdma_mc_display_time_profile)
  {
    enter_mode_start_t = rf_time_get_tick();
  }

  //Set RFLM RUMI flag if MC RUMI flag is set.
  if (rf_tdscdma_use_on_rumi == TRUE)
  {
    rflm_tds_use_on_rumi = TRUE;
  }

  if(RFCOM_TDSCDMA_MODE != rfm_get_current_mode(pri_device)) //to ensure DSDS_RF_TDSCDMA in wakeup0 for ASDiv
  {
    rfm_enter_mode(pri_device, RFCOM_TDSCDMA_MODE, NULL, NULL, 0);
  }

  /* Convert logical device to logical path */
  pri_path = rfcommon_core_device_to_path(pri_device);

  MSG_6(MSG_SSID_RF, MSG_LVL_HIGH,
        "rf_tdscdma_mc_enter_mode(pri_device=%d (path=%d), sec_device=%d, rxlm_buf_idx=%d, rxlm_mode=%d, is_dsds=%d)",
        pri_device, pri_path, sec_device, rxlm_buffer_idx, rxlm_mode, is_dsds);

  /* Parameter Check:   													 */
  /*=========================================================================*/
  if (pri_path != RF_PATH_0)
  {
    // RF_PATH_0 = PRIMARY CHAIN
    // Must pass a valid pri_device into mc_enter_mode() which must be on the primary chain, i.e., RF_PATH_0
    MSG_2(MSG_SSID_RF, MSG_LVL_ERROR,
          "rf_tdscdma_mc_enter_mode: called on wrong primary device %d (path %d)",
          pri_device, pri_path);
    return RF_TDSCDMA_MC_INVALID_PARAM;
  }

  /* State Check: on primary chain ONLY 									 */
  /*=========================================================================*/
  if (rf_tdscdma_mc_state.rf_state[RF_PATH_0] != RF_TDSCDMA_STATE_DEFAULT)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_enter_mode: called in wrong state %d", rf_tdscdma_mc_state.rf_state[RF_PATH_0]);
    return RF_TDSCDMA_MC_SM_ERROR;
  }

  if (rf_tdscdma_use_on_rumi == TRUE)
  {
    rf_tdscdma_mc_state.single_valid_band = RFCOM_BAND_TDSCDMA_B34; //hack for RUMI
  }
  else
  {
    /* Determine single valid band: Derived from TDS_BC_CONFIG */
    if(rfcommon_nv_tbl.rf_tds_bc_config[0] & 0x0001)
    {
      /* 0x0000 0001b is for B34 */
      rf_tdscdma_mc_state.single_valid_band = RFCOM_BAND_TDSCDMA_B34;
    }
    else if(rfcommon_nv_tbl.rf_tds_bc_config[0] & 0x0020)
    {
      /* 0x0010 0000b is for B39 */
      rf_tdscdma_mc_state.single_valid_band = RFCOM_BAND_TDSCDMA_B39;
    }
    else if(rfcommon_nv_tbl.rf_tds_bc_config[0] & 0x0010)
    {
      /* 0x0001 0000b is for B40 */
      rf_tdscdma_mc_state.single_valid_band = RFCOM_BAND_TDSCDMA_B40;
    }
    else
    {
      /* Need to crash the phone because TDS_BC_CONFIG doesn't enable any valid band! */
      ASSERT(0);
    }

    MSG_2(MSG_SSID_RF_DEBUG, MSG_LVL_HIGH,
          "single_valid_band=%d, TDS_BC_CONFIG=0x%x",
          rf_tdscdma_mc_state.single_valid_band, rfcommon_nv_tbl.rf_tds_bc_config[0]);
  }

  if(!is_dsds)
  {
    /* Skip HW operation or those require MCPM when is_dsds= TRUE. Moved to enable_rx() */

    /* vote on Rx VREGs */
    /* Only need to call once on pri_device and assume it is band independent */
    rfc_tdscdma_manage_vregs(pri_device, RFCOM_BAND_TDSCDMA_B34, RF_PATH_RX_STATE);

    // vote for RFCMD app
    rf_hal_bus_enable(TRUE, &rf_tdscdma_hal_bus_client_handle, RF_HAL_RFCMD_APP_CLIENT(TDSCDMA));

    /* Program the GPIO/GRFC/RF_CTL signals for the TDSCDMA mode */
    /* Only need to call once on pri_device */
    rfc_tdscdma_mode_config(pri_device);

    /* Enable TCXO buffer */
    /* Only need to call once on pri_device */

    if(rf_tdscdma_mc_display_time_profile)
    {
      enter_mode_start_t = rf_time_get_tick();
    }    
    
    dev_voting_status = rfcommon_core_dev_power_vote(pri_device,
                                                     RFCMN_CORE_DEV_ON,
                                                     RFCMN_CORE_DEV_NORMAL_VOTE);

    if( dev_voting_status == FALSE )
    {
      MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_enter_mode(): Device: %d Pwr Up failed", pri_device);
    }

    if(rf_tdscdma_mc_display_time_profile)
    {
      enter_mode_time = rf_time_get_elapsed(enter_mode_start_t, RF_USEC);
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Rx Timeline: rfcommon_core_dev_power_vote on takes %d usec", enter_mode_time);
      enter_mode_start_t = rf_time_get_tick();
    }
  

    /* rfdevice RX/TX init which to clean up rfdevice internal state variables and get band-port mapping */
    /* Only need to call once on pri_device */
    rfdevice_tdscdma_rx_init(pri_device,
                             RFCOM_TDSCDMA_MODE,
                             rf_tdscdma_mc_state.single_valid_band,
                             RFDEVICE_EXECUTE_IMMEDIATE,
                             NULL);

    rfdevice_tdscdma_tx_init(pri_device,
                             RFCOM_TDSCDMA_MODE,
                             rf_tdscdma_mc_state.single_valid_band,
                             RFDEVICE_EXECUTE_IMMEDIATE,
                             NULL);
  }
  
  /* Initialize the double buffer indexes */
  rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx = 0;
  rf_tdscdma_mc_dbl_bufs.rx_tgt_buf_idx = 1;
  rf_tdscdma_mc_dbl_bufs.tx_wrk_buf_idx = 0;
  rf_tdscdma_mc_dbl_bufs.tx_tgt_buf_idx = 1;

  // Check if sawless RF card is used
  is_sawless = rf_tdscdma_mc_is_rx_sawless();
  
  if(!is_dsds)
  {
    /* One-time initialization */
    if (rf_tdscdma_mc_state.init_once_flag == TRUE)
    {
      int mdsp_init_vars;

      /* Initialize TDSCDMA mDSP interface and program mdsp one time static settings
      (both rx and tx, band-independent) */
      rf_tdscdma_mdsp_init(pri_device,
      (void*)&mdsp_init_vars,
      FALSE //is_irat
                          );

      /* calculate rx cal chan freqs using rx cal channels for each band.
         rx cal chan freqs will be used for rx freq comp calculation */
      /* Calcuation need to be done on both primary and secondary path */
      rf_tdscdma_core_rxctl_calc_rx_cal_chan_freq(RF_PATH_0, RFCOM_BAND_TDSCDMA_B34);
      rf_tdscdma_core_rxctl_calc_rx_cal_chan_freq(RF_PATH_0, RFCOM_BAND_TDSCDMA_B39);
      //rf_tdscdma_core_rxctl_calc_rx_cal_chan_freq(RF_PATH_0, RFCOM_BAND_TDSCDMA_B40);
      rf_tdscdma_core_rxctl_calc_rx_cal_chan_freq(RF_PATH_1, RFCOM_BAND_TDSCDMA_B34);
      rf_tdscdma_core_rxctl_calc_rx_cal_chan_freq(RF_PATH_1, RFCOM_BAND_TDSCDMA_B39);
      //rf_tdscdma_core_rxctl_calc_rx_cal_chan_freq(RF_PATH_1, RFCOM_BAND_TDSCDMA_B40);

      // Initialize the debug&functional flags
      rf_tdscdma_mc_init_debug_and_func_flags();

      rf_tdscdma_mc_state.init_once_flag = FALSE;
    }
  }
  

  if(!is_dsds)
  {
    if (pri_path == RF_PATH_0)
    {
      /* Disable temp comp and MTPL */
      rf_tdscdma_core_temp_comp_enable(FALSE);
      rf_tdscdma_core_txplim_enable(FALSE);

      rfdevice_tdscdma_rx_cmd_dispatch(RFM_DEVICE_0,
                                       RFCOM_TDSCDMA_MODE,
                                       rf_tdscdma_mc_state.single_valid_band,
                                       RFDEVICE_TDSCDMA_GET_HDET_INFO,
                                       &rf_tdscdma_mc_state.hdet_info);

      if ((!IS_FTM_IN_TEST_MODE()) || (!rfm_get_calibration_state()))
      {
        int4 t_unused = 0;

        /* Read one therm value */
        rf_tdscdma_mc_state.curr_pri_device = pri_device;
        rf_tdscdma_core_temp_comp_timer(t_unused);
      }
      rf_tdscdma_mc_state.pdet_dc_cal_done = FALSE;
    }
  }

  if(!is_dsds)
  {
    /*set antenna position during enter_mode() */
#ifdef FEATURE_RF_ASDIV
    ant_position = rfcommon_asdiv_get_current_position(pri_device);
#else
    ant_position = 0; // hard-coded the default position if feature is not defined
#endif

    // [Jay]To-DO: Move to MDSP and check if ok to hard-coded B34
    rf_tdscdma_mc_asdiv_script(ant_position, pri_device, RFCOM_BAND_TDSCDMA_B34);

    /* FBRX enter mode to get FBRX LM handle */
    if (rflm_tds_is_fbrx_enabled())
    {
      if(RFCOMMON_FBRX_ERROR == rfcommon_fbrx_mc_enter_mode(&rf_tdscdma_fbrx_lm_handle, RFM_TDSCDMA_MODE))
      {
        MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "TDS FBRX enter mode failed!");
      }
      else
      {
        MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "TDS FBRX enter mode successful!");
      }
    }
  }

  if(!is_dsds) 
  {
    rf_card_band_type rfc_band;
    rfcmn_status_type alt_path_status;
    uint8 alt_path = 0;
    boolean rfc_alt_path_status;
    uint8 loop_device, loop_band;
    rfm_device_enum_type logic_device;
    
    /* Update alt_path														 */
    /*=========================================================================*/
    /* Loop for all logic_devices and all bands */
    for(loop_device=0; loop_device<2; loop_device++)
    {
      if(loop_device==0)
      {
        logic_device = pri_device;
      }
      else if (loop_device==1)
      {
        logic_device = pri_device+1; // Take assumption that the sec_device always = pri_device + 1
      }
      
      for(loop_band = (uint8)(RFCOM_BAND_TDSCDMA_B34); loop_band <= (uint8)(RFCOM_BAND_TDSCDMA_B39); loop_band++)
      {
        rfc_band = rfcommon_core_convert_tds_band_to_rfc_band(loop_band);
        alt_path_status =  rfcmn_concurrency_mgr_update_rx_path(logic_device,
                                                                RFCOM_TDSCDMA_MODE,
                                                                (int)loop_band,
                                                                &alt_path);
        if (alt_path_status == RFCMN_PATH_SEL_FAILED)
        {
          MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH, 
                "Cannot get alt_path for device %d, TDS band %d, rfc_band %d", logic_device, loop_band, rfc_band);
        }

        if (IS_FTM_IN_TEST_MODE())
        {
          // In RFcal mode, always uses alt_path from tools
          if (rfm_get_calibration_state() == TRUE) // RF calibration mode
          {
            alt_path = ftm_tdscdma_current_pri_alt_path;
          
            MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, 
                  "Use the alt_path %d passed by Tools for Rx0 in RFcal mode", ftm_tdscdma_current_pri_alt_path);  
          }
          // In FTM RF mode, use alt_pth from tools if ftm_tdscdma_override_alt_path is TRUE
          else // FTM RF mode
          {
            if(ftm_tdscdma_override_alt_path == TRUE)
            {
              alt_path = ftm_tdscdma_current_pri_alt_path;
          
              MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, 
                    "Use the alt_path %d passed by Tools for Rx0 in FTM RF mode", ftm_tdscdma_current_pri_alt_path);      
            }            
          }
        }

        rfc_alt_path_status = rfc_update_alt_port_mapping(logic_device, 
                                                          RFCOM_TDSCDMA_MODE, 
                                                          rfc_band, 
                                                          alt_path);
        if (rfc_alt_path_status == FALSE)
        {
          MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH, 
                "Cannot get alt_path for device %d, TDS band %d, rfc_band %d", logic_device, loop_band, rfc_band);
        }
      }  

      MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, 
            "Updated ALT_PATH to %d for device %d", alt_path, logic_device);
    }
    
    /* Build Rx LNA scripts for primary chain */
    rf_tds_build_event_data.pri_device = pri_device;
    rf_tds_build_event_data.sec_device = pri_device+1; // Hack because L1 might not provide sec_device at this time
    rf_tds_build_event_data.tx_device = pri_device;
    rf_tds_build_event_data.drx_enable = FALSE;
    rf_tds_build_event_data.is_irat = FALSE;
    rf_tds_build_event_data.rx_lna_update_type = RF_TDSCDMA_MDSP_RX_LNA_UPDATE_FOR_SCELL;
    rf_tds_build_event_data.is_bho_update = FALSE;

    // Invalid all bands and set the desired bands for Rx LNA script update
    for(loop_band = (uint8)RFCOM_BAND_TDSCDMA_B34; loop_band <= (uint8) RFCOM_NUM_TDSCDMA_BANDS; loop_band++)
    {
      rf_tds_build_event_data.to_build_rx_lna_script_on_bands[loop_band] = FALSE;
    }
    rf_tds_build_event_data.to_build_rx_lna_script_on_bands[RFCOM_BAND_TDSCDMA_B34] = TRUE;
    rf_tds_build_event_data.to_build_rx_lna_script_on_bands[RFCOM_BAND_TDSCDMA_B39] = TRUE;
    
    rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_BUILD_RX_LNA, &rf_tds_build_event_data);

    /* Build Rx LNA scripts for the secondary chain */
    rf_tds_build_event_data.drx_enable = TRUE;
    rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_BUILD_RX_LNA, &rf_tds_build_event_data);   

    rf_tdscdma_mc_state.last_pri_device_used_to_build_rx_lna_scpt = pri_device;
  }
  
  /*Program IP2 Cal data*/
  rf_tds_build_event_data.pri_device = RFM_DEVICE_0;
  rf_tds_build_event_data.sec_device = RFM_DEVICE_1;
  rf_tds_build_event_data.tx_device = RFM_DEVICE_0;
  rf_tds_build_event_data.drx_enable = FALSE;
  rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_DO_IP2_CAL, &rf_tds_build_event_data);




  /* Initialize state variables */
  rf_tdscdma_mc_state.curr_band_rx = RFCOM_BAND_TDSCDMA_INVALID;
  rf_tdscdma_mc_state.rf_state[RF_PATH_0] = RF_TDSCDMA_STATE_INIT;
  rf_tdscdma_mc_state.rf_state[RF_PATH_1] = RF_TDSCDMA_STATE_INIT;

  /* Invalid the alt_path information */
  for(loop_path = RF_PATH_0; loop_path < RF_PATH_MAX; loop_path++)
  {
    rf_tdscdma_mc_state.alt_path[loop_path] = 0xFF;
    rf_tdscdma_mc_state.alt_path_used_to_build_ifreq[loop_path] = 0xFF;

    rf_tdscdma_mc_state.scell_wb_adc_index_on_devices[loop_path] = 0xFF;
    rf_tdscdma_mc_state.ncell_wb_adc_index_on_devices[loop_path] = 0xFF;
  }  

#ifdef FEATURE_RF_ASDIV
    rf_tdscdma_mc_state.curr_ant_sw_pos = rfcommon_asdiv_get_current_position(pri_device);
    rf_tdscdma_mc_state.next_ant_sw_pos = rfcommon_asdiv_get_current_position(pri_device);
#else
    rf_tdscdma_mc_state.curr_ant_sw_pos = 0;
    rf_tdscdma_mc_state.next_ant_sw_pos = 0;
#endif
  

  if (rfcommon_autopin_is_enabled(RFM_TDSCDMA_MODE))
  {
    if(RFCOMMON_AUTOPIN_ERROR == rfcommon_autopin_mc_enter_mode(RFM_TDSCDMA_MODE))
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "TDS AutoPin enter mode failed!");
    }
    else
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "TDS AutoPin enter mode successful!");
    }
  }

  if (!rf_tdscdma_rf_mode_tx_lock_init_once)
  {
    rf_common_init_critical_section(&rf_tdscdma_rf_mode_tx_lock);
    rf_tdscdma_rf_mode_tx_lock_init_once = TRUE;
  }

  if(rf_tdscdma_mc_display_time_profile)
  {
    enter_mode_time = rf_time_get_elapsed(enter_mode_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Enter mode time profile: %d usec", enter_mode_time);
  }

  return RF_TDSCDMA_MC_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Called to exit TDSCDMA mode.

  @details
  The design is that mc_exit_mode() is called only once on both primary
  device and secondary device.

  The agreement between L1 and RFSW is that mc_exit_mode() will be called
  only once regardless if diversity is enabled. pri_device must be valid but
  sec_device can be garbage.

  Based on this agreement, mc_exit_mode() will do following (not strictly sequence):
	*  State machine check ONLY on primary chain
	*  Enforce to check if mc_exit_mode() is called two times in a row without
	   calling mc_enter_mode() in between
	*  Enforce to check if mc_exit_mode() is called without calling enter_mode before
	*  TCXO buffer voting ONLY happen on primary chain
	*  Vreg voting ONLY happen on primary chain
	*  Cleanup state variables on both primary chain and secondary chain

  @param pri_device
  Primary device (logical device)

  @param sec_device
  Secondary device (logical device)

  @return
  rf_tdscdma_mc_status_type

*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_exit_mode
(
  rfm_device_enum_type pri_device,
  rfm_device_enum_type sec_device,
  boolean wait_for_fw_release
)
{
  uint8 i = 0;
  boolean dev_voting_status;
  rf_path_enum_type pri_path, sec_path;
  uint8 loop_band;

  rf_time_tick_type exit_mode_start_t = {0};
  rf_time_type exit_mode_time;

  if(rf_tdscdma_mc_display_time_profile)
  {
    exit_mode_start_t = rf_time_get_tick();
  }

  /* Convert rfcom_device_enum_type to rf_path_enum_type */
  pri_path = rfcommon_core_device_to_path(pri_device);
  sec_path = rfcommon_core_device_to_path(sec_device);

  MSG_4(MSG_SSID_RF, MSG_LVL_HIGH, 
        "rf_tdscdma_mc_exit_mode(pri_device=%d (path %d), sec_device=%d (path %d))", 
        pri_device, pri_path, sec_device, sec_path);

  if (pri_path != RF_PATH_0)
  {
    // RF_PATH_0 = PRIMARY CHAIN
    // Must pass valid pri_device into exit_mode() which must be on primary chain
    MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, 
          "rf_tdscdma_mc_exit_mode: called on wrong primary device %d (path %d)", pri_device, pri_path);
    return RF_TDSCDMA_MC_INVALID_PARAM;
  }

  if(pri_device != rf_tdscdma_mc_state.curr_pri_device)                                               
  {
    MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, 
          "rf_tdscdma_mc_exit_mode: called on wrong primary device %d (should be %d) ", 
          pri_device, rf_tdscdma_mc_state.curr_pri_device);    
  }

  if((rf_tdscdma_mc_state.div_enable == TRUE) && (sec_device != rf_tdscdma_mc_state.curr_sec_device))   
  {
    MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, 
          "rf_tdscdma_mc_exit_mode: called on wrong secondary device %d (should be %d) ", 
          sec_device, rf_tdscdma_mc_state.curr_sec_device);    
  }

  /* State Check															 */
  /*=========================================================================*/
  if ((rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RXTX )    ||
      (rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_DEFAULT)   )
  {
    /* *******************************************************************************
      ** Notes:
      ** 1. If current state = RF_TDSCDMA_STATE_RXTX, wrong state to call mc_exit_mode().
      **    L1 should call disable_tx() prior to mc_exit_mode().
      ** 2. If current state = RF_TDSCDMA_STATE_DEFAULT, wrong state to call mc_exit_mode().
      **    It means L1 didn't call mc_enter_mode() prior to calling mc_exit_mode() OR L1 has called
      **    mc_exit_mode() before. mc_exit_mode() can't be called two times in a row without calling
      **    mc_enter_mode in between.
      ** *******************************************************************************/

    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "rf_tdscdma_mc_exit_mode: called in wrong state %d", 
          rf_tdscdma_mc_state.rf_state[RF_PATH_0]);
    return RF_TDSCDMA_MC_SM_ERROR;
  }

#ifdef FEATURE_RF_ASDIV
  /* de-allocate asd memory */
  if (rf_tdscdma_mc_lm_bufs.rx_buf_wrk[pri_path].is_buf_allocated)
  {
    rflm_cmn_asd_abort_pending_event((uint32)rf_tdscdma_mc_lm_bufs.rx_buf_wrk[pri_path].buf_idx);
    (void)rflm_cmn_asd_deallocate_memory((uint32)(rf_tdscdma_mc_lm_bufs.rx_buf_wrk[pri_path].buf_idx));
    MSG_1(MSG_SSID_RF_DEBUG, MSG_LVL_HIGH, 
          "ASD script has been de-allocated for RxLM handle %d", 
          rf_tdscdma_mc_lm_bufs.rx_buf_wrk[pri_path].buf_idx);

  }
  else if (rf_tdscdma_mc_lm_bufs.rx_buf_tgt[pri_path].is_buf_allocated)
  {
    rflm_cmn_asd_abort_pending_event((uint32)rf_tdscdma_mc_lm_bufs.rx_buf_tgt[pri_path].buf_idx);
    (void)rflm_cmn_asd_deallocate_memory((uint32)(rf_tdscdma_mc_lm_bufs.rx_buf_tgt[pri_path].buf_idx));
    MSG_1(MSG_SSID_RF_DEBUG, MSG_LVL_HIGH, 
          "ASD script has been de-allocated for RxLM handle %d", 
          rf_tdscdma_mc_lm_bufs.rx_buf_tgt[pri_path].buf_idx);
  }
  else
  {
    MSG(MSG_SSID_RF_DEBUG, MSG_LVL_HIGH, "No ASD script has been created. No action in exit_mode()");
  }
#endif

  /* de-allocate AOL and CL memory */
  if (rf_tdscdma_mc_lm_bufs.rx_buf_wrk[pri_path].is_buf_allocated)
  {
    (void)rflm_tds_aol_cl_deallocate_memory((uint32)(rf_tdscdma_mc_lm_bufs.rx_buf_wrk[pri_path].buf_idx),
                                            TRUE // bollean isAol
                                           );

    MSG_1(MSG_SSID_RF_DEBUG, MSG_LVL_HIGH,
          "AOL script have been de-allocated for RxLM handle %d",
          rf_tdscdma_mc_lm_bufs.rx_buf_wrk[pri_path].buf_idx);
  }
  else if (rf_tdscdma_mc_lm_bufs.rx_buf_tgt[pri_path].is_buf_allocated)
  {
    (void)rflm_tds_aol_cl_deallocate_memory((uint32)(rf_tdscdma_mc_lm_bufs.rx_buf_tgt[pri_path].buf_idx),
                                            TRUE // bollean isAol
                                            );
    MSG_1(MSG_SSID_RF_DEBUG, MSG_LVL_HIGH,
          "AOL script have been de-allocated for RxLM handle %d",
          rf_tdscdma_mc_lm_bufs.rx_buf_tgt[pri_path].buf_idx);
  }
  else
  {
    MSG(MSG_SSID_RF_DEBUG, MSG_LVL_HIGH, "No AOL nor CL script has been created. No action in exit_mode()");
  }

  /* Disable temp comp and MTPL */
  rf_tdscdma_core_temp_comp_enable(FALSE);
  rf_tdscdma_core_txplim_enable(FALSE);

  /* Cleanup MDSP */
  rf_tdscdma_mdsp_exit(FALSE, // is_irat
                       wait_for_fw_release);

  /* rfdevice RX/TX exit which to clean up rfdevice internal state variables */
  /* Only need to call once on pri_device */
  rfdevice_tdscdma_rx_exit(rf_tdscdma_mc_state.curr_pri_device,
                           RFCOM_TDSCDMA_MODE,
                           rf_tdscdma_mc_state.single_valid_band,
                           RFDEVICE_EXECUTE_IMMEDIATE,
                           NULL);

  rfdevice_tdscdma_tx_exit(rf_tdscdma_mc_state.curr_pri_device,
                           RFCOM_TDSCDMA_MODE,
                           rf_tdscdma_mc_state.single_valid_band,
                           RFDEVICE_EXECUTE_IMMEDIATE,
                           NULL);

  if(rf_tdscdma_mc_state.rf_state[0] != RF_TDSCDMA_STATE_SLEEP)
  {
    /* Put WTR Rx and Tx to LPM */
    /* Put both devices into sleep */
    rfdevice_tdscdma_rx_sleep(rf_tdscdma_mc_state.curr_pri_device,
                              RFCOM_TDSCDMA_MODE,
                              rf_tdscdma_mc_state.curr_band_rx,
                              RFDEVICE_EXECUTE_IMMEDIATE,
                              NULL);

    // added for ASM off
    rfdevice_tdscdma_asm_disable_rx(rf_tdscdma_mc_state.curr_pri_device, 
				                    rf_tdscdma_mc_state.curr_band_rx, 
				                    NULL, 
				                    RFDEVICE_EXECUTE_IMMEDIATE, 
				                    RFCOM_INVALID_TIMING_OFFSET, 
				                    TRUE, 
				                    NULL, 
				                    TDS_ASM_INCLUDE_ALL, 
				                    NULL); 

    rfdevice_tdscdma_asm_sleep_rx(rf_tdscdma_mc_state.curr_pri_device,
                                  rf_tdscdma_mc_state.curr_band_rx,
                                  NULL,
                                  RFDEVICE_EXECUTE_IMMEDIATE,
                                  RFCOM_INVALID_TIMING_OFFSET);

    /* Do once on primary chain ONLY */
    rfc_tdscdma_sleep(rf_tdscdma_mc_state.curr_pri_device);

    if(rf_tdscdma_mc_state.div_enable == TRUE)
    {
      // Do this only when sec_device is valid
      rfdevice_tdscdma_rx_sleep(rf_tdscdma_mc_state.curr_sec_device,
                                RFCOM_TDSCDMA_MODE,
                                rf_tdscdma_mc_state.curr_band_rx,
                                RFDEVICE_EXECUTE_IMMEDIATE,
                                NULL);

      // added for ASM off
      rfdevice_tdscdma_asm_disable_rx(rf_tdscdma_mc_state.curr_sec_device, 
				                      rf_tdscdma_mc_state.curr_band_rx, 
				                      NULL, 
				                      RFDEVICE_EXECUTE_IMMEDIATE, 
				                      RFCOM_INVALID_TIMING_OFFSET, 
				                      TRUE, 
				                      NULL, 
				                      TDS_ASM_INCLUDE_ALL, 
				                      NULL); 

      rfdevice_tdscdma_asm_sleep_rx(rf_tdscdma_mc_state.curr_sec_device,
                                    rf_tdscdma_mc_state.curr_band_rx,
                                    NULL,
                                    RFDEVICE_EXECUTE_IMMEDIATE,
                                    RFCOM_INVALID_TIMING_OFFSET);
    }

#ifdef FEATURE_RF_HAS_QTUNER
    (void)rf_tdscdma_core_antenna_tuner_disable(rf_tdscdma_mc_state.curr_pri_device,
                                                rf_tdscdma_mc_state.curr_band_rx,
                                                RFDEVICE_EXECUTE_IMMEDIATE,
                                                NULL,
                                                0);

    if(rf_tdscdma_mc_state.div_enable == TRUE)
    {
      // Do this only when sec_device is valid
      (void)rf_tdscdma_core_antenna_tuner_disable(rf_tdscdma_mc_state.curr_sec_device,
                                                  rf_tdscdma_mc_state.curr_band_rx,
                                                  RFDEVICE_EXECUTE_IMMEDIATE,
                                                  NULL,
                                                  0);
    }
#endif

  //check RFNV_SPARE_1_I value to know LPM feature enabled through NV
  if (rfcommon_nv_get_lpm_feature_enabled())
  {
    if(rf_tdscdma_mc_display_time_profile)
    {
      exit_mode_start_t = rf_time_get_tick();
    }
    /*vote for wtr pwr sleep*/
    if(rfdevice_tdscdma_rx_pwr_sleep(rf_tdscdma_mc_state.curr_pri_device, rf_tdscdma_mc_state.curr_band_rx))
    {
      if(rf_tdscdma_mc_display_time_profile)
      {
        exit_mode_time = rf_time_get_elapsed(exit_mode_start_t, RF_USEC);
        MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Rx Timeline: wtr pri_dev rx sleep takes %d usec", exit_mode_time);
        exit_mode_start_t = rf_time_get_tick();
      }
    }
    else
    {
  	  MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_tdscdma_mc_exit_mode: rfdevice_tdscdma_rx_pwr_sleep failed for band %d and device %d",
            rf_tdscdma_mc_state.curr_band_rx, rf_tdscdma_mc_state.curr_pri_device);
    }    
    
    if((rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_BOTH)||
    	 (rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_DIV)) //check RxD status
    {
      if(rfdevice_tdscdma_rx_pwr_sleep(rf_tdscdma_mc_state.curr_sec_device, rf_tdscdma_mc_state.curr_band_rx))
      {
        if(rf_tdscdma_mc_display_time_profile)
        {
          exit_mode_time = rf_time_get_elapsed(exit_mode_start_t, RF_USEC);
          MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Rx Timeline: wtr sec_dev rx sleep takes %d usec", exit_mode_time);
          exit_mode_start_t = rf_time_get_tick();
        }
      }
      else
      {
  	    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_tdscdma_mc_exit_mode: rfdevice_tdscdma_rx_pwr_sleep failed for band %d and device %d",
              rf_tdscdma_mc_state.curr_band_rx, rf_tdscdma_mc_state.curr_sec_device);
      }
    }
  }
    /* Disable TCXO buffer */
    /* Only need to call once on pri_device */
    dev_voting_status = rfcommon_core_dev_power_vote(rf_tdscdma_mc_state.curr_pri_device,
                                                     RFCMN_CORE_DEV_OFF,
                                                     RFCMN_CORE_DEV_NO_VOTE);

    if(rf_tdscdma_mc_display_time_profile)
    {
      exit_mode_time = rf_time_get_elapsed(exit_mode_start_t, RF_USEC);
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Rx Timeline: rfcommon_core_dev_power_vote off takes %d usec", exit_mode_time);
      exit_mode_start_t = rf_time_get_tick();
    }

    if( dev_voting_status == FALSE )
    {
      MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_exit_mode(): Device: %d Pwr Up failed", pri_device);
    }

    /*Reset WTR */
    MSG_1(MSG_SSID_RF_DEBUG, MSG_LVL_HIGH, "rf_tdscdma_mc_exit_mode: Reset WTR on device %d", pri_device);
    rfdevice_tdscdma_rx_cmd_dispatch(rf_tdscdma_mc_state.curr_pri_device, 
                                     RFCOM_TDSCDMA_MODE, 
                                     rf_tdscdma_mc_state.curr_band_rx, 
                                     RFDEVICE_RESET_WAN, 
                                     NULL);

    // release vote for RFCMD app
    rf_hal_bus_enable(FALSE, &rf_tdscdma_hal_bus_client_handle, RF_HAL_RFCMD_APP_CLIENT(TDSCDMA));

    /* Vote off VREGs */
    /* Only need to call once on pri_device */
    rfc_tdscdma_manage_vregs(rf_tdscdma_mc_state.curr_pri_device, 
                             rf_tdscdma_mc_state.curr_band_rx, 
                             RF_PATH_SLEEP_STATE);

    /* Update the mode to PARKED since we are exiting */
    rfm_mode_update(rf_tdscdma_mc_state.curr_pri_device, RFCOM_PARKED_MODE);

    MSG_2(MSG_SSID_RF_DEBUG, MSG_LVL_HIGH, 
          "rf_tdscdma_mc_exit_mode: TcXOBuf/RFHal/Vreg voting/Tuner disable "
          "when previously RF is not in sleep on Device %d, state=%d", 
          rf_tdscdma_mc_state.curr_pri_device, 
          rf_tdscdma_mc_state.rf_state[0]);
  }
  else
  {
    /* when current state is SLEEP, it implies L1 call mc_exit_mode without TRM lock. 
           For this scenario, shouldn't touch HW */
    MSG_2(MSG_SSID_RF_DEBUG, MSG_LVL_HIGH, 
          "rf_tdscdma_mc_exit_mode: Skipped TcXOBuf/RFHal/Vreg voting/Tuner disable "
          "when previously RF in sleep (DSDS case) on Device %d, , state=%d", 
          rf_tdscdma_mc_state.curr_pri_device, 
          rf_tdscdma_mc_state.rf_state[0]);
  }

  /* FBRX exit mode */
  if (rflm_tds_is_fbrx_enabled())
  {
    if(RFCOMMON_FBRX_ERROR == rfcommon_fbrx_mc_exit(rf_tdscdma_fbrx_lm_handle, RFM_TDSCDMA_MODE))
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "TDS FBRX exit mode failed!");
    }
    else
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "TDS FBRX exit mode successful!");
    }

    if (rfcommon_autopin_is_enabled(RFM_TDSCDMA_MODE))
    {
      if(RFCOMMON_AUTOPIN_ERROR == rfcommon_autopin_mc_exit_mode(RFM_TDSCDMA_MODE))
      {
        MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "TDS AutoPin exit mode failed!");
      }
      else
      {
        MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "TDS AutoPin exit mode successful!");
      }
    }
  }

  /* Cleanup State variables                                                 */
  /*=========================================================================*/
  for (i = (uint8)RF_PATH_0; i < (uint8)RF_PATH_MAX; i++ )
  {
    rf_tdscdma_mc_state.rf_state[i] = RF_TDSCDMA_STATE_DEFAULT;

    /* Invalid the alt_path information */
    rf_tdscdma_mc_state.alt_path[i] = 0xFF;
    rf_tdscdma_mc_state.alt_path_used_to_build_ifreq[i] = 0xFF;

    rf_tdscdma_mc_state.scell_wb_adc_index_on_devices[i] = 0xFF;
    rf_tdscdma_mc_state.ncell_wb_adc_index_on_devices[i] = 0xFF;
  }  

  rf_tdscdma_mc_state.curr_band_rx = RFCOM_BAND_TDSCDMA_INVALID;
  rf_tdscdma_mc_state.curr_band_tx = RFCOM_BAND_TDSCDMA_INVALID;
  rf_tdscdma_mc_state.curr_chan_rx = 0;
  rf_tdscdma_mc_state.curr_chan_tx = 0;

  for (i = 0; i < 2; i++ )
  {
    rf_tdscdma_mc_lm_bufs.rx_buf_wrk[i].is_buf_allocated = FALSE;
    rf_tdscdma_mc_lm_bufs.rx_buf_wrk[i].buf_idx = 0;
    rf_tdscdma_mc_lm_bufs.rx_buf_wrk[i].mode = RFA_TDSCDMA_RXLM_MODE_MAX;
    rf_tdscdma_mc_lm_bufs.rx_buf_tgt[i].is_buf_allocated = FALSE;
    rf_tdscdma_mc_lm_bufs.rx_buf_tgt[i].buf_idx = 0;
    rf_tdscdma_mc_lm_bufs.rx_buf_tgt[i].mode = RFA_TDSCDMA_RXLM_MODE_MAX;
  }

  rf_tdscdma_mc_lm_bufs.tx_buf_wrk.is_buf_allocated = FALSE;
  rf_tdscdma_mc_lm_bufs.tx_buf_wrk.buf_idx = 0;
  rf_tdscdma_mc_lm_bufs.tx_buf_tgt.is_buf_allocated = FALSE;
  rf_tdscdma_mc_lm_bufs.tx_buf_tgt.buf_idx = 0;

  /* Reset the double buf index */
  rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx = 0;
  rf_tdscdma_mc_dbl_bufs.rx_tgt_buf_idx = 1;
  rf_tdscdma_mc_dbl_bufs.tx_wrk_buf_idx = 0;
  rf_tdscdma_mc_dbl_bufs.tx_tgt_buf_idx = 1;

  rf_tdscdma_mc_state.primary_path = RF_PATH_0;
  rf_tdscdma_mc_state.init_once_flag = TRUE;

  rf_tdscdma_mc_state.pdet_dc_cal_done = FALSE;

  rf_tdscdma_mc_state.last_rxlm_handle_for_AOL_update = 0xFF;

  rf_tdscdma_mc_state.div_enable = FALSE;
  rf_tdscdma_mc_state.hdet_info = 0xFF;
  rf_tdscdma_mc_state.device_wakeup_script_built_flag = FALSE;
  rf_tdscdma_mc_state.device_wakeup_script_exec_flag = FALSE;

  rf_tdscdma_mc_state.curr_pri_device = RFM_INVALID_DEVICE;
  rf_tdscdma_mc_state.curr_sec_device = RFM_INVALID_DEVICE;
  rf_tdscdma_mc_state.curr_rx_path = RFA_TDSCDMA_RX_PATH_PRIM;

  rf_tdscdma_mc_state.single_valid_band = RFCOM_BAND_TDSCDMA_INVALID;
  rf_tdscdma_mc_state.port_state = RF_TDSCDMA_PORT_STATE_A;

  rf_tdscdma_mc_state.last_pri_device_used_to_build_rx_lna_scpt = RFM_INVALID_DEVICE;

  for(loop_band = (uint8)RFCOM_BAND_TDSCDMA_B34; loop_band < (uint8) RFCOM_NUM_TDSCDMA_BANDS; loop_band++)
  {
    rf_tdscdma_mc_state.rx_lna_script_built_on_bands[loop_band] = FALSE;    
  }

  if(rf_tdscdma_mc_display_time_profile)
  {
    exit_mode_time = rf_time_get_elapsed(exit_mode_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Exit mode time profile: %d usec", exit_mode_time);
  }

  return RF_TDSCDMA_MC_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function enables the receiver chain, and tunes to the specified channel.
  This function is also used for the rxlm mode update/transition.

  @details
  The agreement between L1 and RFSW is that mc_enable_rx() can be called
  many times. pri_device must be valid. sec_device must be valid if diversity is
  enabled. sec_device can be garbage if diversity is not enabled. Diversity enabled
  or not is determined by the flag rx_path.

  Based on this agreement, mc_enable_rx() will do following (not strictly sequence):
	*  Enforce to check if a valid pri_device is passed in
	*  Enforce to check if a valid sec_device is passed in (when diversity is enabled)
	*  Enforce to check if correct channel is passed in
	*  Return carrier frequency in Hz
	*  Return working buffer index
	*  State machine checking ONLY on primary chain
	*  Update DL switching point on primary chain
	*  Update DL switching point on secondary chain (when diversity is enabled)
	*  Update RxLM on primary chain
	*  Update RxLM on secondary chain (when diversity is enabled)
	*  Tune WTR on primary chain
	*  Tune WTR on primary chain (when diversity is enabled)
	*  Build all RF scripts and publish to FW shared memory
	*  Configure ASM and GRFCs
	*  Apply frequency compensation on primary chain
	*  Apply frequency compensation on secondary chain (when diversity is enabled)
	*  Update state variables on primary chain
	*  Update state variables on secondary chain (when diversity is enabled)

  @param pri_device
  Primary device (logical device)

  @param sec_device
  Secondary device (logical device)

  @param rx_channel
  DL channel number

  @param rxlm_buffer_idx
  RxLM buffer index for primary chain

  @param rxlm_mode
  RxLM mode for primary chain

  @param rx_path
  To indicate whether diversity is enabled

  @param rxlm_buffer_idx_div
  RxLM buffer index for diversity chain

  @param rxlm_mode_div
  RxLM mode for diversity chain

  @param *carrier_freq
  To return carrier frequency in Hz

  @param *rx_wrk_buffer_idx
  To return DL working buffer index

  @return
  rf_tdscdma_mc_status_type

*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_enable_rx
(
  rfm_device_enum_type    pri_device,
  rfm_device_enum_type    sec_device,
  uint16   			   rx_channel,
  lm_handle_type   	   rxlm_buffer_idx,
  rfa_tdscdma_rxlm_mode_t rxlm_mode,
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  rfa_tdscdma_rx_path_t   rx_path,
  lm_handle_type   	   rxlm_buffer_idx_div,
  rfa_tdscdma_rxlm_mode_t rxlm_mode_div,
#endif
  uint32 *carrier_freq,
  uint8 *rx_wrk_buffer_idx,
  boolean is_dsds,
  uint8 rx_lin_state,
  boolean use_sw_tune_in_acq
)
{
  rf_path_enum_type pri_path, sec_path;
  rfcom_tdscdma_band_type band;
  rf_time_tick_type enable_rx_start_t = {0};
  rf_time_type enable_rx_time;
  boolean script_status;
  boolean call_in_wakeup = FALSE;
  rftdscdma_mdsp_common_event_data_type rf_tds_build_event_data;

  /* dsds_enable_rx portion */
  boolean dev_voting_status;
  uint32 ant_position;

  if(rf_tdscdma_mc_display_time_profile)
  {
    enable_rx_start_t = rf_time_get_tick(); /* time spent in API */
  }

  /* Convert rfm_device to rf_path_enum_type */
  pri_path = rfcommon_core_device_to_path(pri_device);
  sec_path = rfcommon_core_device_to_path(sec_device);

  /* Check L1 parameters													 */
  /*=========================================================================*/

  MSG_7(MSG_SSID_RF, MSG_LVL_HIGH,
        "rf_tdscdma_mc_enable_rx(pri_device=%d (path %d), sec_device=%d (path %d), rx_chan=%d, rxlm_buf_idx=%d, rxlm_mode=%d)",
        pri_device, pri_path, sec_device, sec_path, rx_channel, rxlm_buffer_idx, rxlm_mode);

  MSG_3(MSG_SSID_RF, MSG_LVL_HIGH,
        "rf_tdscdma_mc_enable_rx(is_dsds=%d, rx_lin_state=%d, use_sw_tune_in_acq=%d)",
        is_dsds, rx_lin_state, use_sw_tune_in_acq);

#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  MSG_3(MSG_SSID_RF, MSG_LVL_HIGH,
        "rf_tdscdma_mc_enable_rx(rx_path=%d, rxlm_buffer_idx_div=%d, rxlm_mode_div=%d)",
        rx_path, rxlm_buffer_idx_div, rxlm_mode_div);
#endif

  if (pri_path != RF_PATH_0)
  {
    // RF_PATH_0 = PRIMARY CHAIN.
    // Must pass valid pri_device into mc_enable_rx() which must be on primary chain
    MSG_2(MSG_SSID_RF, MSG_LVL_ERROR,
    "rf_tdscdma_mc_enable_rx: called on wrong primary device %d (path %d)", pri_device, pri_path);
    return RF_TDSCDMA_MC_INVALID_PARAM;
  }

#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  if((rx_path == RFA_TDSCDMA_RX_PATH_BOTH)  ||
     (rx_path == RFA_TDSCDMA_RX_PATH_DIV)    )
  {
    if (sec_path != RF_PATH_1)
    {
      // RF_PATH_1 = SECONDARY CHAIN
      // When diversity is enabled, Must pass valid sec_device into mc_enable_rx()
      // which must be on secondary chain
      MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_enable_rx: called on wrong secondary device %d (path %d)", sec_device, sec_path);
      return RF_TDSCDMA_MC_INVALID_PARAM;
    }
  }
#endif /* RF_TDSCDMA_RX_DIVERSITY_ENABLE */


  /* SW INIT Section														 */
  /*=========================================================================*/

  /* Get the band for the given channel num */
  band = rf_tdscdma_core_util_get_band_from_uarfcn(rx_channel);

  /* If band is invalid this means channel is wrong */
  if( band == RFCOM_BAND_TDSCDMA_INVALID )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_enable_rx: wrong channel %d", rx_channel);
    return RF_TDSCDMA_MC_INVALID_PARAM;
  }

  /* Notify GNSS of TDSCDMA Band and Channel */
  RFGNSS_MC_V(tdscdma_band_chan)(band, rx_channel);

  /* send carrier freq and rx working buffer index to L1 */
  if (carrier_freq != NULL)
  {
    /* carrier freq in Hz */
    *carrier_freq = rf_tdscdma_core_util_get_freq_from_uarfcn(rx_channel, band)*1000;
  }

  if (rx_wrk_buffer_idx != NULL)
  {
    *rx_wrk_buffer_idx = rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx;
  }


  /* State Check															 */
  /*=========================================================================*/
  if ( (rf_tdscdma_mc_state.rf_state[pri_path] != RF_TDSCDMA_STATE_INIT) &&
       (rf_tdscdma_mc_state.rf_state[pri_path] != RF_TDSCDMA_STATE_RX) )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "rf_tdscdma_mc_enable_rx: called in wrong state (current state=%d)", 
          rf_tdscdma_mc_state.rf_state[pri_path]);
    return RF_TDSCDMA_MC_SM_ERROR;
  }

  if(rf_tdscdma_mc_display_time_profile)
  {
    enable_rx_start_t = rf_time_get_tick(); /* time spent in API */
  }

  //check RFNV_SPARE_1_I value to know LPM feature enabled through NV
 if (rfcommon_nv_get_lpm_feature_enabled())
 {
  if(rf_tdscdma_mc_state.curr_band_rx == band)
  {
    if(rf_tdscdma_mc_state.curr_rx_path != rx_path)
    {
      if((rx_path == RFA_TDSCDMA_RX_PATH_BOTH)||(rx_path == RFA_TDSCDMA_RX_PATH_DIV))
      {
        //wakeup sec_device	
        if(rfdevice_tdscdma_rx_pwr_wakeup(sec_device, band))
        {
          if(rf_tdscdma_mc_display_time_profile)
          {
            enable_rx_time = rf_time_get_elapsed(enable_rx_start_t, RF_USEC);
            MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Rx Timeline: wtr sec_dev rx wakeup takes %d usec", enable_rx_time);
            enable_rx_start_t = rf_time_get_tick();
          }          
        }
        else
        {
          MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_tdscdma_mc_enable_rx: rfdevice_tdscdma_rx_pwr_wakeup failed for band %d and device %d",
                band, sec_device);
        }
      }
      else
      {
      	//sleep sec_device	
        if(rfdevice_tdscdma_rx_pwr_sleep(sec_device, band))
        {
          if(rf_tdscdma_mc_display_time_profile)
          {
            enable_rx_time = rf_time_get_elapsed(enable_rx_start_t, RF_USEC);
            MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Rx Timeline: wtr sec_dev rx sleep takes %d usec", enable_rx_time);
            enable_rx_start_t = rf_time_get_tick();
          }          
        }
        else
        {
          MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_tdscdma_mc_enable_rx: rfdevice_tdscdma_rx_pwr_sleep failed for band %d and device %d",
                band, sec_device);
        }
      }
    }
  }
  else
  {
  	if(rf_tdscdma_mc_state.curr_band_rx != RFCOM_BAND_TDSCDMA_INVALID)
  	{
  	  if(rfdevice_tdscdma_rx_pwr_sleep(pri_device, rf_tdscdma_mc_state.curr_band_rx))
      {
        if(rf_tdscdma_mc_display_time_profile)
        {
          enable_rx_time = rf_time_get_elapsed(enable_rx_start_t, RF_USEC);
          MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Rx Timeline: wtr pri_dev rx sleep takes %d usec", enable_rx_time);
                enable_rx_start_t = rf_time_get_tick();
        }
      }
      else
      {
        MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_tdscdma_mc_enable_rx: rfdevice_tdscdma_rx_pwr_sleep failed for band %d and device %d",
              rf_tdscdma_mc_state.curr_band_rx, pri_device);
      }
  	
  	  if((rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_BOTH)||(rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_DIV)) //check RxD status
      {
  	    if(rfdevice_tdscdma_rx_pwr_sleep(sec_device, rf_tdscdma_mc_state.curr_band_rx))
        {
          if(rf_tdscdma_mc_display_time_profile)
          {
            enable_rx_time = rf_time_get_elapsed(enable_rx_start_t, RF_USEC);
            MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Rx Timeline: wtr sec_dev rx sleep takes %d usec", enable_rx_time);
                  enable_rx_start_t = rf_time_get_tick();
          }
        }
        else
        {
  	      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_tdscdma_mc_wakeup_step1: rfdevice_tdscdma_rx_pwr_sleep failed for band %d and device %d",
                rf_tdscdma_mc_state.curr_band_rx, sec_device);
        }
      }
    }
  	
  	if(rfdevice_tdscdma_rx_pwr_wakeup(pri_device, band))
    {
      if(rf_tdscdma_mc_display_time_profile)
      {
        enable_rx_time = rf_time_get_elapsed(enable_rx_start_t, RF_USEC);
        MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Rx Timeline: wtr pri_dev rx wakeup takes %d usec", enable_rx_time);
              enable_rx_start_t = rf_time_get_tick();
      }
    }
    else
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_tdscdma_mc_enable_rx: rfdevice_tdscdma_rx_pwr_wakeup failed for band %d and device %d",
            band, pri_device);
    }

    if((rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_BOTH)||(rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_DIV)) //check RxD status
    {
  	  if(rfdevice_tdscdma_rx_pwr_wakeup(sec_device, band))
      {
        if(rf_tdscdma_mc_display_time_profile)
        {
          enable_rx_time = rf_time_get_elapsed(enable_rx_start_t, RF_USEC);
          MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Rx Timeline: wtr sec_dev rx wakeup takes %d usec", enable_rx_time);
                enable_rx_start_t = rf_time_get_tick();
        }
      }
      else
      {
  	    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_tdscdma_mc_wakeup_step1: rfdevice_tdscdma_rx_pwr_wakeup failed for band %d and device %d",
              band, sec_device);
      }
    }

  }
 }
  /* State variables update (Step 1)										 */
  /*=========================================================================*/
  /* For those variables when be used in below API calls					 */

  /* Update the device information */
  rf_tdscdma_mc_state.curr_pri_device = pri_device;
  rf_tdscdma_mc_state.curr_sec_device = sec_device;

  /* Update Tx/Rx NV ptr */
  rfnv_tdscdma_rx_tbl_ptr = rf_tdscdma_core_util_init_rx_nv_table(band);
  rfnv_tdscdma_tx_tbl_ptr = rf_tdscdma_core_util_init_tx_nv_table(band);

  /* Update the rx_path information */
  rf_tdscdma_mc_state.curr_rx_path = rx_path;

  if( (rf_tdscdma_msm_compare_is_same_rxlm_mode(rxlm_mode, RFA_TDSCDMA_RXLM_MODE_ACQ)) ||
      (rf_tdscdma_msm_compare_is_same_rxlm_mode(rxlm_mode, RFA_TDSCDMA_RXLM_MODE_PWR_SCAN)))
  {
    rf_tdscdma_mc_state.rx_lin_state = rx_lin_state;
  }
  else
  {
    rf_tdscdma_mc_state.rx_lin_state = 0;
  }
  
  /* Check if RxLM buffer index changed */
  if ( rxlm_buffer_idx != rf_tdscdma_mc_lm_bufs.rx_buf_wrk[pri_path].buf_idx )
  {
    MSG_2(MSG_SSID_RF_DEBUG, MSG_LVL_MED, "rf_tdscdma_mc_enable_rx: Changed to RxLM buffer idx=%d from %d", 
          rxlm_buffer_idx,
          rf_tdscdma_mc_lm_bufs.rx_buf_wrk[pri_path].buf_idx);
  }

  /* Save RxLM mode and index */
  rf_tdscdma_mc_lm_bufs.rx_buf_wrk[pri_path].is_buf_allocated = TRUE;
  rf_tdscdma_mc_lm_bufs.rx_buf_wrk[pri_path].mode = rxlm_mode;
  rf_tdscdma_mc_lm_bufs.rx_buf_wrk[pri_path].buf_idx = rxlm_buffer_idx;

  if( rfm_get_calibration_state() == FALSE )
  {
    #ifdef FEATURE_RF_ASDIV
    /* allocate asd script buffer */  
    script_status = rflm_cmn_asd_allocate_memory((uint32)(rf_tdscdma_mc_lm_bufs.rx_buf_wrk[pri_path].buf_idx));
    if(script_status)
    {
      MSG_1(MSG_SSID_RF, MSG_LVL_HIGH,
      "Allocated ASD script for RxLM handle %d",
      rf_tdscdma_mc_lm_bufs.rx_buf_wrk[pri_path].buf_idx);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LVL_ERROR,
      "Can't allocated ASD script for RxLM handle %d",
      rf_tdscdma_mc_lm_bufs.rx_buf_wrk[pri_path].buf_idx);
    }  
    #endif

    /* allocate AOL script buffer */
    script_status = rflm_tds_aol_cl_allocate_memory((uint32)(rf_tdscdma_mc_lm_bufs.rx_buf_wrk[pri_path].buf_idx),
                                                   TRUE // boolean isAol
                                                   );
    script_status &= rflm_tds_aol_cl_allocate_memory((uint32)(rf_tdscdma_mc_lm_bufs.rx_buf_wrk[pri_path].buf_idx),
                                                    FALSE // boolean isCl
                                                    );
                                                   
    if(script_status)
    {
      MSG_1(MSG_SSID_RF, MSG_LVL_HIGH,
      "Allocated AOL script for RxLM handle %d",
      rf_tdscdma_mc_lm_bufs.rx_buf_wrk[pri_path].buf_idx);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LVL_ERROR,
      "Can't allocated AOL script for RxLM handle %d",
      rf_tdscdma_mc_lm_bufs.rx_buf_wrk[pri_path].buf_idx);
    }
  }


#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  if ( (rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
       (rx_path == RFA_TDSCDMA_RX_PATH_DIV)   )
  {
    rf_tdscdma_mc_state.div_enable = TRUE;

    if ( rxlm_buffer_idx_div != rf_tdscdma_mc_lm_bufs.rx_buf_wrk[sec_path].buf_idx )
    {
      /* Flag an error but continue */
      MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_enable_rx: wrong buffer idx (for Rx1)=%d", rxlm_buffer_idx_div);
    }

    /* Save RxLM mode and index */
    rf_tdscdma_mc_lm_bufs.rx_buf_wrk[sec_path].is_buf_allocated = TRUE;
    rf_tdscdma_mc_lm_bufs.rx_buf_wrk[sec_path].mode = rxlm_mode_div;
    rf_tdscdma_mc_lm_bufs.rx_buf_wrk[sec_path].buf_idx = rxlm_buffer_idx_div;
  }
#endif /* RF_TDSCDMA_RX_DIVERSITY_ENABLE */

  /* Set the flag for FTM RF mode */
  rflmTdsRfModeCtlState.isRfFtmTestMode = FALSE; 
  if ( IS_FTM_IN_TEST_MODE() && (rfm_get_calibration_state() == FALSE) )
  {
    rflmTdsRfModeCtlState.isRfFtmTestMode = TRUE; 
  }

  if(is_dsds)
  {
    rf_card_band_type rfc_band;
    rfcmn_status_type alt_path_status;
    uint8 alt_path = 0;
    boolean rfc_alt_path_status;
    uint8 loop_device, loop_band;
    rfm_device_enum_type logic_device;
    
    /* Update alt_path														 */
    /*=========================================================================*/
    /* Loop for all logic_devices and all bands */
    for(loop_device=0; loop_device<2; loop_device++)
    {
      if(loop_device==0)
      {
        logic_device = pri_device;
      }
      else if (loop_device==1)
      {
        logic_device = pri_device+1; // Take assumption that the sec_device always = pri_device + 1
      }
                 
      for(loop_band = (uint8)(RFCOM_BAND_TDSCDMA_B34); loop_band <= (uint8)(RFCOM_BAND_TDSCDMA_B39); loop_band++)
      {
        rfc_band = rfcommon_core_convert_tds_band_to_rfc_band(loop_band);
        alt_path_status =  rfcmn_concurrency_mgr_update_rx_path(logic_device,
                                                                RFCOM_TDSCDMA_MODE,
                                                                (int)loop_band,
                                                                &alt_path);
        if (alt_path_status == RFCMN_PATH_SEL_FAILED)
        {
          MSG_3(MSG_SSID_RF_DEBUG, MSG_LEGACY_HIGH, 
                "Cannot get alt_path for device %d, TDS band %d, rfc_band %d", logic_device, loop_band, rfc_band);
        }

        if (IS_FTM_IN_TEST_MODE())
        {
          // In RFcal mode, always uses alt_path from tools
          if (rfm_get_calibration_state() == TRUE) // RF calibration mode
          {
            alt_path = ftm_tdscdma_current_pri_alt_path;
          
            MSG_1(MSG_SSID_RF_DEBUG, MSG_LEGACY_HIGH, 
                  "Use the alt_path %d passed by Tools for Rx0 in RFcal mode", ftm_tdscdma_current_pri_alt_path);  
          }
          // In FTM RF mode, use alt_pth from tools if ftm_tdscdma_override_alt_path is TRUE
          else // FTM RF mode
          {            
            if(ftm_tdscdma_override_alt_path == TRUE)
            {
              alt_path = ftm_tdscdma_current_pri_alt_path;
          
              MSG_1(MSG_SSID_RF_DEBUG, MSG_LEGACY_HIGH, 
                    "Use the alt_path %d passed by Tools for Rx0 in FTM RF mode", ftm_tdscdma_current_pri_alt_path);      
            }            
          }
        }

        rfc_alt_path_status = rfc_update_alt_port_mapping(logic_device, 
                                                          RFCOM_TDSCDMA_MODE, 
                                                          rfc_band, 
                                                          alt_path);
        if (rfc_alt_path_status == FALSE)
        {
          MSG_3(MSG_SSID_RF_DEBUG, MSG_LEGACY_HIGH, 
                "Cannot get alt_path for device %d, TDS band %d, rfc_band %d", logic_device, loop_band, rfc_band);
        }
      }  

      MSG_2(MSG_SSID_RF_DEBUG, MSG_LEGACY_HIGH, 
            "Updated ALT_PATH to %d for device %d", alt_path, logic_device);
    }

    /* is_dsds = TRUE used in DSDS wakeup. So need to set below flag */
    call_in_wakeup = TRUE;

    /* Move those require HW operation or MCPM here: effectively dsds_enable_rx() */

    /* vote on Rx VREGs */
    /* Only need to call once on pri_device and assume it is band independent */
    rfc_tdscdma_manage_vregs(pri_device, RFCOM_BAND_TDSCDMA_B34, RF_PATH_RX_STATE);

    // vote for RFCMD app
    rf_hal_bus_enable(TRUE, &rf_tdscdma_hal_bus_client_handle, RF_HAL_RFCMD_APP_CLIENT(TDSCDMA));

    /* Program the GPIO/GRFC/RF_CTL signals for the TDSCDMA mode */
    /* Only need to call once on pri_device */
    rfc_tdscdma_mode_config(pri_device);

    /* Enable TCXO buffer */
    /* Only need to call once on pri_device */
    dev_voting_status = rfcommon_core_dev_power_vote(pri_device,
                                                     RFCMN_CORE_DEV_ON,
                                                     RFCMN_CORE_DEV_NORMAL_VOTE);

    if( dev_voting_status == FALSE )
    {
      MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_enter_mode(): Device: %d Pwr Up failed", pri_device);
    }

    /* rfdevice RX/TX init which to clean up rfdevice internal state variables and get band-port mapping */
    /* Only need to call once on pri_device */
    rfdevice_tdscdma_rx_init(pri_device,
                             RFCOM_TDSCDMA_MODE,
                             rf_tdscdma_mc_state.single_valid_band,
                             RFDEVICE_EXECUTE_IMMEDIATE,
                             NULL);

    rfdevice_tdscdma_tx_init(pri_device,
                             RFCOM_TDSCDMA_MODE,
                             rf_tdscdma_mc_state.single_valid_band,
                             RFDEVICE_EXECUTE_IMMEDIATE,
                             NULL);


     /* One-time initialization */
   if (rf_tdscdma_mc_state.init_once_flag == TRUE)
   {
    int mdsp_init_vars;
   
    /* Initialize TDSCDMA mDSP interface and program mdsp one time static settings
    (both rx and tx, band-independent) */
    rf_tdscdma_mdsp_init(pri_device,
    (void*)&mdsp_init_vars,
    FALSE //is_irat
                        );
   
    /* calculate rx cal chan freqs using rx cal channels for each band.
    rx cal chan freqs will be used for rx freq comp calculation */
    // Calcuation need to be done on both primary and secondary path
    rf_tdscdma_core_rxctl_calc_rx_cal_chan_freq(RF_PATH_0, RFCOM_BAND_TDSCDMA_B34);
    rf_tdscdma_core_rxctl_calc_rx_cal_chan_freq(RF_PATH_0, RFCOM_BAND_TDSCDMA_B39);
    //rf_tdscdma_core_rxctl_calc_rx_cal_chan_freq(RF_PATH_0, RFCOM_BAND_TDSCDMA_B40);
   
    rf_tdscdma_core_rxctl_calc_rx_cal_chan_freq(RF_PATH_1, RFCOM_BAND_TDSCDMA_B34);
    rf_tdscdma_core_rxctl_calc_rx_cal_chan_freq(RF_PATH_1, RFCOM_BAND_TDSCDMA_B39);
    //rf_tdscdma_core_rxctl_calc_rx_cal_chan_freq(RF_PATH_1, RFCOM_BAND_TDSCDMA_B40);
   
    // Initialize the debug&functional flags
    rf_tdscdma_mc_init_debug_and_func_flags();
   
    rf_tdscdma_mc_state.init_once_flag = FALSE;
   }

    //if (pri_path == RF_PATH_0)
    {
      /* Disable temp comp and MTPL */
      rf_tdscdma_core_temp_comp_enable(FALSE);
      rf_tdscdma_core_txplim_enable(FALSE);

      rfdevice_tdscdma_rx_cmd_dispatch(RFM_DEVICE_0,
                                       RFCOM_TDSCDMA_MODE,
                                       rf_tdscdma_mc_state.single_valid_band,
                                       RFDEVICE_TDSCDMA_GET_HDET_INFO,
                                       &rf_tdscdma_mc_state.hdet_info);

      if ((!IS_FTM_IN_TEST_MODE()) || (!rfm_get_calibration_state()))
      {
        #if 0
        int4 t_unused = 0;

        /* Read one therm value */
        rf_tdscdma_mc_state.curr_pri_device = pri_device;
        rf_tdscdma_core_temp_comp_timer(t_unused);
        #endif      
      }
      rf_tdscdma_mc_state.pdet_dc_cal_done = FALSE;
    }

    /*set antenna position during enter_mode() */
#ifdef FEATURE_RF_ASDIV
    ant_position = rfcommon_asdiv_get_current_position(pri_device);
#else
    ant_position = 0; // hard-coded the default position if feature is not defined
#endif

    // [Jay]To-DO: Move to MDSP and check if ok to hard-coded B34
    rf_tdscdma_mc_asdiv_script(ant_position, pri_device, RFCOM_BAND_TDSCDMA_B34);

    /* FBRX enter mode to get FBRX LM handle */
    if (rflm_tds_is_fbrx_enabled())
    {
      if(RFCOMMON_FBRX_ERROR == rfcommon_fbrx_mc_enter_mode(&rf_tdscdma_fbrx_lm_handle, RFM_TDSCDMA_MODE))
      {
        MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "TDS FBRX enter mode failed!");
      }
      else
      {
        MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "TDS FBRX enter mode successful!");
      }
    }

    /* Build Rx LNA scripts for primary chain */
    rf_tds_build_event_data.pri_device = pri_device;
    rf_tds_build_event_data.sec_device = pri_device+1;
    rf_tds_build_event_data.tx_device = pri_device;
    rf_tds_build_event_data.drx_enable = FALSE;
    rf_tds_build_event_data.is_irat = FALSE;
    rf_tds_build_event_data.rx_lna_update_type = RF_TDSCDMA_MDSP_RX_LNA_UPDATE_FOR_SCELL;
    rf_tds_build_event_data.is_bho_update = FALSE;

    // Invalid all bands and set the desired bands for Rx LNA script update
    for(loop_band = (uint8)RFCOM_BAND_TDSCDMA_B34; loop_band <= (uint8) RFCOM_NUM_TDSCDMA_BANDS; loop_band++)
    {
      rf_tds_build_event_data.to_build_rx_lna_script_on_bands[loop_band] = FALSE;
    }
    rf_tds_build_event_data.to_build_rx_lna_script_on_bands[RFCOM_BAND_TDSCDMA_B34] = TRUE;
    rf_tds_build_event_data.to_build_rx_lna_script_on_bands[RFCOM_BAND_TDSCDMA_B39] = TRUE;
    
    rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_BUILD_RX_LNA, &rf_tds_build_event_data);

    /* Build Rx LNA scripts for secondary chain */
    rf_tds_build_event_data.drx_enable = TRUE;
    rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_BUILD_RX_LNA, &rf_tds_build_event_data);
  }


  /* Update Rx MDSP (DM/SHM content)										 */
  /*=========================================================================*/
  rf_tdscdma_mc_update_rx_mdsp(pri_device,
                               pri_path,
                               band,
                               rx_channel,
                               rxlm_buffer_idx,
                               rxlm_mode,
                               call_in_wakeup,
                               rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx,
                               FALSE,
                               RFA_TDSCDMA_WAKEUP_NA);


  /* Config Rx Chain (RF/HW)												 */
  /*=========================================================================*/
  rf_tdscdma_mc_config_rx_chain(pri_device,
                                pri_path,
                                band,
                                rx_channel,
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
                                rx_path,
#endif
                                rxlm_buffer_idx,
                                rxlm_mode,
                                call_in_wakeup,
                                rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx,
                                rf_tdscdma_mc_state.rx_lin_state,
                                use_sw_tune_in_acq);


#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  if( (rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
      (rx_path == RFA_TDSCDMA_RX_PATH_DIV)    )
  {
    /* Update Rx MDSP (DM/SHM content)                       */
    /*=========================================================================*/
    rf_tdscdma_mc_update_rx_mdsp(sec_device,
                                 sec_path,
                                 band,
                                 rx_channel,
                                 rxlm_buffer_idx_div,
                                 rxlm_mode_div,
                                 call_in_wakeup,
                                 rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx,
                                 FALSE,
                                 RFA_TDSCDMA_WAKEUP_NA);

    /* Config Rx Chain (RF/HW)  											   */
    /*=========================================================================*/
    rf_tdscdma_mc_config_rx_chain(sec_device,
                                  sec_path,
                                  band,
                                  rx_channel,
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
                                  rx_path,
#endif
                                  rxlm_buffer_idx_div,
                                  rxlm_mode_div,
                                  call_in_wakeup,
                                  rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx,
                                  rf_tdscdma_mc_state.rx_lin_state,
                                  use_sw_tune_in_acq);
  }
#endif /* RF_TDSCDMA_RX_DIVERSITY_ENABLE */


  /* State machine updates (Step 2) 										 */
  /*=========================================================================*/

  /* Update the band information */
  rf_tdscdma_mc_state.curr_band_rx = band;

  /* Update the channel information */
  rf_tdscdma_mc_state.curr_chan_rx = rx_channel;

  rflmTdsRfModeCtlState.rxdEn = rf_tdscdma_mc_state.div_enable;
  rflmTdsRfModeCtlState.homeBufIdx = rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx; 
  rflmTdsRfModeCtlState.rxHomeBufIdx = rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx;    

  /* transition to RX state */
  if (rf_tdscdma_mc_state.rf_state[pri_path] != RF_TDSCDMA_STATE_RXTX)
  {
    rf_tdscdma_mc_state.rf_state[pri_path] = RF_TDSCDMA_STATE_RX;

    if((rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
       (rx_path == RFA_TDSCDMA_RX_PATH_DIV)   )
    {
      rf_tdscdma_mc_state.rf_state[sec_path] = RF_TDSCDMA_STATE_RX;
    }
  }

  if( rfm_get_calibration_state() == FALSE )   
  {
    /* Note: Can't put this into update_rx_mdsp() because the JDET 
    **  	 script are not double buffered.								   */   
    if(rflmTdsRfModeCtlState.jdetInfo.enabled)
    {
      rf_tds_build_event_data.pri_device = rf_tdscdma_mc_state.curr_pri_device;
      rf_tds_build_event_data.sec_device = rf_tdscdma_mc_state.curr_sec_device;
      rf_tds_build_event_data.tx_device = rf_tdscdma_mc_state.curr_pri_device;
      rf_tds_build_event_data.band = band;
      rf_tds_build_event_data.channel = rx_channel;
      rf_tds_build_event_data.drx_enable = FALSE;
      rf_tds_build_event_data.buf_id = rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx;
      rf_tds_build_event_data.is_bho_update = FALSE;

      rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_DO_JDET, &rf_tds_build_event_data);
    }
   
    #ifdef FEATURE_RF_ASDIV
    ant_position = rfcommon_asdiv_get_current_position(pri_device);
    rf_tdscdma_mc_asdiv_script(ant_position, pri_device, band);
    #endif
  }

  if(rf_tdscdma_mc_display_time_profile)
  {
    enable_rx_time = rf_time_get_elapsed(enable_rx_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Enable Rx time profile: %d usec", enable_rx_time);
  }

  return RF_TDSCDMA_MC_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
   This function initializes tx for TDSCDMA uplink operation.
   Reserved for future use - in case rf_tdscdma_mc_enable_tx is too long

  @details
*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_init_tx(void)
{

  rf_time_tick_type init_tx_start_t = {0};
  rf_time_type init_tx_time;

  if(rf_tdscdma_mc_display_time_profile)
  {
    init_tx_start_t = rf_time_get_tick();
  }

  MSG(MSG_SSID_RF, MSG_LVL_HIGH,"rf_tdscdma_mc_init_tx");

  /* State Check															 */
  /*=========================================================================*/
  if ( rf_tdscdma_mc_state.rf_state[RF_PATH_0] != RF_TDSCDMA_STATE_RX )
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_init_tx: called in wrong state");
    return RF_TDSCDMA_MC_SM_ERROR;
  }

  if (!IS_FTM_IN_TEST_MODE())
  {
    rf_tdscdma_core_txplim_init();
  }

  if(rf_tdscdma_mc_display_time_profile)
  {
    init_tx_time = rf_time_get_elapsed(init_tx_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Init Tx time profile: %d usec", init_tx_time);
  }

  return RF_TDSCDMA_MC_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function enables the rf transmit chain for TDSCDMA uplink operation.

  @details

  @param device
  TX device (logical device)

  @param tx_channel
  UL channel number

  @param txlm_buffer_idx
  TxLM buffer index

  @param *tx_wrk_buffer_idx
  To return UL working buffer index

  @return
  rf_tdscdma_mc_status_type

*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_enable_tx
(
  rfm_device_enum_type device,
  uint16 tx_channel,
  lm_handle_type txlm_buffer_idx,
  uint8 *tx_wrk_buffer_idx
)
{
  rf_path_enum_type path;
  rf_time_tick_type enable_tx_start_t = {0};
  rf_time_tick_type overall_enable_tx_start_t = {0};
  rf_time_type enable_tx_time, overall_enable_tx_time;
  rfcom_tdscdma_band_type band;
  int32 ret_val;
  rfcommon_fbrx_error_type rf_tdscdma_mc_fbrx_status = RFCOMMON_FBRX_ERROR;
  uint16 fbrx_chan_list[RFCOMMON_FBRX_1_CARRIER] = {0xFFFF};
  rftdscdma_mdsp_common_event_data_type rf_tds_build_event_data;
  rfdevice_trx_tdscdma_tx **device_ptr = NULL;
  rfdevice_rxtx_common_class* cmn_device_ptr = NULL;
  uint8 device_idx = 0;

  band = rf_tdscdma_core_util_get_band_from_uarfcn(tx_channel);

  /* Crash in case either band has New/Old NV mixed*/
  if (rfnv_tdscdma_b34_tx_get_mixed_nv_flag()||rfnv_tdscdma_b39_tx_get_mixed_nv_flag())
  {
    MSG_ERROR("New NV items should not be compatible with old NV ones!",0,0,0);
    //ASSERT(0);
  }

  if (rf_tdscdma_use_on_rumi == TRUE)
  {
    /* Work-around for Rumi UL only test */

    /* update TxLM setting */
    rf_tdscdma_msm_update_txlm_buffer(txlm_buffer_idx, band);

    MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, "rf_tdscdma_mc_enable_tx returns after populating txlm(txlm_buffer_id=%d) on RUMI", txlm_buffer_idx);

    return RF_TDSCDMA_MC_SUCCESS;
  }

  if(rf_tdscdma_mc_display_time_profile || rf_tdscdma_mc_tx_time_debug)
  {
    enable_tx_start_t = rf_time_get_tick();
    overall_enable_tx_start_t = rf_time_get_tick();
  }

  path = rfcommon_core_device_to_path(device);
  MSG_4(MSG_SSID_RF, MSG_LVL_HIGH, "rf_tdscdma_mc_enable_tx(device=%d (path %d), tx_chan=%d, txlm_buf_idx=%d)", device, path, tx_channel, txlm_buffer_idx);

  /* TX must be on primary chain */
  if( path != RF_PATH_0 )
  {
    MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_enable_tx: Unsupported Device %d (path %d)", device, path);
    return RF_TDSCDMA_MC_INVALID_PARAM;
  }

  /* Send tx working buffer index to L1 */
  if( tx_wrk_buffer_idx != NULL )
  {
    *tx_wrk_buffer_idx = rf_tdscdma_mc_dbl_bufs.tx_wrk_buf_idx;
  }

  /* State Check															 */
  /*=========================================================================*/
  if ( rf_tdscdma_mc_state.rf_state[path] != RF_TDSCDMA_STATE_RX )
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_enable_tx: called in wrong state");
    return RF_TDSCDMA_MC_SM_ERROR;
  }

  if (tx_channel != rf_tdscdma_mc_state.curr_chan_rx )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR,"rf_tdscdma_mc_enable_tx: wrong tx channel(tx_chan=%d)", tx_channel);
    return RF_TDSCDMA_MC_INVALID_PARAM;
  }
  else
  {
    rf_tdscdma_mc_state.curr_chan_tx = tx_channel;
    rf_tdscdma_mc_state.curr_band_tx = rf_tdscdma_mc_state.curr_band_rx;
  }

  /* Notify GNSS of TDSCDMA Tx On */
  RFGNSS_MC(wwan_tx_status,ret_val)(device, TRUE);

  if (!ret_val)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_enable_tx: RFGNSS_MC(device %d, TRUE) returns error", device);
  }

  if (rf_tdscdma_mc_tx_time_debug)
  {
    enable_tx_time = rf_time_get_elapsed(enable_tx_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Tx Timeline: param check and GPS notification takes %d usec", enable_tx_time);
    enable_tx_start_t = rf_time_get_tick();
  }

  /* vote for TX VREGs */
  rfc_tdscdma_manage_vregs(device, rf_tdscdma_mc_state.curr_band_tx, RF_PATH_RXTX_STATE);

  if (rf_tdscdma_mc_tx_time_debug)
  {
    enable_tx_time = rf_time_get_elapsed(enable_tx_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Tx Timeline: Vreg voting takes %d usec", enable_tx_time);
    enable_tx_start_t = rf_time_get_tick();
  }

  //check RFNV_SPARE_1_I value to know LPM feature enabled through NV
  if (rfcommon_nv_get_lpm_feature_enabled())
  {
    /*vote for wtr pwr wakeup*/
    if(rfdevice_tdscdma_tx_pwr_wakeup(device, band))
    {
      if (rf_tdscdma_mc_tx_time_debug)
      {
        enable_tx_time = rf_time_get_elapsed(enable_tx_start_t, RF_USEC);
        MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Tx Timeline: wtr tx pwr up takes %d usec", enable_tx_time);
        enable_tx_start_t = rf_time_get_tick();
      }
    }
    else
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mc_enable_tx: rfdevice_tdscdma_tx_pwr_wakeup failed for band %d and device %d",
            band, device);
    }
  }
  /* Update Tx MDSP (DM/SHM content)										 */
  /*=========================================================================*/

  // Will do this only when band change
  rf_tdscdma_core_txplim_init();

  /* Apply the freq comp and temp comp delta to the mtpl */
  rf_tdscdma_core_txplim_adjust_mtpl_initial();

  if (rf_tdscdma_mc_tx_time_debug)
  {
    enable_tx_time = rf_time_get_elapsed(enable_tx_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Tx Timeline: txplim_init and adjust_mtpl takes %d usec", enable_tx_time);
    enable_tx_start_t = rf_time_get_tick();
  }

  rf_tdscdma_mc_update_tx_mdsp(device,
                               path,
                               band,
                               tx_channel,
                               txlm_buffer_idx,
                               rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx,
                               FALSE);

  if (rf_tdscdma_mc_tx_time_debug)
  {
    enable_tx_start_t = rf_time_get_tick();
  }

  /* Configure Tx Chain (RF/HW) 											 */
  /*=========================================================================*/

  rf_tdscdma_mc_config_tx_chain(device,
                                path,
                                band,
                                tx_channel,
                                txlm_buffer_idx,
                                rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx);

  if (rf_tdscdma_mc_tx_time_debug)
  {
    enable_tx_start_t = rf_time_get_tick();
  }

  /* FBRX setup*/
  if( rflm_tds_is_fbrx_enabled() )
  {
    /* FBRX init */
    rf_tdscdma_mc_fbrx_status = rfcommon_fbrx_mc_init_tx(rf_tdscdma_fbrx_lm_handle, RFCOMMON_FBRX_TDS, txlm_buffer_idx);
    if (rf_tdscdma_mc_fbrx_status == RFCOMMON_FBRX_ERROR)
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_tdscdma_mc_enable_tx: FBRX init TX failed for band %d and device %d",
            band, device);
    }
    else
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mc_enable_tx: FBRX init TX successful for band %d and device %d",
            band, device);
    }

    /* FBRX tune to channel */
    if (rf_tdscdma_mc_fbrx_status == RFCOMMON_FBRX_SUCCESS)
    {
      fbrx_chan_list[0] = tx_channel;
      rf_tdscdma_mc_fbrx_status = rfcommon_fbrx_mc_tune_to_chan(txlm_buffer_idx,
                                                                RFM_TDSCDMA_MODE,
                                                                *((rfcom_band_type_u *)(&band)),
                                                                fbrx_chan_list,
                                                                RFCOMMON_FBRX_1_CARRIER,
                                                                rf_tdscdma_fbrx_lm_handle);

      if (rfcommon_autopin_is_enabled(RFM_TDSCDMA_MODE))
      {
        rf_tdscdma_mc_cmn_device_ptr = rf_tdscdma_mc_get_curr_common_device(device,
                                                                            band);

        rfdevice_cmn_get_autopin_par_support(rf_tdscdma_mc_cmn_device_ptr,
                                             &rflmTdsRfModeCtlState.autopinInfo.enabled);

        rf_tdscdma_msm_autopin_push_capture_params(txlm_buffer_idx,
                                                   rf_tdscdma_mc_cmn_device_ptr,
                                                   band,
                                                   tx_channel);

        rfcommon_autopin_mc_enable_tx(txlm_buffer_idx,
                                      RFM_TDSCDMA_MODE);
      }
    }

    if (rf_tdscdma_mc_fbrx_status == RFCOMMON_FBRX_ERROR)
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_tdscdma_mc_enable_tx: FBRX tune to chan failed for band %d and device %d",
            band, device);
    }
    else
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mc_enable_tx: FBRX tune to chan successful for band %d and device %d",
            band, device);
    }

    /* Enable FBRX, write FBRX settings to HW */
    if (rf_tdscdma_mc_fbrx_status == RFCOMMON_FBRX_SUCCESS)
    {
      rf_tdscdma_mc_fbrx_status = rfcommon_fbrx_enable(rf_tdscdma_fbrx_lm_handle);
    }

    if (rf_tdscdma_mc_fbrx_status == RFCOMMON_FBRX_ERROR)
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_tdscdma_mc_enable_tx: FBRX enable failed for band %d and device %d",
            band, device);
    }
    else
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mc_enable_tx: FBRX enable successful for band %d and device %d",
            band, device);
    }
  } /* if( rflm_tds_is_fbrx_enabled() ) */

  if (rf_tdscdma_mc_tx_time_debug)
  {
    enable_tx_time = rf_time_get_elapsed(enable_tx_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Tx Timeline: fbrx_programing takes %d usec", enable_tx_time);
    enable_tx_start_t = rf_time_get_tick();
  }

  if(rfm_get_calibration_state())
  {
    if(!(rf_tdscdma_core_txlin_config_txagc(device, band, txlm_buffer_idx)))
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_tdscdma_mc_enable_tx: TXAGC config in calibration mode failed for band %d and device %d",
            band, device);
    }
    else
    {
      MSG_2(MSG_SSID_RF_DEBUG, MSG_LEGACY_MED, "rf_tdscdma_mc_enable_tx: TXAGC config in calibration mode done for band %d and device %d",
            band, device);
    }
  }

  if (rf_tdscdma_mc_tx_time_debug)
  {
    enable_tx_time = rf_time_get_elapsed(enable_tx_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Tx Timeline: txlin_config_txagc takes %d usec", enable_tx_time);
    enable_tx_start_t = rf_time_get_tick();
  }

  rf_tdscdma_mc_lm_bufs.tx_buf_wrk.is_buf_allocated = TRUE;
  rf_tdscdma_mc_lm_bufs.tx_buf_wrk.buf_idx = txlm_buffer_idx;


  if ( !rfm_get_calibration_state() )
  {
    /* Build HDET/Therm scripts 											   */
    /*=========================================================================*/

    /* Note: Can't put this into update_tx_mdsp() because the HDET and therm
     **  	 script are not double buffered.								   */
    rf_tds_build_event_data.pri_device = rf_tdscdma_mc_state.curr_pri_device;
    rf_tds_build_event_data.sec_device = rf_tdscdma_mc_state.curr_sec_device;
    rf_tds_build_event_data.tx_device = device;
    rf_tds_build_event_data.band = band;
    rf_tds_build_event_data.channel = tx_channel;
    rf_tds_build_event_data.drx_enable = FALSE;
    rf_tds_build_event_data.buf_id = rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx;
    rf_tds_build_event_data.is_bho_update = FALSE;
    rf_tds_build_event_data.txlm_buffer_index = txlm_buffer_idx;

    rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_DO_HDET, &rf_tds_build_event_data);     
    rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_DO_THERM, &rf_tds_build_event_data);
    rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_BUILD_AUTOPIN, &rf_tds_build_event_data);
  }

  if (rf_tdscdma_mc_tx_time_debug)
  {
    enable_tx_time = rf_time_get_elapsed(enable_tx_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Tx Timeline: build_therm_script takes %d usec", enable_tx_time);
    enable_tx_start_t = rf_time_get_tick();
  }

  /* Enable Temp comp and MTPL  											 */
  /*=========================================================================*/

  if ( ( !IS_FTM_IN_TEST_MODE() ) ||
       ( IS_FTM_IN_TEST_MODE() && ( !rfm_get_calibration_state() ) ) )
  {
    rf_tdscdma_core_temp_comp_enable(TRUE);

    if(rf_tdscdma_mc_enable_hdet_max_pwr_limiting)
    {
      rf_tdscdma_core_txplim_enable(TRUE);
    }

    rflm_tds_mc_fbrx_enable(TRUE);

    /* Update the hdet interval value (init to 2 subframes = 10 ms) to mdsp (send msg) */
    rf_tdscdma_core_txplim_interval_update(2);
    rf_tdscdma_mc_state.hdet_enable= TRUE;

    /* Start the 10s timer */
    rf_tdscdma_core_temp_comp_start();
#ifdef FEATURE_RF_HAS_QTUNER
    /* Start the 10s timer for TUNER temp comp*/
    rf_tdscdma_core_tuner_temp_comp_start();
#endif /* FEATURE_RF_HAS_QTUNER */

    /* Enable Tuner CL by configuring its threashold */
    rflm_tds_config_tuner_close_loop_threshold((uint32)(rf_tdscdma_mc_lm_bufs.rx_buf_wrk[RF_PATH_0].buf_idx));
  }

  if (rf_tdscdma_mc_tx_time_debug)
  {
    enable_tx_time = rf_time_get_elapsed(enable_tx_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Tx Timeline: enable_temp_comp_and_mtpl takes %d usec", enable_tx_time);
    enable_tx_start_t = rf_time_get_tick();
  }

  /* Below PDET cal is for FTM mode only (online implementation is different) */
  if (IS_FTM_IN_TEST_MODE())
  {
    rfdevice_tdscdma_tx_cmd_dispatch(device,
                                     RFCOM_TDSCDMA_MODE,
                                     rf_tdscdma_mc_state.curr_band_tx,
                                     RFDEVICE_TDSCDMA_DO_PDET_CAL,
                                     NULL);
  }

  if (rf_tdscdma_mc_tx_time_debug)
  {
    enable_tx_time = rf_time_get_elapsed(enable_tx_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Tx Timeline: do_pdet_cal takes %d usec", enable_tx_time);
    enable_tx_start_t = rf_time_get_tick();
  }


  /* State update   														 */
  /*=========================================================================*/
  rf_tdscdma_mc_state.rf_state[path] = RF_TDSCDMA_STATE_RXTX;

  if(rf_tdscdma_mc_display_time_profile || rf_tdscdma_mc_tx_time_debug)
  {
    overall_enable_tx_time = rf_time_get_elapsed(overall_enable_tx_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Enable Tx time profile: Overall takes %d usec", overall_enable_tx_time);
  }

  return RF_TDSCDMA_MC_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable rf transmit chain operation

  @details

  @param device
  TX device (logical device)

  @return
  rf_tdscdma_mc_status_type

*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_disable_tx(rfm_device_enum_type device)
{
  rf_path_enum_type path;
  int32 ret_val;

  rf_time_tick_type disable_tx_start_t = {0};
  rf_time_type disable_tx_time;
  boolean status;

  if(rf_tdscdma_mc_display_time_profile)
  {
    disable_tx_start_t = rf_time_get_tick();
  }

  path = rfcommon_core_device_to_path(device);
  MSG_2(MSG_SSID_RF, MSG_LVL_HIGH,"rf_tdscdma_mc_disable_tx, device=%d (path=%d)", device, path);

  /* TX must be on primary RX chain */
  if(path != RF_PATH_0)
  {
    MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_disable_tx: Unsupported device %d (path %d)", device, path);
    return RF_TDSCDMA_MC_INVALID_PARAM;
  }

  if ( rf_tdscdma_mc_state.rf_state[path] != RF_TDSCDMA_STATE_RXTX )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_disable_tx: called in wrong state %d", rf_tdscdma_mc_state.rf_state[path]);
    return RF_TDSCDMA_MC_SM_ERROR;
  }

  if (!IS_FTM_IN_TEST_MODE() ||
      (IS_FTM_IN_TEST_MODE() && (!rfm_get_calibration_state())))
  {
    /* Stop the 10s timer */
    rf_tdscdma_core_temp_comp_stop();

#ifdef FEATURE_RF_HAS_QTUNER
    /* Stop the 10s timer */
    rf_tdscdma_core_tuner_temp_comp_stop();
#endif
  }

  /* disable tx temp comp and disable tx pwr limiting */
  rf_tdscdma_core_temp_comp_enable(FALSE);
  rf_tdscdma_core_txplim_enable(FALSE);
  /* disable fbrx */
  rflm_tds_mc_fbrx_enable(FALSE);

  if (rfcommon_autopin_is_enabled(RFM_TDSCDMA_MODE))
  {
    rfcommon_autopin_mc_disable_tx(rf_tdscdma_mc_lm_bufs.tx_buf_wrk.buf_idx,
                                   RFM_TDSCDMA_MODE);
  }

  /* reset the coexistence limit upon ending the call */
  rf_tdscdma_core_txplim_reset_coex_limit();

  /* Disable WTR Tx Path */
  rfdevice_tdscdma_tx_disable(device,
                              RFCOM_TDSCDMA_MODE,
                              rf_tdscdma_mc_state.curr_band_tx,
                              RFDEVICE_EXECUTE_IMMEDIATE,
                              NULL);

  status = rfdevice_tdscdma_asm_sleep_tx(device,
                                         rf_tdscdma_mc_state.curr_band_tx,
                                         NULL,
                                         RFDEVICE_EXECUTE_IMMEDIATE,
                                         RFCOM_INVALID_TIMING_OFFSET);

  status = rfdevice_tdscdma_pa_sleep(device,
                                     rf_tdscdma_mc_state.curr_band_tx,
                                     NULL,
                                     RFDEVICE_EXECUTE_IMMEDIATE,
                                     RFCOM_INVALID_TIMING_OFFSET);

  if (!status)
  {
    MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, "Error when put PA in sleep on Device %d, Band %d", device, rf_tdscdma_mc_state.curr_band_tx);
  }

  /* Disable QPOT*/
  if (rfdevice_tdscdma_use_papm(device, rf_tdscdma_mc_state.curr_band_tx))
  {
    status = rfdevice_tdscdma_papm_disable(device,
                                           rf_tdscdma_mc_state.curr_band_tx,
                                           NULL,
                                           RFDEVICE_EXECUTE_IMMEDIATE);

    if (!status)
    {
      MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, "Error when disable power tracker on Device %d, Band %d", device, rf_tdscdma_mc_state.curr_band_tx);
    }
  }
  else
  {
    MSG_2(MSG_SSID_RF_DEBUG, MSG_LVL_MED, "papm device is not present for Device %d and Band %d", device, rf_tdscdma_mc_state.curr_band_tx);
  }

  /* Free DPD tables if it's XPT enabled */
  if ( rf_tdscdma_core_xpt_is_enabled(rf_tdscdma_mc_state.curr_band_tx) )
  {
    /* To Fix XPT IQ capture in Char */
    if (!IS_FTM_IN_TEST_MODE())
    {
      rf_common_xpt_init_dpd(device);
    }
  }

#ifdef FEATURE_RF_HAS_QTUNER
    /* Tuner */
  (void)rf_tdscdma_core_antenna_tuner_tx_mode_disable(device,
                                                      rf_tdscdma_mc_state.curr_band_tx, 
                                                      rf_tdscdma_mc_state.curr_chan_tx,
                                                      RFDEVICE_EXECUTE_IMMEDIATE,
                                                      NULL,
                                                      0);
#endif /* FEATURE_RF_HAS_QTUNER */

  /* vote off TX VREGs */
  rfc_tdscdma_manage_vregs(device, rf_tdscdma_mc_state.curr_band_rx, RF_PATH_RX_STATE);

  rf_tdscdma_mc_lm_bufs.tx_buf_wrk.is_buf_allocated = FALSE;
  rf_tdscdma_mc_lm_bufs.tx_buf_wrk.buf_idx = 0;
  rf_tdscdma_mc_state.rf_state[path] = RF_TDSCDMA_STATE_RX;

  //check RFNV_SPARE_1_I value to know LPM feature enabled through NV
  if (rfcommon_nv_get_lpm_feature_enabled())
  {
    /*vote for wtr pwr sleep*/
    if(rfdevice_tdscdma_tx_pwr_sleep(device, rf_tdscdma_mc_state.curr_band_tx))
    {
      if (rf_tdscdma_mc_tx_time_debug)
      {
        disable_tx_time = rf_time_get_elapsed(disable_tx_start_t, RF_USEC);
        MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Tx Timeline: wtr tx sleep takes %d usec", disable_tx_time);
        disable_tx_start_t = rf_time_get_tick();
      }
    }
    else
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mc_disable_tx: rfdevice_tdscdma_tx_pwr_sleep failed for band %d and device %d",
            rf_tdscdma_mc_state.curr_band_tx, device);
    }
  }
  /* Notify GNSS of TDSCDMA Tx Off */
  RFGNSS_MC(wwan_tx_status,ret_val)(device, FALSE);

  if (!ret_val)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_enable_tx: RFGNSS_MC(device %d, TRUE) returns error", device);
  }

  rf_tdscdma_mc_state.hdet_enable= FALSE;

  if(rf_tdscdma_mc_display_time_profile)
  {
    disable_tx_time = rf_time_get_elapsed(disable_tx_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Disable Tx mode time profile: %d usec", disable_tx_time);
  }

  return RF_TDSCDMA_MC_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Put RF into sleep

  @details

	@param pri_device
  Primary device (logical device)

  @param sec_device
  Secondary device (logical device)

  @return
  rf_tdscdma_mc_status_type

*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_sleep
(
  rfm_device_enum_type pri_device,
  rfm_device_enum_type sec_device
)
{
  rf_path_enum_type pri_path, sec_path;
  boolean dev_voting_status;

  rf_time_tick_type sleep_start_t = {0}, overall_sleep_start_t = {0};
  rf_time_type sleep_time, overall_sleep_time;
  
  if(rf_tdscdma_use_on_rumi == TRUE)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH,"rf_tdscdma_mc_sleep: Always return Success on RUMI");
    return RF_TDSCDMA_MC_SUCCESS;
  }

  //if(rf_tdscdma_mc_display_time_profile)
  {
    overall_sleep_start_t = rf_time_get_tick();
  }

  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    sleep_start_t = rf_time_get_tick();
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Sleep Timeline: Start", 0);
  }

  pri_path = rfcommon_core_device_to_path(pri_device);
  sec_path = rfcommon_core_device_to_path(sec_device);

  MSG_4(MSG_SSID_RF, MSG_LVL_HIGH,"rf_tdscdma_mc_sleep(pri_device=%d (path %d), sec_device=%d (path %d))", pri_device, pri_path, sec_device, sec_path);

  if (pri_path != RF_PATH_0)
  {
    // RF_PATH_0 = PRIMARY CHAIN.
    // Must pass valid pri_device into sleep() which must be on primary chain
    MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_sleep: called on wrong primary device %d (path %d)", pri_device, pri_path);
    return RF_TDSCDMA_MC_INVALID_PARAM;
  }

  if ( rf_tdscdma_mc_state.rf_state[pri_path] != RF_TDSCDMA_STATE_RX )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_sleep: called in wrong state %d", rf_tdscdma_mc_state.rf_state[pri_path]);
    return RF_TDSCDMA_MC_SM_ERROR;
  }

  rf_tdscdma_mc_state.last_rx_chan_before_sleep = rf_tdscdma_mc_state.curr_chan_rx;
  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, "rf_tdscdma_mc_sleep: the last rx channel before sleep =%d", rf_tdscdma_mc_state.last_rx_chan_before_sleep );
  }

#ifdef FEATURE_RF_HAS_QTUNER
  (void)rf_tdscdma_core_antenna_tuner_disable(pri_device,
                                              rf_tdscdma_mc_state.curr_band_rx,
                                              RFDEVICE_EXECUTE_IMMEDIATE,
                                              NULL,
                                              0);
#endif

  /* Put WTR Rx and Tx to LPM */
  /* Put both devices into sleep */
  rfdevice_tdscdma_rx_sleep(pri_device,
                            RFCOM_TDSCDMA_MODE,
                            rf_tdscdma_mc_state.curr_band_rx,
                            RFDEVICE_EXECUTE_IMMEDIATE,
                            NULL);

  // added for ASM off
  rfdevice_tdscdma_asm_disable_rx(pri_device, 
				                  rf_tdscdma_mc_state.curr_band_rx, 
				                  NULL, 
				                  RFDEVICE_EXECUTE_IMMEDIATE, 
				                  RFCOM_INVALID_TIMING_OFFSET, 
				                  TRUE, 
				                  NULL, 
				                  TDS_ASM_INCLUDE_ALL, 
				                  NULL); 

  rfdevice_tdscdma_asm_sleep_rx(pri_device,
                                rf_tdscdma_mc_state.curr_band_rx,
                                NULL,
                                RFDEVICE_EXECUTE_IMMEDIATE,
                                RFCOM_INVALID_TIMING_OFFSET);


  if(sec_path == RF_PATH_1)
  {
#ifdef FEATURE_RF_HAS_QTUNER
    (void)rf_tdscdma_core_antenna_tuner_disable(sec_device,
                                                rf_tdscdma_mc_state.curr_band_rx,
                                                RFDEVICE_EXECUTE_IMMEDIATE,
                                                NULL,
                                                0);
#endif

    // Do this only when sec_device is valid
    rfdevice_tdscdma_rx_sleep(sec_device,
                              RFCOM_TDSCDMA_MODE,
                              rf_tdscdma_mc_state.curr_band_rx,
                              RFDEVICE_EXECUTE_IMMEDIATE,
                              NULL);

    // added for ASM off
    rfdevice_tdscdma_asm_disable_rx(sec_device, 
				                    rf_tdscdma_mc_state.curr_band_rx, 
				                    NULL, 
				                    RFDEVICE_EXECUTE_IMMEDIATE, 
				                    RFCOM_INVALID_TIMING_OFFSET, 
				                    TRUE, 
				                    NULL, 
				                    TDS_ASM_INCLUDE_ALL, 
				                    NULL); 

    rfdevice_tdscdma_asm_sleep_rx(sec_device,
                                  rf_tdscdma_mc_state.curr_band_rx,
                                  NULL,
                                  RFDEVICE_EXECUTE_IMMEDIATE,
                                  RFCOM_INVALID_TIMING_OFFSET);

  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_sleep: Invalid sec_device %d", sec_device);
  }

  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    sleep_time = rf_time_get_elapsed(sleep_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Sleep Timeline: device_sleep takes %d usec", sleep_time);
    sleep_start_t = rf_time_get_tick();
  }

 //check RFNV_SPARE_1_I value to know LPM feature enabled through NV
 if (rfcommon_nv_get_lpm_feature_enabled())
 {
  /*vote for wtr pwr sleep*/
  if(rfdevice_tdscdma_rx_pwr_sleep(pri_device, rf_tdscdma_mc_state.curr_band_rx))
  {
    if (rf_tdscdma_mc_wakeup_time_debug)
    {
      sleep_time = rf_time_get_elapsed(sleep_start_t, RF_USEC);
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Rx Timeline: wtr pri_dev rx sleep takes %d usec", sleep_time);
      sleep_start_t = rf_time_get_tick();
    }
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_tdscdma_mc_sleep: rfdevice_tdscdma_rx_pwr_sleep failed for band %d and device %d",
          rf_tdscdma_mc_state.curr_band_rx, pri_device);
  }    
    
  if((rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_BOTH)||
  	 (rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_DIV)) //check RxD status
  {
    if(rfdevice_tdscdma_rx_pwr_sleep(sec_device, rf_tdscdma_mc_state.curr_band_rx))
    {
      if(rf_tdscdma_mc_wakeup_time_debug)
      {
        sleep_time = rf_time_get_elapsed(sleep_start_t, RF_USEC);
        MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Rx Timeline: wtr sec_dev rx sleep takes %d usec", sleep_time);
              sleep_start_t = rf_time_get_tick();
      }
    }
    else
    {
  	  MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_tdscdma_mc_sleep: rfdevice_tdscdma_rx_pwr_sleep failed for band %d and device %d",
            rf_tdscdma_mc_state.curr_band_rx, sec_device);
    }
  }
 }

#if 0
  /* rfdevice RX/TX exit which to clean up rfdevice internal state variables */
  /* Only need to call once on pri_device */
  rfdevice_tdscdma_rx_exit(pri_device,
                           RFCOM_TDSCDMA_MODE,
                           rf_tdscdma_mc_state.curr_band_rx,
                           RFDEVICE_EXECUTE_IMMEDIATE,
                           NULL);

  rfdevice_tdscdma_tx_exit(pri_device,
                           RFCOM_TDSCDMA_MODE,
                           rf_tdscdma_mc_state.curr_band_rx,
                           RFDEVICE_EXECUTE_IMMEDIATE,
                           NULL);

  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    sleep_time = rf_time_get_elapsed(sleep_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Sleep Timeline: device_rx/tx_exit takes %d usec", sleep_time);
    sleep_start_t = rf_time_get_tick();
  }
#endif

  /* Enable TCXO buffer */
  /* Do once on primary chain ONLY */
  dev_voting_status = rfcommon_core_dev_power_vote(pri_device,
                                                   RFCMN_CORE_DEV_OFF,
                                                   RFCMN_CORE_DEV_NO_VOTE);

  if( dev_voting_status == FALSE )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_sleep(): Device: %d Pwr Up failed", pri_device);
  }

  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    sleep_time = rf_time_get_elapsed(sleep_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Sleep Timeline: device_power_vote takes %d usec", sleep_time);
    sleep_start_t = rf_time_get_tick();
  }

#if 0
  /*Disable the GSBI2 Clk*/
  rfcommon_ant_tuner_device_shutdown();
#endif

  /* Do once on primary chain ONLY */
  rfc_tdscdma_sleep(pri_device);

  /* Abort pending AsDiv Event */
  #ifdef FEATURE_RF_ASDIV
    rflm_cmn_asd_abort_pending_event((uint32)rf_tdscdma_mc_lm_bufs.rx_buf_wrk[pri_path].buf_idx);
    rflm_cmn_asd_abort_pending_event((uint32)rf_tdscdma_mc_lm_bufs.rx_buf_tgt[pri_path].buf_idx);
  #endif
  
  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    sleep_time = rf_time_get_elapsed(sleep_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Sleep Timeline: rfc_sleep takes %d usec", sleep_time);
    sleep_start_t = rf_time_get_tick();
  }

  /* Do once on primary chain ONLY */
  if(!rf_tdscdma_disable_powercollapse)
  {
    /* vote off VREGs */
    rfc_tdscdma_manage_vregs(pri_device, rf_tdscdma_mc_state.curr_band_rx, RF_PATH_SLEEP_STATE);
  }

  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    sleep_time = rf_time_get_elapsed(sleep_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Sleep Timeline: manage_vregs takes %d usec", sleep_time);
    sleep_start_t = rf_time_get_tick();
  }

  // release vote for RFCMD app
  rf_hal_bus_enable(FALSE, &rf_tdscdma_hal_bus_client_handle, RF_HAL_RFCMD_APP_CLIENT(TDSCDMA));

#ifdef FEATURE_JOLOKIA_MODEM
  /* Release TQ */
  rflm_tds_rf_ccs_tq_free(rflmTdsRfModeCtlState.ccsInfo.rfCcsIntf.tqPairIdx);
#endif

  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    sleep_time = rf_time_get_elapsed(sleep_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Sleep Timeline: release_RFCMD_app takes %d usec", sleep_time);
    sleep_start_t = rf_time_get_tick();
  }

  rf_tdscdma_mc_state.rf_state[pri_path] = RF_TDSCDMA_STATE_SLEEP;
  rf_tdscdma_mc_state.rf_state[sec_path] = RF_TDSCDMA_STATE_SLEEP;

  // clean the flag to force execute wakeup script
  rf_tdscdma_mc_state.device_wakeup_script_exec_flag = FALSE;

  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Sleep Timeline: End", 0);
  }

  //if(rf_tdscdma_mc_display_time_profile)
  {
    overall_sleep_time = rf_time_get_elapsed(overall_sleep_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF_DEBUG, MSG_LEGACY_MED, "Sleep Timeline: Overall %d usec", overall_sleep_time);
  }

  return RF_TDSCDMA_MC_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wake up Step0 Part 1 (to update alt_path and RxLM ONLY)

  @details

*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_wakeup_step0p1
(
  rfm_device_enum_type pri_device,
  rfm_device_enum_type sec_device,
  uint16 rx_channel,
  lm_handle_type rxlm_buffer_idx,
  rfa_tdscdma_rxlm_mode_t rxlm_mode
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  ,rfa_tdscdma_rx_path_t   rx_path,
  lm_handle_type   	   rxlm_buffer_idx_div,
  rfa_tdscdma_rxlm_mode_t rxlm_mode_div
#endif
  ,boolean* rebuild_ifreq_script
)
{
  rf_path_enum_type pri_path, sec_path;
  rfcom_tdscdma_band_type band;
  rf_time_tick_type overall_wakeup_start_t = {0};
  rf_time_type overall_wakeup_time;

  // record previous state information to restore
  rfcom_tdscdma_band_type prev_band_rx;
  rfm_device_enum_type    prev_pri_device;
  rfm_device_enum_type    prev_sec_device;
  rfa_tdscdma_rx_path_t   prev_rx_path;
  uint16                  prev_rx_chan;

  if(rf_tdscdma_use_on_rumi == TRUE)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH,"rf_tdscdma_mc_wakeup_step0p1: Always return Success on RUMI");
    return RF_TDSCDMA_MC_SUCCESS;
  }  

  //if(rf_tdscdma_mc_display_time_profile)
  {
    overall_wakeup_start_t = rf_time_get_tick();
  }

  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: Start of wakeup_step0p1()", 0);
  }

  /* Check L1 parameters													 */
  /*=========================================================================*/

  pri_path = rfcommon_core_device_to_path(pri_device);
  sec_path = rfcommon_core_device_to_path(sec_device);

  MSG_7(MSG_SSID_RF, MSG_LVL_HIGH, 
        "rf_tdscdma_mc_wakeup_step0p1(pri_device=%d (path %d), sec_device=%d (path %d), channel=%d, rxlm_idx=%d, rxlm_mode=%d)",
        pri_device, pri_path, sec_device, sec_path, rx_channel, rxlm_buffer_idx, rxlm_mode);


  if (pri_path != RF_PATH_0)
  {
    // RF_PATH_0 = PRIMARY CHAIN.
    // Must pass valid pri_device into wakeup() which must be on primary chain
    MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, 
          "rf_tdscdma_mc_wakeup_step0p1: called on wrong primary device %d (path %d)", pri_device, pri_path);
    return RF_TDSCDMA_MC_INVALID_PARAM;
  }

#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  MSG_3(MSG_SSID_RF, MSG_LVL_HIGH, 
        "rf_tdscdma_mc_wakeup_step0p1(rx_path=%d, rxlm_buffer_idx_div=%d, rxlm_mode_div=%d)", rx_path, rxlm_buffer_idx_div, rxlm_mode_div);

  if ((rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
      (rx_path == RFA_TDSCDMA_RX_PATH_DIV)   )
  {    
    if (sec_path != RF_PATH_1)
    {
      // RF_PATH_1 = SECONDARY CHAIN
      // Must pass valid sec_device into wakeup() which must be on secondary chain
      MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, 
            "rf_tdscdma_mc_wakeup_step0p1: called on wrong secondary device %d (path %d)", sec_device, sec_path);
      return RF_TDSCDMA_MC_INVALID_PARAM;
    }
  }
#endif /* RF_TDSCDMA_RX_DIVERSITY_ENABLE */

  /* Get the band for the given channel num */
  band = rf_tdscdma_core_util_get_band_from_uarfcn(rx_channel);

  /* if band is invalid this means channel is wrong */
  if( band == RFCOM_BAND_TDSCDMA_INVALID )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "rf_tdscdma_mc_wakeup_step0p1: wrong channel %d", rx_channel);
    return RF_TDSCDMA_MC_INVALID_PARAM;
  }

  if( rf_tdscdma_mc_state.rf_state[pri_path] != RF_TDSCDMA_STATE_SLEEP )
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
        "rf_tdscdma_mc_wakeup_step0p1: called in wrong state");
    return RF_TDSCDMA_MC_SM_ERROR;
  }

  if( band != rf_tdscdma_mc_state.curr_band_rx )
  {
    rfnv_tdscdma_rx_tbl_ptr = rf_tdscdma_core_util_init_rx_nv_table(band);
    rfnv_tdscdma_tx_tbl_ptr = rf_tdscdma_core_util_init_tx_nv_table(band);
  }

  if( rxlm_buffer_idx != rf_tdscdma_mc_lm_bufs.rx_buf_wrk[pri_path].buf_idx )
  {
    /* flag an error but continue */
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "rf_tdscdma_mc_wakeup_step0p1: wrong buffer idx=%d", rxlm_buffer_idx);
  }

  /* When wakeup rxlm should be TRK_IDLE mode
     If the caller uses other mode, print the error msg but continue */
  if(!rf_tdscdma_msm_compare_is_same_rxlm_mode(rxlm_mode, RFA_TDSCDMA_RXLM_MODE_TRK_IDLE))
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR,"rf_tdscdma_mc_wakeup_step0p1(rxlm_mode=%d)", rxlm_mode);
  }

  if ((rf_tdscdma_mc_state.curr_pri_device == pri_device)            &&
      (rf_tdscdma_mc_state.last_rx_chan_before_sleep == rx_channel) &&
      (rf_tdscdma_mc_state.curr_rx_path == rx_path)                  )
  {
    if (rf_tdscdma_mc_wakeup_time_debug)
    {
      MSG_3(MSG_SSID_RF, MSG_LEGACY_MED,
            "Debug Wakeup Timeline: rf_tdscdma_mc_update_rx_mdsp() has been skipped on device %d, channel %d and rx_path %d",
            pri_device, rx_channel, rx_path);
    }
  }
  else
  {
    MSG_6(MSG_SSID_RF_DEBUG, MSG_LEGACY_HIGH,
          "Need to update MDSP in wakeup_step0p1. last_device: %d, wakeup_device: %d, last_chan: %d, wakeup_chan: %d, previous_rx_path: %d, wakeup_rx_path: %d",
          rf_tdscdma_mc_state.curr_pri_device, pri_device, 
          rf_tdscdma_mc_state.last_rx_chan_before_sleep, rx_channel,
          rf_tdscdma_mc_state.curr_rx_path, rx_path);

    /* Record the device information to restore */
    prev_band_rx = rf_tdscdma_mc_state.curr_band_rx;
    prev_pri_device = rf_tdscdma_mc_state.curr_pri_device;
    prev_sec_device = rf_tdscdma_mc_state.curr_sec_device;
    prev_rx_path = rf_tdscdma_mc_state.curr_rx_path;
    prev_rx_chan = rf_tdscdma_mc_state.curr_chan_rx;

    /* Update the device information */
    rf_tdscdma_mc_state.curr_band_rx = band;        
    rf_tdscdma_mc_state.curr_pri_device = pri_device;
    rf_tdscdma_mc_state.curr_sec_device = sec_device;
    rf_tdscdma_mc_state.curr_rx_path = rx_path;
    rf_tdscdma_mc_state.curr_chan_rx = rx_channel;

    /* Need to update MDSP content for Rx chain */
    rf_tdscdma_mc_update_rx_mdsp(pri_device,
                                 pri_path,
                                 band,
                                 rx_channel,
                                 rxlm_buffer_idx,
                                 rxlm_mode,
                                 TRUE,
                                 rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx,
                                 FALSE,
                                 RFA_TDSCDMA_WAKEUP_STEP0P1);

#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
    if((rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
       (rx_path == RFA_TDSCDMA_RX_PATH_DIV)   )
    {
      rf_tdscdma_mc_update_rx_mdsp(sec_device,
                                   sec_path,
                                   band,
                                   rx_channel,
                                   rxlm_buffer_idx_div,
                                   rxlm_mode_div,
                                   TRUE,
                                   rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx,
                                   FALSE,
                                   RFA_TDSCDMA_WAKEUP_STEP0P1);
    }
#endif /* RF_TDSCDMA_RX_DIVERSITY_ENABLE */

    /* Restore device information for wakeup_step0p2 */
    rf_tdscdma_mc_state.curr_band_rx = prev_band_rx;
    rf_tdscdma_mc_state.curr_pri_device = prev_pri_device;
    rf_tdscdma_mc_state.curr_sec_device = prev_sec_device;
    rf_tdscdma_mc_state.curr_rx_path = prev_rx_path;
    rf_tdscdma_mc_state.curr_chan_rx = prev_rx_chan;
  }

  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: End of wakeup_step0p1", 0);
  }

  //if(rf_tdscdma_mc_display_time_profile)
  {
    overall_wakeup_time = rf_time_get_elapsed(overall_wakeup_start_t, RF_USEC);
    if (rf_tdscdma_mc_wakeup_time_debug)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, 
            "Debug Wakeup Timeline: Overall wakeup_step0p1 takes %d usec", overall_wakeup_time);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, 
            "Wakeup Timeline: Overall wakeup_step0p1 takes %d usec", overall_wakeup_time);
    }
  }

  return RF_TDSCDMA_MC_SUCCESS;
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wake up Step0 Part 2 (in parallel with MCPM)
  Alt_path and RxLM has been updated in wakeup_step0p2

  @details

*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_wakeup_step0p2
(
  rfm_device_enum_type pri_device,
  rfm_device_enum_type sec_device,
  uint16 rx_channel,
  lm_handle_type rxlm_buffer_idx,
  rfa_tdscdma_rxlm_mode_t rxlm_mode
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  ,rfa_tdscdma_rx_path_t   rx_path,
  lm_handle_type   	   rxlm_buffer_idx_div,
  rfa_tdscdma_rxlm_mode_t rxlm_mode_div
#endif
  ,boolean* rebuild_ifreq_script
)
{
  rf_path_enum_type pri_path, sec_path;
  rfcom_tdscdma_band_type band;
  rf_time_tick_type overall_wakeup_start_t = {0};
  rf_time_type overall_wakeup_time;

  //if(rf_tdscdma_mc_display_time_profile)
  {
    overall_wakeup_start_t = rf_time_get_tick();
  }

  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: Start of wakeup_step0p2()", 0);
  }

  pri_path = rfcommon_core_device_to_path(pri_device);
  sec_path = rfcommon_core_device_to_path(sec_device);

  /* Get the band for the given channel num */
  band = rf_tdscdma_core_util_get_band_from_uarfcn(rx_channel);

  if ((rf_tdscdma_mc_state.curr_pri_device == pri_device)            &&
      (rf_tdscdma_mc_state.last_rx_chan_before_sleep == rx_channel) &&
      (rf_tdscdma_mc_state.curr_rx_path == rx_path)                  )
  {
    if (rf_tdscdma_mc_wakeup_time_debug)
    {
      MSG_3(MSG_SSID_RF, MSG_LEGACY_MED,
            "Debug Wakeup Timeline: rf_tdscdma_mc_update_rx_mdsp() has been skipped on device %d, channel %d and rx_path %d",
            pri_device, rx_channel, rx_path);
    }
  }
  else
  {
    MSG_6(MSG_SSID_RF_DEBUG, MSG_LEGACY_HIGH,
          "Need to update MDSP in wakeup_step0p2. last_device: %d, wakeup_device: %d, last_chan: %d, wakeup_chan: %d, previous_rx_path: %d, wakeup_rx_path: %d",
          rf_tdscdma_mc_state.curr_pri_device, pri_device, 
          rf_tdscdma_mc_state.last_rx_chan_before_sleep, rx_channel,
          rf_tdscdma_mc_state.curr_rx_path, rx_path);

    
    /* Update the device information */
    rf_tdscdma_mc_state.curr_band_rx = band;        
    rf_tdscdma_mc_state.curr_pri_device = pri_device;
    rf_tdscdma_mc_state.curr_sec_device = sec_device;
    rf_tdscdma_mc_state.curr_rx_path = rx_path;
    rf_tdscdma_mc_state.curr_chan_rx = rx_channel;

    /* Need to update MDSP content for Rx chain */
    rf_tdscdma_mc_update_rx_mdsp(pri_device,
                                 pri_path,
                                 band,
                                 rx_channel,
                                 rxlm_buffer_idx,
                                 rxlm_mode,
                                 TRUE,
                                 rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx,
                                 FALSE,
                                 RFA_TDSCDMA_WAKEUP_STEP0P2);

#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
    if((rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
       (rx_path == RFA_TDSCDMA_RX_PATH_DIV)   )
    {
      rf_tdscdma_mc_update_rx_mdsp(sec_device,
                                   sec_path,
                                   band,
                                   rx_channel,
                                   rxlm_buffer_idx_div,
                                   rxlm_mode_div,
                                   TRUE,
                                   rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx,
                                   FALSE,
                                   RFA_TDSCDMA_WAKEUP_STEP0P2);
    }
#endif /* RF_TDSCDMA_RX_DIVERSITY_ENABLE */
  }

  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: End of wakeup_step0p2", 0);
  }

  //if(rf_tdscdma_mc_display_time_profile)
  {
    overall_wakeup_time = rf_time_get_elapsed(overall_wakeup_start_t, RF_USEC);
    if (rf_tdscdma_mc_wakeup_time_debug)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: Overall wakeup_step0p2 takes %d usec", overall_wakeup_time);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Wakeup Timeline: Overall wakeup_step0p2 takes %d usec", overall_wakeup_time);
    }
  }

  /* Decide if need to re-build IFREQ script */ 
  if(rf_tdscdma_mc_state.ncell_wb_adc_index_on_devices[RF_PATH_0] != 0xFF)
  {
    /* IFREQ script has been built on RF_PATH_0 */
    if(rf_tdscdma_mc_state.ncell_wb_adc_index_on_devices[RF_PATH_0] != rf_tdscdma_mc_state.scell_wb_adc_index_on_devices[RF_PATH_0])
    {
      /* WB ADC changed: Need to re-build IFREQ scpt*/
      *rebuild_ifreq_script = TRUE;
      MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "Need to re-build IFREQ script. WB ADC needs to change from %d to %d", 
            rf_tdscdma_mc_state.ncell_wb_adc_index_on_devices[RF_PATH_0],
            rf_tdscdma_mc_state.scell_wb_adc_index_on_devices[RF_PATH_0]);
    }
     else
    {
      /* No need to re-build as they are on the same WB ADC */
      *rebuild_ifreq_script = FALSE;
      MSG(MSG_SSID_RF_DEBUG, MSG_LEGACY_HIGH, "No need to re-build IFREQ script because no WB ADC change");
    }
  }
  else
  {
    /* IFREQ script has not been built on RF_PATH_0: No need to re-build */
    *rebuild_ifreq_script = FALSE;
    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "No need to re-build IFREQ script because it has not been built yet");
  }
  
  if(RFCOM_TDSCDMA_MODE != rfm_get_current_mode(pri_device)) //to ensure DSDS_RF_TDSCDMA in wakeup0 for ASDiv
  {
    rfm_enter_mode(pri_device, RFCOM_TDSCDMA_MODE, NULL, NULL, 0);
  }

  return RF_TDSCDMA_MC_SUCCESS;
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wake up Step1 (after MCPM)

  @details

*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_wakeup_step1
(
  rfm_device_enum_type pri_device,
  rfm_device_enum_type sec_device,
  uint16 rx_channel,
  lm_handle_type rxlm_buffer_idx,
  rfa_tdscdma_rxlm_mode_t rxlm_mode
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  ,rfa_tdscdma_rx_path_t   rx_path,
  lm_handle_type   	   rxlm_buffer_idx_div,
  rfa_tdscdma_rxlm_mode_t rxlm_mode_div
#endif
)
{
  rf_path_enum_type pri_path, sec_path;
  rfcom_tdscdma_band_type band;
  boolean dev_voting_status;
  boolean rxd_enable = FALSE;
  rflm_tds_mc_wakeup_in_t in_data;
  rf_time_tick_type wakeup_start_t = {0}, overall_wakeup_start_t = {0};
  rf_time_type wakeup_time, overall_wakeup_time;
  uint32 ant_position;
  
  if(rf_tdscdma_use_on_rumi == TRUE)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH,"rf_tdscdma_mc_wakeup_step1: Always return Success on RUMI");
    return RF_TDSCDMA_MC_SUCCESS;
  }  

  //if(rf_tdscdma_mc_display_time_profile)
  {
    overall_wakeup_start_t = rf_time_get_tick();
  }

  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    wakeup_start_t = rf_time_get_tick();
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: Start of wakeup_step1", 0);
  }

  /* Check L1 parameters													 */
  /*=========================================================================*/

  pri_path = rfcommon_core_device_to_path(pri_device);
  sec_path = rfcommon_core_device_to_path(sec_device);

  MSG_7(MSG_SSID_RF, MSG_LVL_HIGH, "rf_tdscdma_mc_wakeup_step1(pri_device=%d (path %d), sec_device=%d (path %d), channel=%d, rxlm_idx=%d, rxlm_mode=%d)",
        pri_device, pri_path, sec_device, sec_path, rx_channel, rxlm_buffer_idx, rxlm_mode);

  if (pri_path != RF_PATH_0)
  {
    // RF_PATH_0 = PRIMARY CHAIN.
    // Must pass valid pri_device into wakeup() which must be on primary chain
    MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_wakeup_step1: called on wrong primary device %d (path %d)", pri_device, pri_path);
    return RF_TDSCDMA_MC_INVALID_PARAM;
  }

#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  MSG_3(MSG_SSID_RF, MSG_LVL_HIGH, "rf_tdscdma_mc_wakeup_step1(rx_path=%d, rxlm_buffer_idx_div=%d, rxlm_mode_div=%d)", rx_path, rxlm_buffer_idx_div, rxlm_mode_div);

  if ((rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
      (rx_path == RFA_TDSCDMA_RX_PATH_DIV)   )
  {
    rxd_enable = TRUE;
    
    if (sec_path != RF_PATH_1)
    {
      // RF_PATH_1 = SECONDARY CHAIN
      // Must pass valid sec_device into wakeup() which must be on secondary chain
      MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_wakeup_step1: called on wrong secondary device %d (path %d)", sec_device, sec_path);
      return RF_TDSCDMA_MC_INVALID_PARAM;
    }
  }
#endif /* RF_TDSCDMA_RX_DIVERSITY_ENABLE */

  /* Get the band for the given channel num */
  band = rf_tdscdma_core_util_get_band_from_uarfcn(rx_channel);

  /* if band is invalid this means channel is wrong */
  if( band == RFCOM_BAND_TDSCDMA_INVALID )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_wakeup_step1: wrong channel %d", rx_channel);
    return RF_TDSCDMA_MC_INVALID_PARAM;
  }

  if( rf_tdscdma_mc_state.rf_state[pri_path] != RF_TDSCDMA_STATE_SLEEP )
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_wakeup_step1: called in wrong state");
    return RF_TDSCDMA_MC_SM_ERROR;
  }

  
  in_data.priDevice = pri_device;
  in_data.secDevice = sec_device;
  in_data.rxdEn = rxd_enable; 

  rflm_tds_mc_wakeup(&in_data);

#ifdef FEATURE_JOLOKIA_MODEM
  /* Request TQ */
  rflm_tds_rf_ccs_tq_lock(pri_device);
#endif

  /* Turn ON Vreg/RFHAL_BUS/CXO Buffer  									 */
  /*=========================================================================*/

  /* vote on Rx VREGs */
  /* Do once on primary chain only */
  rfc_tdscdma_manage_vregs(pri_device, band, RF_PATH_RX_STATE);
  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    wakeup_time = rf_time_get_elapsed(wakeup_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: manage_vreg takes %d usec", wakeup_time);
    wakeup_start_t = rf_time_get_tick();
  }

  // vote for RFCMD app
  rf_hal_bus_enable(TRUE, &rf_tdscdma_hal_bus_client_handle, RF_HAL_RFCMD_APP_CLIENT(TDSCDMA));
  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    wakeup_time = rf_time_get_elapsed(wakeup_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: enable_RFCMD_cpp takes %d usec", wakeup_time);
    wakeup_start_t = rf_time_get_tick();
  }

  /* Do once on primary chain only */
  dev_voting_status = rfcommon_core_dev_power_vote(pri_device,
                                                   RFCMN_CORE_DEV_ON,
                                                   RFCMN_CORE_DEV_NORMAL_VOTE);

  if( dev_voting_status == FALSE )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_wakeup_step1(): Device: %d Pwr Up failed", pri_device);
  }

  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    wakeup_time = rf_time_get_elapsed(wakeup_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: power_vote takes %d usec", wakeup_time);
    wakeup_start_t = rf_time_get_tick();
  }

  //check RFNV_SPARE_1_I value to know LPM feature enabled through NV
 if (rfcommon_nv_get_lpm_feature_enabled())
 {
  /*vote for wtr pwr wakeup*/
  if(rfdevice_tdscdma_rx_pwr_wakeup(pri_device, rf_tdscdma_mc_state.curr_band_rx))
  {
    if (rf_tdscdma_mc_wakeup_time_debug)
    {
      wakeup_time = rf_time_get_elapsed(wakeup_start_t, RF_USEC);
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Rx Timeline: wtr pri_dev rx wakeup takes %d usec", wakeup_time);
      wakeup_start_t = rf_time_get_tick();
    }
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_tdscdma_mc_wakeup_step1: rfdevice_tdscdma_rx_pwr_wakeup failed for band %d and device %d",
          rf_tdscdma_mc_state.curr_band_rx, pri_device);
  }    
   
  if((rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_BOTH)||
  	 (rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_DIV)) //check RxD status
  {
  	if(rfdevice_tdscdma_rx_pwr_wakeup(sec_device, rf_tdscdma_mc_state.curr_band_rx))
    {
      if(rf_tdscdma_mc_display_time_profile)
      {
        wakeup_time = rf_time_get_elapsed(wakeup_start_t, RF_USEC);
        MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Rx Timeline: wtr sec_dev rx sleep takes %d usec", wakeup_time);
        wakeup_start_t = rf_time_get_tick();
      }
    }
    else
    {
  	  MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mc_wakeup_step1: rfdevice_tdscdma_rx_pwr_wakeup failed for band %d and device %d",
            rf_tdscdma_mc_state.curr_band_rx, sec_device);
    }
  }
 }

  /* Configure GRFCs and AsDIV  											 */
  /*=========================================================================*/
  /* Do once on primary chain only */
  rfc_tdscdma_wakeup(pri_device);

  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    wakeup_time = rf_time_get_elapsed(wakeup_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: rfc_wakeup takes %d usec", wakeup_time);
    wakeup_start_t = rf_time_get_tick();
  }


  /* AsDIV: Set antenna position to before sleep*/
#ifdef FEATURE_RF_ASDIV
  ant_position = rfcommon_asdiv_get_current_position(pri_device);
#else
  ant_position = 0; // hard-coded the default position if feature is not defined
#endif

  rf_tdscdma_mc_asdiv_script(ant_position,pri_device,rf_tdscdma_mc_state.curr_band_rx);

  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    wakeup_time = rf_time_get_elapsed(wakeup_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: AsDiv_set takes %d usec", wakeup_time);
    wakeup_start_t = rf_time_get_tick();
  }

  /* Need to Config RF/HW for Rx chain */
  rf_tdscdma_mc_config_rx_chain(pri_device,
                                pri_path,
                                band,
                                rx_channel,
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
                                rx_path,
#endif
                                rxlm_buffer_idx,
                                rxlm_mode,
                                TRUE,
                                rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx,
                                rf_tdscdma_mc_state.rx_lin_state,
                                TRUE        //Always set this flag to TRUE in wakeup
                                );

#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  if( (rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
      (rx_path == RFA_TDSCDMA_RX_PATH_DIV)    )
  {
    rf_tdscdma_mc_config_rx_chain(sec_device,
                                  sec_path,
                                  band,
                                  rx_channel,
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
                                  rx_path,
#endif
                                  rxlm_buffer_idx_div,
                                  rxlm_mode_div,
                                  TRUE,
                                  rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx,
                                  rf_tdscdma_mc_state.rx_lin_state,
                                  TRUE        //Always set this flag to TRUE in wakeup
                                  );
  }
#endif /* RF_TDSCDMA_RX_DIVERSITY_ENABLE */

  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    wakeup_start_t = rf_time_get_tick();
  }

  rf_tdscdma_mc_state.rf_state[pri_path] = RF_TDSCDMA_STATE_RX;
  rf_tdscdma_mc_state.rf_state[sec_path] = RF_TDSCDMA_STATE_RX;

  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: End of wakeup_step1", 0);
  }

  //if(rf_tdscdma_mc_display_time_profile)
  {
    overall_wakeup_time = rf_time_get_elapsed(overall_wakeup_start_t, RF_USEC);
    if (rf_tdscdma_mc_wakeup_time_debug)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: Overall wakeup_step1 takes %d usec", overall_wakeup_time);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Wakeup Timeline: Overall wakeup_step1 takes %d usec", overall_wakeup_time);
    }
  }

  return RF_TDSCDMA_MC_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  update the max tx power limit(in dBm) from network

  @details
*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_set_tx_pwr_limit(int16 tx_pwr_limit)
{

  rf_time_tick_type set_tx_pwr_limit_start_t;
  rf_time_type set_tx_pwr_limit_time;

  if(rf_tdscdma_mc_display_time_profile)
  {
    set_tx_pwr_limit_start_t = rf_time_get_tick();
  }


  MSG_1(MSG_SSID_RF_DEBUG, MSG_LVL_HIGH, "rf_tdscdma_mc_set_tx_pwr_limit(tx_limit=%d)", tx_pwr_limit);

  if (rf_tdscdma_use_on_rumi == FALSE) // Bypass the state machine check on Rumi
  {
    if ( (rf_tdscdma_mc_state.rf_state[RF_PATH_0] != RF_TDSCDMA_STATE_RX) &&
         (rf_tdscdma_mc_state.rf_state[RF_PATH_0] != RF_TDSCDMA_STATE_RXTX) )
    {
      MSG(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_set_tx_pwr_limit: called in wrong state");
      return RF_TDSCDMA_MC_SM_ERROR;
    }
  }

  //TODO - to delete chan type from below
  /* take the network limit and re-calc mtpl */
  rf_tdscdma_core_txplim_set_mtpl(tx_pwr_limit, RFCOM_TXPLIM_DCH);

  if(rf_tdscdma_mc_display_time_profile)
  {
    set_tx_pwr_limit_time = rf_time_get_elapsed(set_tx_pwr_limit_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Set Tx Power Limit time profile: %d usec", set_tx_pwr_limit_time);
  }

  return RF_TDSCDMA_MC_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns rf warmup time

  @details
*/
uint16 rf_tdscdma_mc_get_rf_warmup_time(void)
{
  rfc_tdscdma_core_config_type* rfc_core_config;

  rf_time_tick_type get_rf_warmup_time_start_t = {0};
  rf_time_type get_rf_warmup_time_time;

  if(rf_tdscdma_mc_display_time_profile)
  {
    get_rf_warmup_time_start_t = rf_time_get_tick();
  }

  /* rfc not configured, return error */
  if ( rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_DEFAULT )
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_get_rf_warmup_time: called in wrong state");
    return 0xFFFF;
  }

  rfc_core_config = (rfc_tdscdma_core_config_type*)rfc_tdscdma_mode_config_get(RFM_DEVICE_0, RFC_MODE_CORE_DATA);

  if(rf_tdscdma_mc_display_time_profile)
  {
    get_rf_warmup_time_time = rf_time_get_elapsed(get_rf_warmup_time_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Get RF Warmup Time time profile: %d usec", get_rf_warmup_time_time);
  }

  /* check NULL pointer from the returned structure */
  if ( rfc_core_config == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_get_rf_warmup_time: NULL pointer");
    return 0xFFFF;
  }
  else
  {
#ifndef FEATURE_TRITON_MODEM
    return ((rfcommon_nv_tbl.rf_hw_config == 16) ? 2000 : 2500);
#else
    return(rfc_core_config->rf_warmup_time);
#endif /* !FEATURE_TRITON_MODEM */
  }
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
   This function prepares RF for Baton handover.

  @details
*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_update_bho
(
  rfm_device_enum_type pri_device,
  rfm_device_enum_type sec_device,
  uint16 tgt_prim_channel,
  uint16 tgt_channel,
  lm_handle_type rxlm0_tgt_buffer_idx,
  rfa_tdscdma_rxlm_mode_t rxlm_mode,
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  rfa_tdscdma_rx_path_t  rx_path,
  lm_handle_type rxlm_tgt_buffer_idx_div,
  rfa_tdscdma_rxlm_mode_t rxlm_mode_div,
#endif /* RF_TDSCDMA_RX_DIVERSITY_ENABLE */
  lm_handle_type txlm_tgt_buffer_idx,
  uint32 *prim_carrier_freq,
  uint32 *carrier_freq,
  uint8 *rx_tgt_buffer_idx,
  uint8 *tx_tgt_buffer_idx
)
{
  rf_path_enum_type pri_path, sec_path;
  rfcom_tdscdma_band_type band, prim_band;
  rf_time_tick_type update_bho_start_t = {0};
  rf_time_tick_type overall_update_bho_start_t = {0};
  rf_time_type update_bho_time, overall_update_bho_time;
  rftdscdma_mdsp_common_event_data_type rf_tds_build_event_data;

  boolean script_status;
  rfcommon_fbrx_error_type rf_tdscdma_mc_fbrx_status = RFCOMMON_FBRX_ERROR;
  uint16 fbrx_chan_list[RFCOMMON_FBRX_1_CARRIER] = {0xFFFF};


  if(rf_tdscdma_mc_display_time_profile || rf_tdscdma_mc_tx_time_debug)
  {
    update_bho_start_t = rf_time_get_tick();
    overall_update_bho_start_t = rf_time_get_tick();
  }

  /* Convert rfm_device to rf_path_enum_type */
  pri_path = rfcommon_core_device_to_path(pri_device);
  sec_path = rfcommon_core_device_to_path(sec_device);



  MSG_7(MSG_SSID_RF, MSG_LVL_HIGH,
        "rf_tdscdma_mc_update_bho(pri_device=%d, sec_device=%d, prim_chan=%d, tgt_chan=%d, rxlm_buf_idx=%d, rxlm_mode=%d, txlm_buf_idx=%d)",
        pri_device, sec_device, tgt_prim_channel, tgt_channel, rxlm0_tgt_buffer_idx, rxlm_mode, txlm_tgt_buffer_idx);

#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  MSG_3(MSG_SSID_RF, MSG_LVL_HIGH,
        "rf_tdscdma_mc_update_bho(rx_path=%d, rxlm_tgt_buffer_idx_div=%d, rxlm_mode_div=%d)",
        rx_path, rxlm_tgt_buffer_idx_div, rxlm_mode_div);
#endif /* RF_TDSCDMA_RX_DIVERSITY_ENABLE */


  /* State and Param Check  												 */
  /*=========================================================================*/

  if (pri_path != RF_PATH_0)
  {
    // RF_PATH_0 = PRIMARY CHAIN.
    // Must pass valid pri_device into mc_enable_rx() which must be on primary chain
    MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_update_bho: called on wrong primary device %d (path %d)", pri_device, pri_path);
    return RF_TDSCDMA_MC_INVALID_PARAM;
  }

#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  if((rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
     (rx_path == RFA_TDSCDMA_RX_PATH_DIV)   )
  {
    if (sec_path != RF_PATH_1)
    {
      // RF_PATH_1 = SECONDARY CHAIN
      // When diversity is enabled, Must pass valid sec_device into mc_enable_rx()
      // which must be on secondary chain
      MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_update_bho: called on wrong secondary device %d (path %d)", sec_device, sec_path);
      return RF_TDSCDMA_MC_INVALID_PARAM;
    }
  }
#endif /* RF_TDSCDMA_RX_DIVERSITY_ENABLE */

  if ( rf_tdscdma_mc_state.rf_state[RF_PATH_0] != RF_TDSCDMA_STATE_RXTX )
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_update_bho: called in wrong state");
    return RF_TDSCDMA_MC_SM_ERROR;
  }

  /* Get the band for the target channel num */
  band = rf_tdscdma_core_util_get_band_from_uarfcn(tgt_channel);

  /* if band is invalid this means channel is wrong */
  if( band == RFCOM_BAND_TDSCDMA_INVALID )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_update_bho: wrong tgt_channel %d", tgt_channel);
    return RF_TDSCDMA_MC_INVALID_PARAM;
  }

  /* Get the band for the target prim channel num */
  prim_band = rf_tdscdma_core_util_get_band_from_uarfcn(tgt_prim_channel);

  /* if prim band is invalid this means tgt prim channel is wrong */
  if( prim_band == RFCOM_BAND_TDSCDMA_INVALID )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_update_bho: wrong prim tgt_channel %d", tgt_prim_channel);
    return RF_TDSCDMA_MC_INVALID_PARAM;
  }

  /* send carrier freq to L1 */
  /* carrier freq in Hz */
  *carrier_freq = rf_tdscdma_core_util_get_freq_from_uarfcn(tgt_channel, band)*1000;
  *prim_carrier_freq = rf_tdscdma_core_util_get_freq_from_uarfcn(tgt_prim_channel, prim_band)*1000;

  /* send rx/tx target rf buffer index to L1 */
  *rx_tgt_buffer_idx = rf_tdscdma_mc_dbl_bufs.rx_tgt_buf_idx;
  *tx_tgt_buffer_idx = rf_tdscdma_mc_dbl_bufs.tx_tgt_buf_idx;

  if (rf_tdscdma_mc_tx_time_debug)
  {
    update_bho_time = rf_time_get_elapsed(update_bho_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Update_BHO Timeline: param_check takes %d usec", update_bho_time);
    update_bho_start_t = rf_time_get_tick();
  }

  /* stop the 10s timer */
  rf_tdscdma_core_temp_comp_stop();

  /* suspend the hdet and temp comp during the handover period */
  rf_tdscdma_core_temp_comp_enable(FALSE);
  rf_tdscdma_core_txplim_enable(FALSE);

  /* suspend FBRx during handover period */
  rflm_tds_mc_fbrx_enable(FALSE);

  if (rfcommon_autopin_is_enabled(RFM_TDSCDMA_MODE))
  {
    rfcommon_autopin_mc_disable_tx(rf_tdscdma_mc_lm_bufs.tx_buf_wrk.buf_idx,
                                   RFM_TDSCDMA_MODE);
  }

  if (rf_tdscdma_mc_tx_time_debug)
  {
    update_bho_time = rf_time_get_elapsed(update_bho_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Update_BHO Timeline: disable_temp_comp takes %d usec", update_bho_time);
    update_bho_start_t = rf_time_get_tick();
  }

  /* Band-dependent Init if necessary   									 */
  /*=========================================================================*/

  /* if target band is different than the working band,
     do band-dependent init and setting 									 */
  if (band != rf_tdscdma_mc_state.curr_band_rx)
  {
    /* Switch the nv table pointer to the new band */
    rfnv_tdscdma_rx_tbl_ptr = rf_tdscdma_core_util_init_rx_nv_table(band);
    rfnv_tdscdma_tx_tbl_ptr = rf_tdscdma_core_util_init_tx_nv_table(band);

    /* txplim init on f3(calc AGC vs HDET etc) */
    rf_tdscdma_core_txplim_init();
  }

  if (rf_tdscdma_mc_tx_time_debug)
  {
    update_bho_time = rf_time_get_elapsed(update_bho_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Update_BHO Timeline: txplim_init takes %d usec", update_bho_time);
    update_bho_start_t = rf_time_get_tick();
  }


  /* State variables update (Step 1)										 */
  /*=========================================================================*/
  /* For those variables when be used in below API calls					 */

  /* Update the device information */
  rf_tdscdma_mc_state.curr_pri_device = pri_device;
  rf_tdscdma_mc_state.curr_sec_device = sec_device;

  /* Update the band information */
  rf_tdscdma_mc_state.curr_band_rx = band;
  rf_tdscdma_mc_state.curr_band_tx = band;

  /* Update the channel information */
  rf_tdscdma_mc_state.curr_chan_rx = tgt_channel;
  rf_tdscdma_mc_state.curr_chan_tx = tgt_channel;

  /* Update the rx_path information */
  rf_tdscdma_mc_state.curr_rx_path = rx_path;

#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  if ( (rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
       (rx_path == RFA_TDSCDMA_RX_PATH_DIV) )
  {
    rf_tdscdma_mc_state.div_enable = TRUE;
  }
#endif /* RF_TDSCDMA_RX_DIVERSITY_ENABLE */


  /* Update Tx MDSP (DM/SHM content)										 */
  /*=========================================================================*/

  /* Apply the freq comp and temp comp to the mtpl */
  rf_tdscdma_core_txplim_adjust_mtpl_bho();

  if (rf_tdscdma_mc_tx_time_debug)
  {
    update_bho_time = rf_time_get_elapsed(update_bho_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Update_BHO Timeline: adjust_mtpl_bho takes %d usec", update_bho_time);
    update_bho_start_t = rf_time_get_tick();
  }

  rf_tdscdma_mc_update_tx_mdsp(pri_device,
                               pri_path,
                               band,
                               tgt_channel,
                               txlm_tgt_buffer_idx,
                               rf_tdscdma_mc_dbl_bufs.tx_tgt_buf_idx,
                               TRUE);

  if (rf_tdscdma_mc_tx_time_debug)
  {
    update_bho_start_t = rf_time_get_tick();
  }

  /* Update TxLM buffer index */
  rf_tdscdma_mc_lm_bufs.tx_buf_tgt.is_buf_allocated = TRUE;
  rf_tdscdma_mc_lm_bufs.tx_buf_tgt.buf_idx = txlm_tgt_buffer_idx;


  /* FBRX setup*/
  if( rflm_tds_is_fbrx_enabled() )
  {
    /* FBRX init */
    rf_tdscdma_mc_fbrx_status = rfcommon_fbrx_mc_init_tx(rf_tdscdma_fbrx_lm_handle, RFCOMMON_FBRX_TDS, txlm_tgt_buffer_idx);
    if (rf_tdscdma_mc_fbrx_status == RFCOMMON_FBRX_ERROR)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_tdscdma_mc_update_bho: FBRX init TX failed for band %d",band);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mc_update_bho: FBRX init TX successful for band %d",band);
    }

    /* FBRX tune to channel */
    if (rf_tdscdma_mc_fbrx_status == RFCOMMON_FBRX_SUCCESS)
    {
      fbrx_chan_list[0] = tgt_channel;
      rf_tdscdma_mc_fbrx_status = rfcommon_fbrx_mc_tune_to_chan(txlm_tgt_buffer_idx,
                                                                RFM_TDSCDMA_MODE,
                                                                *((rfcom_band_type_u *)(&band)),
                                                                fbrx_chan_list,
                                                                RFCOMMON_FBRX_1_CARRIER,
                                                                rf_tdscdma_fbrx_lm_handle);
    }

    if (rf_tdscdma_mc_fbrx_status == RFCOMMON_FBRX_ERROR)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_tdscdma_mc_update_bho: FBRX tune to chan failed for band %d",band);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mc_update_bho: FBRX tune to chan successful for band %d",band);
    }

    /* Enable FBRX, write FBRX settings to HW */
    if (rf_tdscdma_mc_fbrx_status == RFCOMMON_FBRX_SUCCESS)
    {
      rf_tdscdma_mc_fbrx_status = rfcommon_fbrx_enable(rf_tdscdma_fbrx_lm_handle);
    }

    if (rf_tdscdma_mc_fbrx_status == RFCOMMON_FBRX_ERROR)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_tdscdma_mc_update_bho: FBRX enable failed for band %d",band);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mc_update_bho: FBRX enable successful for band %d",band);
    }
  } /* if( rflm_tds_is_fbrx_enabled() ) */

  if (rf_tdscdma_mc_tx_time_debug)
  {
    update_bho_time = rf_time_get_elapsed(update_bho_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Update_BHO Timeline: FBRx setup takes %d usec", update_bho_time);
    update_bho_start_t = rf_time_get_tick();
  }


  /* BHO rxlm should be TRK mode for the f3 rxlm buffer
     If L1 uses other mode, print error msg but continue */
  if (!rf_tdscdma_msm_compare_is_same_rxlm_mode(rxlm_mode, RFA_TDSCDMA_RXLM_MODE_TRK ))
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR,"Error: rf_tdscdma_mc_update_bho(rxlm_mode=%d)", rxlm_mode);
  }

  /* Update Rx MDSP (DM/SHM content)										 */
  /*=========================================================================*/

  rf_tdscdma_mc_update_rx_mdsp(pri_device,
                               pri_path,
                               band,
                               tgt_channel,
                               rxlm0_tgt_buffer_idx,
                               rxlm_mode,
                               FALSE,
                               rf_tdscdma_mc_dbl_bufs.rx_tgt_buf_idx,
                               TRUE,
                               RFA_TDSCDMA_WAKEUP_NA);

  if (rf_tdscdma_mc_tx_time_debug)
  {
    update_bho_start_t = rf_time_get_tick();
  }

  /* Update RxLM buffer index */
  rf_tdscdma_mc_lm_bufs.rx_buf_tgt[pri_path].is_buf_allocated = TRUE;
  rf_tdscdma_mc_lm_bufs.rx_buf_tgt[pri_path].buf_idx = rxlm0_tgt_buffer_idx;
  rf_tdscdma_mc_lm_bufs.rx_buf_tgt[pri_path].mode = rxlm_mode;

#ifdef FEATURE_RF_ASDIV
  script_status = rflm_cmn_asd_allocate_memory((uint32)(rf_tdscdma_mc_lm_bufs.rx_buf_tgt[pri_path].buf_idx));
  if(script_status)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_HIGH,
          "Allocated AsD script for RxLM handle %d",
          rf_tdscdma_mc_lm_bufs.rx_buf_tgt[pri_path].buf_idx);
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR,
          "Can't allocated AsD script for RxLM handle %d",
          rf_tdscdma_mc_lm_bufs.rx_buf_tgt[pri_path].buf_idx);
  }
#endif

  /* allocate AOL script buffer */
  script_status = rflm_tds_aol_cl_allocate_memory((uint32)(rf_tdscdma_mc_lm_bufs.rx_buf_tgt[pri_path].buf_idx),
                                                  TRUE // boolean isAol
                                                 );
  script_status = rflm_tds_aol_cl_allocate_memory((uint32)(rf_tdscdma_mc_lm_bufs.rx_buf_tgt[pri_path].buf_idx),
                                                  FALSE // boolean isCl
                                                 );                                                 
  if(script_status)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_HIGH,
          "Allocated AOL script for RxLM handle %d",
          rf_tdscdma_mc_lm_bufs.rx_buf_tgt[pri_path].buf_idx);
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR,
          "Can't allocated AOL script for RxLM handle %d",
          rf_tdscdma_mc_lm_bufs.rx_buf_tgt[pri_path].buf_idx);
  }


#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  if ( (rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
       (rx_path == RFA_TDSCDMA_RX_PATH_DIV)   )
  {
    rf_tdscdma_mc_update_rx_mdsp(sec_device,
                                 sec_path,
                                 band,
                                 tgt_channel,
                                 rxlm_tgt_buffer_idx_div,
                                 rxlm_mode_div,
                                 FALSE,
                                 rf_tdscdma_mc_dbl_bufs.rx_tgt_buf_idx,
                                 TRUE,
                                 RFA_TDSCDMA_WAKEUP_NA);

    rf_tdscdma_mc_lm_bufs.rx_buf_tgt[sec_path].is_buf_allocated = TRUE;
    rf_tdscdma_mc_lm_bufs.rx_buf_tgt[sec_path].buf_idx = rxlm_tgt_buffer_idx_div;
    rf_tdscdma_mc_lm_bufs.rx_buf_tgt[sec_path].mode = rxlm_mode_div;

    if (rf_tdscdma_mc_tx_time_debug)
    {
      update_bho_start_t = rf_time_get_tick();
    }
  }
#endif /* RF_TDSCDMA_RX_DIVERSITY_ENABLE */


  /* BHO startup script: TX f1->f3  											*/
  /*============================================================================*/

  rf_tds_build_event_data.pri_device = pri_device;
  rf_tds_build_event_data.sec_device = sec_device;
  rf_tds_build_event_data.tx_device  = pri_device;
  rf_tds_build_event_data.band = band;
  rf_tds_build_event_data.channel = tgt_channel;
  rf_tds_build_event_data.drx_enable = FALSE;
  rf_tds_build_event_data.buf_id = rf_tdscdma_mc_dbl_bufs.tx_tgt_buf_idx;
  rf_tds_build_event_data.is_bho_update = TRUE;

    script_status = rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_BUILD_WAKEUP, &rf_tds_build_event_data);
  
    if (script_status)
    {
      MSG_6(MSG_SSID_RF, MSG_LVL_MED,
            "Rx wakeup script built for pri_device %d, sec_device %d, Band %d, Chan %d, Div_EN %d, is_BHO %d",
            rf_tds_build_event_data.pri_device,
            rf_tds_build_event_data.sec_device,
            rf_tds_build_event_data.band,
            rf_tds_build_event_data.channel,
            rf_tds_build_event_data.drx_enable,
            rf_tds_build_event_data.is_bho_update);
    }
    else
    {
      MSG_6(MSG_SSID_RF, MSG_LVL_ERROR,
            "Rx wakeup script built ERROR for pri_device %d, sec_device %d, Band %d, Chan %d, Div_EN %d, is_BHO %d",
            rf_tds_build_event_data.pri_device,
            rf_tds_build_event_data.sec_device,
            rf_tds_build_event_data.band,
            rf_tds_build_event_data.channel,
            rf_tds_build_event_data.drx_enable,
            rf_tds_build_event_data.is_bho_update);
    }

  if (rf_tdscdma_mc_tx_time_debug)
  {
    update_bho_time = rf_time_get_elapsed(update_bho_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Update_BHO Timeline: build_bho_startup_script takes %d usec", update_bho_time);
    update_bho_start_t = rf_time_get_tick();
  }



  if(rf_tdscdma_mc_display_time_profile || rf_tdscdma_mc_tx_time_debug)
  {
    overall_update_bho_time = rf_time_get_elapsed(overall_update_bho_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Update Baton Handover time profile: Overall %d usec", overall_update_bho_time);
  }

  return RF_TDSCDMA_MC_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
   This function switches the buffers after Baton handover.

  @details
*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_switch_buffer
(
  rfa_tdscdma_buf_swap_t buf_swap,
  lm_handle_type rxlm0_tgt_buffer_idx,
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  lm_handle_type rxlm_tgt_buffer_idx_div,
#endif
  lm_handle_type txlm_tgt_buffer_idx
)
{
  uint8 temp_idx;

  rf_time_tick_type switch_buffer_start_t = {0};
  rf_time_type switch_buffer_time;


  if(rf_tdscdma_mc_display_time_profile)
  {
    switch_buffer_start_t = rf_time_get_tick();
  }

  MSG_3(MSG_SSID_RF, MSG_LVL_HIGH, "rf_tdscdma_mc_switch_buffer(buf_swap=%d, rxlm_buf_idx=%d, txlm_buf_idx=%d)",
        buf_swap,rxlm0_tgt_buffer_idx,txlm_tgt_buffer_idx);

#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, "rf_tdscdma_mc_switch_buffer(rxlm_tgt_buffer_idx_div=%d)", rxlm_tgt_buffer_idx_div);
#endif

  /* State and Param Check  												 */
  /*=========================================================================*/
  if ( rf_tdscdma_mc_state.rf_state[RF_PATH_0] != RF_TDSCDMA_STATE_RXTX )
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_switch_buffer: called in wrong state");
    return RF_TDSCDMA_MC_SM_ERROR;
  }

  if ( (rxlm0_tgt_buffer_idx != rf_tdscdma_mc_lm_bufs.rx_buf_tgt[0].buf_idx) &&
       ((buf_swap == RFA_TDSCDMA_BUF_RX)||(buf_swap == RFA_TDSCDMA_BUF_RXTX)) )
  {
    /* flag an error but continue */
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_switch_buffer: wrong rx buffer idx");
  }

  if ( (txlm_tgt_buffer_idx != rf_tdscdma_mc_lm_bufs.tx_buf_tgt.buf_idx) &&
       ((buf_swap == RFA_TDSCDMA_BUF_TX)||(buf_swap == RFA_TDSCDMA_BUF_RXTX)) )
  {
    /* flag an error but continue */
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_switch_buffer: wrong tx buffer idx");
  }


  /* Buffer Index Swap  													 */
  /*=========================================================================*/
  if (buf_swap == RFA_TDSCDMA_BUF_RXTX) /* 0ms BHO */
  {
    /* save txlm_tgt_buffer_idx to wrk tx LM buf */
    rf_tdscdma_mc_lm_bufs.tx_buf_wrk.buf_idx = txlm_tgt_buffer_idx;
    rf_tdscdma_mc_lm_bufs.tx_buf_tgt.is_buf_allocated = FALSE;

    /* save rxlm0_tgt_buffer_idx to wrk rx LM buf */
    rf_tdscdma_mc_lm_bufs.rx_buf_wrk[0].buf_idx = rxlm0_tgt_buffer_idx;
    rf_tdscdma_mc_lm_bufs.rx_buf_tgt[0].is_buf_allocated = FALSE;
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
    rf_tdscdma_mc_lm_bufs.rx_buf_wrk[1].buf_idx = rxlm_tgt_buffer_idx_div;
    rf_tdscdma_mc_lm_bufs.rx_buf_tgt[1].is_buf_allocated = FALSE;
#endif

    /* swap rx/tx f3/f1 buffer(agc,grfc,SSBI) index */
    temp_idx = rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx;
    rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx = rf_tdscdma_mc_dbl_bufs.rx_tgt_buf_idx;
    rf_tdscdma_mc_dbl_bufs.rx_tgt_buf_idx = temp_idx;
    temp_idx = rf_tdscdma_mc_dbl_bufs.tx_wrk_buf_idx;
    rf_tdscdma_mc_dbl_bufs.tx_wrk_buf_idx = rf_tdscdma_mc_dbl_bufs.tx_tgt_buf_idx;
    rf_tdscdma_mc_dbl_bufs.tx_tgt_buf_idx = temp_idx;

    /* Update the hdet interval value (init to 2 subframes = 10 ms) to mdsp (send msg) */
    rf_tdscdma_core_txplim_interval_update(2);

    /* enable the hdet and temp comp on the new chan */
    rf_tdscdma_core_temp_comp_enable(TRUE);
    if(rf_tdscdma_mc_enable_hdet_max_pwr_limiting)
    {
      rf_tdscdma_core_txplim_enable(TRUE);
    }

    /* Start the 10s timer */
    rf_tdscdma_core_temp_comp_start();

    /* enable FBRx on the new chan */
    rflm_tds_mc_fbrx_enable(TRUE);

    if (rfcommon_autopin_is_enabled(RFM_TDSCDMA_MODE))
    {
      rf_tdscdma_msm_autopin_push_capture_params(rf_tdscdma_mc_lm_bufs.tx_buf_wrk.buf_idx,
                                                 rf_tdscdma_mc_cmn_device_ptr,
                                                 rf_tdscdma_mc_state.curr_band_tx,
                                                 rf_tdscdma_mc_state.curr_chan_tx);

      rfcommon_autopin_mc_enable_tx(rf_tdscdma_mc_lm_bufs.tx_buf_wrk.buf_idx,
                                    RFM_TDSCDMA_MODE);
    }
  }
  else if (buf_swap == RFA_TDSCDMA_BUF_TX)
  {
    /* save txlm_tgt_buffer_idx to wrk tx LM buf */
    rf_tdscdma_mc_lm_bufs.tx_buf_wrk.buf_idx = txlm_tgt_buffer_idx;
    rf_tdscdma_mc_lm_bufs.tx_buf_tgt.is_buf_allocated = FALSE;

    /* swap tx f3/f1 buffer(txagc,grfc,SSBI) index */
    temp_idx = rf_tdscdma_mc_dbl_bufs.tx_wrk_buf_idx;
    rf_tdscdma_mc_dbl_bufs.tx_wrk_buf_idx = rf_tdscdma_mc_dbl_bufs.tx_tgt_buf_idx;
    rf_tdscdma_mc_dbl_bufs.tx_tgt_buf_idx = temp_idx;

    /* Update the hdet interval value (init to 2 subframes = 10 ms) to mdsp (send msg) */
    rf_tdscdma_core_txplim_interval_update(2);

    /* enable the hdet and temp comp on the new chan */
    rf_tdscdma_core_temp_comp_enable(TRUE);
    if(rf_tdscdma_mc_enable_hdet_max_pwr_limiting)
    {
      rf_tdscdma_core_txplim_enable(TRUE);
    }
    /* Start the 10s timer */
    rf_tdscdma_core_temp_comp_start();

    /* enable FBRx on the new chan */
    rflm_tds_mc_fbrx_enable(TRUE);

    if (rfcommon_autopin_is_enabled(RFM_TDSCDMA_MODE))
    {
      rf_tdscdma_msm_autopin_push_capture_params(rf_tdscdma_mc_lm_bufs.tx_buf_wrk.buf_idx,
                                                 rf_tdscdma_mc_cmn_device_ptr,
                                                 rf_tdscdma_mc_state.curr_band_tx,
                                                 rf_tdscdma_mc_state.curr_chan_tx);

      rfcommon_autopin_mc_enable_tx(rf_tdscdma_mc_lm_bufs.tx_buf_wrk.buf_idx,
                                    RFM_TDSCDMA_MODE);
    }
  }
  else if (buf_swap == RFA_TDSCDMA_BUF_RX)
  {
    /* save rxlm0_tgt_buffer_idx to wrk rx LM buf */
    rf_tdscdma_mc_lm_bufs.rx_buf_wrk[0].buf_idx = rxlm0_tgt_buffer_idx;
    rf_tdscdma_mc_lm_bufs.rx_buf_tgt[0].is_buf_allocated = FALSE;
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
    /* save rxlm_tgt_buffer_idx_div to wrk rx LM buf */
    rf_tdscdma_mc_lm_bufs.rx_buf_wrk[1].buf_idx = rxlm_tgt_buffer_idx_div;
    rf_tdscdma_mc_lm_bufs.rx_buf_tgt[1].is_buf_allocated = FALSE;
#endif

    /* swap rx f3/f1 buffer(rxagc,grfc,SSBI) index */
    temp_idx = rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx;
    rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx = rf_tdscdma_mc_dbl_bufs.rx_tgt_buf_idx;
    rf_tdscdma_mc_dbl_bufs.rx_tgt_buf_idx = temp_idx;
  }

  rflmTdsRfModeCtlState.rxHomeBufIdx = rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx;  
  
  if(rf_tdscdma_mc_display_time_profile)
  {
    switch_buffer_time = rf_time_get_elapsed(switch_buffer_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Switch Buffer time profile: %d usec", switch_buffer_time);
  }

  return RF_TDSCDMA_MC_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns UE max tx power in dBm (from NV)

  @details
*/
int16 rf_tdscdma_mc_get_max_tx_pwr(void)
{
  int16 max_tx_pwr = RF_TDSCDMA_UE_MAX_TX_POWER;

  rf_time_tick_type get_max_tx_pwr_start_t = {0};
  rf_time_type get_max_tx_pwr_time;

  if(rf_tdscdma_mc_display_time_profile)
  {
    get_max_tx_pwr_start_t = rf_time_get_tick();
  }

  if ( rf_tdscdma_mc_state.curr_band_tx == RFCOM_BAND_TDSCDMA_INVALID )
  {
    max_tx_pwr = ((rf_tdscdma_core_util_init_tx_nv_table(RFCOM_BAND_TDSCDMA_B34)->max_tx_power)/10) \
                 + (RF_TDSCDMA_MIN_TX_POWER_DBM);
  }
  else
  {
    max_tx_pwr = (rfnv_tdscdma_tx_tbl_ptr->max_tx_power / 10) + (RF_TDSCDMA_MIN_TX_POWER_DBM);
  }

  /* NV not set */
  if (max_tx_pwr <= 0)
  {
    max_tx_pwr = RF_TDSCDMA_UE_MAX_TX_POWER;
  }

  if(rf_tdscdma_mc_display_time_profile)
  {
    get_max_tx_pwr_time = rf_time_get_elapsed(get_max_tx_pwr_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "UE Max Tx power time profile: %d usec", get_max_tx_pwr_time);
  }

  return(max_tx_pwr);
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process the therm mitigation backoff

  @details
*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_therm_backoff(uint8 backoff)
{
  rf_time_tick_type therm_backoff_start_t = {0};
  rf_time_type therm_backoff_time;

  if(rf_tdscdma_mc_display_time_profile)
  {
    therm_backoff_start_t = rf_time_get_tick();
  }

  if ( rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RXTX )
  {
    //MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_therm_backoff: backoff=%d", backoff);
    rf_tdscdma_core_txplim_reduce_mptl_db_for_therm(backoff);
  }

  if(rf_tdscdma_mc_display_time_profile)
  {
    therm_backoff_time = rf_time_get_elapsed(therm_backoff_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Thermal Mitigation back off time profile: %d usec", therm_backoff_time);
  }


  return RF_TDSCDMA_MC_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  update max tx power limit(in dBm x10 ) from MCS

  @details
*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_set_coex_tx_pwr_limit
(
  int16 sar_tx_pwr_limit,
  int16 coex_tx_pwr_limit
)
{
  rf_time_tick_type set_coex_tx_pwr_limit_start_t = {0};
  rf_time_type set_coex_tx_pwr_limit_time;

  rf_tdscdma_coex_tx_pwr_limit = coex_tx_pwr_limit;

  if(rf_tdscdma_mc_display_time_profile)
  {
    set_coex_tx_pwr_limit_start_t = rf_time_get_tick();
  }

  MSG_2(MSG_SSID_RF, MSG_LVL_HIGH,"rf_tdscdma_mc_set_coex_tx_pwr_limit(sar_limit=%d, coex_limit=%d)",
        sar_tx_pwr_limit,coex_tx_pwr_limit);

  if ( rf_tdscdma_mc_state.rf_state[RF_PATH_0] != RF_TDSCDMA_STATE_RXTX )
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_set_coex_tx_pwr_limit: called in wrong state");
    return RF_TDSCDMA_MC_SM_ERROR;
  }

  rf_tdscdma_core_txplim_set_coex_limit(sar_tx_pwr_limit,coex_tx_pwr_limit);

  if(rf_tdscdma_mc_display_time_profile)
  {
    set_coex_tx_pwr_limit_time = rf_time_get_elapsed(set_coex_tx_pwr_limit_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Set Coex Tx Power limit time profile: %d usec", set_coex_tx_pwr_limit_time);
  }

  return RF_TDSCDMA_MC_SUCCESS;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process the hdet readings from mdsp

  @details
*/
void rf_tdscdma_mc_proc_hdet_reading
(
  uint16 num_ssbi,
  int16 mdsp_txagc,
  uint8 raw_reading[]
)
{
  rfdevice_tdscdma_calc_hdet_measurement_type  hdet_meas_data;

  rf_time_tick_type proc_hdet_reading_start_t = {0};
  rf_time_type proc_hdet_reading_time;

  if(rf_tdscdma_mc_display_time_profile)
  {
    proc_hdet_reading_start_t = rf_time_get_tick();
  }

  if ((rf_tdscdma_mc_state.hdet_info & 0x10) == 0x10)
  {
    // HDET is coupled with Therm read
    if (rf_tdscdma_mc_hdet_debug)
    {
      MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, "HDET debug: HDET is coupled with Therm on HW_ID=%d", rfcommon_nv_tbl.rf_hw_config);
    }
    /* hdet-based max pwr limiting should not be enabled until pdet dc cal has been done once */
    if(!rf_tdscdma_mc_state.pdet_dc_cal_done)
    {
      if (rf_tdscdma_mc_hdet_debug)
      {
        MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, "HDET debug: rf_tdscdma_mc_state.pdet_dc_cal_done=%d", rf_tdscdma_mc_state.pdet_dc_cal_done);
      }

      return;
    }
  }

  if(num_ssbi==0)
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, "HDET debug: FW HDET read failed with zero read!");
    return;
  }

  /* Get averaged hdet from RF Device */
  hdet_meas_data.total_number = num_ssbi;
  hdet_meas_data.hdet_raw_reading = raw_reading;

  if (is_wtr_hdet)
  {
    rfdevice_tdscdma_tx_cmd_dispatch(RFM_DEVICE_0,
                                     RFCOM_TDSCDMA_MODE,
                                     rf_tdscdma_mc_state.single_valid_band,
                                     RFDEVICE_TDSCDMA_CALC_HDET_MEASUREMENT,
                                     &hdet_meas_data);
  }
  else
  {
#if 0
    (void)rfdevice_hdet_tdscdma_tx_hdet_read(RFM_DEVICE_0,
                                             rf_tdscdma_mc_state.single_valid_band,
                                             TRUE,
                                             &hdet_meas_data,
                                             void *buff_obj_ptr,
                                             RFDEVICE_EXECUTE_IMMEDIATE
                                            )
#endif

      (void)rfdevice_hdet_tdscdma_calc_hdet_measurement(RFM_DEVICE_0,
                                                        rf_tdscdma_mc_state.single_valid_band,
                                                        &hdet_meas_data);
  }

  if ((rf_tdscdma_mc_state.hdet_info & 0x01) == 0x01)
  {
    // Enable HDET/MTPL
    if (rf_tdscdma_mc_hdet_debug)
    {
      MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, "HDET debug: HDET is enabled on HW_ID=%d", rfcommon_nv_tbl.rf_hw_config);
    }

    if(hdet_meas_data.hdet_converg_flag == TRUE)
    {
      if (rf_tdscdma_mc_hdet_debug)
      {
        MSG_3(MSG_SSID_RF, MSG_LVL_ERROR, "HDET debug: hdet_avg_meas=%d, txagc=%d, converted txagc=%d ", \
              hdet_meas_data.hdet_avg_measurement,                             \
              rf_tdscdma_mdsp_txagc_mdsp_to_db10(mdsp_txagc),   																			 \
              rf_tdscdma_core_txplim_convert_hdet_adc_to_txagc(hdet_meas_data.hdet_avg_measurement));
      }

      rf_tdscdma_core_txplim_do_mtpl(hdet_meas_data.hdet_avg_measurement,
                                     rf_tdscdma_mdsp_txagc_mdsp_to_db10(mdsp_txagc));
    }
    else
    {
      if (rf_tdscdma_mc_hdet_debug)
      {
        MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "HDET debug: HDET is not converged!", 0);
      }
    }

    /* Special HDET handling for RF Mode Continuous TX */
    if(IS_FTM_IN_TEST_MODE() && !rfm_get_calibration_state())
    {
      rf_tdscdma_mc_rf_mode_hdet_comp_handler();
    }
  }
  else
  {
    // Disable HDET/MTPL
    if (rf_tdscdma_mc_hdet_debug)
    {
      MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, "HDET debug: HDET is disabled on HW_ID=%d!", rfcommon_nv_tbl.rf_hw_config);
    }
  }

  if(rf_tdscdma_mc_display_time_profile)
  {
    proc_hdet_reading_time = rf_time_get_elapsed(proc_hdet_reading_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Process HDET reading time profile: %d usec", proc_hdet_reading_time);
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Trigger HDET

  @details
*/

void rf_tdscdma_mc_trigger_hdet(int16 rflm_txagcdb10)
{
  rfdevice_tdscdma_calc_hdet_measurement_type  hdet_meas_data;
  uint16 raw_reading;

  rf_time_tick_type trigger_hdet_start_t = {0};
  rf_time_type trigger_hdet_time;

  MSG(MSG_SSID_RF, MSG_LVL_HIGH, "HDET: rf_tdscdma_mc_trigger_hdet: Enter");

  if(rf_tdscdma_mc_display_time_profile)
  {
    trigger_hdet_start_t = rf_time_get_tick();
  }  

  if ((rf_tdscdma_mc_state.hdet_info & 0x10) == 0x10)
  {
    // HDET is coupled with Therm read
    if (rf_tdscdma_mc_hdet_debug)
    {
      MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, "HDET debug: HDET is coupled with Therm on HW_ID=%d", rfcommon_nv_tbl.rf_hw_config);
    }
    /* hdet-based max pwr limiting should not be enabled until pdet dc cal has been done once */
    if(!rf_tdscdma_mc_state.pdet_dc_cal_done)
    {
      if (rf_tdscdma_mc_hdet_debug)
      {
        MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, "HDET debug: rf_tdscdma_mc_state.pdet_dc_cal_done=%d", rf_tdscdma_mc_state.pdet_dc_cal_done);
      }

      return;
    }
  }

  if (is_wtr_hdet)
  {
    rfdevice_tdscdma_tx_cmd_dispatch(RFM_DEVICE_0, 
                                     RFCOM_TDSCDMA_MODE, 
                                     rf_tdscdma_mc_state.single_valid_band,
                                     RFDEVICE_TDSCDMA_CALC_HDET_MEASUREMENT,
                                     &hdet_meas_data);
  }
  else
  { 
    
  	(void)rfdevice_hdet_tdscdma_tx_hdet_read(RFM_DEVICE_0,
                                             rf_tdscdma_mc_state.single_valid_band,
                                             TRUE,
                                             &raw_reading,
                                             NULL,
                                             RFDEVICE_EXECUTE_IMMEDIATE);

   
    //hdet_meas_data.hdet_raw_reading = &raw_reading;
    hdet_meas_data.hdet_avg_measurement = raw_reading;

    /*(void)rfdevice_hdet_tdscdma_calc_hdet_measurement(RFM_DEVICE_0,
                                                     rf_tdscdma_mc_state.single_valid_band,
                                                     &hdet_meas_data);*/
  }

  //Clear FED flags to fecilitate next HDET trigger.
  rflmTdsRfModeCtlState.pdetInfo.triggerExecuted = FALSE;
  rflmTdsRfModeCtlState.pdetInfo.triggerScheSlotNum = RFLM_TDS_TS_INVALID;
  rflmTdsRfModeCtlState.pdetInfo.inProcess = FALSE;

  //Hack - Hardcode HDET converge flag to TRUE
  hdet_meas_data.hdet_converg_flag = TRUE;

  if ((rf_tdscdma_mc_state.hdet_info & 0x01) == 0x01)
  {
    // Enable HDET/MTPL
    if (rf_tdscdma_mc_hdet_debug)
    {
      MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, "HDET debug: HDET is enabled on HW_ID=%d", rfcommon_nv_tbl.rf_hw_config);
    }

    if(hdet_meas_data.hdet_converg_flag == TRUE) 
    {
      if (rf_tdscdma_mc_hdet_debug)
      {
        MSG_3(MSG_SSID_RF, MSG_LVL_ERROR, "HDET debug: hdet_avg_meas=%d, txagc=%d, converted txagc=%d ", \
          hdet_meas_data.hdet_avg_measurement,                             \
    		  rflm_txagcdb10,   																			 \
    		  rf_tdscdma_core_txplim_convert_hdet_adc_to_txagc(hdet_meas_data.hdet_avg_measurement));
      }

      rf_tdscdma_core_txplim_do_mtpl(hdet_meas_data.hdet_avg_measurement, rflm_txagcdb10+700);
    }
    else
    {
      if (rf_tdscdma_mc_hdet_debug)
      {
        MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "HDET debug: HDET is not converged!", 0);
      }
    }

    /* Special HDET handling for RF Mode Continuous TX */
    if(IS_FTM_IN_TEST_MODE() && !rfm_get_calibration_state())
    {
      rf_tdscdma_mc_rf_mode_hdet_comp_handler();
    }
  }
  else
  {
    // Disable HDET/MTPL
    if (rf_tdscdma_mc_hdet_debug)
    {
      MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, "HDET debug: HDET is disabled on HW_ID=%d!", rfcommon_nv_tbl.rf_hw_config);
    }
  }

  if(rf_tdscdma_mc_display_time_profile)
  {
    trigger_hdet_time = rf_time_get_elapsed(trigger_hdet_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Trigger HDET time profile: %d usec", trigger_hdet_time);
  }
}

void rf_tdscdma_mc_trigger_therm()
{
  uint16 therm_read;

  rfdevice_tdscdma_tx_cmd_dispatch(RFM_DEVICE_0,
                                   RFCOM_TDSCDMA_MODE,
                                   rf_tdscdma_mc_state.single_valid_band,
                                   RFDEVICE_GET_THERMISTER_VALUE,
                                   &therm_read);

  //Clear FED flags to fecilitate next Therm trigger.
  rflmTdsRfModeCtlState.thermInfo.triggerExecuted = FALSE;
  rflmTdsRfModeCtlState.thermInfo.triggerScheSlotNum = RFLM_TDS_TS_INVALID;
  rflmTdsRfModeCtlState.thermInfo.inProcess = FALSE;

  if (rf_tdscdma_mc_therm_debug)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_HIGH,
          "Therm debug: therm_adc=%d", therm_read);
  }

  rf_tdscdma_core_temp_comp_proc_therm_reading(therm_read);

  /* Special Temp Comp handling for RF Mode Continuous TX */
  if(IS_FTM_IN_TEST_MODE() && !rfm_get_calibration_state())
  {
    rf_tdscdma_mc_rf_mode_temp_comp_handler();
  }
}


void rf_tdscdma_mc_autopin_trigger_read()
{
  rfcommon_autopin_capture_control_type control_input = { 0 };
  boolean status = FALSE;

  status = rfdevice_cmn_fbrx_autopin_read_and_disable(rf_tdscdma_mc_cmn_device_ptr,
                                                      &control_input.wtr_capture_result,
                                                      FALSE,
                                                      NULL);

  rflmTdsRfModeCtlState.autopinInfo.triggerExecuted = FALSE;
  rflmTdsRfModeCtlState.autopinInfo.inProcess = FALSE;

  control_input.capture_type = AUTOPIN_PAR_WTR_CAPTURE;
  control_input.valid_capture = status;
  control_input.par = control_input.wtr_capture_result.par;

#if 0
  RF_MSG_3(RF_HIGH, "AutoPin WTR: handle %d, valid_capture %d, par %d",
           rf_tdscdma_mc_lm_bufs.tx_buf_wrk.buf_idx,
           control_input.valid_capture,
           control_input.par);
#endif

  rfcommon_autopin_capture_control(rf_tdscdma_mc_lm_bufs.tx_buf_wrk.buf_idx,
                                   control_input);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process the therm readings from mdsp

  @details
*/
void rf_tdscdma_mc_proc_therm_reading
(
  uint16 num_ssbi,
  uint8 raw_reading[],
  boolean skip_calc,
  uint16 therm_read
)
{
  rfdevice_tdscdma_calc_therm_measurement_type  therm_meas_data;
  rftdscdma_mdsp_common_event_data_type rf_tds_build_event_data;

  rf_time_tick_type proc_therm_reading_start_t = {0};
  rf_time_type proc_therm_reading_time;
#ifndef FEATURE_TRITON_MODEM
  uint32 iqmc_cfg1_0 = 0x1000000, iqmc_cfg1_1 = 0x1000000;
#endif

  if(rf_tdscdma_mc_display_time_profile)
  {
    proc_therm_reading_start_t = rf_time_get_tick();
  }

  if (rf_tdscdma_mc_therm_debug)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, "Therm debug: rf_tdscdma_mc_state.hdet_info=%x", rf_tdscdma_mc_state.hdet_info);
  }

  if (!skip_calc)
  {
    /* Call device API to calculate therm_read value */
    if(num_ssbi==0)
    {
      MSG(MSG_SSID_RF, MSG_LVL_ERROR, "Therm debug: FW can't read therm!");
      return;
    }

    /* Get therm value from RF Device */
    therm_meas_data.total_number = num_ssbi;
    therm_meas_data.therm_raw_reading = raw_reading;
    rfdevice_tdscdma_tx_cmd_dispatch(RFM_DEVICE_0,
                                     RFCOM_TDSCDMA_MODE,
                                     rf_tdscdma_mc_state.single_valid_band,
                                     RFDEVICE_TDSCDMA_CALC_THERM_MEASUREMENT,
                                     &therm_meas_data);
  }
  else
  {
    if (therm_read != 0xFFFF )
    {
      therm_meas_data.therm_converg_flag = TRUE;
    }
    else
    {
      therm_meas_data.therm_converg_flag = FALSE;
    }
    therm_meas_data.temperature_measurement = therm_read;
  }

  if (rf_tdscdma_mc_therm_debug)
  {
    MSG_3(MSG_SSID_RF, MSG_LVL_HIGH,
          "Therm debug: therm_adc=%d, therm_conv=%d, skip_calc=%d",
          therm_meas_data.temperature_measurement, therm_meas_data.therm_converg_flag, skip_calc);
  }

  if(therm_meas_data.therm_converg_flag==TRUE)
  {
    rf_tdscdma_core_temp_comp_proc_therm_reading(therm_meas_data.temperature_measurement);
  }
  else
  {
    if (rf_tdscdma_mc_therm_debug)
    {
      MSG(MSG_SSID_RF, MSG_LVL_ERROR, "Therm debug: therm data not converged!");
    }
    return;
  }

  if ((rf_tdscdma_mc_state.hdet_info & 0x10) == 0x10)
  {
    if (rf_tdscdma_mc_therm_debug)
    {
      MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, "Therm debug: HDET is coupled with Therm on HW_ID=%d", rfcommon_nv_tbl.rf_hw_config);
    }
    /* After therm measurement is done, need to re-build HDET script to take PDET cal update */
    // This is needed when HDET is coupled with Therm
    // Hard-coded device info for now
    rf_tds_build_event_data.tx_device = RFM_DEVICE_0;
    rf_tds_build_event_data.band = rf_tdscdma_mc_state.single_valid_band;
    rf_tds_build_event_data.channel = 10055;  // Chan info is not needed for the script building
    rf_tds_build_event_data.drx_enable = FALSE;
    rf_tds_build_event_data.buf_id = rf_tdscdma_mc_dbl_bufs.tx_wrk_buf_idx;
    rf_tds_build_event_data.is_bho_update = FALSE;
    
    rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_DO_HDET, &rf_tds_build_event_data);

    /* Send msdp msg to notify hdet script was updated */
    //rf_tdscdma_core_txplim_interval_update(rf_tdscdma_core_txplim_get_hdet_interval());
  }

  rf_tdscdma_mc_state.pdet_dc_cal_done = TRUE;

#ifndef FEATURE_TRITON_MODEM
  /* Process RSB temperature compensation update */
  iqmc_cfg1_0 = rf_tdscdma_msm_calc_iqmc_setting(RFM_DEVICE_0,
                                                 rf_tdscdma_mc_state.curr_chan_rx,
                                                 rf_tdscdma_mc_lm_bufs.rx_buf_wrk[RF_PATH_0].buf_idx);

  MSG_1(MSG_SSID_RF_DEBUG, MSG_LVL_HIGH, "Therm debug: iqmc_cfg1_0=0x%X", iqmc_cfg1_0);

  if (rf_tdscdma_mc_state.div_enable)
  {
    iqmc_cfg1_1 = rf_tdscdma_msm_calc_iqmc_setting(RFM_DEVICE_1,
                                                   rf_tdscdma_mc_state.curr_chan_rx,
                                                   rf_tdscdma_mc_lm_bufs.rx_buf_wrk[RF_PATH_1].buf_idx);
  }
  MSG_1(MSG_SSID_RF_DEBUG, MSG_LVL_HIGH, "Therm debug: iqmc_cfg1_1=0x%X", iqmc_cfg1_1);

  /* Send iqmc_cfg values to mdsp */
  rf_tdscdma_mdsp_update_iqmc(iqmc_cfg1_0, iqmc_cfg1_1, rf_tdscdma_mc_state.div_enable);
#endif /* ! FEATURE_TRITON_MODEM */

  /* Special Temp Comp handling for RF Mode Continuous TX */
  if(IS_FTM_IN_TEST_MODE() && !rfm_get_calibration_state())
  {
    rf_tdscdma_mc_rf_mode_temp_comp_handler();
  }

  if(rf_tdscdma_mc_display_time_profile)
  {
    proc_therm_reading_time = rf_time_get_elapsed(proc_therm_reading_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Process Thermal reading time profile: %d usec", proc_therm_reading_time);
  }

}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is called from rfm_init(). It loads NV and registers TDS module msg
  dispatch function

  @details

  @param common_init_success: if common initialization was successful.
  @param caller_tcb_ptr: Calling task TCB pointer
  @param task_nv_wait_sig: Task signal to wait for when reading NV
  @param task_wait_func_ptr: Task's wait function to be called when reading NV
*/
boolean rf_tdscdma_mc_init
(
  boolean common_init_successful,
  rex_tcb_type *caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (*task_wait_func_ptr)( rex_sigs_type )
)
{
  rfcommon_nv_status_type nv_status = RFCOMMON_NV_READ_FAILED;
  static boolean nv_items_read = FALSE;

  rf_time_tick_type init_start_t = {0};
  rf_time_type init_time;

  if(rf_tdscdma_mc_display_time_profile)
  {
    init_start_t = rf_time_get_tick();
  }


  if ( common_init_successful )
  {
    if (!nv_items_read)
    {
      nv_status = rfnv_tdscdma_retrieve_items(caller_tcb_ptr, task_nv_wait_sig,
                                              task_wait_func_ptr);

      if (nv_status == RFCOMMON_NV_READ_SUCCESS)
      {
        /* Not missing critical NV items */
      }

      MSG(MSG_SSID_RF_DEBUG, MSG_LEGACY_MED, "MC Init: Call FBRX NV retrieve API");
      rfcommon_fbrx_nv_retrieve_tdscdma_data(caller_tcb_ptr, task_nv_wait_sig,
                                             task_wait_func_ptr);

      /* multi-lin data NV read is handled separately*/
      rf_tdscdma_core_txlin_init();


      /* Remember that we've read the NV. */
      nv_items_read = TRUE;
    }
  }

  /* register TDSCDMA module msg dispatch function */
  rf_tdscdma_msg_dispatch_register();

  /* register the IRAT APIs with meas module */
  rf_tdscdma_mc_meas_init();

  /* Register RFLM messages*/
  rf_tdscdma_msgr_rflm_register();

  if(rf_tdscdma_mc_display_time_profile)
  {
    init_time = rf_time_get_elapsed(init_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "MC Init time profile: %d usec", init_time);
  }


  return TRUE;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Reload NV for TDSCDMA.

  @details
  Called in rfm_reload_nv. Retrieves the NV for TDSCDMA. This is called as part of the
  LOAD_RF_NV function case where we need to read the calibration NV and write
  them without having to reset.

  @param caller_tcb_ptr 	: Calling task's TCB pointer
  @param task_nv_wait_sig   : Task signal to wait for when reading NV
  @param task_wait_func_ptr : Task's wait function to be called when reading NV

  @retval Returns TRUE if NV retrieve is successful, otherwise returns FALSE.
*/
boolean rf_tdscdma_mc_reload_nv
(
  rex_tcb_type *caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (*task_wait_func_ptr)( rex_sigs_type )
)
{
  rf_time_tick_type reload_nv_start_t = {0};
  rf_time_type reload_nv_time;
  rfcommon_nv_status_type nv_status = RFCOMMON_NV_READ_FAILED;

  if(rf_tdscdma_mc_display_time_profile)
  {
    reload_nv_start_t = rf_time_get_tick();
  }

  rfnv_tdscdma_reset_valid_flags();

  nv_status = rfnv_tdscdma_retrieve_items(caller_tcb_ptr, task_nv_wait_sig,
                                          task_wait_func_ptr);

  if (nv_status == RFCOMMON_NV_READ_SUCCESS)
  {
    /* Not missing critical NV items */
  }

  MSG(MSG_SSID_RF_DEBUG, MSG_LEGACY_MED, "MC Init: Call FBRX NV retrieve API");
  rfcommon_fbrx_nv_retrieve_tdscdma_data(caller_tcb_ptr, task_nv_wait_sig,
                                         task_wait_func_ptr);

  /* multi-lin data NV read is handled separately*/
  rf_tdscdma_core_txlin_init();

  if(rf_tdscdma_mc_display_time_profile)
  {
    reload_nv_time = rf_time_get_elapsed(reload_nv_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Reload NV time profile: %d usec", reload_nv_time);
  }

  return TRUE;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update MDSP content for Rx Chain. Need to call on each chain.

  @details
  Update MDSP content, for Rx Chain, including
	-  Rx swpt
	-  Rx Freq Comp
	-  RxLM SHM
	-  RF scripts (e.g. for OFF-to-RX and RX-to-OFF transitions)
	-  RxAGC script (e.g., LNA script)
  Need to call on each chain.

*/
void rf_tdscdma_mc_update_rx_mdsp
(
  rfm_device_enum_type device,
  rf_path_enum_type path,
  rfcom_tdscdma_band_type band,
  uint16 rx_channel,
  lm_handle_type rxlm_buffer_idx,
  rfa_tdscdma_rxlm_mode_t rxlm_mode,
  boolean call_in_wakeup,
  uint8 buf_id,
  boolean call_for_bho_update,
  rfa_tdscdma_wakeup_indicator_t wakeup_step_ind
)
{
  rftdscdma_mdsp_common_event_data_type rf_tds_build_event_data;
  rf_time_tick_type update_rx_mdsp_start_t = {0}, overall_update_rx_mdsp_start_t = {0};
  rf_time_type update_rx_mdsp_time, overall_update_rx_mdsp_time;
  uint8 ppBufIdx;
  rf_path_enum_type rf_path;
  rf_card_band_type rfc_band;
  rfcmn_status_type alt_path_status;
  uint8 alt_path = 0;
  boolean rfc_alt_path_status;
  uint8 wb_adc_idx = 0;
  uint8 ifreq_idx = 0;
  rfcom_tdscdma_band_type loop_band;

  if(rf_tdscdma_mc_display_time_profile || rf_tdscdma_mc_wakeup_time_debug || rf_tdscdma_mc_tx_time_debug)
  {
    overall_update_rx_mdsp_start_t = rf_time_get_tick(); /* time spent in API */
  }

  if (rf_tdscdma_mc_wakeup_time_debug || rf_tdscdma_mc_tx_time_debug)
  {
    update_rx_mdsp_start_t =  rf_time_get_tick();
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: update_rx_mdsp: call on Path %d", path);
  }

  if((wakeup_step_ind == RFA_TDSCDMA_WAKEUP_NA)      ||
     (wakeup_step_ind == RFA_TDSCDMA_WAKEUP_STEP0P1)  )
  {
    /* Update alt_path														 */
    /*=========================================================================*/
    /* Only update on PATH_0
          Need to update for both chains if the diversity is enabled     */

    if (path == RF_PATH_0)
    {
      if ( (IS_FTM_IN_TEST_MODE() == TRUE) && (rfm_get_calibration_state() == FALSE) )
      {
        // Add the check for FTM RF mode only
        boolean validate_device;
        rfcom_band_type_u u_band;
        ftm_concurrency_manager_params_type conmgr_params;
        
        u_band.tdscdma_band = band;
        
        conmgr_params.band = u_band;
        conmgr_params.carrier_index = 0;
        conmgr_params.prx_drx = RFC_PRX_PATH;
        conmgr_params.rfm_dev = rf_tdscdma_mc_state.curr_pri_device;
        conmgr_params.tech = RFM_TDSCDMA_MODE;
  	  
        validate_device = ftm_concurrency_manager_validate_operation(&conmgr_params);

        if(validate_device == FALSE)
        {        
          ERR_FATAL("Invalid combination to test FTM RF concurrency! Device: %d, band: %d, Path: %d", 
                    rf_tdscdma_mc_state.curr_pri_device,
                    band,
                    RFC_PRX_PATH);
        }

        if ((rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
            (rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_DIV)   )
        {
          conmgr_params.prx_drx = RFC_DRX_PATH;
          conmgr_params.rfm_dev = rf_tdscdma_mc_state.curr_sec_device;
          conmgr_params.tech = RFM_TDSCDMA_MODE;
  		
          validate_device = ftm_concurrency_manager_validate_operation(&conmgr_params);   

          if(validate_device == FALSE)
          {
            ERR_FATAL("Invalid combination to test FTM RF concurrency! Device: %d, band: %d, Path: %d", 
                      rf_tdscdma_mc_state.curr_sec_device,
                      band,
                      RFC_DRX_PATH);
          }
        }
      }
    
      for(loop_band = RFCOM_BAND_TDSCDMA_B34; loop_band <= RFCOM_BAND_TDSCDMA_B39; loop_band++)
      {
       rf_path = rfcommon_core_device_to_path(rf_tdscdma_mc_state.curr_pri_device);
       rfc_band = rfcommon_core_convert_tds_band_to_rfc_band(loop_band);
       alt_path_status =  rfcmn_concurrency_mgr_update_rx_path(rf_tdscdma_mc_state.curr_pri_device,
                                                              RFCOM_TDSCDMA_MODE,
                                                              (int)loop_band,
                                                              &alt_path);
      if (alt_path_status == RFCMN_PATH_SEL_FAILED)
      {
        ERR_FATAL("cannot get alt_path for device %d, TDS band %d, rfc_band %d", 
                  rf_tdscdma_mc_state.curr_pri_device, loop_band, rfc_band);
      }

      if (IS_FTM_IN_TEST_MODE())
      {
        // In RFcal mode, always uses alt_path from tools
        if (rfm_get_calibration_state() == TRUE) // RF calibration mode
        {
          alt_path = ftm_tdscdma_current_pri_alt_path;
            
          MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, 
                "Use the alt_path %d passed by Tools for Rx0 in RFcal mode", ftm_tdscdma_current_pri_alt_path);  
        }
        // In FTM RF mode, use alt_pth from tools if ftm_tdscdma_override_alt_path is TRUE
        else // FTM RF mode
        {
          if(ftm_tdscdma_override_alt_path == TRUE)
          {
            alt_path = ftm_tdscdma_current_pri_alt_path;
            
            MSG_1(MSG_SSID_RF_DEBUG, MSG_LEGACY_HIGH, 
                  "Use the alt_path %d passed by Tools for Rx0 in FTM RF mode", ftm_tdscdma_current_pri_alt_path);      
          }            
        }
      }

      rfc_alt_path_status = rfc_update_alt_port_mapping(rf_tdscdma_mc_state.curr_pri_device, 
                                                        RFCOM_TDSCDMA_MODE, 
                                                        rfc_band, 
                                                        alt_path);
      if (rfc_alt_path_status == FALSE)
      {
        ERR_FATAL("cannot get RFC alt_path for device %d, TDS band %d, rfc_band %d", 
                  rf_tdscdma_mc_state.curr_pri_device, loop_band, rfc_band);
      }

      rf_tdscdma_mc_state.alt_path[rf_path] = alt_path;
      
      MSG_6(MSG_SSID_RF, MSG_LEGACY_HIGH, 
            "Updated ALT_PATH to %d for device %d, TDS band %d, rfc_band %d, alt_path[%d] = %d", 
            alt_path, 
            rf_tdscdma_mc_state.curr_pri_device, 
            loop_band, 
            rfc_band, 
            rf_path, 
            rf_tdscdma_mc_state.alt_path[rf_path]);
      

        rf_path = rfcommon_core_device_to_path(rf_tdscdma_mc_state.curr_sec_device);
        rfc_band = rfcommon_core_convert_tds_band_to_rfc_band(loop_band);
        alt_path_status =  rfcmn_concurrency_mgr_update_rx_path(rf_tdscdma_mc_state.curr_sec_device,
                                                                RFCOM_TDSCDMA_MODE,
                                                                (int)loop_band,
                                                                &alt_path);
        if (alt_path_status == RFCMN_PATH_SEL_FAILED)
        {
          ERR_FATAL("cannot get alt_path for device %d, TDS band %d, rfc_band %d", 
                    rf_tdscdma_mc_state.curr_sec_device, loop_band, rfc_band);
        }

        if (IS_FTM_IN_TEST_MODE())
        {
          // In RFcal mode, always uses alt_path from tools
          if (rfm_get_calibration_state() == TRUE) // RF calibration mode
          {
            alt_path = ftm_tdscdma_current_sec_alt_path;
            
            MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, 
                  "Use the alt_path %d passed by Tools for Rx1 in RFcal mode", ftm_tdscdma_current_sec_alt_path);  
          }
          // In FTM RF mode, use alt_pth from tools if ftm_tdscdma_override_alt_path is TRUE
          else // FTM RF mode
          {
            if(ftm_tdscdma_override_alt_path == TRUE)
            {
              alt_path = ftm_tdscdma_current_sec_alt_path;
            
              MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, 
                    "Use the alt_path %d passed by Tools for Rx1 in FTM RF mode", ftm_tdscdma_current_sec_alt_path);      
            }            
          }
        }

        rfc_alt_path_status = rfc_update_alt_port_mapping(rf_tdscdma_mc_state.curr_sec_device, 
                                                          RFCOM_TDSCDMA_MODE, 
                                                          rfc_band, 
                                                          alt_path);
        if (rfc_alt_path_status == FALSE)
        {
          ERR_FATAL("cannot get RFC alt_path for device %d, TDS band %d, rfc_band %d", 
                    rf_tdscdma_mc_state.curr_sec_device, loop_band, rfc_band);
        }

        rf_tdscdma_mc_state.alt_path[rf_path] = alt_path;     

        MSG_6(MSG_SSID_RF, MSG_LEGACY_HIGH, 
              "Updated ALT_PATH to %d for device %d, TDS band %d, rfc_band %d, alt_path[%d] = %d", 
              alt_path, 
              rf_tdscdma_mc_state.curr_sec_device, 
              loop_band, 
              rfc_band, 
              rf_path, 
              rf_tdscdma_mc_state.alt_path[rf_path]);
      }
    }
    
    /* Update rxlm buffer(both static and dynamic setting) */
    /* RxLM update                               */
    /*=========================================================================*/
    
    /* Update rxlm buffer(both static and dynamic setting) */
    rf_tdscdma_msm_update_rxlm_buffer(device, rx_channel, rxlm_mode, rxlm_buffer_idx, &wb_adc_idx);
    rf_tdscdma_mc_state.scell_wb_adc_index_on_devices[path] = wb_adc_idx;
    if(path == RF_PATH_0)
    {
      rflmTdsRfModeCtlState.wbAdcInfo.rx0Idx = wb_adc_idx;
    }
    else if(path == RF_PATH_1)
    {
      rflmTdsRfModeCtlState.wbAdcInfo.rx1Idx = wb_adc_idx;
    }  
    
    if (rf_tdscdma_mc_wakeup_time_debug || rf_tdscdma_mc_tx_time_debug)
    {
      update_rx_mdsp_time = rf_time_get_elapsed(update_rx_mdsp_start_t, RF_USEC);
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: update_rx_mdsp: update_rxlm_buffer takes %d usec", update_rx_mdsp_time);
      update_rx_mdsp_start_t = rf_time_get_tick();
    }
  }
  else if (wakeup_step_ind == RFA_TDSCDMA_WAKEUP_STEP0P2)
  {
    MSG_1(MSG_SSID_RF_DEBUG, MSG_LVL_HIGH, 
          "rf_tdscdma_mc_update_rx_mdsp: Skipped alt_path update and RxLM for RFA_TDSCDMA_WAKEUP_STEP0P2", 0);
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, 
          "rf_tdscdma_mc_update_rx_mdsp: Wrong wakeup_step_ind=%d has passed in", wakeup_step_ind);
  }

  
  if (rf_tdscdma_use_on_rumi ==TRUE )
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_update_rx_mdsp: Return after populating RxLM buffer on RUMI", 0);

    MSG_7(MSG_SSID_RF, MSG_LVL_HIGH, "rf_tdscdma_mc_update_rx_mdsp(device %d, path %d, band %d, rx_channel %d, rxlm_buffer_idx %d, rxlm_mode %d, call_in_wakup) is complete", \
          device, path, band, rx_channel, rxlm_buffer_idx, rxlm_mode, call_in_wakeup);

    if(rf_tdscdma_mc_display_time_profile || rf_tdscdma_mc_tx_time_debug || rf_tdscdma_mc_wakeup_time_debug)
    {
      overall_update_rx_mdsp_time = rf_time_get_elapsed(overall_update_rx_mdsp_start_t, RF_USEC);
      MSG_3(MSG_SSID_RF, MSG_LEGACY_MED,
            "Debug Wakeup Timeline: update_rx_mdsp time profile: subtotal %d usec (in_wakeup=%d, path=%d)",
            overall_update_rx_mdsp_time, call_in_wakeup, path);
    }
      
    return; //return after rxlm update on RUMI
  }

  if(wakeup_step_ind == RFA_TDSCDMA_WAKEUP_STEP0P1) 
  {
    MSG_1(MSG_SSID_RF_DEBUG, MSG_LVL_HIGH, 
          "rf_tdscdma_mc_update_rx_mdsp: Return after alt_path update and populating RxLM buffer for wakeup_step0p1", 0);

    MSG_7(MSG_SSID_RF, MSG_LVL_HIGH, "rf_tdscdma_mc_update_rx_mdsp(device %d, path %d, band %d, rx_channel %d, rxlm_buffer_idx %d, rxlm_mode %d, call_in_wakup) is complete", \
          device, path, band, rx_channel, rxlm_buffer_idx, rxlm_mode, call_in_wakeup);
    
    if(rf_tdscdma_mc_display_time_profile || rf_tdscdma_mc_tx_time_debug || rf_tdscdma_mc_wakeup_time_debug)
    {
      overall_update_rx_mdsp_time = rf_time_get_elapsed(overall_update_rx_mdsp_start_t, RF_USEC);
      MSG_3(MSG_SSID_RF, MSG_LEGACY_MED,
            "Debug Wakeup Timeline: update_rx_mdsp time profile: subtotal %d usec (in_wakeup=%d, path=%d)",
            overall_update_rx_mdsp_time, call_in_wakeup, path);
    }
    return; 
  }


  /* Rx Swpt update 														 */
  /*=========================================================================*/

  /* Update Rx switch points for various mode */
  /* Populate the swpts in SMEM with TRK mode SWPTS if we are in FTM RF Burst mode */
  if ( IS_FTM_IN_TEST_MODE() )
  {
    if(rfm_get_calibration_state() == FALSE)
    {
      // FTM RF mode
      rf_tdscdma_mc_update_rx_swpt(device,
                                   band,
                                   RFA_TDSCDMA_RXLM_MODE_TRK,
                                   RF_TDSCDMA_MDSP_RX_BUF_TYPE,
                                   buf_id);
    }
  }
  else
  {
    rf_tdscdma_mc_update_rx_swpt(device,
                                 band,
                                 rxlm_mode,
                                 RF_TDSCDMA_MDSP_RX_BUF_TYPE,
                                 buf_id);
  }

  if (rf_tdscdma_mc_wakeup_time_debug || rf_tdscdma_mc_tx_time_debug)
  {
    update_rx_mdsp_time = rf_time_get_elapsed(update_rx_mdsp_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: update_rx_mdsp: update_rx_swpt takes %d usec", update_rx_mdsp_time);
    update_rx_mdsp_start_t = rf_time_get_tick();
  }

  /* Rx Freq Comp update													 */
  /*=========================================================================*/

  if ( !rfm_get_calibration_state() )
  {
    /* Calc rx gain for the freq and write to mdsp shared memory */
    rf_tdscdma_core_rxctl_update_freq_comp(device,
                                           path,
                                           band,
                                           rx_channel,
                                           (int16)RF_TDSCDMA_MDSP_RX_BUF_TYPE,
                                           buf_id);
  }

  if (rf_tdscdma_mc_wakeup_time_debug || rf_tdscdma_mc_tx_time_debug)
  {
    update_rx_mdsp_time = rf_time_get_elapsed(update_rx_mdsp_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: update_rx_mdsp: update_freq_comp takes %d usec", update_rx_mdsp_time);
    update_rx_mdsp_start_t = rf_time_get_tick();
  }

  /* FED RxAGC population */
  /*=========================================================================*/
  if ( !rfm_get_calibration_state() )
  {
    rf_tdscdma_mc_rflm_init_rxagc( (rflm_dm_handle_id_t)rxlm_buffer_idx);
    rf_tdscdma_mc_update_rflm_rxagc(device,
                                    path,
                                    band,
                                    rx_channel,
                                    rxlm_buffer_idx,
                                    rxlm_mode,
                                    FALSE, // is_ifreq
                                    ifreq_idx,
                                    buf_id,
                                    FALSE  // is_irat
                                   );
  
  
    if (rf_tdscdma_mc_wakeup_time_debug || rf_tdscdma_mc_tx_time_debug)
    {
      update_rx_mdsp_time = rf_time_get_elapsed(update_rx_mdsp_start_t, RF_USEC);
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: update_rx_mdsp: rxagc_update takes %d usec", update_rx_mdsp_time);
      update_rx_mdsp_start_t = rf_time_get_tick();
    }    
  }




  /* RF scripts update  													 */
  /*========================================================================= */
  /*     1. Rx Wakeup script                                               */
  /*     2. Rx Bursts script                                                  */
  /*     3. Rx LNA script                                                     */

  if (path == RF_PATH_0)
  {
    boolean script_result;

    if( (call_for_bho_update == TRUE) ||
        ((rfm_get_calibration_state() == TRUE) && (rf_tdscdma_mc_state.device_wakeup_script_built_flag == TRUE)) )
    {
      MSG_2(MSG_SSID_RF_DEBUG, MSG_LVL_HIGH, 
            "Skipped Rx wakeup script re-building for BHO update %d or Cal mode %d", 
            call_for_bho_update,
            rfm_get_calibration_state() );
    }
    else
    {
      /* 1. Rx Wakeup script */
      rf_tds_build_event_data.pri_device = rf_tdscdma_mc_state.curr_pri_device;
      rf_tds_build_event_data.sec_device = rf_tdscdma_mc_state.curr_sec_device;
      rf_tds_build_event_data.tx_device  = rf_tdscdma_mc_state.curr_pri_device;
      rf_tds_build_event_data.band = band;
      rf_tds_build_event_data.channel = rx_channel;
      rf_tds_build_event_data.is_bho_update = FALSE;

      if(rfm_get_calibration_state() == FALSE)
      {
        // Force to re-build rxWakeup Script with RxD ON in RFcal mode
        if ((rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
            (rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_DIV)   )
        {
          rf_tds_build_event_data.drx_enable = TRUE;
        }
        else
        {
          rf_tds_build_event_data.drx_enable = FALSE;
        }
      }
      else
      {
        // Always enable diversity in FTM RFcal mode
        rf_tds_build_event_data.drx_enable = TRUE;
      }
      rf_tds_build_event_data.buf_id = buf_id;

      script_result = rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_BUILD_WAKEUP,
                                                            &rf_tds_build_event_data);
      if (script_result)
      {
        // Set the flag to indicate the script is ready for execution
        rf_tdscdma_mc_state.device_wakeup_script_built_flag = TRUE;

        MSG_6(MSG_SSID_RF, MSG_LVL_MED,
              "Rx wakeup script built for pri_device %d, sec_device %d, Band %d, Chan %d, Div_EN %d, is_BHO %d",
              rf_tds_build_event_data.pri_device,
              rf_tds_build_event_data.sec_device,
              rf_tds_build_event_data.band,
              rf_tds_build_event_data.channel,
              rf_tds_build_event_data.drx_enable,
              rf_tds_build_event_data.is_bho_update);
      }
      else
      {
        MSG_6(MSG_SSID_RF, MSG_LVL_ERROR,
              "Rx wakeup script built ERROR for pri_device %d, sec_device %d, Band %d, Chan %d, Div_EN %d, is_BHO %d",
              rf_tds_build_event_data.pri_device,
              rf_tds_build_event_data.sec_device,
              rf_tds_build_event_data.band,
              rf_tds_build_event_data.channel,
              rf_tds_build_event_data.drx_enable,
              rf_tds_build_event_data.is_bho_update);
      }

      if (rf_tdscdma_mc_wakeup_time_debug || rf_tdscdma_mc_tx_time_debug)
      {
        update_rx_mdsp_time = rf_time_get_elapsed(update_rx_mdsp_start_t, RF_USEC);
        MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, 
              "Debug Wakeup Timeline: update_rx_mdsp: build device_rx_wakeup_script takes %d usec", 
              update_rx_mdsp_time);
        update_rx_mdsp_start_t = rf_time_get_tick();
      }
    }

    if(rfdevice_tdscdma_asm_set_rx_switch_state(rf_tdscdma_mc_state.curr_pri_device, band, rf_tdscdma_mc_state.port_state))
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, 
            "Update RX mdsp: switch state update successful for state: %d", 
            rf_tdscdma_mc_state.port_state);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
            "Update RX mdsp: switch state update failed for state: %d", 
            rf_tdscdma_mc_state.port_state);
    }
    
    
    /* 2. Rx Bursts script */
    /* Build Rx burst scripts for other conditions */
    rf_tds_build_event_data.pri_device = rf_tdscdma_mc_state.curr_pri_device;
    rf_tds_build_event_data.sec_device = rf_tdscdma_mc_state.curr_sec_device;
    rf_tds_build_event_data.tx_device = rf_tdscdma_mc_state.curr_pri_device;
    rf_tds_build_event_data.band = band;
    rf_tds_build_event_data.channel = rx_channel;
    /* Build PRx only script */
    rf_tds_build_event_data.drx_enable = FALSE;
    rf_tds_build_event_data.buf_id = buf_id;
    rf_tds_build_event_data.is_bho_update = FALSE;

    /* Set the flag to indicate scpt is building */
    ppBufIdx=rflmTdsRfModeCtlState.ppInfo.prepBufIdx[buf_id];
    rflmTdsRfModeCtlState.ppInfo.rxOnlyScptBuilding[buf_id][ppBufIdx] = TRUE;

    rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_OFF_TO_RX, &rf_tds_build_event_data);
    rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_RX_TO_OFF, &rf_tds_build_event_data);

    if ((rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
        (rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_DIV)   )
    {
      /* Build PRx+DRx script */
      rf_tds_build_event_data.drx_enable = TRUE;
      rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_OFF_TO_RX, &rf_tds_build_event_data);
      rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_RX_TO_OFF, &rf_tds_build_event_data);
    }

    if (rf_tdscdma_mc_wakeup_time_debug || rf_tdscdma_mc_tx_time_debug)
    {
      update_rx_mdsp_time = rf_time_get_elapsed(update_rx_mdsp_start_t, RF_USEC);
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, 
            "Debug Wakeup Timeline: update_rx_mdsp: build rx_burst_script takes %d usec", 
            update_rx_mdsp_time);
      update_rx_mdsp_start_t = rf_time_get_tick();
    }

    /* 3. Rx LNA script */
    if((call_for_bho_update == TRUE) && (rf_tdscdma_mc_state.rx_lna_script_built_on_bands[band] == TRUE))  
    {
      // Skip RxLNA script update if it is for BHO update AND RxLNA has been built on this band
      MSG_4(MSG_SSID_RF_DEBUG, MSG_LVL_HIGH, 
            "Skipped RxLNA script rebuilding for BHO update %d and this band %d has been built. Record [B34, B39]={%d, %d}", 
            call_for_bho_update, 
            band, 
            rf_tdscdma_mc_state.rx_lna_script_built_on_bands[RFCOM_BAND_TDSCDMA_B34],
            rf_tdscdma_mc_state.rx_lna_script_built_on_bands[RFCOM_BAND_TDSCDMA_B39]);
    }
    else if((rfm_get_calibration_state() == TRUE) && (rf_tdscdma_mc_state.rx_lna_script_built_on_bands[band] == TRUE))   
    {
      // Skip RxLNA script update if it is for Cal state AND RxLNA has been built on this band
      MSG_4(MSG_SSID_RF_DEBUG, MSG_LVL_HIGH, 
            "Skipped RxLNA script rebuilding for Cal state %d and this band %d has been built. Record [B34, B39]={%d, %d}", 
            rfm_get_calibration_state(), 
            band, 
            rf_tdscdma_mc_state.rx_lna_script_built_on_bands[RFCOM_BAND_TDSCDMA_B34],
            rf_tdscdma_mc_state.rx_lna_script_built_on_bands[RFCOM_BAND_TDSCDMA_B39]);
    }
    else    
    {
      uint8 loop_band;
      
      /* Build Rx LNA scripts for primary chain */
      rf_tds_build_event_data.pri_device = rf_tdscdma_mc_state.curr_pri_device;
      rf_tds_build_event_data.sec_device = rf_tdscdma_mc_state.curr_sec_device;
      rf_tds_build_event_data.tx_device = rf_tdscdma_mc_state.curr_pri_device;
      rf_tds_build_event_data.drx_enable = FALSE;
      rf_tds_build_event_data.is_irat = FALSE;
      rf_tds_build_event_data.rx_lna_update_type = RF_TDSCDMA_MDSP_RX_LNA_UPDATE_FOR_SCELL;
      rf_tds_build_event_data.is_bho_update = call_for_bho_update;

      // Invalid all bands and set the desired bands for Rx LNA script update
      for(loop_band = (uint8)RFCOM_BAND_TDSCDMA_B34; loop_band < (uint8) RFCOM_NUM_TDSCDMA_BANDS; loop_band++)
      {
        rf_tds_build_event_data.to_build_rx_lna_script_on_bands[loop_band]=FALSE;
      }

      if (band >= RFCOM_NUM_TDSCDMA_BANDS)
      {
        ERR_FATAL("Wrong band %d", band, 0, 0);
      }
      rf_tds_build_event_data.to_build_rx_lna_script_on_bands[band] = TRUE;
      
      rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_BUILD_RX_LNA, &rf_tds_build_event_data);

      if ((rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
          (rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_DIV)   )
      {
        /* Build Rx LNA scripts for secondary chain */
        rf_tds_build_event_data.drx_enable = TRUE;
        rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_BUILD_RX_LNA, &rf_tds_build_event_data);
      }

      rf_tdscdma_mc_state.last_pri_device_used_to_build_rx_lna_scpt = rf_tdscdma_mc_state.curr_pri_device;

      MSG_5(MSG_SSID_RF, MSG_LVL_HIGH, 
            "Built RxLNA script for BHO update %d and this band %d on pri_device %d. Record [B34, B39]={%d, %d}", 
            call_for_bho_update, 
            band, 
            rf_tdscdma_mc_state.curr_pri_device,
            rf_tdscdma_mc_state.rx_lna_script_built_on_bands[RFCOM_BAND_TDSCDMA_B34],
            rf_tdscdma_mc_state.rx_lna_script_built_on_bands[RFCOM_BAND_TDSCDMA_B39]);
    }

    if (rf_tdscdma_mc_wakeup_time_debug || rf_tdscdma_mc_tx_time_debug)
    {
      update_rx_mdsp_time = rf_time_get_elapsed(update_rx_mdsp_start_t, RF_USEC);
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: update_rx_mdsp: build rx_LNA_script takes %d usec", update_rx_mdsp_time);
      update_rx_mdsp_start_t = rf_time_get_tick();
    }
        
    /* Clear the flag to indicate scpt building is done */
    rflmTdsRfModeCtlState.ppInfo.rxOnlyScptBuilding[buf_id][ppBufIdx] = FALSE;
  }


  if (rf_tdscdma_mc_wakeup_time_debug || rf_tdscdma_mc_tx_time_debug)
  {
    update_rx_mdsp_time = rf_time_get_elapsed(update_rx_mdsp_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: update_rx_mdsp: build_rxagc_script takes %d usec", update_rx_mdsp_time);
    update_rx_mdsp_start_t = rf_time_get_tick();
  }


  MSG_7(MSG_SSID_RF, MSG_LVL_HIGH, "rf_tdscdma_mc_update_rx_mdsp(device %d, path %d, band %d, rx_channel %d, rxlm_buffer_idx %d, rxlm_mode %d, call_in_wakup) is complete", \
        device, path, band, rx_channel, rxlm_buffer_idx, rxlm_mode, call_in_wakeup);

  if(rf_tdscdma_mc_display_time_profile || rf_tdscdma_mc_tx_time_debug || rf_tdscdma_mc_wakeup_time_debug)
  {
    overall_update_rx_mdsp_time = rf_time_get_elapsed(overall_update_rx_mdsp_start_t, RF_USEC);
    MSG_3(MSG_SSID_RF, MSG_LEGACY_MED,
          "Debug Wakeup Timeline: update_rx_mdsp time profile: subtotal %d usec (in_wakeup=%d, path=%d)",
          overall_update_rx_mdsp_time, call_in_wakeup, path);
  }
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Confiure RF/HW for Rx Chain. Need to call on each chain.

  @details
  Confiure RF/HW, for Rx Chain, including:
	- Program WTR Tx Common (temporary work-around for single-PLL chip)
	- Program WTR Tx BSP&BW (temporary work-around for single-PLL chip)
	- Program WTR Rx Common
	- Program LNA on primary path only
	  + Program WTR Primary Rx LNA = G0
	  + Program WTR Secondary Rx LNA = G0 (if diversity is enabled)
	- Program WTR Rx BSP
	- Program Tuner for Rx init and Rx mode
	- Program Rx control signals
	  + If rxlm_mode = ACQ/PWR_SCAN,
		* Clean Tx control signals
		* Set Rx control signals (including ASM, GRFCs and RX_ON)
	  + If rxlm_mode = other modes,
		* Clean Tx control signals
		* Clean Rx control signals (including ASM, GRFCs and RX_ON)

  Need to call on each chain.

*/
void rf_tdscdma_mc_config_rx_chain
(
  rfm_device_enum_type device,
  rf_path_enum_type path,
  rfcom_tdscdma_band_type band,
  uint16 rx_channel,
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  rfa_tdscdma_rx_path_t   rx_path,
#endif
  lm_handle_type rxlm_buffer_idx,
  rfa_tdscdma_rxlm_mode_t rxlm_mode,
  boolean call_in_wakeup,
  uint8 buf_id,
  uint8 rx_lin_state,
  boolean use_sw_tune_in_acq
)
{
  boolean asm_status = TRUE;
  boolean sw_set_rx_ant_sel = FALSE, sw_clean_rx_ant_sel = FALSE;
  rfc_tdscdma_grfc_notime_control_type tds_grfc_notime_control_data;
  boolean program_wtr_tx_during_enable_rx = FALSE;
  rflm_err_t exec_result;
  rflm_tds_schedule_rf_internal_event_in_t in_data;
  rf_time_tick_type config_rx_chain_start_t = {0}, overall_config_rx_chain_start_t = {0};
  rf_time_type config_rx_chain_time, overall_config_rx_chain_time;
#ifdef RF_TDSCDMA_USE_DEVICE_IMMEDIATE_WRITE
  rfdevice_tdscdma_set_lna_range_type set_lna_range_data;
#endif

  if(rf_tdscdma_mc_display_time_profile || rf_tdscdma_mc_wakeup_time_debug)
  {
    overall_config_rx_chain_start_t = rf_time_get_tick(); /* time spent in API */
  }

  if(rf_tdscdma_mc_wakeup_time_debug)
  {
    config_rx_chain_start_t = rf_time_get_tick();
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: config_rx_chain: call on Path %d", path);
  }


#ifdef RF_TDSCDMA_USE_DEVICE_IMMEDIATE_WRITE
  /* Program device HW using RF CMD interface: Legecy way */

  /* WTR Rx SSBI															 */
  /*=========================================================================*/
  /* Sequence:
     1. rx_enter_mode() on PATH_0
     2. rx_enter_mode() on PATH_1 (if diversity is enabled)

     3. set_rx_gain()   on PATH_0
     4. set_rx_gain()   on PATH_1 (if diversity is enabled)
     Note: Step 3 and 4 are not necessary for WTR3925
     as it has been included in tunet_to_chan

     5. rx_tune_to_chan() on PATH_0
      * rx_tune_to_chan() on PATH_0 will set LNA G0 for both chains
        (if diversity is enabled)
     6. rx_tune_to_chan() on PATH_1 (if diversity is enabled)  		   */

  if (path == RF_PATH_0)
  {
    rfdevice_tdscdma_rx_enter_mode(rf_tdscdma_mc_state.curr_pri_device,
                                   RFCOM_TDSCDMA_MODE,
                                   band,
                                   RFDEVICE_EXECUTE_IMMEDIATE,
                                   NULL);

    if ((rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
        (rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_DIV)   )
    {
      rfdevice_tdscdma_rx_enter_mode(rf_tdscdma_mc_state.curr_sec_device,
                                     RFCOM_TDSCDMA_MODE,
                                     band,
                                     RFDEVICE_EXECUTE_IMMEDIATE,
                                     NULL);
    }

    // Note: Move the LNA programing to after PLL programing. Might be an issue for legacy WTRs.
    rfdevice_tdscdma_rx_tune_chan(rf_tdscdma_mc_state.curr_pri_device,
                                  RFCOM_TDSCDMA_MODE,
                                  band,
                                  rx_channel,
                                  RFDEVICE_EXECUTE_IMMEDIATE,
                                  NULL,
                                  rx_lin_state);

    if ((rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
        (rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_DIV)   )
    {
      rfdevice_tdscdma_rx_tune_chan(rf_tdscdma_mc_state.curr_sec_device,
                                    RFCOM_TDSCDMA_MODE,
                                    band,
                                    rx_channel,
                                    RFDEVICE_EXECUTE_IMMEDIATE,
                                    NULL,
                                    rx_lin_state);
    }

    /* Set default LNA state to G0 (do both chains if needed once on primary path) */
    /* Direct programing G0 to HW after PLL start tuning */
    set_lna_range_data.band = band;
    set_lna_range_data.lna_range = 0;
    set_lna_range_data.program_pri_chain = TRUE;
    set_lna_range_data.program_sec_chain = FALSE;
    set_lna_range_data.pri_chain_action = RFDEVICE_EXECUTE_IMMEDIATE;
    //TODO hard code to 0
    set_lna_range_data.lin_state = rx_lin_state;

    rfdevice_tdscdma_rx_cmd_dispatch(rf_tdscdma_mc_state.curr_pri_device,
                                     RFCOM_TDSCDMA_MODE,
                                     band,
                                     RFDEVICE_TDSCDMA_SET_RX_GAIN_STATE,
                                     (uint8 *)&set_lna_range_data);

    if ((rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
        (rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_DIV)   )
    {
      /* Program DRx LNA (together with PRx LNA) when diversity is enabled */
      set_lna_range_data.program_pri_chain = FALSE;
      set_lna_range_data.program_sec_chain = TRUE;
      set_lna_range_data.sec_chain_action = RFDEVICE_EXECUTE_IMMEDIATE;
      rfdevice_tdscdma_rx_cmd_dispatch(rf_tdscdma_mc_state.curr_sec_device,
                                       RFCOM_TDSCDMA_MODE,
                                       band,
                                       RFDEVICE_TDSCDMA_SET_RX_GAIN_STATE,
                                       (uint8 *)&set_lna_range_data);
    }
  } /* if (path == RF_PATH_0): Program Device HW once on PATH_0 */
#else
  /* Program device HW using FED interface: for wakeup timeline optimization */

  if (path == RF_PATH_0)
  {
    /* Execute Wakeup script only if the script was built successfully */
    if (rf_tdscdma_mc_state.device_wakeup_script_built_flag == TRUE)
    {
      /* Call FED to execute only once every enter_mode/exit_mode OR sleep/wakeup cycle */
      in_data.effectTime = 0xFFFFFFFF; // Immediate execution
      in_data.scriptType = RFLM_TDS_RX_WAKEUP_SCRIPT;
      in_data.actionCmd  = RFLM_TDS_EXECUTE_SCRIPT;
      in_data.scriptInfo.rxWakeupScript.callInWakeup = call_in_wakeup;

      in_data.scriptInfo.rxBurstScript.linState = 0; //Hard code to default
      // Always build and exec with diversity enabled
      if ((rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
          (rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_DIV)   )
      {
        in_data.scriptInfo.rxWakeupScript.divEn = TRUE;
      }
      else
      {
        in_data.scriptInfo.rxWakeupScript.divEn = FALSE;
      }

      exec_result = rflm_tds_schedule_rf_internal_event(&in_data);

      if (exec_result == RFLM_ERR_NONE)
      {
        // Set the flag to avoid executing script again
        // during the same enter_mode/exit_mode OR sleep/wakeup cycle
        rf_tdscdma_mc_state.device_wakeup_script_exec_flag = TRUE;

        MSG_6(MSG_SSID_RF, MSG_LVL_MED,
              "Rx wakeup script executed for pri_device: %d, sec_device: %d, "
              "Band: %d, Chan: %d, Div_EN: %d, in_Sleep: %d",
              rf_tdscdma_mc_state.curr_pri_device,
              rf_tdscdma_mc_state.curr_sec_device,
              band,
              rx_channel,
              in_data.scriptInfo.rxWakeupScript.divEn,
              call_in_wakeup);
      }
      else
      {
        MSG_7(MSG_SSID_RF, MSG_LVL_ERROR,
              "Rx wakeup script executed ERROR (code %d) for pri_device: %d, sec_device: %d, "
              "Band: %d, Chan: %d, Div_EN: %d, in_Sleep: %d",
              exec_result,
              rf_tdscdma_mc_state.curr_pri_device,
              rf_tdscdma_mc_state.curr_sec_device,
              band,
              rx_channel,
              in_data.scriptInfo.rxWakeupScript.divEn,
              call_in_wakeup);
      }
    }
    else
    {
      // Wakeup script built ERROR, skip execution
      MSG_1(MSG_SSID_RF, MSG_LVL_ERROR,
            "Skipped Rx wakeup script execution because script building FAILED", 0);
    } /* rf_tdscdma_mc_state.device_wakeup_script_built_flag == TRUE */

    if (rf_tdscdma_mc_wakeup_time_debug)
    {
      config_rx_chain_time = rf_time_get_elapsed(config_rx_chain_start_t, RF_USEC);
      MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,
            "Debug Wakeup Timeline: config_rx_chain: exec_device_rx_wakeup_script Enqueue/Exec takes %d usec",
            config_rx_chain_time);
      config_rx_chain_start_t = rf_time_get_tick();
    }

    /* Skip Rx burst and LNA script on RUMI*/
    if (rf_tdscdma_use_on_rumi == FALSE )
    {
      /* Execute Rx burst script */
      if(IS_FTM_IN_TEST_MODE() && !rfm_get_calibration_state())
      {
        MSG(MSG_SSID_RF, MSG_LEGACY_MED, "Skip Rx tuning in FTM mode");      
      }
      else
      {
        if (!call_in_wakeup)
        {
          /* Execute Rx burst and LNA script if NOT in wakeup */
          in_data.effectTime = 0xFFFFFFFF; // Immediate execution
          in_data.scriptType = RFLM_TDS_RX_BURST_SCRIPT;
          in_data.actionCmd  = RFLM_TDS_EXECUTE_SCRIPT;
          in_data.scriptInfo.rxBurstScript.bufId = buf_id;
          in_data.scriptInfo.rxBurstScript.gainState[0] = 0; // Hard-coded G0
          in_data.scriptInfo.rxBurstScript.gainState[1] = 0; // Hard-coded G0
          in_data.scriptInfo.rxBurstScript.linState = 0; //Hard code to default
          in_data.scriptInfo.rxBurstScript.rxLmHandle[0] = rf_tdscdma_mc_lm_bufs.rx_buf_wrk[0].buf_idx;
          in_data.scriptInfo.rxBurstScript.rxLmHandle[1] = rf_tdscdma_mc_lm_bufs.rx_buf_wrk[1].buf_idx;
          if ((rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
              (rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_DIV)   )
          {
            /* Execute PRx+DRx script */
            in_data.scriptInfo.rxBurstScript.divEn = TRUE;
          }
          else
          {
            /* Execute PRx Only script */
            in_data.scriptInfo.rxBurstScript.divEn = FALSE;
          }

          exec_result = rflm_tds_schedule_rf_internal_event(&in_data);

          if (exec_result == RFLM_ERR_NONE)
          {
            MSG_5(MSG_SSID_RF_DEBUG, MSG_LVL_MED,
                  "Rx burst script executed for Band: %d, Chan: %d, buf_id: %d, div_en: %d, in_wakeup: %d",
                  band, rx_channel, buf_id, in_data.scriptInfo.rxBurstScript.divEn, call_in_wakeup);
          }
          else
          {
            MSG_6(MSG_SSID_RF, MSG_LVL_ERROR,
                  "Primary Chain LNA script executed ERROR (code %d) for Band: %d, Chan: %d, buf_id: %d, div_en: %d, in_wakeup: %d",
                  exec_result, band, rx_channel, buf_id, in_data.scriptInfo.rxBurstScript.divEn, call_in_wakeup);
          } /* Check script execution result */

          if (rf_tdscdma_mc_wakeup_time_debug)
          {
            config_rx_chain_time = rf_time_get_elapsed(config_rx_chain_start_t, RF_USEC);
            MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,
                  "Debug Wakeup Timeline: config_rx_chain: rx_burst_script Enqueue/Exec takes %d usec",
                  config_rx_chain_time);
            config_rx_chain_start_t = rf_time_get_tick();
          }
        }
        else
        {
          /* Skip execution of tune_to_chan and LNA when in WAKEUP */
          MSG_1(MSG_SSID_RF, MSG_LVL_MED, "Skipped exec tune_to_chan and LNA when in wakeup", 0);
        } /* if (!call_in_wakeup) */
      }
    } /* if (rf_tdscdma_use_on_rumi == FALSE ) */
  } /* if (path==RF_PATH_0): Program device HW only on path_0
       due to device programing sequence requirement */
#endif /* #ifdef RF_TDSCDMA_USE_DEVICE_IMMEDIATE_WRITE */


  /* Configure Rx tuner 													 */
  /*=========================================================================*/
#ifdef FEATURE_RF_HAS_QTUNER
  (void)rf_tdscdma_core_antenna_tuner_init(device, band, RFDEVICE_EXECUTE_IMMEDIATE, NULL);

  /*Program QFE for Rx mode*/
  (void)rf_tdscdma_core_antenna_tuner_program_rx_mode(device,
                                                      band,
                                                      rx_channel,
                                                      RFDEVICE_EXECUTE_IMMEDIATE,
                                                      NULL,
                                                      0);
  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    config_rx_chain_time = rf_time_get_elapsed(config_rx_chain_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: config_rx_chain: tuner_config takes %d usec", config_rx_chain_time);
    config_rx_chain_start_t = rf_time_get_tick();
  }
#endif /* FEATURE_RF_HAS_QTUNER */

  if (rf_tdscdma_enable_sw_tune)
  {
    /* If this flag is set, RFSW should set rx ant_sel regardless RxLM modes
       This is short term hack for bring-up */
    sw_set_rx_ant_sel = TRUE;
    sw_clean_rx_ant_sel = FALSE;
  }
  else /* !rf_tdscdma_enable_sw_tune */
  {
    /* This is the normal operation */
    if (call_in_wakeup)
    {
      /* if called in wakeup, RFSW doesn't need to set nor clean Rx ant_sel
         (instead FW/FED will do) */
      sw_set_rx_ant_sel = FALSE;
      sw_clean_rx_ant_sel = FALSE;
    }
    else /* !call_in_wakeup*/
    {
      /* If not called in wakeup, it is called from exable_rx() */
    } /* if (call_in_wakeup) */
  } /* rf_tdscdma_enable_sw_tune */

  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    MSG_5(MSG_SSID_RF, MSG_LVL_HIGH,
          "Debug: (sw_tune_rx_ant_sel = %d, sw_clean_rx_ant_sel = %d) for RxLM mode %d, "
          "calibration_state %d and call_in_wakeup %d",
          sw_set_rx_ant_sel, sw_clean_rx_ant_sel, rxlm_mode,
          rfm_get_calibration_state(), call_in_wakeup);
  }

  asm_status = rfdevice_tdscdma_asm_wakeup_rx(device,
                                              band,
                                              NULL,
                                              RFDEVICE_EXECUTE_IMMEDIATE,
                                              RFCOM_INVALID_TIMING_OFFSET);

  /* Configure Rx control signals   										 */
  /*=========================================================================*/

  if ( sw_set_rx_ant_sel )
  {
    /* Set Rx (and clean Tx ) control signals when in ACQ/PWR_SCAN mode and in RFcal mode */
    if (rfdevice_tdscdma_use_asm(device, band))
    {
      asm_status = rfdevice_tdscdma_asm_enable_rx(device,
                                                  band,
                                                  NULL,
                                                  RFDEVICE_EXECUTE_IMMEDIATE,
                                                  RFCOM_INVALID_TIMING_OFFSET,
                                                  TRUE,
                                                  NULL,
                                                  TDS_ASM_INCLUDE_ALL);

      if (!asm_status)
      {
        MSG_3(MSG_SSID_RF, MSG_LVL_ERROR,
              "ASM configuration ERROR on Device %d (path %d) and Band %d",
              device, path, band);
      }
    }
    else
    {
      MSG_3(MSG_SSID_RF_DEBUG, MSG_LVL_MED,
            "ASM not presented on Device %d (path %d) and Band %d",
            device, path, band);
    }

    tds_grfc_notime_control_data.band = band;
    tds_grfc_notime_control_data.sig_type = RFC_SIG_TYPE_INVALID; // To configure all
    tds_grfc_notime_control_data.override_logic = RFC_LOGIC_INVALID; // No override logic, use RFC AG logic
    if ( path == RF_PATH_0 )
    {
      /* Clean Tx once only on primary path */
      (void)rfc_tdscdma_command_dispatch(device,
                                         RFC_TDSCDMA_CLEAN_TX_GRFC_NO_TIME,
                                         (void*)(&tds_grfc_notime_control_data));

      /* Turn ON RX_ON once only on primary path */
      //rf_tdscdma_msm_set_rx_on(device, band, TRUE);
    }
    (void)rfc_tdscdma_command_dispatch(device,
                                       RFC_TDSCDMA_SET_RX_GRFC_NO_TIME,
                                       (void*)(&tds_grfc_notime_control_data));
  }

  if (sw_clean_rx_ant_sel)
  {
    /* Clean Tx&Rx control signals when in TRK/TRK_IDLE */
    tds_grfc_notime_control_data.band = band;
    tds_grfc_notime_control_data.sig_type = RFC_SIG_TYPE_INVALID; // To configure all
    tds_grfc_notime_control_data.override_logic = RFC_LOGIC_INVALID; // No override logic, use RFC AG logic
    if ( path == RF_PATH_0 )
    {
      /* Clean Tx once only on primary path */
      (void)rfc_tdscdma_command_dispatch(device,
                                         RFC_TDSCDMA_CLEAN_TX_GRFC_NO_TIME,
                                         (void*)(&tds_grfc_notime_control_data));

      /* Turn OFF RX_ON only once on primary path */
      //rf_tdscdma_msm_set_rx_on(device, band, FALSE);
    }
    (void)rfc_tdscdma_command_dispatch(device,
                                       RFC_TDSCDMA_CLEAN_RX_GRFC_NO_TIME,
                                       (void*)(&tds_grfc_notime_control_data));
  } /* if (sw_clean_rx_ant_sel) */

  if (rf_tdscdma_mc_wakeup_time_debug)
  {
    config_rx_chain_time = rf_time_get_elapsed(config_rx_chain_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Wakeup Timeline: config_rx_chain: RF signal config takes %d usec", config_rx_chain_time);
    config_rx_chain_start_t = rf_time_get_tick();
  }

  MSG_8(MSG_SSID_RF, MSG_LVL_HIGH, 
        "rf_tdscdma_mc_config_rx_chain(device %d, path %d, band %d, rx_channel %d, rxlm_buffer_idx %d, rxlm_mode %d, call_in_wakeup=%d, sw_tune_in_acq=%d) is complete", \
        device, path, band, rx_channel, rxlm_buffer_idx, rxlm_mode, call_in_wakeup, use_sw_tune_in_acq);

  if(rf_tdscdma_mc_display_time_profile || rf_tdscdma_mc_wakeup_time_debug)
  {
    overall_config_rx_chain_time = rf_time_get_elapsed(overall_config_rx_chain_start_t, RF_USEC);
    MSG_3(MSG_SSID_RF, MSG_LEGACY_MED,
          "Debug Wakeup Timeline: config_rx_chain time profile: subtotal %d usec (in_wakeup=%d, path=%d)",
          overall_config_rx_chain_time, call_in_wakeup, path);
  }
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update MDSP content for Tx Chain.

  @details
  Update MDSP content, for Tx Chain, including



*/
void rf_tdscdma_mc_update_tx_mdsp
(
  rfm_device_enum_type device,
  rf_path_enum_type path,
  rfcom_tdscdma_band_type band,
  uint16 tx_channel,
  lm_handle_type txlm_buffer_idx,
  uint8 buf_id,
  boolean is_bho_update
)
{
  rf_tdscdma_core_temp_comp_value_type therm_data;
  rftdscdma_mdsp_common_event_data_type rf_tds_build_event_data;
  rf_time_tick_type update_tx_mdsp_start_t = {0};
  rf_time_tick_type overall_update_tx_mdsp_start_t = {0};
  rf_time_type update_tx_mdsp_time, overall_update_tx_mdsp_time;
  int32 mtpl_dBm10, coex_mtpl_dBm10, minpwr_dBm10;
  int32 iqgain[3]; /* IQ gain values for APT, EPT & ET modes */
  int32 envScale;
  uint8 ppBufIdx;
  boolean script_result;

  if(rf_tdscdma_mc_display_time_profile || rf_tdscdma_mc_tx_time_debug)
  {
    update_tx_mdsp_start_t = rf_time_get_tick(); /* time spent in API */
    overall_update_tx_mdsp_start_t = rf_time_get_tick(); /* time spent in API */
  }

  /* TxLM Buffer update 													 */
  /*=========================================================================*/

  /* update TxLM setting and write to mdsp shared memory */
  rf_tdscdma_msm_update_txlm_buffer(txlm_buffer_idx, band);

  if(rfdevice_tdscdma_asm_set_tx_switch_state(device,band,rf_tdscdma_mc_state.port_state))
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Update TX mdsp: switch state update successful for state: %d", rf_tdscdma_mc_state.port_state);
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Update TX mdsp: switch state update failed for state: %d", rf_tdscdma_mc_state.port_state);
  }

  /* Tx Wakeup script */
  rf_tds_build_event_data.pri_device = rf_tdscdma_mc_state.curr_pri_device;
  rf_tds_build_event_data.sec_device = rf_tdscdma_mc_state.curr_sec_device;
  rf_tds_build_event_data.tx_device  = device;
  rf_tds_build_event_data.band = band;
  rf_tds_build_event_data.channel = tx_channel;
  rf_tds_build_event_data.is_bho_update = is_bho_update;
  rf_tds_build_event_data.buf_id = buf_id;

  script_result = rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_BUILD_TX_WAKEUP,
                                                        &rf_tds_build_event_data);
  if (script_result)
  {
    // Set the flag to indicate the script is ready for execution    

    MSG_5(MSG_SSID_RF, MSG_LVL_MED,
          "Tx wakeup script built for tx_device %d, Band %d, Chan %d, Div_EN %d, is_BHO %d",
          rf_tds_build_event_data.tx_device, 
          rf_tds_build_event_data.band,
          rf_tds_build_event_data.channel,
          rf_tds_build_event_data.drx_enable,
          rf_tds_build_event_data.is_bho_update);
  }
  else
  {
    MSG_5(MSG_SSID_RF, MSG_LVL_ERROR,
          "Tx wakeup script built ERROR for tx_device %d, Band %d, Chan %d, Div_EN %d, is_BHO %d",
          rf_tds_build_event_data.tx_device,
          rf_tds_build_event_data.band,
          rf_tds_build_event_data.channel,
          rf_tds_build_event_data.drx_enable,
          rf_tds_build_event_data.is_bho_update);
  }

  /* Build RF scripts (OffTx and TxOFF)  										 */
  /*=========================================================================*/
  /*  Need to use OffTx and TxOff scripts in CAL mode                        */

  rf_tds_build_event_data.pri_device = rf_tdscdma_mc_state.curr_pri_device;
  rf_tds_build_event_data.sec_device = rf_tdscdma_mc_state.curr_sec_device;
  rf_tds_build_event_data.tx_device = device;
  rf_tds_build_event_data.band = band;
  rf_tds_build_event_data.channel = tx_channel;
  rf_tds_build_event_data.drx_enable = FALSE;
  rf_tds_build_event_data.buf_id = buf_id;
  rf_tds_build_event_data.txlm_buffer_index = txlm_buffer_idx;
  rf_tds_build_event_data.is_bho_update = is_bho_update;

  /* Set the flag to indicate scpt is building */
  ppBufIdx=rflmTdsRfModeCtlState.ppInfo.prepBufIdx[buf_id];
  rflmTdsRfModeCtlState.ppInfo.rxTxScptBuilding[buf_id][ppBufIdx] = TRUE;

  rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_OFF_TO_TX, &rf_tds_build_event_data);
  rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_TX_TO_OFF, &rf_tds_build_event_data);

  /* Clear the flag to indicate scpt building is done */
  rflmTdsRfModeCtlState.ppInfo.rxTxScptBuilding[buf_id][ppBufIdx] = FALSE;

  if (rfm_get_calibration_state())
  {
    /* In Cal mode, Only need to populate TxLM setting and build OffTx and TxOff scripts */
    MSG_1(MSG_SSID_RF_DEBUG, MSG_LEGACY_MED, "Tx script building and TxAGC population has been skipped in calibration state", 0);
    return;
  }


  /* Build RF scripts (Tx related)  										 */
  /*=========================================================================*/
  /*  Skip script building if in calibration state  						 */

  rf_tds_build_event_data.pri_device = rf_tdscdma_mc_state.curr_pri_device;
  rf_tds_build_event_data.sec_device = rf_tdscdma_mc_state.curr_sec_device;
  rf_tds_build_event_data.tx_device = device;
  rf_tds_build_event_data.band = band;
  rf_tds_build_event_data.channel = tx_channel;
  rf_tds_build_event_data.drx_enable = FALSE;
  rf_tds_build_event_data.buf_id = buf_id;
  rf_tds_build_event_data.txlm_buffer_index = txlm_buffer_idx;
  rf_tds_build_event_data.is_bho_update = is_bho_update;

  /* Set the flag to indicate scpt is building */
  ppBufIdx=rflmTdsRfModeCtlState.ppInfo.prepBufIdx[buf_id];
  rflmTdsRfModeCtlState.ppInfo.rxTxScptBuilding[buf_id][ppBufIdx] = TRUE;

  /* OffTx and TxOff scripts have been build earlier (search above in this API; so skip from the next */
  rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_TX_TO_TX, &rf_tds_build_event_data);
  rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_TX_TO_RX, &rf_tds_build_event_data);
  rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_RX_TO_TX, &rf_tds_build_event_data);

#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  if ( (rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
       (rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_DIV) )
  {
    rf_tds_build_event_data.pri_device = rf_tdscdma_mc_state.curr_pri_device;
    rf_tds_build_event_data.sec_device = rf_tdscdma_mc_state.curr_sec_device;
    rf_tds_build_event_data.tx_device  = device;
    rf_tds_build_event_data.band = band;
    rf_tds_build_event_data.channel = tx_channel;
    rf_tds_build_event_data.drx_enable = TRUE;
    rf_tds_build_event_data.buf_id = buf_id;

    rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_TX_TO_RX, &rf_tds_build_event_data);
    rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_RX_TO_TX, &rf_tds_build_event_data);
  }
#endif /* RF_TDSCDMA_RX_DIVERSITY_ENABLE */

  /* Clear the flag to indicate scpt building is done */
  rflmTdsRfModeCtlState.ppInfo.rxTxScptBuilding[buf_id][ppBufIdx] = FALSE;

  if (rf_tdscdma_mc_tx_time_debug)
  {
    update_tx_mdsp_time = rf_time_get_elapsed(update_tx_mdsp_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Tx Timeline: update_tx_mdsp: script_building takes %d usec", update_tx_mdsp_time);
    update_tx_mdsp_start_t = rf_time_get_tick();
  }


  /* TxAGC and other MDSP info update   									 */
  /*=========================================================================*/
  /*  Skip TxAGC population if in calibration state 						 */

  /*Read the thermal for the very first time and update thermal value*/
  rf_tdscdma_mc_trigger_therm();
  
  /* Populate shared memory buffers when in on-line mode or in ftm bursty test mode */

  // Temporarily bypass TX LIN update for RUMI bring-up
  if( rf_tdscdma_use_on_rumi == FALSE )
  {
    /* Update the linearizer from multi-lin NV for the tx chan */
    /* txlin freq comp/temp comp is handled inside */
    if( !rf_tdscdma_core_txlin_update(rf_tdscdma_mc_state.curr_band_tx, tx_channel, buf_id, txlm_buffer_idx) )
    {
      MSG(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_enable_tx: error when update lin");
    }
  }
  else
  {
    MSG(MSG_SSID_RF, MSG_LVL_HIGH, "rf_tdscdma_mc_enable_tx: Skip TxLIN when on Rumi");
  }

  if (rf_tdscdma_mc_tx_time_debug)
  {
    update_tx_mdsp_time = rf_time_get_elapsed(update_tx_mdsp_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Tx Timeline: update_tx_mdsp: txlin_update takes %d usec", update_tx_mdsp_time);
    update_tx_mdsp_start_t = rf_time_get_tick();
  }

  /* Freq comp for txplim(internal param update to f1. no write to mdsp shared memory) */
  rf_tdscdma_core_txplim_update_lim_vs_freq(rf_tdscdma_mc_state.curr_band_tx,
                                            tx_channel,
                                            buf_id);

  if (rf_tdscdma_mc_tx_time_debug)
  {
    update_tx_mdsp_time = rf_time_get_elapsed(update_tx_mdsp_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Tx Timeline: update_tx_mdsp: txlim_vs_freq takes %d usec", update_tx_mdsp_time);
    update_tx_mdsp_start_t = rf_time_get_tick();
  }

  //temp comp for Lin is done in rf_tdscdma_core_txlin_update()
  //temp comp for LIM, use a separate API here
#if 1
  /* Get the stored therm raw value */
  therm_data = rf_tdscdma_core_temp_comp_get_therm_data();

  /* Update the desired tx pwr limit value */
  rf_tdscdma_core_temp_comp_update_txlim(therm_data.last_raw_temp_value);
#endif

  if (rf_tdscdma_mc_tx_time_debug)
  {
    update_tx_mdsp_time = rf_time_get_elapsed(update_tx_mdsp_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Tx Timeline: update_tx_mdsp: temp_comp takes %d usec", update_tx_mdsp_time);
    update_tx_mdsp_start_t = rf_time_get_tick();
  }


  /* Update TX data into RFLM */
  mtpl_dBm10 = (int32)(rf_tdscdma_core_txplim_get_desired_limit());
  coex_mtpl_dBm10 = (int32)(rf_tdscdma_core_txplim_get_desired_limit_coex());
  minpwr_dBm10 = RFLM_TDS_MIN_PWR_DB10;
  /* Unity IQ gain remains same for all modes (APT, EPT, ET)*/
  iqgain[0] = (int32)rf_tdscdma_core_xpt_get_cfg_param(band, EPT_CFG_CMN_IQ_GAIN_UNITY);
  iqgain[1] = (int32)rf_tdscdma_core_xpt_get_cfg_param(band, EPT_CFG_CMN_IQ_GAIN_UNITY);
  iqgain[2] = (int32)rf_tdscdma_core_xpt_get_cfg_param(band, EPT_CFG_CMN_IQ_GAIN_UNITY);
  envScale = (int32)rf_tdscdma_core_xpt_get_cfg_param(band, EPT_CFG_CMN_EVN_SCALE_UNITY);

  /* If either of the values is zero it implies that NV is not present.
     In this case, just use the default values for iqgain/envscale.
     */
  if (iqgain[0] == 0 || iqgain[1] == 0 || iqgain[2] == 0 || envScale == 0)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Unity IQGain and/or unity envScale values not present in NV! Using defaults", 0);
    rflm_tds_set_tx_data(mtpl_dBm10, coex_mtpl_dBm10, minpwr_dBm10, 
                         RFLM_TDS_DEFAULT_IQ_GAIN, 
                         RFLM_TDS_DEFAULT_IQ_GAIN, 
                         RFLM_TDS_DEFAULT_IQ_GAIN, 
                         RFLM_TDS_DEFAULT_ENV_SCALE);
  }
  else
  {
    rflm_tds_set_tx_data(mtpl_dBm10, coex_mtpl_dBm10, minpwr_dBm10, iqgain[0], iqgain[1], iqgain[2], envScale);
  }

  if (rf_tdscdma_mc_tx_time_debug)
  {
    update_tx_mdsp_time = rf_time_get_elapsed(update_tx_mdsp_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Tx Timeline: update_tx_mdsp: FED set_tx_data takes %d usec", update_tx_mdsp_time);
    update_tx_mdsp_start_t = rf_time_get_tick();
  }

  /* Update MPR into RFLM */
  rflm_tds_set_mpr( (int16*)rfnv_tdscdma_tx_tbl_ptr->mpr_backoff_val_v2 );

   /*Update mtpl as initial value*/
  rflm_tds_set_mtpl((int32)rf_tdscdma_core_txplim_get_desired_limit(),
                    (int32)rf_tdscdma_core_txplim_get_desired_limit_coex());

  if (rf_tdscdma_mc_tx_time_debug)
  {
    update_tx_mdsp_time = rf_time_get_elapsed(update_tx_mdsp_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Tx Timeline: update_tx_mdsp: FED set_mpr takes %d usec", update_tx_mdsp_time);
    update_tx_mdsp_start_t = rf_time_get_tick();
  }    

  if (rf_tdscdma_mc_tx_time_debug)
  {
    update_tx_mdsp_time = rf_time_get_elapsed(update_tx_mdsp_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Tx Timeline: update_tx_mdsp: mpr_mtpl_population takes %d usec", update_tx_mdsp_time);
    update_tx_mdsp_start_t = rf_time_get_tick();
  }

  MSG_6(MSG_SSID_RF, MSG_LVL_HIGH, "rf_tdscdma_mc_update_tx_mdsp(device %d, path %d, band %d, rx_channel %d, rxlm_buffer_idx %d, buf_id=%d) is complete", \
        device, path, band, tx_channel, txlm_buffer_idx, buf_id);

  if(rf_tdscdma_mc_display_time_profile || rf_tdscdma_mc_tx_time_debug)
  {
    overall_update_tx_mdsp_time = rf_time_get_elapsed(overall_update_tx_mdsp_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "update_tx_mdsp time profile: subtotal takes %d usec", overall_update_tx_mdsp_time);
  }
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Confiure RF/HW for Tx Chain.

  @details
  Confiure RF/HW, for Tx Chain.

  rf_tdscdma_mc_config_tx_chain() will be called after update_tx_mdsp(). 



*/
void rf_tdscdma_mc_config_tx_chain
(
  rfm_device_enum_type device,
  rf_path_enum_type path,
  rfcom_tdscdma_band_type band,
  uint16 tx_channel,
  lm_handle_type txlm_buffer_idx,
  uint8 buf_id
)
{  
  rflm_tds_schedule_rf_internal_event_in_t in_data;
  rflm_err_t exec_result;
    
  rf_time_tick_type config_tx_chain_start_t = {0};
  rf_time_tick_type overall_config_tx_chain_start_t = {0};
  rf_time_type config_tx_chain_time, overall_config_tx_chain_time;


  if(rf_tdscdma_mc_display_time_profile || rf_tdscdma_mc_tx_time_debug)
  {
    config_tx_chain_start_t = rf_time_get_tick(); /* time spent in API */
    overall_config_tx_chain_start_t = rf_time_get_tick(); /* time spent in API */
  }
  
  /* DEVICE Tx SSBI (direct programming and scripts)		             */
  /*=========================================================================*/

  if(rfm_get_calibration_state() || IS_FTM_IN_TEST_MODE())
  {
    /* Call FED to execute Tx burst script */
    in_data.effectTime = 0xFFFFFFFF; // Immediate execution
    in_data.scriptType = RFLM_TDS_TX_BURST_SCRIPT;
    in_data.actionCmd  = RFLM_TDS_EXECUTE_SCRIPT;
    in_data.scriptInfo.txBurstScript.bufId = buf_id;
    in_data.scriptInfo.txBurstScript.txPwrdBmQ9 = 0x7FFF; // min power
    in_data.scriptInfo.txBurstScript.txLmHandle = txlm_buffer_idx;
    in_data.scriptInfo.txBurstScript.callInCalMode = TRUE;

    exec_result = rflm_tds_schedule_rf_internal_event(&in_data);

    if (exec_result == RFLM_ERR_NONE)
    {
      MSG_3(MSG_SSID_RF, MSG_LVL_MED,
            "Tx burst script executed for bufId: %d, txPwr: 0x%x, handle: %d",
            in_data.scriptInfo.txBurstScript.bufId,
            in_data.scriptInfo.txBurstScript.txPwrdBmQ9,
            in_data.scriptInfo.txBurstScript.txLmHandle);
    }
    else
    {
      MSG_4(MSG_SSID_RF, MSG_LVL_ERROR,
            "Tx burst script executed ERROR (code %d) for bufId: %d, txPwr: 0x%x, handle: %d",
            exec_result,
            in_data.scriptInfo.txBurstScript.bufId,
            in_data.scriptInfo.txBurstScript.txPwrdBmQ9,
            in_data.scriptInfo.txBurstScript.txLmHandle);
    }
  }

  if (rf_tdscdma_mc_tx_time_debug)
  {
    config_tx_chain_time = rf_time_get_elapsed(config_tx_chain_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Tx Timeline: config_tx_chain: exec_tx_burst takes %d usec", config_tx_chain_time);
    config_tx_chain_start_t = rf_time_get_tick();
  }
  

  /* Configure Tuner Tx mode												 */
  /*=========================================================================*/
#ifdef FEATURE_RF_HAS_QTUNER
  /* Tuner */
  (void)rf_tdscdma_core_antenna_tuner_program_tx_mode(device,
                                                      rf_tdscdma_mc_state.curr_band_tx,
                                                      tx_channel,
                                                      RFDEVICE_EXECUTE_IMMEDIATE,
                                                      NULL,
                                                      0);
#endif /* FEATURE_RF_HAS_QTUNER */

  if (rf_tdscdma_mc_tx_time_debug)
  {
    config_tx_chain_time = rf_time_get_elapsed(config_tx_chain_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Tx Timeline: config_tx_chain: config_tuner_tx_mode takes %d usec", config_tx_chain_time);
    config_tx_chain_start_t = rf_time_get_tick();
  }


  /* For Tx bring-up														 */
  /*=========================================================================*/

  /* Temporary work-around for Tx online bring-up */
#if 0
  /* Set Tx GRFCs */
  tds_grfc_notime_control_data.band = band;
  tds_grfc_notime_control_data.sig_type = RFC_ANT_SEL; // To configure all
  tds_grfc_notime_control_data.override_logic = RFC_LOGIC_INVALID; // No override logic, use RFC AG logic
  (void)rfc_tdscdma_command_dispatch(device, RFC_TDSCDMA_CLEAN_RX_GRFC_NO_TIME, (void*)(&tds_grfc_notime_control_data));
#endif /* No need as FW/FED has to switch antenna anyway */

  if (!rf_tdscdma_enable_sw_tune)
  {
    /* Turn Off RX_ON/TX_ON if not using SW tune */
    //rf_tdscdma_msm_set_rx_on(device, band, FALSE);
    //rf_tdscdma_msm_set_tx_on(device, band, FALSE);
  }


  if (rfm_get_calibration_state())
  {
    MSG_2(MSG_SSID_RF, MSG_LVL_HIGH, "Skip TxAGC pre-configure in calibratoin mode", 0,0);
  }
  else
  {
    if(rf_tdscdma_enable_sw_tune)
    {
      /* If integrated with common TxAGC, set default WTR Tx/PA only if using SW tune */
      /* If not integrated with common TxAGC, regardless using SW tune or not must set default WTR Tx/PA */
      /*   For D3925+ APAC_CA configuration,
           if TxAGC/RxAGC is not skipped (on FW side), those default setting will be overwritten
           if TxAGC/RxAGC is skipped, those default setting will be kept

           For other configurations, TxAGC/RxAGC control are skipped on FW side and so the default
           setting will be kept 																		*/

      /* Set PA_ON = HIGH */
      rf_tdscdma_msm_set_pa_on(device, band, TRUE);

      /* Set default PA_Range = 0 */
      rf_tdscdma_msm_set_pa_range(device, band, 0);

      /* Configure Tx ASM if the flag is set */
      if (rf_tdscdma_use_tx_asm_in_sw_tune)
      {
        if (rfdevice_tdscdma_use_asm(device, band))
        {
          boolean asm_status;

          asm_status = rfdevice_tdscdma_asm_enable_tx(device,
                                                      band,
                                                      NULL,
                                                      RFDEVICE_EXECUTE_IMMEDIATE,
                                                      RFCOM_INVALID_TIMING_OFFSET,
                                                      TRUE,
                                                      NULL,
                                                      TDS_ASM_INCLUDE_ALL);

          if (!asm_status)
          {
            MSG_3(MSG_SSID_RF, MSG_LVL_ERROR, "Tx ASM configuration ERROR on Device %d (path %d) and Band %d", device, path, band);
          }
        }
        else
        {
          MSG_3(MSG_SSID_RF_DEBUG, MSG_LVL_MED, "Tx ASM not presented on Device %d (path %d) and Band %d", device, path, band);
        }
      } /* if (rf_tdscdma_use_tx_asm_in_sw_tune) */

      /* for all other configurations (include D3925+WTR3925), must use new tx override interface */
      {
        /* for common RGI interface */
        rfdevice_tx_lut_data_type lut_data;
        rfcom_band_type_u tech_band;
        tech_band.tdscdma_band = band;

        if(rf_tdscdma_use_on_rumi == FALSE)
        {
#if 0
          rfdevice_trx_tdscdma_tx** tds_device = NULL;
          uint8 device_idx = 0;

          tds_device = (rfdevice_trx_tdscdma_tx**) rfc_tdscdma_get_rf_device_obj(device,
                                                                                 RFC_CONFIG_TX,
                                                                                 band,
                                                                                 RFDEVICE_TRANSCEIVER);

          if (tds_device != NULL)
          {
            while (tds_device[device_idx] != NULL)
            {
              device_ptr = rfdevice_tdscdma_tx_get_common_device_pointer(tds_device[device_idx]);
              device_idx++;
            }
          }
          else
          {
            MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,
                  "rfdevice_tdscdma_get_cmn_device_ptr fail: Null pointer detected. device: %d, band: %d",
                  device, band);
          }
#endif

          rf_tdscdma_mc_cmn_device_ptr = rf_tdscdma_mc_get_curr_common_device(device,
                                                                              band);

          if ( NULL != rf_tdscdma_mc_cmn_device_ptr )
          {
            if ( rfdevice_cmn_is_cmn_tx_intf_supported(rf_tdscdma_mc_cmn_device_ptr) )
            {
              /* common TxAGC API */
              (void)rfdevice_cmn_get_tx_lut_scripts(rf_tdscdma_mc_cmn_device_ptr,
                                                    RFM_TDSCDMA_MODE,
                                                    tech_band,
                                                    0,   		   //pa_state,
                                                    rf_tdscdma_pdm, //rgi,
                                                    RFDEVICE_EXECUTE_IMMEDIATE,
                                                    &lut_data) ;
            }
            else
            {
              MSG(MSG_SSID_RF, MSG_LVL_ERROR, "rfdevice_cmn_tx_intf is not supported! ");
            }
          }
          else
          {
            MSG(MSG_SSID_RF, MSG_LVL_ERROR, "device_ptr is NULL. Unable to get Tx lut table");
          }
        }
      } /* Use WTR CMN Tx override interface */
    } /* if (rf_tdscdma_enable_sw_tune) */
  } /* if NOT in calibration state */

  if (rf_tdscdma_mc_tx_time_debug)
  {
    config_tx_chain_time = rf_time_get_elapsed(config_tx_chain_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "Debug Tx Timeline: config_tx_chain: tx_sw_tune takes %d usec", config_tx_chain_time);
    config_tx_chain_start_t = rf_time_get_tick();
  }

  MSG_5(MSG_SSID_RF, MSG_LVL_HIGH,
        "rf_tdscdma_mc_config_tx_chain(device %d, path %d, band %d, tx_channel %d, txlm_buffer_idx %d) is complete",
        device, path, band, tx_channel, txlm_buffer_idx);

  if(rf_tdscdma_mc_display_time_profile || rf_tdscdma_mc_tx_time_debug)
  {
    overall_config_tx_chain_time = rf_time_get_elapsed(overall_config_tx_chain_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LEGACY_MED, "config_tx_chain time profile: subtotal takes %d usec", overall_config_tx_chain_time);
  }
}





/*----------------------------------------------------------------------------*/
/*!
  @brief
  Expose to L1 to allow update RxLM mode only

  @details
  Expose to L1 to allow update RxLM mode only. Mainly use for InterRAT

*/
void rf_tdscdma_mc_update_rxlm_mode
(
  uint16 rx_channel,
  lm_handle_type rxlm_buffer_idx,
  rfa_tdscdma_rxlm_mode_t rxlm_mode
)
{
  uint8 wb_adc_idx;

  /* Update rxlm buffer(both static and dynamic setting) */
  rf_tdscdma_msm_update_rxlm_buffer(RFM_DEVICE_0, rx_channel, rxlm_mode, rxlm_buffer_idx, &wb_adc_idx);

  MSG_3(MSG_SSID_RF, MSG_LVL_HIGH, "rf_tdscdma_mc_update_rxlm_mode(%d, %d, %d)", rx_channel, rxlm_buffer_idx, rxlm_mode);

}


void rf_tdscdma_mc_update_irat_rxlm_mode
(
  rfm_device_enum_type device,
  uint16 rx_channel,
  lm_handle_type rxlm_buffer_idx,
  rfa_tdscdma_rxlm_mode_t rxlm_mode
)
{
  uint8 wb_adc_idx;

  /* Update rxlm buffer(both static and dynamic setting) */
  rf_tdscdma_msm_update_rxlm_buffer(device, rx_channel, rxlm_mode, rxlm_buffer_idx, &wb_adc_idx);

  MSG_4(MSG_SSID_RF, MSG_LVL_HIGH, "rf_tdscdma_mc_update_rxlm_mode(%d, %d, %d, %d)", device, rx_channel, rxlm_buffer_idx, rxlm_mode);

}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Send FTM Burst TX Message

  @details
  This function sends the Burst TX ON/OFF message

*/
void rf_tdscdma_mc_burst_tx
(
  boolean on_off,
  rf_tdscdma_ftm_burst_tx_params_type tx_params,
  uint32 txlm_bufidx,
  rf_tdscdma_rfmode_cont_tx_msg_subid_type subID
)
{
  uint32 tx_freq = 0;

  rf_common_enter_critical_section(&rf_tdscdma_rf_mode_tx_lock);

  tx_freq = rf_tdscdma_core_util_get_freq_from_uarfcn(rf_tdscdma_mc_state.curr_chan_tx, rf_tdscdma_mc_state.curr_band_tx);
  tx_freq = tx_freq * 1000;

  if (subID == TDSCDMA_RFMODE_CONT_TX_DEFAULT_SUBID)
  {
    rf_tdscdma_mc_set_burst_tx_cmd_status(TDSCDMA_RFMODE_TX_CMD_RSP_PENDING);
  }
  rf_tdscdma_mdsp_burst_tx(on_off, tx_params, txlm_bufidx, tx_freq, subID);
  rf_common_leave_critical_section(&rf_tdscdma_rf_mode_tx_lock);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process FTM Burst TX Message response

  @details
  This function processes the Burst TX ON/OFF message response from FW

*/
void rf_tdscdma_mc_proc_burst_tx_rsp
(
  boolean status,
  boolean reset_config,
  uint32 rsp_subID
)
{
  /* Only update the FTM_TX status if the subID is for FTM command */
  if (rsp_subID == TDSCDMA_RFMODE_CONT_TX_DEFAULT_SUBID)
  {
    if (status)
    {
      rf_tdscdma_rfmode_tx_cmd_rsp_status = TDSCDMA_RFMODE_TX_CMD_RSP_SUCCESS;
    }
    else
    {
      rf_tdscdma_rfmode_tx_cmd_rsp_status = TDSCDMA_RFMODE_TX_CMD_RSP_FAILURE;
      if (reset_config)
      {
        MSG_1(MSG_SSID_RF_DEBUG, MSG_LVL_LOW, "rf_tdscdma_mc_proc_burst_tx_rsp: FTM_TX cmd parameters reset. rsp_subID: %d", rsp_subID);
      }
      else
      {
        MSG_1(MSG_SSID_RF_DEBUG, MSG_LVL_LOW, "rf_tdscdma_mc_proc_burst_tx_rsp: FTM_TX cmd parameters updated. rsp_subID: %d", rsp_subID);
      }
    }
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process FTM IQ capture Message response

  @details
  This function processes the IQ capture message response from FW

*/
void rf_tdscdma_mc_proc_iq_capture_rsp
(
  boolean status
)
{
  if (status)
  {
    rf_tdscdma_rfmode_iq_capture_status = TDSCDMA_RFMODE_CMD_RSP_SUCCESS;
    MSG(MSG_SSID_RF_DEBUG, MSG_LVL_LOW, "rf_tdscdma_mc_proc_iq_capture_rsp: IQ capture successful");

  }
  else
  {
    rf_tdscdma_rfmode_iq_capture_status = TDSCDMA_RFMODE_CMD_RSP_FAILURE;
    MSG(MSG_SSID_RF_DEBUG, MSG_LVL_LOW, "rf_tdscdma_mc_proc_iq_capture_rsp: IQ capture failed");
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return the current status of FTM_TX MSGR command

*/
rf_tdscdma_rfmode_tx_cmd_rsp_status_type rf_tdscdma_mc_get_burst_tx_cmd_status(void)
{
  return rf_tdscdma_rfmode_tx_cmd_rsp_status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the current status of FTM_TX MSGR command

*/
void rf_tdscdma_mc_set_burst_tx_cmd_status(rf_tdscdma_rfmode_tx_cmd_rsp_status_type status)
{
  rf_tdscdma_rfmode_tx_cmd_rsp_status = status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API will only be invoked by Temp Comp loop update in RF Mode
  Continuous TX

*/
void rf_tdscdma_mc_rf_mode_temp_comp_handler(void)
{
  rf_tdscdma_ftm_burst_tx_params_type rf_mode_tx_params;
  uint32 lm_idx = 0;

  rf_mode_tx_params = ftm_tdscdma_get_rf_mode_tx_params();
  lm_idx = ftm_tdscdma_get_lm_idx();

  if ((rf_mode_tx_params.num_slots == 0) && (!rf_mode_tx_params.conTX_tx_reconfig_block))
  {
    MSG_1(MSG_SSID_RF_DEBUG, MSG_LVL_LOW, "rf_tdscdma_mc_rf_mode_temp_comp_handler: Therm based TX reconfig: %d", rf_mode_tx_params.tspwrlvl);
    rf_tdscdma_mc_burst_tx(TRUE,rf_mode_tx_params, lm_idx, TDSCDMA_RFMODE_CONT_TX_THERM_RECONFIG_SUBID);
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API will only be invoked by Temp Comp loop update in RF Mode
  Continuous TX

*/
void rf_tdscdma_mc_rf_mode_hdet_comp_handler(void)
{
  rf_tdscdma_ftm_burst_tx_params_type rf_mode_tx_params;
  uint32 lm_idx = 0;

  rf_mode_tx_params = ftm_tdscdma_get_rf_mode_tx_params();
  lm_idx = ftm_tdscdma_get_lm_idx();

  if ((rf_mode_tx_params.num_slots == 0) && (!rf_mode_tx_params.conTX_tx_reconfig_block))
  {
    MSG_1(MSG_SSID_RF_DEBUG, MSG_LVL_LOW, "rf_tdscdma_mc_rf_mode_hdet_comp_handler: HDET based TX reconfig: %d", rf_mode_tx_params.tspwrlvl);
    rf_tdscdma_mc_burst_tx(TRUE,rf_mode_tx_params, lm_idx, TDSCDMA_RFMODE_CONT_TX_HDET_RECONFIG_SUBID);
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Send FTM Burst RX Message

  @details
  This function sends the Burst RX ON/OFF message

*/
void rf_tdscdma_mc_burst_rx
(
  boolean on_off,
  rf_tdscdma_ftm_burst_rx_params_type rx_params,
  uint32 rxlm_bufidx,
  uint32 rxlm_bufidx_div
)
{
  uint32 prim_freq = 0;
  uint32 work_freq = 0;

  prim_freq = rf_tdscdma_core_util_get_freq_from_uarfcn(rf_tdscdma_mc_state.curr_chan_rx, rf_tdscdma_mc_state.curr_band_rx);
  prim_freq = prim_freq * 1000;
  work_freq = rf_tdscdma_core_util_get_freq_from_uarfcn(rf_tdscdma_mc_state.curr_chan_rx, rf_tdscdma_mc_state.curr_band_rx);
  work_freq = work_freq * 1000;
  rf_tdscdma_mc_set_burst_rx_cmd_status(TDSCDMA_RFMODE_RX_CMD_RSP_PENDING);
  rf_tdscdma_mdsp_burst_rx(on_off, rx_params, rxlm_bufidx, rxlm_bufidx_div, prim_freq, work_freq);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process FTM Burst RX Message response

  @details
  This function processes the Burst RX ON/OFF message response from FW

*/
void rf_tdscdma_mc_proc_burst_rx_rsp
(
  boolean status
)
{
  if (status)
  {
    rf_tdscdma_rfmode_rx_cmd_rsp_status = TDSCDMA_RFMODE_RX_CMD_RSP_SUCCESS;
  }
  else
  {
    rf_tdscdma_rfmode_rx_cmd_rsp_status = TDSCDMA_RFMODE_RX_CMD_RSP_FAILURE;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return the current status of FTM_RX MSGR command

*/
rf_tdscdma_rfmode_rx_cmd_rsp_status_type rf_tdscdma_mc_get_burst_rx_cmd_status(void)
{
  return rf_tdscdma_rfmode_rx_cmd_rsp_status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the current status of FTM_RX MSGR command

*/
void rf_tdscdma_mc_set_burst_rx_cmd_status(rf_tdscdma_rfmode_rx_cmd_rsp_status_type status)
{
  rf_tdscdma_rfmode_rx_cmd_rsp_status = status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API forces the MC state machine to RX only state

  @param
  device - device type

  @details
  This API forces the MC state machine to RX only state. This API is used ONLY in
  RF Mode TX. The state machine needs to be force to RX in order for the enable_tx
  API to proceed with initializing the various TX parameters and the HDET/THERM
  timers and loop variables. The MC state machine will be re-set back when RF Mode
  TX OFF is called.

*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_set_sm_rfmode_tx(rfcom_device_enum_type device)
{
  rf_path_enum_type path;

  /* only support RFCOM_TRANSCEIVER_0 for TX */
  if(device != RFCOM_TRANSCEIVER_0)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rf_tdscdma_mc_set_sm_rfmode_tx: Unsupported Device %d", device);
    return RF_TDSCDMA_MC_INVALID_PARAM;
  }

  path = rfcommon_core_device_to_path(device);
  rf_tdscdma_mc_state.rf_state[path] = RF_TDSCDMA_STATE_RX;
  return RF_TDSCDMA_MC_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function enables the flag so to enable writes to SMEM in FTM burst mode

  @details
*/
void rf_tdscdma_mc_set_bursty_mode_flag(boolean set)
{
#if 0
  if(set)
  {
    rf_tdscdma_enable_ftm_bursty_mode = TRUE;
  }
  else
  {
    rf_tdscdma_enable_ftm_bursty_mode = FALSE;
  }
#endif
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process the tuner therm readings from mdsp

  @details
*/
void rf_tdscdma_mc_proc_tuner_therm_reading
(
  uint16 numSsbi,
  uint8 address[],
  uint8 measurement[]
)
{
  (void)rfdevice_hdet_tdscdma_measure_temp_comp_cb(rf_tdscdma_mc_state.curr_pri_device,
  rf_tdscdma_mc_state.curr_band_tx,
  numSsbi,
  address,
  measurement);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API process the get multi synth rsp from FW

  @param
  uint8 cmd_status,
  uint8 rx_pll_status,
  uint8 tx_pll_status,
  uint8 is_cont_tx_valid


  @details

*/
void rf_tdscdma_mc_get_multi_synth_rsp(boolean cmd_status, uint8 rx_pll_status,
uint8 tx_pll_status, boolean is_cont_tx_valid)
{
  if (cmd_status)
  {
    if(is_cont_tx_valid)
    {
      rf_tdscdma_rfmode_multi_synth_status = TDSCDMA_RFMODE_CMD_RSP_SUCCESS;
      MSG_1(MSG_SSID_RF_DEBUG, MSG_LVL_HIGH, "Cont Tx valid, Cmd Success", 0);
    }
    else
    {
      rf_tdscdma_rfmode_multi_synth_status = TDSCDMA_RFMODE_CMD_RSP_SUCCESS;
      MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "Come to corner case for Continuous Tx, Tx PLL Result might be Invalid", 0);
    }
  }
  else
  {
    rf_tdscdma_rfmode_multi_synth_status = TDSCDMA_RFMODE_CMD_RSP_FAILURE;
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "CMD Failed", 0);
  }

  rf_tdscdma_tx_pll_status = tx_pll_status;
  rf_tdscdma_rx_pll_status = rx_pll_status;

}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API return the current rf_tdscdma_mc_get_blocking_rsp_status

  @param

  @details

*/
rf_tdscdma_rfmode_cmd_rsp_status_type  rf_tdscdma_mc_get_blocking_rsp_status(rf_tdscdma_mdsp_msg_t ftm_rf_cmd_msg)
{
  rf_tdscdma_rfmode_cmd_rsp_status_type ret = TDSCDMA_RFMODE_CMD_RSP_MAX;

  switch(ftm_rf_cmd_msg)
  {
  case RF_TDSCDMA_MDSP_GET_MULTI_SYNTH_MSG:
    ret = rf_tdscdma_rfmode_multi_synth_status;
    break;

    /*IQ capture*/
  case RF_TDSCDMA_MDSP_IQ_CAPTURE_CFG_MSG:
    ret = rf_tdscdma_rfmode_iq_capture_status;
    break;
    /*For future flexibility*/
    /*


    case RF_TDSCDMA_MDSP_FTM_RF_RX_BURST_MSG:
    	ret = rf_tdscdma_rfmode_xxxx_status;
    	break;

    case RF_TDSCDMA_MDSP_FTM_RF_TX_BURST_MSG:
    	ret = rf_tdscdma_rfmode_xxxx_status;
    	break;

    */

  default :
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "This Response has not been supported yet!", 0);
    break;
  }

  return ret;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API return the current rf_tdscdma_rfmode_multi_synth_status

  @param

  @details

*/

rf_tdscdma_rfmode_cmd_rsp_status_type  rf_tdscdma_mc_get_multi_synth_rsp_status()
{
  return rf_tdscdma_rfmode_multi_synth_status;
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API set the current rf_tdscdma_rfmode_multi_synth_status

  @param

  @details

*/
void rf_tdscdma_mc_set_multi_synth_rsp_status(rf_tdscdma_rfmode_cmd_rsp_status_type status)
{
  rf_tdscdma_rfmode_multi_synth_status = status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API set the current rf_tdscdma_mc_set_blocking_cmd_rsp_status

  @param

  @details

*/
void rf_tdscdma_mc_set_blocking_cmd_rsp_status
(
  rf_tdscdma_mdsp_msg_t ftm_rf_cmd_msg,
  rf_tdscdma_rfmode_cmd_rsp_status_type status
)
{
  switch(ftm_rf_cmd_msg)
  {
  case RF_TDSCDMA_MDSP_GET_MULTI_SYNTH_MSG:
    rf_tdscdma_rfmode_multi_synth_status = status;
    break;

    /*IQ capture*/
  case RF_TDSCDMA_MDSP_IQ_CAPTURE_CFG_MSG:
    rf_tdscdma_rfmode_iq_capture_status = status;
    break;

    /*For future flexibility*/
    /*
    case RF_TDSCDMA_MDSP_IQ_CAPTURE_MSG:
    	rf_tdscdma_rfmode_xxx_status = status;
    	break;

    case RF_TDSCDMA_MDSP_FTM_RF_RX_BURST_MSG:
    	rf_tdscdma_rfmode_xxx_status = status;
    	break;

    case RF_TDSCDMA_MDSP_FTM_RF_TX_BURST_MSG:
    	rf_tdscdma_rfmode_xxx_status = status;
    	break;

    */
  default :
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "This Response has not been supported yet!", 0);
    break;

  }
}


uint8 rf_tdscdma_mc_get_tx_pll_status(void)
{
  return rf_tdscdma_tx_pll_status;
}

uint8 rf_tdscdma_mc_get_rx_pll_status(void)
{
  return rf_tdscdma_rx_pll_status;
}
/*----------------------------------------------------------------------------*/

/*!
  @brief
  This function provides API for channel to band conversion for L1

  @details
*/
uint32 rf_l1_tdscdma_get_band_from_uarfcn(uint32 chan)
{
  rfcom_tdscdma_band_type band = RFCOM_BAND_TDSCDMA_INVALID;
  band = rf_tdscdma_core_util_get_band_from_uarfcn((uint16)chan);

  return ((uint32)band);

}
/*----------------------------------------------------------------------------*/
#ifdef FEATURE_RF_HAS_QTUNER
/*============================================================================

FUNCTION
rf_tdscdma_mc_program_qfe_scenario

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

boolean rf_tdscdma_mc_program_qfe_scenario(void)
{
  //uint32 carrier_index = 0;
  rfm_device_enum_type p_device;
  rfm_device_enum_type s_device;
  void** tuner_mgr_p;
  void** tuner_mgr_s;
  rfcom_band_type_u curr_band;
  uint8 tuner_mgr_index = 0;
  boolean status =TRUE;
  script_token_type dummy_token;

  if( (rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RX) || (rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RXTX) )
  {
    /*Grab the primary and secondary device to locals*/
    p_device = rf_tdscdma_mc_state.curr_pri_device;
    s_device = rf_tdscdma_mc_state.curr_sec_device;

    /* Program the scenario for the Primary device */
    /* Get tuner manager from RFC */
    tuner_mgr_p = (void**)rfc_tdscdma_get_rf_device_obj(p_device,
                                                        RFC_CONFIG_RX,
                                                        rf_tdscdma_mc_state.curr_band_rx,
                                                        RFDEVICE_TUNER_MANAGER);


    curr_band.tdscdma_band = rf_tdscdma_mc_state.curr_band_rx;

    if((tuner_mgr_p != NULL) && (tuner_mgr_p[0] != NULL))
    {
      tuner_mgr_index = 0;

      while (tuner_mgr_p[tuner_mgr_index] != NULL)
      {
        status &= rfcommon_atuner_manager_force_detune( tuner_mgr_p[tuner_mgr_index], 
                                                           RFM_TDSCDMA_MODE,
                                                           curr_band,rf_tdscdma_mc_state.curr_chan_rx,
                                                           FALSE,
                                                           RFDEVICE_EXECUTE_IMMEDIATE,
                                                           NULL,
                                                           0,
                                                           &dummy_token);
        tuner_mgr_index++;
      }
      if (status == FALSE)
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf_tdscdma_mc_program_qfe_scenario failed for device(%d)",p_device);
      }
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rflte_mc_program_qfe_scenario:Tuner device failed for device(%d)",p_device);
      status = FALSE;
    }

    /* Program the tune code  for secondary  device*/
    /* Get tuner object from RFC */
    tuner_mgr_s = (void**)rfc_tdscdma_get_rf_device_obj(s_device,
                                                        RFC_CONFIG_RX,
                                                        rf_tdscdma_mc_state.curr_band_rx,
                                                        RFDEVICE_TUNER_MANAGER);

    curr_band.tdscdma_band = rf_tdscdma_mc_state.curr_band_rx;

    if((tuner_mgr_s != NULL) && (tuner_mgr_s[0] != NULL))
    {
      tuner_mgr_index = 0;
      while (tuner_mgr_s[tuner_mgr_index] != NULL)
      {
        status &= rfcommon_atuner_manager_force_detune( tuner_mgr_s[tuner_mgr_index], 
                                                           RFM_TDSCDMA_MODE,
                                                           curr_band,
                                                           rf_tdscdma_mc_state.curr_chan_rx,
                                                           FALSE,
                                                           RFDEVICE_EXECUTE_IMMEDIATE,
                                                           NULL,
                                                           0,
                                                           &dummy_token);
        tuner_mgr_index++;
      }
      if (status == FALSE)
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf_tdscdma_mc_program_qfe_scenario failed for device(%d)"
              ,s_device);
      }
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf_tdscdma_mc_program_qfe_scenario:Tuner device failed for"
            "device(%d)",s_device);
      status = FALSE;
    }
  }
  else
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf_tdscdma_mc_program_qfe_scenario: rf tdscdma mc state is not in the right");
  }

  return status;
}
#endif /* FEATURE_RF_HAS_QTUNER */


/*----------------------------------------------------------------------------*/

#ifdef FEATURE_RF_HAS_QTUNER
/*!
  @brief
  This function is program tune code in mission mode


  @details
  Called by the command dispatch when a tick is received from TL1


  @param override_flag
  Tuner override flag: Any value greater than '1' indicates the current tuner settings
  can be overriden.

  @param data
  Pointer to the tuner tunecode config structure

  @param tuner_nv_ptr
  Pointer to the NV data structure

  @return
  uint32 status

*/
void rf_tdscdma_mc_update_tuner_code(boolean is_immediate)
{
  rf_buffer_intf *tuner_script;
  boolean status_flag = TRUE;
  uint16 tuner_mdsp_chain = RF_TUNER_CHAIN_0;
  rfcommon_mdsp_event_handle** tuner_event_handle_ptr;
  rfcommon_tuner_update_cb_data_type* tuner_cb_data;
  script_token_type* master_chain_token_ptr;
  script_token_type* slave_chain_token_ptr;
  void** tuner_mgr_p;
  void** tuner_mgr_s;
  rfcom_band_type_u curr_band;
  rfcom_device_enum_type p_device = RFM_INVALID_DEVICE, s_device = RFM_INVALID_DEVICE;
  uint8 tuner_mgr_index = 0;
  rf_device_execution_type execution_type = RFDEVICE_CREATE_SCRIPT;
  boolean is_this_rx_operation = TRUE;
  rfcom_tdscdma_band_type tds_curr_band = rf_tdscdma_mc_state.curr_band_rx;
  uint16 tds_curr_chan = rf_tdscdma_mc_state.curr_chan_rx;
  rflm_dm_handle_id_t rflm_handle;
  uint32* buf_ptr;
  rftdscdma_mdsp_common_event_data_type rf_tds_build_event_data;

  /* Assuming PATH is always 0 and only update tuner code on WRK buffer */
  rflm_handle = (rflm_dm_handle_id_t)rf_tdscdma_mc_lm_bufs.rx_buf_wrk[RF_PATH_0].buf_idx;

  if(rflm_handle != rf_tdscdma_mc_state.last_rxlm_handle_for_AOL_update)
  {
    boolean status;

    if(rf_tdscdma_mc_state.last_rxlm_handle_for_AOL_update != 0xFF)
    {
      // Deallocate AOL script ptr for previous RxLM handle (it will abort pending AOL event as well)
      // 0xFF is NOT a valid RxLM handle
      rflm_tds_aol_cl_deallocate_memory(rf_tdscdma_mc_state.last_rxlm_handle_for_AOL_update, TRUE);
    }
    // Allocated AOL script ptr for the new RxLM handle
    status = rflm_tds_aol_cl_allocate_memory(rflm_handle, TRUE);

    if(status == TRUE)
    {
      rf_tdscdma_mc_state.last_rxlm_handle_for_AOL_update = rflm_handle;
    }    
  }

  buf_ptr = rflm_dm_get_script_buf_ptr(rflm_handle, RFLM_TDS_TUNER_AOL_EVENT);

  if(buf_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "rf_tdscdma_mc_update_tuner_code: No script buff created for Handle %d",
          rflm_handle);
    return;
  }

  /*Store Primary and Secondary device type to locals.*/
  p_device = rf_tdscdma_mc_state.curr_pri_device;
  s_device = rf_tdscdma_mc_state.curr_sec_device;

  MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH,
        "rf_tdscdma_mc_update_tuner_code:Updating for prx_device %d drx_device %d",
        p_device, s_device);


#ifdef FEATURE_RF_ASDIV
  if (rfcommon_asdiv_is_switch_in_progress() == TRUE)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_MED,
        "rf_tdscdma_mc_update_tuner_code: [ASDIV] Switch in progress, Skip update");
    return;
  }
#endif

  tuner_cb_data = rfcommon_tuner_event_create_cb_data(buf_ptr, RFM_TDSCDMA_MODE, (uint32)rflm_handle);
  if(tuner_cb_data == NULL)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "rf_tdscdma_mc_update_tuner_code: mem_alloc failed(%d %d)",
          p_device,s_device);
    return;
  }
  else
  {
    /* Obtain ccs event handle from call back data */
    tuner_event_handle_ptr = &(tuner_cb_data->tuner_ccs_event_handle);     
  }

  /* Allocate mem for tuner script */
  tuner_script = rf_buffer_create (0, RFTDSCDMA_STANDALONE_MAX_RFFE_SCRIPT_SIZE, 0) ;

  if(tuner_script == NULL)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "rf_tdscdma_mc_update_tuner_code: Cannot create script buffer(%d %d)",
          p_device,s_device);
    return;
  }

  if (rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RX )
  {
    is_this_rx_operation = TRUE;
    tds_curr_band = rf_tdscdma_mc_state.curr_band_rx;
    tds_curr_chan = rf_tdscdma_mc_state.curr_chan_rx;
  }
  else if (rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RXTX )
  {
    is_this_rx_operation = FALSE;
    tds_curr_band = rf_tdscdma_mc_state.curr_band_tx;
    tds_curr_chan = rf_tdscdma_mc_state.curr_chan_tx;
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "rf_tdscdma_mc_update_tuner_code: not in correct rf_state %d ",
          rf_tdscdma_mc_state.rf_state[RF_PATH_0]);
		  
	/* Clean up all allocated memory */
	if (tuner_script != NULL)
	{
	  rf_buffer_destroy( tuner_script );
	}
	
    /* Clean up all allocated memory */
    if(tuner_cb_data != NULL)
    {
      rfcommon_tuner_event_cleanup( tuner_cb_data );
    }	

	/* if not in the right state , we should not do anything here and return */
	return;
  }

  /* Update tune code for primary device */
  /* Get tuner object from RFC */
  tuner_mgr_p = (void**)rfc_tdscdma_get_rf_device_obj(p_device,
                                                      RFC_CONFIG_RX,
                                                      tds_curr_band,
                                                      RFDEVICE_TUNER_MANAGER);

  if((tuner_mgr_p != NULL) && (tuner_mgr_p[0] != NULL))
  {
    curr_band.tdscdma_band = tds_curr_band;

    tuner_cb_data->tuner_data[tuner_mdsp_chain].ant_tuner = tuner_mgr_p[0];
    master_chain_token_ptr = &(tuner_cb_data->tuner_data[tuner_mdsp_chain].tuner_token);

    /*Init value*/
    *master_chain_token_ptr = ATUNER_INVALID_TOKEN_VAL;

    /* Obtain the RF script on Primary device */
    tuner_mgr_index = 0;
    while (tuner_mgr_p[tuner_mgr_index] != NULL)
    {
      status_flag &= rfcommon_atuner_manager_update_tune_code(tuner_mgr_p[tuner_mgr_index],
                                                              RFM_TDSCDMA_MODE,
                                                              curr_band,tds_curr_chan,
                                                              is_this_rx_operation,
                                                              execution_type,
                                                              tuner_script,
                                                              0,
                                                              master_chain_token_ptr);
      tuner_mgr_index++;
    }

    /* Build/Execute tuner script for the diversity device */
    if (status_flag == TRUE)
    { 
      if (rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_DIV ||
          rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_BOTH)
      {
        /* Get tuner object from RFC */
        tuner_mgr_s = rfc_tdscdma_get_rf_device_obj(s_device,
                                                    RFC_CONFIG_RX,
                                                    rf_tdscdma_mc_state.curr_band_rx,
                                                    RFDEVICE_TUNER_MANAGER);

        if((tuner_mgr_s != NULL) && (tuner_mgr_s[0] != NULL))
        {
          /* Assign tuner device pointer to call back data */
          tuner_cb_data->tuner_data[RF_TUNER_CHAIN_1].ant_tuner = tuner_mgr_s[0];

          /*Obtain ccs event handle from call back data*/
          slave_chain_token_ptr = &(tuner_cb_data->tuner_data[RF_TUNER_CHAIN_1].tuner_token);
          
          /*Init value*/
          *slave_chain_token_ptr = ATUNER_INVALID_TOKEN_VAL;
          
          /* Obtain the RF script on Secondary device */
          tuner_mgr_index = 0;

          while (tuner_mgr_s[tuner_mgr_index] != NULL)
          {
            status_flag &= rfcommon_atuner_manager_update_tune_code(tuner_mgr_s[tuner_mgr_index], 
                                                                    RFM_TDSCDMA_MODE,
                                                                    curr_band,tds_curr_chan,
                                                                    TRUE,
                                                                    execution_type,
                                                                    tuner_script,
                                                                    0,
                                                                    slave_chain_token_ptr);
            tuner_mgr_index++;
          }

          if (status_flag == FALSE)
          {
            MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
                  "rf_tdscdma_mc_program_tune_code failed for device(%d)", s_device);
          }
        }
        else
        {
          MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
                "rf_tdscdma_mc_update_tuner_code:Tuner device failed for device(%d)",s_device);
          status_flag = FALSE;
        }
      }
      else
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_MED,
              "rf_tdscdma_mc_update_tuner_code: diversity not enabled, rx_path= %d",
              rf_tdscdma_mc_state.curr_rx_path);
      }
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
            "rf_tdscdma_mc_program_tune_code failed for device(%d)", p_device);
    }
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "rf_tdscdma_mc_update_tuner_code:Tuner device failed for device(%d)", p_device);
    status_flag = FALSE;
  }

  /* Update homeRestore script regarding less if tuner code update is success or not */
  #ifdef FEATURE_RF_ASDIV
  rf_tdscdma_mc_state.curr_ant_sw_pos = rfcommon_asdiv_get_current_position(rf_tdscdma_mc_state.curr_pri_device);
  rf_tdscdma_mc_state.next_ant_sw_pos = rfcommon_asdiv_get_current_position(rf_tdscdma_mc_state.curr_pri_device);
  #else
  rf_tdscdma_mc_state.curr_ant_sw_pos = 0;
  rf_tdscdma_mc_state.next_ant_sw_pos = 0;     
  #endif /* #ifdef FEATURE_RF_ASDIV */

  rf_tds_build_event_data.pri_device = rf_tdscdma_mc_state.curr_pri_device;
  rf_tds_build_event_data.sec_device = rf_tdscdma_mc_state.curr_sec_device;
  rf_tds_build_event_data.tx_device  = rf_tdscdma_mc_state.curr_pri_device;
  rf_tds_build_event_data.band       = rf_tdscdma_mc_state.curr_band_rx;
  rf_tds_build_event_data.channel    = rf_tdscdma_mc_state.curr_chan_rx;
  rf_tds_build_event_data.drx_enable = rf_tdscdma_mc_state.div_enable;
  rf_tds_build_event_data.ant_sw_pos = rf_tdscdma_mc_state.next_ant_sw_pos;

  rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_BUILD_HOME_RESTORE,
                                        &rf_tds_build_event_data);    

  /* Update mdsp data and register callback for FW response */
  if( status_flag == TRUE )      
  {
    *tuner_event_handle_ptr = NULL;

    /* Common Call for both SBDC and DBDC */
    status_flag &= rfcommon_mdsp_tuner_open_loop_queue_script(buf_ptr,
                                                              RFLM_TDS_TUNER_AOL_EVENT,
                                                              tuner_script,
                                                              tuner_event_handle_ptr,
                                                              rfcommon_tuner_open_loop_response_handler,
                                                              (void*)tuner_cb_data);
  } /*  if( status_flag == TRUE )  */
  else
  {
    /* No need to continue, force cleaning up all allocated memory */
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "rf_tdscdma_mc_update_tuner_code:Cleaning tuner call back and script buffer(%d %d)",
          p_device, s_device);
  }

  /* Clean up all allocated memory */
  if (tuner_script != NULL)
  {
    rf_buffer_destroy( tuner_script );
  }

  /* Clean up all allocated memory */
  if((status_flag == FALSE) && (tuner_cb_data != NULL))
  {
    rfcommon_tuner_event_cleanup( tuner_cb_data );
  }

  MSG_8(MSG_SSID_RF_DEBUG, MSG_LEGACY_HIGH,
        "rf_tdscdma_mc_update_tuner_code: p_device %d, s_device %d, Band %d, Chan: %d, is_immediate %d, is_rx_operation %d, rxLMHandle %d, status_flag %d",
        p_device, s_device, tds_curr_band, tds_curr_chan, is_immediate, is_this_rx_operation, rflm_handle, status_flag);
}
#endif /* FEATURE_RF_HAS_QTUNER */



/*----------------------------------------------------------------------------*/
#ifdef FEATURE_RF_ASDIV
void rf_tdscdma_mc_send_switch_ant_msgr
(
  rfm_device_enum_type device,
  uint8 to_pos,
  void *cb_func,
  void *cb_data,
  boolean action_immediate
)
{
  rfa_tdscdma_ant_sw_req_s ant_sw_msg;
  errno_enum_type status = E_SUCCESS;

  msgr_init_hdr(&(ant_sw_msg.hdr), MSGR_RFA_RF_TDSCDMA, RFA_RF_TDSCDMA_ANT_SW_REQ);
  ant_sw_msg.ant_sw_params.device = device;
  ant_sw_msg.ant_sw_params.pos = to_pos;
  ant_sw_msg.ant_sw_params.cb_func = cb_func;
  ant_sw_msg.ant_sw_params.cb_data = cb_data;
  ant_sw_msg.ant_sw_params.is_immediate = action_immediate;
  status = msgr_send(&(ant_sw_msg.hdr),sizeof(rfa_tdscdma_ant_sw_req_s));
  if ( status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Cannot send RFA_RF_TDSCDMA_ANT_SW_REQ: status %d", status);
  }
}

void rf_tdscdma_mc_send_switch_ant_abort_msgr
(
  rfm_device_enum_type device,
  uint8 to_pos,
  void *cb_func,
  void *cb_data
)
{
  rfa_tdscdma_ant_sw_abort_ind_s ant_sw_abort_msg;
  errno_enum_type status = E_SUCCESS;

  msgr_init_hdr(&(ant_sw_abort_msg.hdr), MSGR_RFA_RF_TDSCDMA, RFA_RF_TDSCDMA_ANT_SW_ABORT_IND);
  status = msgr_send(&(ant_sw_abort_msg.hdr),sizeof(rfa_tdscdma_ant_sw_abort_ind_s));
  if ( status != E_SUCCESS)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Cannot send RFA_RF_TDSCDMA_ANT_SW_ABORT_IND: status %d", status);
  }
}

#endif


rflm_tds_rx_mode_e rf_tdscdna_mc_rxagc_convert_rxlm_mode
(
  rfa_tdscdma_rxlm_mode_t tds_rxlm_mode,
  boolean is_ifreq
)
{
  rflm_tds_rx_mode_e rflm_mode = RFLM_TDS_ACQ_MODE;

  switch (tds_rxlm_mode)
  {
  case RFA_TDSCDMA_RXLM_MODE_ACQ:
    rflm_mode = RFLM_TDS_ACQ_MODE;
    break;

  case RFA_TDSCDMA_RXLM_MODE_TRK:
    rflm_mode = RFLM_TDS_TRK_MODE;
    break;

  case RFA_TDSCDMA_RXLM_MODE_TRK_IDLE:
    rflm_mode = RFLM_TDS_TRK_IDLE_MODE;
    break;

  case RFA_TDSCDMA_RXLM_MODE_TRK_IRAT:
    rflm_mode = RFLM_TDS_TRK_IRAT_MODE;
    break;

  case RFA_TDSCDMA_RXLM_MODE_PWR_SCAN:
    rflm_mode = RFLM_TDS_PWR_SCAN_MODE;
    break;

  default:
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "Wrong RxLM mode %d", tds_rxlm_mode);
  }
  } /* switch (tds_rxlm_mode) */

  /* Overwrite the mode for IFREQ */
  if (is_ifreq)
  {
    // for ifreq case, rxlm_mode = RFA_TDSCDMA_RXLM_MODE_TRK_IRAT
    // So there is no way we can differentiate if it is IRAT or IFREQ
    // Have to use is_Ifreq flag
    rflm_mode = RFLM_TDS_IFREQ_MODE;
  }

  if (rf_tdscdma_mc_rxagc_debug)
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdna_mc_rxagc_convert_rxlm_mode, rxlm_mode: %d, is_ifreq: %d, rflm_mode: %d",
    tds_rxlm_mode, is_ifreq, rflm_mode);
  }

  return rflm_mode;
}


void rf_tdscdma_mc_update_rflm_lna_struct
(
  rf_path_enum_type path,
  uint16 rx_channel,
  rfcom_tdscdma_band_type band,
  rfa_tdscdma_rxlm_mode_t rxlm_mode,
  rflm_cmn_rxagc_static_data_t *data_for_cells,
  uint8 lin_state
)
{
  rfnv_tdscdma_rx_tbl_type* tds_rxnv_ptr;
  rf_tdscdma_mdsp_rx_swpt_pair_type tds_rx_swpt[2] = {{NULL}, {NULL}};
  rfcom_tdscdma_band_type full_band = band;
  int8 index = 0, loop = 0;
  int8 rise_gain_idx = 0;
  int8 fall_gain_idx = 0;
  uint8 i=0;
  uint8 j=0;
  int* riseThreshold;
  int* fallThreshold;

  if (path == RF_PATH_0 || path == RF_PATH_1)
  {
    /* Initialize nv table pointer */

    // If input band is a split band, convert it to full band
    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "Assigning nv pointer for band %d", band);
    /* Get the full band from split band. If not a split band, this will not change */
    rf_tdscdma_core_get_fullband_from_subband(&full_band);
    if (full_band != band)
    {
      MSG_1(MSG_SSID_RF_DEBUG, MSG_LEGACY_HIGH, "Full band is %d, assigning NVs from full band", full_band);
    }
    /* Assign nv_tbl_ptr for the fullband */
    tds_rxnv_ptr = rf_tdscdma_core_util_init_rx_nv_table(full_band);

    switch (rxlm_mode)
    {
    case RFA_TDSCDMA_RXLM_MODE_ACQ:
      tds_rx_swpt[0].nb_ptr = tds_rxnv_ptr->lna_range_rise_fall_nb_mode_acq;
      tds_rx_swpt[0].wb_ptr = tds_rxnv_ptr->lna_range_rise_fall_wb_mode_acq;
      break;

    case RFA_TDSCDMA_RXLM_MODE_TRK:
      tds_rx_swpt[0].nb_ptr = tds_rxnv_ptr->lna_range_rise_fall_nb_mode_tracking;
      tds_rx_swpt[0].wb_ptr = tds_rxnv_ptr->lna_range_rise_fall_wb_mode_tracking;
      break;

    case RFA_TDSCDMA_RXLM_MODE_TRK_IDLE:
      tds_rx_swpt[0].nb_ptr = tds_rxnv_ptr->lna_range_rise_fall_nb_mode_idle;
      tds_rx_swpt[0].wb_ptr = tds_rxnv_ptr->lna_range_rise_fall_wb_mode_idle;
      break;

    case RFA_TDSCDMA_RXLM_MODE_TRK_IRAT:
      tds_rx_swpt[0].nb_ptr = tds_rxnv_ptr->lna_range_rise_fall_nb_mode_ifreq;
      tds_rx_swpt[0].wb_ptr = tds_rxnv_ptr->lna_range_rise_fall_wb_mode_ifreq;
      break;

    case RFA_TDSCDMA_RXLM_MODE_PWR_SCAN:
      tds_rx_swpt[0].nb_ptr = tds_rxnv_ptr->lna_range_rise_fall_nb_mode_power_scan;
      tds_rx_swpt[0].wb_ptr = tds_rxnv_ptr->lna_range_rise_fall_wb_mode_power_scan;
      break;

    default:
      MSG_1(MSG_SSID_RF, MSG_LVL_FATAL, "wrong rxlm_mode %d in rf_tdscdma_mc_update_rflm_lna_struct", rxlm_mode);
      break;
    }
  }

  /* ******************************************************************
  **   The mapping between NV data structure (12 items each) and swpt
  **
  **	 NV[0] = G0->G1;  NV[1] = G0->G2;  NV[2] = G0->G4;
  **	 NV[3] = G1->G0;  NV[4] = G1->G2;  NV[5] = G1->G3;
  **	 NV[6] = G2->G0;  NV[7] = G2->G1;  NV[8] = G2->G3;
  **	 NV[9] = G3->G0; NV[10] = G3->G1; NV[11] = G3->G2;
  **
  **   In the other format,
  **
  **	 rise_thresholds[0] <=> G0->G1 <=> NV[0]
  **	 rise_thresholds[1] <=> G0->G2 <=> NV[1]
  **	 rise_thresholds[2] <=> G0->G3 <=> NV[2]
  **	 rise_thresholds[3] <=> G1->G2 <=> NV[4]
  **	 rise_thresholds[4] <=> G1->G3 <=> NV[5]
  **	 rise_thresholds[5] <=> G2->G3 <=> NV[8]
  **
  **	 fall_thresholds[0] <=> G1->G0 <=> NV[3]
  **	 fall_thresholds[1] <=> G2->G0 <=> NV[6]
  **	 fall_thresholds[2] <=> G2->G1 <=> NV[7]
  **	 fall_thresholds[3] <=> G3->G0 <=> NV[9]
  **	 fall_thresholds[4] <=> G3->G1 <=> NV[10]
  **	 fall_thresholds[5] <=> G3->G2 <=> NV[11]
  **
  ** ******************************************************************/

  for (index=0; index<RF_TDSCDMA_MAX_LNA_GAIN_STATES; index++)
  {
    for (rise_gain_idx = index; rise_gain_idx < (RF_TDSCDMA_MAX_LNA_GAIN_STATES-1); rise_gain_idx++)
    {
      // Rise gain index: 01, 02, 03, 12, 13, 23
      if (i < RFLM_CMN_RXAGC_MAX_LNA_STATES) // To suppress KW warnings
      {
        data_for_cells->cell_static_info.rise_thresholds[i] =
        rf_tdscdma_mdsp_smem_rxagc_convert(tds_rx_swpt[0].wb_ptr[index*(RF_TDSCDMA_MAX_LNA_GAIN_STATES-1)+rise_gain_idx]);

        data_for_cells->cell_static_info.rise_thresholds_nb[i] =
        rf_tdscdma_mdsp_smem_rxagc_convert(tds_rx_swpt[0].nb_ptr[index*(RF_TDSCDMA_MAX_LNA_GAIN_STATES-1)+rise_gain_idx]);

        if (rf_tdscdma_mc_rxagc_debug)
        {
          MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "Rise (before conv.) : WB SWPT %d, NB SWPT: %d",
          tds_rx_swpt[0].wb_ptr[index*(RF_TDSCDMA_MAX_LNA_GAIN_STATES-1)+rise_gain_idx],
          tds_rx_swpt[0].nb_ptr[index*(RF_TDSCDMA_MAX_LNA_GAIN_STATES-1)+rise_gain_idx]);

          MSG_4(MSG_SSID_RF, MSG_LEGACY_HIGH, "i: %d, index: %d, rise_gain_idx: %d, ptr_idx: %d, ",
          i, index, rise_gain_idx, index*(RF_TDSCDMA_MAX_LNA_GAIN_STATES-1)+rise_gain_idx );

          MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "Rise (after conv.) : WB SWPT %d, NB SWPT: %d",
          data_for_cells->cell_static_info.rise_thresholds[i],
          data_for_cells->cell_static_info.rise_thresholds_nb[i]);
        }
      }


      i++;
    }

    for (fall_gain_idx = 0; fall_gain_idx < index; fall_gain_idx++)
    {
      // Fall gain index: 10, 20, 21, 30, 31, 32
      if (j < RFLM_CMN_RXAGC_MAX_LNA_STATES) // To suppress KW warnings
      {
        data_for_cells->cell_static_info.fall_thresholds[j] =
        rf_tdscdma_mdsp_smem_rxagc_convert(tds_rx_swpt[0].wb_ptr[index*(RF_TDSCDMA_MAX_LNA_GAIN_STATES-1)+fall_gain_idx]);

        data_for_cells->cell_static_info.fall_thresholds_nb[j] =
        rf_tdscdma_mdsp_smem_rxagc_convert(tds_rx_swpt[0].nb_ptr[index*(RF_TDSCDMA_MAX_LNA_GAIN_STATES-1)+fall_gain_idx]);

        if (rf_tdscdma_mc_rxagc_debug)
        {
          MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "Fall (before conv.): WB SWPT %d, NB SWPT: %d",
          tds_rx_swpt[0].wb_ptr[index*(RF_TDSCDMA_MAX_LNA_GAIN_STATES-1)+fall_gain_idx],
          tds_rx_swpt[0].nb_ptr[index*(RF_TDSCDMA_MAX_LNA_GAIN_STATES-1)+fall_gain_idx]);

          MSG_4(MSG_SSID_RF, MSG_LEGACY_HIGH, "j: %d, index: %d, fall_gain_idx: %d, ptr_idx: %d, ",
          j, index, fall_gain_idx, index*(RF_TDSCDMA_MAX_LNA_GAIN_STATES-1)+fall_gain_idx );

          MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "Fall (after conv.): WB SWPT %d, NB SWPT: %d",
          data_for_cells->cell_static_info.fall_thresholds[j],
          data_for_cells->cell_static_info.fall_thresholds_nb[j]);
        }
      }

      j++;
    }
  }

  /* RxAGC SWPT validity check */
  for (loop = 0; loop<2; loop++)
  {
    if (loop == 0)
    {
      // Check NB swpt
      riseThreshold = data_for_cells->cell_static_info.rise_thresholds_nb;
      fallThreshold = data_for_cells->cell_static_info.fall_thresholds_nb;
      if (rf_tdscdma_mc_rxagc_debug)
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "NB SWPT check:", 0);
      }
    }
    else
    {
      // Check WB swpt
      riseThreshold = data_for_cells->cell_static_info.rise_thresholds;
      fallThreshold = data_for_cells->cell_static_info.fall_thresholds;
      if (rf_tdscdma_mc_rxagc_debug)
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "WB SWPT check:", 0);
      }
    }

    if ((riseThreshold[RFLM_RXAGC_TDS_FALL_G0_TO_G3] < riseThreshold[RFLM_RXAGC_TDS_FALL_G0_TO_G2]) ||
    (riseThreshold[RFLM_RXAGC_TDS_FALL_G0_TO_G2] < riseThreshold[RFLM_RXAGC_TDS_FALL_G0_TO_G1])  )
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "For G0 Threshold, not in a strict descending order!", 0);
      return;
    }

    if ((riseThreshold[RFLM_RXAGC_TDS_FALL_G1_TO_G3] < riseThreshold[RFLM_RXAGC_TDS_FALL_G1_TO_G2]) ||
    (riseThreshold[RFLM_RXAGC_TDS_FALL_G1_TO_G2] < fallThreshold[RFLM_RXAGC_TDS_RISE_G1_TO_G0])  )
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "For G1 Threshold, not in a strict descending order!", 0);
      return;
    }

    if ((riseThreshold[RFLM_RXAGC_TDS_FALL_G2_TO_G3] < fallThreshold[RFLM_RXAGC_TDS_RISE_G2_TO_G1]) ||
    (fallThreshold[RFLM_RXAGC_TDS_RISE_G2_TO_G1] < fallThreshold[RFLM_RXAGC_TDS_RISE_G2_TO_G0])  )
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "For G2 Threshold, not in a strict descending order!", 0);
      return;
    }

    if ((fallThreshold[RFLM_RXAGC_TDS_RISE_G3_TO_G2] < fallThreshold[RFLM_RXAGC_TDS_RISE_G3_TO_G1]) ||
    (fallThreshold[RFLM_RXAGC_TDS_RISE_G3_TO_G1] < fallThreshold[RFLM_RXAGC_TDS_RISE_G3_TO_G0])  )
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "For G3 Threshold, not in a strict descending order!", 0);
      return;
    }
  } /* Loop WB and NB */


  /* Need to check rf_tdscdma_rxfreq_comp's parameter to make sure
   the current frequency has been calculated */
  if ((rf_tdscdma_rxfreq_comp.last_chain_checked == path )             &&
      (rf_tdscdma_rxfreq_comp.last_band_checked[path] == band)         &&
      (rf_tdscdma_rxfreq_comp.last_chan_checked[path] == rx_channel)    )
  {    
    for (index=0; index< RF_TDSCDMA_MAX_LNA_GAIN_STATES ; index++)
    {
      data_for_cells->cell_static_info.lna_gain_offsets[index] =
      ((((int32)rf_tdscdma_rxfreq_comp.rx_gain_vs_freq[path][index][lin_state])<<8)/10);
    
      if (rf_tdscdma_mc_rxagc_debug)
      {
        MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, "rflm_lna_struct, lna_gain_offsets[%d]: %d",
        index, data_for_cells->cell_static_info.lna_gain_offsets[index]);
      }
    }  
  }
  else
  {
    MSG_4(MSG_SSID_RF, MSG_LVL_FATAL, "rf_tdscdma_mc_update_rflm_lna_struct: the rxfreq compensation has not been calculated yet for path %d, band %d, channel %d and rxlm mode %d",
    path, band, rx_channel, rxlm_mode);
  }

  MSG_4(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mc_update_rflm_lna_struct: path: %d, rxlm_mode: %d, band : %d, channel: %d",
  path, rxlm_mode, band, rx_channel);
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Allocate all the buffers when doing initialization for TDS

  @details


  @return

*/
void rf_tdscdma_mc_rflm_init_rxagc(rflm_dm_handle_id_t rxagc_handle)
{
  uint8 i;
  uint8 num_valid_cells = 0;
  rflm_rxagc_cmn_add_delete_cells_t info_for_add_cells;

  rflm_dm_get_rxagc_data(rxagc_handle, RFLM_CMN_RXAGC_GET_NUM_VALID_CELLS, &num_valid_cells);

  if (num_valid_cells >= RFLM_CMN_RXAGC_MAX_CELLS_PER_WB_BUFFER)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,
    "Cells in this handle have already been allocated for Handle: %d, Valid Cells: %d",
    rxagc_handle, num_valid_cells);
  }
  else
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH,
    "Add %d cells for TDS, Handle %d, Current valid Cells: %d (before adding)",
    RFLM_CMN_RXAGC_MAX_CELLS_PER_WB_BUFFER, rxagc_handle, num_valid_cells);
    info_for_add_cells.num_cells = RFLM_CMN_RXAGC_MAX_CELLS_PER_WB_BUFFER;

    /* Note that right now we use rxlm_mode to indentify the cell id here, because TDS RxAGC does not care about frequency */
    for (i=0; i<RFLM_CMN_RXAGC_MAX_CELLS_PER_WB_BUFFER; i++)
    {
      /*cell id is ascending from 0 to 11*/
      info_for_add_cells.cell_ids[i] = i;
    }

    info_for_add_cells.use_events = 0;
    rflm_dm_set_rxagc_data(rxagc_handle,
    RFLM_CMN_RXAGC_ADD_CELLS,
    (void*) &info_for_add_cells );
  }
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Called to update TDSCDMA RX AGC information for FED

  @details


  @return

*/
void rf_tdscdma_mc_update_rflm_rxagc
(
  rfm_device_enum_type device,
  rf_path_enum_type path,
  rfcom_tdscdma_band_type band,
  uint16 rx_channel,
  lm_handle_type rxlm_buffer_idx,
  rfa_tdscdma_rxlm_mode_t rxlm_mode,
  boolean is_ifreq,
  uint8 ifreq_index,
  uint8 buf_id,
  boolean is_irat
)
{
  uint8 index, lin_state, des_lin_state, lin_max;
  rflm_dm_handle_id_t rxagc_handle;
  rflm_cmn_rxagc_static_data_t data_for_cells[2] = {{0},{0}};
  rflm_tds_rx_mode_e rflm_mode;
  des_lin_state = 0; //KW fix

  /* Allocate DM handle */
  /* Assuming the design is: RxAGC Common buffer index align with RxLm buffer index */
  rxagc_handle = (rflm_dm_handle_id_t)rxlm_buffer_idx;

  /*--------------------------------------------------------------------------*/
  /* Step 0: Convert RxLM mode to RFLM mode 								  */
  rflm_mode = rf_tdscdna_mc_rxagc_convert_rxlm_mode(rxlm_mode, is_ifreq);
  if (rf_tdscdma_mc_rxagc_debug)
  {
    MSG_5(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mc_update_rflm_rxagc, "
    "rxlm_mode: %d, rflm_mode: %d, is_ifreq: %d, ifreq_index: %d, buf_id: %d",
    rxlm_mode, rflm_mode, is_ifreq, ifreq_index, buf_id);
  }

  /*--------------------------------------------------------------------------*/
  /* Step 1: Specify which cell to update   								  */
  /*		 Note: use RFLM mode to find the cell   						  */
  data_for_cells[0].cell_id = rflm_tds_rxagc_find_cell_id(rflm_mode, ifreq_index);
  if (rf_tdscdma_mc_rxagc_debug)
  {
    MSG_4(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mc_update_rflm_rxagc, cell_id: %d, "
          "rflm_mode: %d, ifreq_index: %d, is_ifreq: %d",
          data_for_cells[0].cell_id , rflm_mode, ifreq_index, is_ifreq);
  }

  /*--------------------------------------------------------------------------*/
  /* Step 2: Populate static data (e.g., swpt and LNA offset) for each cell   */
  /*		 Note: use RxLM mode for population 							  */
 
  if (rflmTdsRfModeCtlState.jdetInfo.enabled)
  {
    lin_max = (uint8) RFLM_TDS_NUM_LIN_STATES;
  }
  else
  {
    lin_max = (uint8) RFLM_TDS_RX_LIN_STATE_DEFAULT + 1;
    des_lin_state = (uint8) RFLM_TDS_RX_LIN_STATE_DEFAULT;
  }

  for (lin_state=0; lin_state<lin_max; lin_state++)
  {
    rf_tdscdma_mc_update_rflm_lna_struct(path, rx_channel, band, rxlm_mode, &(data_for_cells[lin_state]), lin_state);
  }

  /* Register ptr and lna offset in rflm data structure */
  if (is_irat)
  {
    for (lin_state=0; lin_state <lin_max; lin_state++)
    {    
      // IRAT
      rflmTdsRfModeCtlState.rxAgc[lin_state].irat.rxLmHandle = rxlm_buffer_idx;

      if (rflm_mode >= RFLM_TDS_MODE_INVALID)
      {
        MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,
              "rf_tdscdma_mc_update_rflm_rxagc wrong rflm_mode: %d, rxlm_mode: %d",
              rflm_mode, 
              rxlm_mode);
        return;
      }

      for (index=0; index<RFLM_CMN_RXAGC_MAX_LNA_STATES; index++)
      {
        rflmTdsRfModeCtlState.rxAgc[lin_state].irat.swpt[rflm_mode][RFLM_TDS_NB_PWR_EST].riseThreshold[index] 
        = data_for_cells[lin_state].cell_static_info.rise_thresholds_nb[index];
        rflmTdsRfModeCtlState.rxAgc[lin_state].irat.swpt[rflm_mode][RFLM_TDS_NB_PWR_EST].fallThreshold[index] 
        = data_for_cells[lin_state].cell_static_info.fall_thresholds_nb[index];
        rflmTdsRfModeCtlState.rxAgc[lin_state].irat.swpt[rflm_mode][RFLM_TDS_WB_PWR_EST].riseThreshold[index] 
        = data_for_cells[lin_state].cell_static_info.rise_thresholds[index];
        rflmTdsRfModeCtlState.rxAgc[lin_state].irat.swpt[rflm_mode][RFLM_TDS_WB_PWR_EST].fallThreshold[index] 
        = data_for_cells[lin_state].cell_static_info.fall_thresholds[index];
      }
      if (rf_tdscdma_mc_rxagc_debug)
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mc_update_rflm_rxagc: scriptPtr 0x%x",
              data_for_cells[lin_state].cell_static_info.rise_thresholds_nb);
      }

      // lna offset
      for (index=0; index< RF_TDSCDMA_MAX_LNA_GAIN_STATES ; index++)
      {
        rflmTdsRfModeCtlState.rxAgc[lin_state].irat.lnaOffset[index] = 
          data_for_cells[lin_state].cell_static_info.lna_gain_offsets[index];
      }
    }
  }
  else
  {
    if (is_ifreq)
    {
      for (lin_state=0; lin_state <lin_max; lin_state++)
      {
        // IFREQ
        rflmTdsRfModeCtlState.rxAgc[lin_state].ifreq.rxLmHandle[path] = rxlm_buffer_idx;
      
        for (index=0; index<RFLM_CMN_RXAGC_MAX_LNA_STATES; index++)
        {
          rflmTdsRfModeCtlState.rxAgc[lin_state].ifreq.swpt[ifreq_index][RFLM_TDS_NB_PWR_EST].riseThreshold[index] 
          = data_for_cells[lin_state].cell_static_info.rise_thresholds_nb[index];
          rflmTdsRfModeCtlState.rxAgc[lin_state].ifreq.swpt[ifreq_index][RFLM_TDS_NB_PWR_EST].fallThreshold[index] 
          = data_for_cells[lin_state].cell_static_info.fall_thresholds_nb[index];
          rflmTdsRfModeCtlState.rxAgc[lin_state].ifreq.swpt[ifreq_index][RFLM_TDS_WB_PWR_EST].riseThreshold[index] 
          = data_for_cells[lin_state].cell_static_info.rise_thresholds[index];
          rflmTdsRfModeCtlState.rxAgc[lin_state].ifreq.swpt[ifreq_index][RFLM_TDS_WB_PWR_EST].fallThreshold[index] 
          = data_for_cells[lin_state].cell_static_info.fall_thresholds[index];
        }
        if (rf_tdscdma_mc_rxagc_debug)
        {
          MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mc_update_rflm_rxagc: scriptPtr 0x%x",
                data_for_cells[lin_state].cell_static_info.rise_thresholds_nb);
        }

        // lna offset
        for (index=0; index< RF_TDSCDMA_MAX_LNA_GAIN_STATES ; index++)
        {
          rflmTdsRfModeCtlState.rxAgc[lin_state].ifreq.lnaOffset[ifreq_index][path][index] = 
            data_for_cells[lin_state].cell_static_info.lna_gain_offsets[index];
        }
      }
    }
    else
    {
      for (lin_state=0; lin_state <lin_max; lin_state++)
      {
        // Home/Target
        rflmTdsRfModeCtlState.rxAgc[lin_state].home[buf_id].rxLmHandle[path] = rxlm_buffer_idx;
        rflmTdsRfModeCtlState.rxAgc[lin_state].home[buf_id].homeTargetScriptBufIndex = buf_id;

        if (rflm_mode >= RFLM_TDS_MODE_INVALID)
        {
          MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,
                "rf_tdscdma_mc_update_rflm_rxagc wrong rflm_mode: %d, rxlm_mode: %d",
                rflm_mode, 
                rxlm_mode);
          return;
        }

        for (index=0; index<RFLM_CMN_RXAGC_MAX_LNA_STATES; index++)
        {
          rflmTdsRfModeCtlState.rxAgc[lin_state].home[buf_id].swpt[rflm_mode][RFLM_TDS_NB_PWR_EST].riseThreshold[index] 
          = data_for_cells[lin_state].cell_static_info.rise_thresholds_nb[index];
          rflmTdsRfModeCtlState.rxAgc[lin_state].home[buf_id].swpt[rflm_mode][RFLM_TDS_NB_PWR_EST].fallThreshold[index] 
          = data_for_cells[lin_state].cell_static_info.fall_thresholds_nb[index];
          rflmTdsRfModeCtlState.rxAgc[lin_state].home[buf_id].swpt[rflm_mode][RFLM_TDS_WB_PWR_EST].riseThreshold[index] 
          = data_for_cells[lin_state].cell_static_info.rise_thresholds[index];
          rflmTdsRfModeCtlState.rxAgc[lin_state].home[buf_id].swpt[rflm_mode][RFLM_TDS_WB_PWR_EST].fallThreshold[index] 
          = data_for_cells[lin_state].cell_static_info.fall_thresholds[index];

          /* Special processing to save RxAGC timeline */
          // RFLM_TDS_ACQ_MODE and RFLM_TDS_STEP0_FAIL_MODE share the same swpt
          // RFLM_TDS_TRK_MODE and RFLM_TDS_TRK_IDLE_MODE share the same swpt
          if (rflm_mode == RFLM_TDS_ACQ_MODE)
          {
            rflmTdsRfModeCtlState.rxAgc[lin_state].home[buf_id].swpt[RFLM_TDS_STEP0_FAIL_MODE][RFLM_TDS_NB_PWR_EST].riseThreshold[index] 
            = data_for_cells[lin_state].cell_static_info.rise_thresholds_nb[index];
            rflmTdsRfModeCtlState.rxAgc[lin_state].home[buf_id].swpt[RFLM_TDS_STEP0_FAIL_MODE][RFLM_TDS_NB_PWR_EST].fallThreshold[index] 
            = data_for_cells[lin_state].cell_static_info.fall_thresholds_nb[index];
            rflmTdsRfModeCtlState.rxAgc[lin_state].home[buf_id].swpt[RFLM_TDS_STEP0_FAIL_MODE][RFLM_TDS_WB_PWR_EST].riseThreshold[index] 
            = data_for_cells[lin_state].cell_static_info.rise_thresholds[index];
            rflmTdsRfModeCtlState.rxAgc[lin_state].home[buf_id].swpt[RFLM_TDS_STEP0_FAIL_MODE][RFLM_TDS_WB_PWR_EST].fallThreshold[index] 
            = data_for_cells[lin_state].cell_static_info.fall_thresholds[index];
          }

          if (rflm_mode == RFLM_TDS_TRK_MODE)
          {
            rflmTdsRfModeCtlState.rxAgc[lin_state].home[buf_id].swpt[RFLM_TDS_TRK_IDLE_MODE][RFLM_TDS_NB_PWR_EST].riseThreshold[index] 
            = data_for_cells[lin_state].cell_static_info.rise_thresholds_nb[index];
            rflmTdsRfModeCtlState.rxAgc[lin_state].home[buf_id].swpt[RFLM_TDS_TRK_IDLE_MODE][RFLM_TDS_NB_PWR_EST].fallThreshold[index] 
            = data_for_cells[lin_state].cell_static_info.fall_thresholds_nb[index];
            rflmTdsRfModeCtlState.rxAgc[lin_state].home[buf_id].swpt[RFLM_TDS_TRK_IDLE_MODE][RFLM_TDS_WB_PWR_EST].riseThreshold[index] 
            = data_for_cells[lin_state].cell_static_info.rise_thresholds[index];
            rflmTdsRfModeCtlState.rxAgc[lin_state].home[buf_id].swpt[RFLM_TDS_TRK_IDLE_MODE][RFLM_TDS_WB_PWR_EST].fallThreshold[index] 
            = data_for_cells[lin_state].cell_static_info.fall_thresholds[index];
          }

          if (rflm_mode == RFLM_TDS_TRK_IDLE_MODE)
          {
            rflmTdsRfModeCtlState.rxAgc[lin_state].home[buf_id].swpt[RFLM_TDS_TRK_MODE][RFLM_TDS_NB_PWR_EST].riseThreshold[index] 
            = data_for_cells[lin_state].cell_static_info.rise_thresholds_nb[index];
            rflmTdsRfModeCtlState.rxAgc[lin_state].home[buf_id].swpt[RFLM_TDS_TRK_MODE][RFLM_TDS_NB_PWR_EST].fallThreshold[index] 
            = data_for_cells[lin_state].cell_static_info.fall_thresholds_nb[index];
            rflmTdsRfModeCtlState.rxAgc[lin_state].home[buf_id].swpt[RFLM_TDS_TRK_MODE][RFLM_TDS_WB_PWR_EST].riseThreshold[index] 
            = data_for_cells[lin_state].cell_static_info.rise_thresholds[index];
            rflmTdsRfModeCtlState.rxAgc[lin_state].home[buf_id].swpt[RFLM_TDS_TRK_MODE][RFLM_TDS_WB_PWR_EST].fallThreshold[index]
            = data_for_cells[lin_state].cell_static_info.fall_thresholds[index];
          }
        }

        // lna offset
        for (index=0; index< RF_TDSCDMA_MAX_LNA_GAIN_STATES ; index++)
        {
          rflmTdsRfModeCtlState.rxAgc[lin_state].home[buf_id].lnaOffset[path][index] = 
          data_for_cells[lin_state].cell_static_info.lna_gain_offsets[index];
        }
      }
    } /* if (if_ifreq) */
  } /* if(is_irat) */

  rflm_dm_set_rxagc_data((rflm_dm_handle_id_t)rxagc_handle,
                          RFLM_CMN_RXAGC_UPDATE_CELL_STATIC_INFO,
                         (void*) &(data_for_cells[0]) );

  if (rflmTdsRfModeCtlState.jdetInfo.enabled)
  {
    rflm_dm_set_rxagc_data((rflm_dm_handle_id_t)rxagc_handle,
                            RFLM_CMN_RXAGC_UPDATE_CELL_STATIC_INFO,
                            (void*) &(data_for_cells[1]) );
  }

  /*--------------------------------------------------------------------------*/
  /* Step 4: Latch the buffer   											  */
  /*  - this indicates that all configuration-related operations completed    */
  rflm_dm_set_rxagc_data((rflm_dm_handle_id_t)rxagc_handle,
                         RFLM_CMN_RXAGC_LATCH_WB_BUF_INFO,
                         NULL);

  MSG_4(MSG_SSID_RF, MSG_LEGACY_HIGH, "rf_tdscdma_mc_update_rflm_rxagc, "
        "device: %d, path: %d, band: %d, rx_channel: %d, ",
        device, path, band, rx_channel);
}



boolean rf_tdscdma_mc_is_coex_backoff_req (rfm_device_enum_type rfm_dev)

{
  boolean is_req = FALSE;

  if (rf_tdscdma_mc_state.curr_pri_device==rfm_dev)
  {
    if (rf_tdscdma_coex_tx_pwr_limit < 300)
    {
      is_req = TRUE;
      MSG_3(MSG_SSID_RF_DEBUG, MSG_LEGACY_HIGH,"rf_tdscdma_mc_is_coex_backoff_req: is_req=%d for device(%d), rf_tdscdma_coex_tx_pwr_limit=%d",is_req,rfm_dev, rf_tdscdma_coex_tx_pwr_limit);
    }
  }
  return (is_req);
}

#ifdef FEATURE_RF_HAS_QTUNER
/*!
  @brief
  This function is to build dynamic tuner scripts for ifreq meas


  @details
  Build dynamic tuner scripts for ifreq meas and populated to FW


  @param ifreq_meas_freq
  The frequency of the coming inter-frequency measurement.

  @param ifreq_buf_idx
  Ping-pong buffer index. Each time TL1 calls RFSW to build the scripts,
  the buffer index is toggled.

  @return
  rf_tdscdma_mc_status_type status
*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_dynamic_script(uint32 ifreq_meas_freq, lm_handle_type ifreq_buf_idx)
{
  rf_time_tick_type tuner_dynamic_script_start_t = {0};
  rf_time_type tuenr_dynamic_script_time;
  rfcom_tdscdma_band_type ifreq_band;
  rftdscdma_mdsp_common_event_data_type rf_tds_build_event_data;
  uint16 ifreq_chan;

  if(rf_tdscdma_mc_display_time_profile)
  {
    tuner_dynamic_script_start_t = rf_time_get_tick();
  }

  ifreq_chan = rf_tdscdma_core_util_get_uarfcn_from_freq(ifreq_meas_freq/1000, 0);
  ifreq_band = rf_tdscdma_core_util_get_band_from_uarfcn(ifreq_chan);
  MSG_4(MSG_SSID_RF, MSG_LVL_HIGH, "rf_tdscdma_mc_ifreq_dynamic_script(ifreq_band=%d, ifreq_channel=%d, ifreq_buf_idx=%d, ifreq_measured_freq=%d)", ifreq_band, ifreq_chan, ifreq_buf_idx, ifreq_meas_freq);

  if (ifreq_band == RFCOM_BAND_TDSCDMA_INVALID || ifreq_buf_idx > 1 )
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR,"rf_tdscdma_mc_ifreq_dynamic_script: wrong ifreq channel or ifreq buf idx");
    return RF_TDSCDMA_MC_INVALID_PARAM;
  }

  rf_tds_build_event_data.pri_device = rf_tdscdma_mc_state.curr_pri_device;
  rf_tds_build_event_data.sec_device = rf_tdscdma_mc_state.curr_sec_device;
  rf_tds_build_event_data.tx_device = rf_tdscdma_mc_state.curr_pri_device;
  rf_tds_build_event_data.band = ifreq_band;
  rf_tds_build_event_data.channel = ifreq_chan;
  rf_tds_build_event_data.drx_enable = rf_tdscdma_mc_state.div_enable; //This one might cause re-configuration to enable DRx!
  rf_tds_build_event_data.buf_id = ifreq_buf_idx;
  rf_tds_build_event_data.is_bho_update = FALSE;

  rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_DYNAMIC_SCRIPT, &rf_tds_build_event_data);

  if(rf_tdscdma_mc_display_time_profile)
  {
    tuenr_dynamic_script_time = rf_time_get_elapsed(tuner_dynamic_script_start_t, RF_USEC);
    MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, "Build IFREQ dynamic_script time profile: %d usec", tuenr_dynamic_script_time);
  }

  return RF_TDSCDMA_MC_SUCCESS;
}

#endif /* FEATURE_RF_HAS_QTUNER */


rf_tdscdma_mc_status_type rf_tdscdma_mc_rxd_override
( 
  boolean rxd_override
)
{
  MSG_2(MSG_SSID_RF_DEBUG, MSG_LVL_HIGH, 
        "rf_tdscdma_mc_rxd_override(rxd_override=%d, current FED RxD Override state=%d)", 
        rxd_override, 
        rflmTdsRfModeCtlState.rxdEnOverride);

  if(rf_tdscdma_mc_state.div_enable)
  {
    rflmTdsRfModeCtlState.rxdEnOverride = rxd_override;    
  }
  else
  {
    MSG(MSG_SSID_RF, MSG_LVL_ERROR, 
          "RxD has not been enabled! RxD override for CMAPI Type 4 has been rejected!!");
  }
  
  MSG_2(MSG_SSID_RF, MSG_LVL_HIGH, 
        "rf_tdscdma_mc_rxd_override(rxd_override=%d, current FED RxD Override state=%d)", 
        rxd_override, 
        rflmTdsRfModeCtlState.rxdEnOverride);

  return RF_TDSCDMA_MC_SUCCESS;
}


/*!
  @brief
  This function is used to trigger a port switch

  @param port
  Port to be switched 

  @return
  API pass/fail status
*/
boolean rf_tdscdma_mc_dynamic_port_switch(uint8 port)
{
  rftdscdma_mdsp_common_event_data_type rf_tds_build_event_data;

  if (port >= TDS_PORT_MAX)
  {
    MSG_1(MSG_SSID_RF_DEBUG, MSG_LVL_HIGH, "Dynamic port switch error. Bad argument: port: %d", port);
    return FALSE;
  }

  rf_tds_build_event_data.pri_device = rf_tdscdma_mc_state.curr_pri_device;
  rf_tds_build_event_data.sec_device = rf_tdscdma_mc_state.curr_sec_device;
  rf_tds_build_event_data.tx_device = rf_tdscdma_mc_state.curr_pri_device;
  rf_tds_build_event_data.band = rf_tdscdma_mc_state.curr_band_rx;
  rf_tds_build_event_data.channel = rf_tdscdma_mc_state.curr_chan_rx;
  rf_tds_build_event_data.buf_id = rf_tdscdma_mc_dbl_bufs.rx_wrk_buf_idx;

  if ((rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_BOTH) ||
  (rf_tdscdma_mc_state.curr_rx_path == RFA_TDSCDMA_RX_PATH_DIV)   )
  {
    rf_tds_build_event_data.drx_enable = TRUE;
  }
  else
  {
    rf_tds_build_event_data.drx_enable = FALSE;
  }

  rf_tds_build_event_data.update_type = TASKNUN_UPDATE_TYPE_MAX; /* unused */
  rf_tds_build_event_data.switch_state = port;
  rf_tds_build_event_data.pri_rxlm_buffer_index = 0xFFFF; /* unused */
  rf_tds_build_event_data.sec_rxlm_buffer_index = 0xFFFF; /* unused */
  rf_tds_build_event_data.txlm_buffer_index = 0xFFFF; /* unused */
  rf_tds_build_event_data.path = RF_PATH_0;
  rf_tds_build_event_data.data_ptr = NULL;
  rf_tds_build_event_data.is_irat = FALSE;
  rf_tds_build_event_data.is_bho_update = FALSE;

  return (rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_DYNAMIC_UPDATE, &rf_tds_build_event_data));

}

/*!
  @brief
  This function is used by TL1 to query TRF for sawless support

  @param 

  @return
  TRUE if sawless support is present
 
*/
boolean rf_tdscdma_mc_is_rx_sawless(void)
{
  boolean status = FALSE;
  status = prx_port_is_sawless;
  is_sawless_enabled = status;

  MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, "rf_tdscdma_mc_is_rx_sawless port: status =%d", status);

  return (status);
}

/*!
  @brief
  This function is used to return current common device pointer

  @param

  @return
  current common device pointer
*/
rfdevice_rxtx_common_class *rf_tdscdma_mc_get_curr_common_device(rfm_device_enum_type device,
                                                                 uint16 band)
{
  rfdevice_rxtx_common_class* cmn_device_ptr = NULL;
  rfdevice_trx_tdscdma_tx** tds_device = NULL;
  uint8 device_idx = 0;

  tds_device = (rfdevice_trx_tdscdma_tx**)rfc_tdscdma_get_rf_device_obj(device,
                                                                        RFC_CONFIG_TX,
                                                                        band,
                                                                        RFDEVICE_TRANSCEIVER);

  if (tds_device != NULL)
  {
    while (tds_device[device_idx] != NULL)
    {
      cmn_device_ptr = rfdevice_tdscdma_tx_get_common_device_pointer(tds_device[device_idx]);
      device_idx++;
    }
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "rf_tdscdma_mc_get_curr_common_device: Null pointer detected. device %d, band %d",
          device, band);
  }

  return cmn_device_ptr;
}

