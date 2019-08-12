/*!
  @file
  rfm_1x_ftm.c

  @brief
  This file defines the RFM-1X interfaces to be used by <b>FTM module only</b>.

  @addtogroup RF_CDMA_1X_RFM_FTM
  @{
*/

/*==============================================================================

  Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/src/rfm_1x_ftm.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/08/15   rs      KW error fix
04/10/15   vr      Add support for XPT cal when Drx path not present
11/12/14   spa     Change tx_nv_path to be per band per logical device
10/21/14   px      Added RFM Device support for PAPM
09/17/14   JJ      Keep CL override mode on 
08/01/14   cd      Add support for FTM AGC logging with FED and common tx AGC 
                   functionality
08/01/14   aa      Move fbrx rsb comp during ftm_common_fbrx_capture_and_process()
07/09/14   aa      Added RSB IQMC changes for mulitple gain state support
07/03/14   JJ      Update rxlm IQMC settings for re-enable FBRx
06/09/14   JJ      Added an api to re-enable fbrx during internal cal
05/27/14   zhw     Check for Tx state and TXLM handle before querying TxAGC data
04/30/14   sn      updated PA interface
04/10/14   JJ      Changed rfm_1x_ftm_config_fw_state() return param
04/08/14   JJ      Added an api to send oqpch_search_done msg to FW
04/09/14   sn      Updated PA interface
11/08/13   cc      1x logging enhancement
03/28/14   APU     Hook up Tx logging interfaces in SW driver to log data
                   provided by RFLM.
02/20/14   spa     Pass in Rx handle when calling rx gain override
02/06/14   cd      Remove hard-coding to retrieve common device instance
01/21/14   spa     Support for immediate LNA writes in cal (debug only)
12/04/13   sar     Fixed compiler warning.
11/25/13   spa     Use int32 math for LNA offset calculation
11/18/13   cd      Provide interface to process Tx override params for RFLM ovr
11/14/13   cd      Support for RFLM-based Tx AGC override
11/11/13   spa     Send in rxlm handle when calling LNA override
11/11/13   JJ      Added one input param in rfm_1x_ftm_config_fw_state
11/06/13   cd      Populate RFLM handle for RFLM overrides
10/23/13   zhw     Remove PA_ON from txagc_override
10/22/13   zhw     Move tech band assignment to before calling QPOET API
10/21/13   cd      Added functionality for baseband IREF override in XPT Tx ovr
10/16/13   cc      Add new ET logging for FTM AGC logs
10/15/13   zhw     LLVM compiler error fix
10/03/13   APU     Tx override LUT table populated.
10/03/13   APU     Add support for getting new common Tx LUT.
09/23/13   sty     compiler warning fixes
08/15/13   Saul    SB. Made rf_state 1x/hdr specific.
08/12/13   JJ      Added dev ptr check to fix KW errors
08/01/13   Saul    XPT. ET delay vs temp FTM char support
07/19/13   Saul    XPT. ET delay vs freq vs temp support.
07/15/13   cri     Immediate MSM register programming for tx override
07/07/13   sbo     Added BW support for PA device
07/02/13   Saul    XPT. ET delay compensation
06/28/13   JJ      Add API for antenna tuner to update tune code in ftm mode
06/18/13   JJ      Add func to register/de-register fw messages
05/31/13   JJ      Change rf_1x_write_nv() input band type
05/30/13   spa     Mainlined "use_new_fwrsp", use new RF dispatch implementation
05/23/13   JJ      Clean un-necessary cdma sys_band to rf_band_type conversion
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
05/13/13   cd      Added support to override delay value from NV
05/06/13   spa     Updated order of device overrides in fw_tx_agc_override API
                   to QPOET->PA->WTR to prevent QPOET bypass
05/07/13   sty     Use rf_1x_mdsp_get_rx_agc_data() instead of
                   rf_1x_mdsp_get_rx_agc()
05/02/13   APU     Check also for RxDiv during rfm_1x_ftm_get_rx_agc_data()
05/02/13   APU     Fixed a Klockwork Error.
05/01/13   APU     Added check if the device is in the RX state or RXTX state.
04/18/13   aro     Inlined MSGR logging function
04/10/13   aro     Unified LNA calibration setup interface
04/10/13   aro     Added AGC ACQ down counter in RxAGC data
04/10/13   aro     Optimized RxAGC query function
04/05/13   spa     Update callback for fw state config
04/03/13   sn      Cleanup PA inteface
04/02/13   spa     Added NULL pointer check for FW housekeeping data (KW fix)
04/01/13   spa     Migrated to RF Dispatch for RF-FW synchronization
03/28/13   zhw     Cleanup obsolete pa_range interface
03/17/13   zhw     Fix APT value & PA state not being updated in get_tx_agc_data
03/22/13   sr      changes to PAPM device interface.
03/19/13   aro     Deleted ET config path
03/19/13   aro     Modularized FB config into transceiver, asm, and rxlm
03/19/13   aro     Added XPT CDMA cal deconfig
03/19/13   aro     Added call to XPT cal config from protocal IQ capture
03/07/13   sr      changed the "pwr_tracker" name to "papm".
03/01/13   hdz     Vote modem bus clk to 144 MHz for the first trig xPT IQ capture
02/28/13   cd      Added ET mode specific Env scale value override
02/26/13   cd      Added Tx override action type for override from NV
02/21/13   spa     Add API to send pilot meas stop stream to FW (FTM IRAT)
02/19/13   spa     Added rfm_1x_ftm_pilot_meas_cfg API
02/17/13   hdz     Fixed QPOET mode programming sequence when exiting ET mode
02/08/13   cri     Get Tx chain info for Tx override from RFC
01/22/13   aro     Code to use unified IQ capture param
01/22/13   aro     Added common XPT capture type
01/22/13   aro     Removed DPD index from IQ capture interface
01/22/13   zhw     Beautify F3s
01/18/13   aro     Renamed function name to have XPT
01/18/13   aro     Deprecated XPT polling code
01/18/13   aro     Deleted nikel-XPT deprecated FW responses
01/18/13   aro     Removed nikel-XPT specific code
01/18/13   aro     Deleted nikel-XPT specific code
01/04/13   sty     use correct band type
12/27/12   shb     Updates for different physical rx_device per band
12/27/12   cd      Fixed invalid value handling for Tx override parameters
12/21/12   cd      IQ gain and envelope scale overrides are 32 bit values
12/19/12   sty     Reworked rfm_1x_ftm_set_tx_iq_gain to fix KW errors
12/19/12   cd      Fixed compiler warning
12/18/12   cd      Add support for delay value programming to common FW
12/18/12   sty     KW fix - reworked rfm_1x_ftm_set_rf_tx_agc_override
12/11/12   zhw     Rewrite fw_tx_agc_override API for using Stop FW TxAGC
12/10/12   zhw     Fix redundant QPOET write in Cal mode causing bad Tx
12/06/12   APU     Replaced LM chains with Modem chains.
12/04/12   aro     F3 message update
11/28/12   Saul    Sample Capture. Temp features to FEATURE_RF_HAS_XPT_SUPPORT
11/19/12   zhw     Cal mode interface change support for APT in Dime
11/21/12   spa     Added support to set QPOET bias in Tx AGC override
11/19/12   zhw     DIME APT. Fix APT data parsing into right orders.
11/16/12   zhw     DIME APT support. PA quiescent current and bias FTM override
11/16/12   Saul    Sample Capture. Use common response.
11/13/12   Saul    Common XPT. Reverted changes made to commit_dpd_data.
11/09/12   cd      Fixed compiler warning
11/08/12   cd      Fixed header definitions
11/08/12   cd      - Added xPT Tx AGC override
                   - Added config EPT path override
10/25/12   cd/Saul Generalized load DPD table API
                   - Changed request packet type
                   - Added response packet type
                   - Made XPT IQ capture common.
10/03/12   Saul    CDMA Sample Capture. Capture done message.
09/28/12   aro     Migrated 1x MC to use new RF-FW Sync
09/20/12   zhw     PA gain range max limit fix
09/19/12   Saul    CDMA Sample Capture. Capture type, do leakage flag,
                   stop tx agc update fw command
09/05/12   aro     Added RF-FW Sync callback parameter in MDSP Interface
09/04/12   aro     Added band and channel to EPT FB config interface
08/22/12   cri     EPT support for CDMA FW interfaces
08/31/12   Saul    CDMA. Added support for EPT_PROCESSING_DONE FW message
08/28/12   rsr     Added support for TXAGC override.
08/17/12   aro     Added RF-FW Sync support on FWS Config comamnd
08/17/12   aro     F3 message update
08/08/12   vs      Added support to query EPT config params
08/06/12   hdz     Added rfm_1x_ftm_set_tx_pwr_limit()
07/25/12   aro     DPD index in IQ Capture command
07/23/12   Saul    CDMA. Now using semaphore APIs from RF COMMON.
07/20/12   aro     Added support to do polling for EPT expansion
07/17/12   aro     Enhanced support of configure feedback path
06/26/12   aro     F3 message update for uniformity
06/26/12   aro     Deleted unused parameter from EPT configure function
06/26/12   aro     Deleted IQ Capture init function
06/26/12   aro     Renamed function to FB configure
06/26/12   aro     Updated variable to flag Expansion at the start of IQ capture
06/25/12   aro     F3 message update
06/25/12   aro     Moved EPT functions to new file
06/25/12   aro     Added code to re-initialize EPT variables
06/25/12   aro     Added code to load EPT config param from NV
06/25/12   aro     Fixed bug where RxLM buffer was not getting updated
06/25/12   aro     Added error checking in EPT override
06/14/12   aro     Added check in IQ Cpature function to prevent sending IQ
                   capture command when last process is still pending
06/13/12   aro     Fixed single instance IQ Capture
06/13/12   aro     Implemented DPD coefficient interface
06/13/12   aro     Update EPT configure, iq capture, and override functions
                   to have devices
06/13/12   aro     Removed unrelevant fields from EPT payloads
06/11/12   aro     Added MDSP path/device to EPT configure function
06/11/12   aro     Function ordering
06/11/12   aro     Removed redundant EPT configure function
06/08/12   aro     Updated EPT IQ capture interface to pass device
06/05/12   anr     Initial EPT Support
05/31/12   spa     Added rfm_1x_ftm_tear_down_radio API definition
05/09/12   aro     Moved the semaphore POP before message is sent to FW to avoid
                   race condition
05/09/12   aro     Updated RX_STOP, TX_START, TX_STOP to use Sem Pool for
                   synchronization
04/18/12   aro     Callback function for Rx/Tx Start/Stop interfaces
04/03/12   aro     Updated FWS config function to perform SemWait in MC
04/03/12   aro     Updated FWS config function to manage state transition in
                   FTM
04/02/12   aro     Added CB as argument in Rx / Tx MDSP interfaces
04/02/12   aro     Added CB as argument in Rx MDSP interfaces
04/02/12   aro     Removed unused 1x Cal v3 functions
03/28/12   aro     Added Current RF State as the parameter for ic calibration
                   to be used for PM override function to check if the state
                   has changed between the time when PM Ocerride command is
                   posted and handled
03/15/12   spa     Added max device check for rf_cdma_get_device_status()
03/09/12   spa     Changed rfm_1x_ftm_get_calibration_compensation arguments to
                   cdma band and chan type
03/08/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan
03/08/12   hdz     Added rfm_1x_ftm_set_tx_freq_adjust
01/03/12   aro     Interface to query TxLM chain for the given TxLM buffer
12/20/11   aro     Enabled logging for APT value and TxLUT index
12/05/11   aro     Added interface to reconfigure RxAGC from ACQ to TRK
12/02/11   sty     Updated rfm_1x_ftm_get_rx_agc_data() to return RxAGC in raw
                   units (FW units)
                   Changed return type of rfm_1x_ftm_get_rx_agc_data() - boolean
                   flag set to true if return values are valid
11/16/11   aro     Renamed FW state change function
11/10/11   aro     Upadated FW state config interface to pass semaphore to
                   wait on for blocking calls
09/27/11   aro     Added call to CDMA HAL to update TXLM with dynamic update
                   for Tx DP IQ Gain value
09/13/11   aro     Added carrier and cumulative Tx total power
09/11/11   aro     Merged CDMA device interface to delete
                   rfdevice_1x_interface.h
09/10/11   sar     Fixed KW errors.
09/01/11   cd      Provide RFM FTM interface to clear PA Range override. This
                   is used when explicitly calling the set Tx override interface
                   in FTM
08/31/11   cd      Fixed KW warnings
08/30/11   aro     Updated Set Tx IQ Gain function to call RF-MDSP
08/30/11   aro     Added support to set Tx DP IQ Gain from FTM
08/24/11   aro     Updated Tx Data query function to have more Tx parameter
08/23/11   sty     Fixed KW warnings
08/17/11   cd      Fixed KW warnings
08/08/11   aro     Added cal v3 rx measurement funciton to pass RxAgc in
                   1/10th dB unit
07/20/11   sty     Renamed AVGA_ to RGI_
                   Changed result check for rfc_cdma_get_tx_lut_table_name() call
07/19/11   Saul    Combined FW enable/disable APIs into one FW config state API.
07/19/11   aro     Updated measure rx function to return status
07/13/11   sty     Ensure that correct value of tx_avga_word is printed in
                   rfm_1x_ftm_set_tx_agc_override
07/12/11   sty     Renamed rfdevice_lut_data_type_new to rfdevice_lut_data_type
07/07/11   sty     Added RFC calls to get correct LUT in
                   rfm_1x_ftm_set_tx_agc_override
07/06/11   sty     Minor documentation update
07/01/11   aro     Added interface to perform Intelliceiver Calibration
06/16/11   sar     Including internal 1x interface.
06/14/11   aro     Renamed TxAGC Override function / F3 message Update
06/14/11   sty     Invoke APIs to convert AGC to requested format
                   Added format arg to agc query APIs
06/07/11   shb     Updated RF CDMA device interface to accept RX/TX device
                   pointers (queried from RFC) instead of the logical path
06/01/11   aro     Added return for Rx measurement function
05/31/11   aro     Added NULL pointer check for Logical Device param
05/31/11   aro     Removed RFM_DEVICE_0 hardcoding to enable SV Tx Calibration
05/26/11   aro     Replaced MDSP chain with logical device mapping
05/24/11   aro     Added Logical Device Mapping to MDSP Chain for SV support
05/16/11   aro     Removed DVGA Calibration interfaces
05/16/11   aro     [1] Updated LNA Calibration interfaces to pass Gain State
                   [2] Documentation Update for Cal data units
                   [3] Updated DVGA Calibration Interfacce to use LNA calibration routine
05/16/11   aro     Added Rx Measurment config and query function
05/16/11   aro     PA Bias Type update
04/21/11   aro     Compiler Warning Fix
04/21/11   aro     Moved Get Calibration compensation function to Common
04/07/11   aro     [1] Updated argument type for TxAGC Override
                   [2] Updated TxAGC Override Implementation
03/24/11   aro     Defined unique RxAGC Read structure for 1x
03/18/11   aro     Added interface to query Calibration Compensation Value
03/16/11   aro     Changed Device Param to MDSP Chain
03/16/11   aro     [1] Interface name update
                   [2] Renamed enum to RF_CDMA_TX_LINEARIZER_NUM
03/16/11   aro     Cleaned up Override Interface + added device argument
03/15/11   aro     Added consolidated TxAGC Read Interface
03/14/11   aro     Consolidated RxAGC Data to one data structure
03/14/11   aro     Merged Set DVGA/LNA Offset to a unified interface
03/14/11   aro     Added FW enable and disable interface
03/14/11   sar     Replaced rfm_1x_sv.h with rfm_1x.h file.
03/08/11   sty     Added implementation for rfm_1x_ftm_set_tx_pwr
03/07/11   sty     Refactored few of rf1x_mdsp* APIs to rf_1x_mdsp*
03/04/11   aro     [1] Moved HDET Read and AVGA Idnex read function to Common
                   [2] Changed type for Set PA Range interface
03/04/11   sty     Added API to get turnaround constant
03/04/11   sty     Changed return type of rfm_1x_read_hdet
                   Modified rfm_1x_ftm_get_tx_agc_data()
03/03/11   sty     Added implementation for rfm_1x_ftm_get_tx_agc_data
03/03/11   sty     Renamed rfm_1x_ftm_get_min_max_lut_idx
03/02/11   sty     Replaced RFCOM_TRANSCEIVER_0 with RFM_DEVICE_0
03/01/11   sty     Fixed minor bug in rfm_1x_ftm_get_min_max_lut_idx
03/01/11   sty     Added rfm_1x_ftm_get_min_max_lut
02/22/11   sty     Added stub rfm_1x_read_hdet()
                   Renamed rfm_pa_r1_r0_type to rfm_pa_range_type
02/18/11   sty     Deleted redundant rfm_1x_ftm_set_freq_offset() and
                   rfm_1x_ftm_set_cw
02/18/11   sty     Added rfm_1x_ftm_set_freq_offset
02/18/11   sty     Changed arg in rfm_1x_ftm_set_tx_avga
02/17/11   aro     Removed RFM Device Check to replace with RFM Mode check
02/17/11   sty     Implemented rfm_1x_ftm_set_pa_state
02/17/11   sty     Fixed message
02/17/11   sty     Added debug message
02/17/11   aro     Moved LNA Gain Step definition to RFM
02/16/11   sty     Added support for Tx operations from FTM
02/16/11   aro     Removed Power Mode Param from Measure LNA/DVGA interfaces
02/15/11   sty     Deleted rfm_1x_ftm_enable_tx()
02/15/11   aro     Moved set DVGA and LNA offset interface to MDSP
02/14/11   sty     Renamed rfm_1x_ftm_set_tx_on to rfm_1x_ftm_enable_tx
                   Deleted call to rf_1x_enable_modulator()
02/13/11   aro     Added Set Tx Power Interface
02/13/11   aro     [1] Updated LNA query interface
                   [2] Added support for Get TxAGC
02/12/11   aro     Changed the interface to return LNA Gain state and Gain word
02/11/11   sty     Fixed doxygen warning
02/11/11   sty     Added rfm_1x_ftm_set_tx_on()
02/09/11   sty     Added rfm_1x_sv.h for rfm_1x_wakeup_tx() and deleted
                   rfm_1x_ftm_set_tx_on
02/09/11   sty     Added rfm_1x_ftm_set_tx_on
02/09/11   aro     Added Interface to set Tx Power in dBm
02/09/11   aro     Added Set TxAVGA Interface
02/09/11   aro     Doxygen Update
02/09/11   aro     Added Set PA State interface
02/02/11   aro     F3 Update
02/02/11   aro     Removed Power Mode Param from Measure DVGA/LNA
02/02/11   aro     [1] Added support to set LNA state
                   [2] Moved functions to appropriate doxygen block
02/01/11   aro     Added support for NV Commit
02/01/11   aro     Added LNA Calibration Support
02/01/11   aro     Renamed function name
02/01/11   aro     Promoted Power Mode types to RFM
02/01/11   aro     Added DVGA Calibrate and Store
01/31/11   aro     Added support to set LNA offset
01/31/11   aro     Added LNA Calibration Support
01/31/11   aro     Added support to set DVGA offset
01/31/11   aro     Added DVGA Calibration Interface
01/31/11   aro     Deleted unused function
01/24/11   aro     Added LNA Range Query Interface
01/15/11   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_cdma_mc.h"
#include "rf_cdma_data.h"
#include "rf_cdma_msm.h"
#include "rf_cdma_tx_agc.h"
#include "rf_cdma_utils.h"
#include "rf_cdma_utils_freq.h"
#include "rfm_cdma_carrier_types.h"
#include "rfm_1x_ftm.h"
#include "rfm_1x.h"
#include "rfm_internal.h"
#include "rf_1x_mdsp.h"
#include "rf_1x_mc.h"
#include "rf_1x_calibration.h"
#include "rfcommon_msg.h"
#include "rfdevice_cdma_interface.h"
#include "rf_1x_msm.h"
#include "rfc_cdma.h"
#include "rf_1x_ept.h"
#include "rf_1x_power_limiting.h"
#include "rfcommon_mdsp_types.h"
#include "rfcommon_mc.h"
#include "rfdevice_papm_intf.h"
#include "rfdevice_pa_intf.h"
#include "ftm_common_xpt.h"
#include "ftm_common_control.h"
#include "rfcommon_core_xpt.h"
#include "rflm_api_fbrx_fw_hal.h"
#include "ftm_common_fbrx.h"
#include "rfcommon_fbrx_types.h"
#include "rfcommon_core_txlin.h"

/*============================================================================*/
/*!
  @name Rx Calibration

  @brief
  Functions used to perform Rx Calibration
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Setup LNA for calibration

  @details
  This function can be called to setup LNA for calibration mode. This will
  override the LNA state, LNA offset, and move RxAGC to acquisition mode.

  @param device
  RFM Device on which LNA is to be set up

  @param gain_state
  LNA gain state

  @param gain_val
  LNA offset to be programmed

  @param acq_duration_us
  RxAGC Acquisition duration to be applied before going to trackking mode

  @return
  Flag indicating teh status of LNA setup
*/
boolean
rfm_1x_ftm_setup_lna_calibration
(
  rfm_device_enum_type device,
  rfm_lna_gain_state_type gain_state,
  int16 gain_val,
  uint16 acq_duration_us
)
{
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  boolean api_status = TRUE; /* used to see if msgr call was successful*/
  lm_handle_type  rx_handle;
  /* Get Logical Device Param */

  /* Get logical device */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL Pointer check */
  if ( logical_dev == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_ftm_setup_lna_calibration: Null Logical Data - "
              "Device %d", device);
    return FALSE;
  } /* if ( logical_dev == NULL ) */

  /* get rx handle for the device */
  rx_handle = rf_cdma_data_get_rxlm_handle(device);

  api_status &= rf_1x_mdsp_setup_lna_calibration( rx_handle,
                logical_dev->rx_mdsp_path,
                gain_state,
                gain_val,
                acq_duration_us );

  /* If flag to do immediate LNA writes during cal is set to TRUE
  query WTR driver for settings and write to the WTR LNA gain state
  register immediately, If this is done, there should be no LNA state
  update scheduled on RFLM side */
  if ( rf_cdma_debug_flags.imm_lna_write_cal == TRUE )
  {
    /* Call API for immediate LNA state write */
    api_status &= rf_cdma_mc_immediate_lna_write(device, gain_state);
  }/* if ( rf_cdma_debug_flags.imm_lna_write_cal == TRUE ) */

  return api_status;
} /* rf_1x_mdsp_setup_lna_calibration */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform LNA Measurement

  @details
  This function shall be used to compute proper LNA compensation value for
  desired AGC value on a particular Device with given LNA Step. This API shall
  be performing one instance of calibration.

  LNA Measurement will be done on current power mode without altering it.

  @param device
  RF Device on which LNA calibration is done

  @param gain_state
  Corresponds to the LNA Gain State.

  @param agc_expected_val
  Expected AGC value. This corresponds to the actual AGC value that is fed to
  phone antenna. This is in units of 1/640th dB.

  @param *lna_offset
  Storage for Calibrated LNA Offset Value

  @return
  Status of execution completion of the function execution
*/
boolean
rfm_1x_ftm_measure_lna_offset
(
  rfm_device_enum_type device,
  rfm_lna_gain_state_type gain_state,
  int32 agc_expected_val,
  int32 *lna_offset
)
{
  /* Get the current mode */
  rfm_mode_enum_type curr_mode = rfm_get_current_mode(device);

  /* Check whether mode is valid */
  if ( curr_mode != RFM_1X_MODE )
  {
    RF_MSG_2( RF_ERROR, "Measure LNA : Invalid Mode %d on Dev %d", curr_mode, \
              device );
    return FALSE;
  }

  /* Perform LNA Calibration */
  *lna_offset = rf_1x_measure_lna_offset(device, gain_state, agc_expected_val);

  return TRUE;
} /* rfm_1x_ftm_measure_lna_offset */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform LNA Calibration and Store on RFNV Structure

  @details
  This function performs LNA calibration on all available power modes. If the
  intelliceiver is not enabled for the system, then LNA calibration will be
  done for Normal Power Mode only. The calibrated values are stores in NV
  table.

  @param device
  RF Device on which DVGA calibration is to be done

  @param power
  Expected RxACG Power to perform DVGA calibration. This is in units of
  1/640th dB.

  @param gain_state
  LNA State for LNA Calibration

  @param *cal_data
  Data Structure to store the calibrate values. This structure will hold the
  number of RF Power Modes on which LNAcalibration was done. Along with this,
  it will also hold the LNA Calibration Offset value measured for each
  RF Power Mode

  @param length
  Size of the Expected Data

  @return
  Status of LNA Calibration. TRUE for success and FALSE for failure
*/
boolean
rfm_1x_ftm_calibrate_lna
(
  rfm_device_enum_type device,
  int16 power,
  rfm_lna_gain_state_type gain_state,
  rfm_1x_lna_cal_data_type *cal_data,
  int32 length
)
{
  /* Get the current mode */
  rfm_mode_enum_type curr_mode = rfm_get_current_mode(device);

  /* Check whether mode is valid */
  if ( curr_mode != RFM_1X_MODE )
  {
    RF_MSG_2( RF_ERROR, "Calibrate LNA : Invalid Mode %d on Dev %d", curr_mode,\
              device );
    return FALSE;
  }
  /* Check whether the gain state is valid */
  if ( gain_state >= RFM_LNA_GAIN_STATE_NUM )
  {
    RF_MSG_2( RF_ERROR, "Calibrate LNA: Invalid Gain State %d on Dev %d",
              gain_state,
              device );
    return FALSE;
  }

  return rf_1x_calibrate_lna( device, power, gain_state, cal_data, length );

} /* rfm_1x_ftm_calibrate_lna */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set RxAGC Gain value

  @details
  This API shall be used to set the RxAGC Gain value for a given LNA State. This
  API shall just update the gain value without any guarantee that the value
  written is retained. This value might be overwritten anytime by the RF
  driver.

  @param device
  RF Device for which Rx Gain is to be set

  @param gain_state
  LNA Gain state for which Rx Gain is to be set

  @param gain_val
  Rx Gain value to be applied. this is in 1/640th of the dB units.

  @return
  Status of execution completion of the function execution
*/
boolean
rfm_1x_ftm_set_rx_gain
(
  rfm_device_enum_type device,
  rfm_lna_gain_state_type gain_state,
  int16 gain_val
)
{
  /* Get the current mode */
  rfm_mode_enum_type curr_mode = rfm_get_current_mode(device);
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device*/
  boolean ret_val = TRUE; /* Return value */

  /* Check whether mode is valid */
  if ( curr_mode != RFM_1X_MODE )
  {
    RF_MSG_2( RF_ERROR, "rfm_1x_ftm_set_rx_gain: Invalid Mode %d on Dev %d",
              curr_mode, device );
    return FALSE;
  }

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL Pointer check */
  if ( logical_dev != NULL )
  {
    /* get rx handle for the device */
    lm_handle_type rx_handle = rf_cdma_data_get_rxlm_handle(device);

    ret_val = rf_1x_mdsp_rx_gain_override( rx_handle,
                                           logical_dev->rx_mdsp_path,
                                           gain_state, gain_val, NULL, NULL,
                                           NULL );
  }
  else
  {
    RF_MSG( RF_ERROR, "rfm_1x_ftm_set_rx_gain : NULL Device Param Pointer" );
    ret_val = FALSE;
  }

  return ret_val;

} /* rfm_1x_ftm_set_rx_gain */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure RxAGC Acquisition mode

  @details
  This function sends a command to firmware to update the RxAGC loop gain
  value to put the RxAGC to acquisition mode, wait for given time, and continue
  tracking mode.

  This function is supposed to be used for FTM mode only for fast RxAGC
  convergence.

  @param device
  RF Device on which RxAGC acquisition mode is to be configured

  @param acq_duration_us
  Duration in microseconds with which FW should wait in acquisition mode until
  mode is changed to tracking mode.

  @return
  Flag inidicating the statud of RxAGC ACQ mode configuration
*/
boolean
rfm_1x_ftm_configure_rxagc_acq
(
  rfm_device_enum_type device,
  uint16 acq_duration_us
)
{
  /* Get the current mode */
  rfm_mode_enum_type curr_mode = rfm_get_current_mode(device);
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device*/
  boolean ret_val = TRUE; /* Return value */

  /* Check whether mode is valid */
  if ( curr_mode != RFM_1X_MODE )
  {
    RF_MSG_2( RF_ERROR, "rfm_1x_ftm_configure_rxagc_acq: Invalid Mode %d on "
              "Dev %d", curr_mode, device );
    return FALSE;
  }

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL Pointer check */
  if ( logical_dev != NULL )
  {
    ret_val = rf_1x_mdsp_configure_rx_agc_acq( logical_dev->rx_mdsp_path,
              acq_duration_us, NULL, NULL,
              NULL );
    RF_MSG_2( RF_MED, "rfm_1x_ftm_configure_rxagc_acq : Dev %d, "
              "Duration %d uS ", device, acq_duration_us );
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_ftm_configure_rxagc_acq : NULL Device Param "
              "Pointer for Dev %d", device );
    ret_val = FALSE;
  }

  return ret_val;

} /* rfm_1x_ftm_configure_rxagc_acq */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Rx Calibration Compensation

  @details
  This function calculates all the Rx calibration compensation required for
  the target and returns the value. This final compensation value is applied
  to the Rx Calibration Data. This calibration value is <b>negated</b> from the
  RF Calibration Data

  @param device
  RF Device whose Rx Calibration Compensation is to be applied

  @param band_chan
  Band and Channel for which Compensation is queried

  @return
  Calibration Compensation value to be negated on Rx Cal Data
*/
int16
rfm_1x_ftm_get_calibration_compensation
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan
)
{
  /* Return the compensation */
  return rf_cdma_get_calibration_compensation( device, band, chan );

} /* rfm_1x_ftm_get_calibration_compensation */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform intelliceiver calibration

  @details
  This function will call device drive to perform intelliceive calibration for
  all power modes.

  @param device
  RF Device for which intelliceiver calibraiton is to be done

  @param power
  Cell IOR set during RF intelliceiver calibrationon 1/10 dB units

  @param cal_data
  Container where the calibration data is stored.

  @return
  Flag indicating if the calibration is successful.

  @retval TRUE
  Indicates that the calibration is successful and the JD Code is a converged
  value.

  @retval FALSE
  Indicates that JD Cide returned is not converged; thus the calibration
  failure is seen
*/
boolean
rfm_1x_ftm_calibrate_intelliceiver
(
  rfm_device_enum_type device,
  int16 power,
  rfm_1x_ic_calibration_data_type *cal_data
)
{
  rf_cdma_data_status_type *dev_status_w; /* Device Status Write Pointer */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Write Pointer*/
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Dev */
  boolean ret_val = FALSE; /* return Variable */

  /* Get Current Device Status for the given Device and check whether the data
  returned is valid. */
  dev_status_r = rf_cdma_get_device_status( device );
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  /*Perform NULL pointer check*/
  if(  ( dev_status_r  == NULL ) || ( dev_status_w  == NULL ) )
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_ftm_calibrate_intelliceiver: NULL Dev %d data",
              device );
    return FALSE;
  }

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  if ( logical_dev == NULL )  /* NULL Pointer check */
  {
    RF_MSG( RF_ERROR, "rfm_1x_ftm_calibrate_intelliceiver : NULL Device Param Pointer" );
    ret_val = FALSE;
  }
  else
  {
    if ( ( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_RX ) &&
         ( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_RXTX ) )
    {
      ret_val = FALSE ;
      RF_MSG_2 ( RF_ERROR ,
                 "rfm_1x_ftm_calibrate_intelliceiver: Dev %d , Device State: %d "
                 "Device not in RF_CDMA_STATE_RX or RXTX state" ,
                 device ,  dev_status_r->data_1x.rf_state ) ;
    }
    else
    {

      /* Perform Intelliceiver Calibration */
      ret_val = rf_1x_calibrate_intelliceiver( device, logical_dev,
                &(dev_status_w->intelliceiver),
                dev_status_r->curr_band,
                dev_status_r->curr_chans[RFM_CDMA_CARRIER_0],
                dev_status_r->data_1x.rf_state, power, cal_data );
    }
  }
  return ret_val;

} /* rfm_1x_ftm_calibrate_intelliceiver */


/*! @} */

/*============================================================================*/
/*!
  @name Rx Calibration - NV

  @brief
  Functions used to perform NV operation during Rx Calibration
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform NV write

  @details
  This function will write all RF calibration items for a particular band to NV.

  @param band
  RF Band for which NV write is done.

  @return
  NV Write Status. If all items are written sucessfully, TRUE is returned,
  otherwise the return code of the first write error is returned.
*/
boolean
rfm_1x_ftm_write_nv
(
  sys_band_class_e_type band
)
{
  return ( rf_1x_write_nv( rf_cdma_sys_band_type_to_rf_band_type( band ) ) );
} /* rfm_1x_ftm_write_nv */

/*! @} */



/*============================================================================*/
/*!
  @name Rx parameter Configuration

  @brief
  Functions used to configure RF parameter in FTM Mode
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Sets LNA decision.

  @details
  This function allows direct control of the LNA Decision bits in
  the HW LNA state machine. These changes will propogate to the
  LNA RANGE and MIXER RANGE pins if they have not been overriden in
  AGC_CTL2.

  @param device
  RF Device under consideration

  @param override_mode
  Flag indicating if the LNA Gain state is to be overriden. If True, then
  LNA State will be overriden to the following state.

  @param state
  Valid Gain States

  @return
  Status of execution completion of the function execution
*/
boolean
rfm_1x_ftm_set_rx_gain_state
(
  rfm_device_enum_type device,
  boolean override_mode,
  rfm_lna_gain_state_type state
)
{
  /* Get the current mode */
  rfm_mode_enum_type curr_mode = rfm_get_current_mode(device);
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  boolean ret_val = TRUE; /* Return value */
  rf_cdma_data_status_type *dev_status_w; /* Device Status */

  /* Check whether mode is valid */
  if ( curr_mode != RFM_1X_MODE )
  {
    RF_MSG_2( RF_ERROR, "Set LNA State : Invalid Mode %d on Dev %d",curr_mode,\
              device );
    return FALSE;
  }

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  /* Get writeable Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  /* NULL Pointer check */
  if (( logical_dev != NULL ) && ( dev_status_w != NULL ))
  {
    /* Based on override_mode value, clear or set the LNA state override */
    if ( override_mode == TRUE )
    {
      ret_val = rf_1x_mdsp_lna_state_override( logical_dev->rx_mdsp_path,
                dev_status_w->rxlm_handle,
                state,
                NULL, NULL, NULL );

      /* If in cal state and flag for imm writes set to TRUE, call API to do
      immediate LNA gain state write */
      if ( (ret_val == TRUE) &&
           (rfm_get_calibration_state() == TRUE ) &&
           (rf_cdma_debug_flags.imm_lna_write_cal == TRUE ))
      {
        ret_val = rf_cdma_mc_immediate_lna_write( device, state );
      }
    }
    else
    {
      ret_val = rf_1x_mdsp_lna_state_clear_override( logical_dev->rx_mdsp_path,
                NULL, NULL, NULL );
    }
  }
  else
  {
    RF_MSG( RF_ERROR, "rfm_1x_ftm_set_rx_gain_state : NULL Device Param Ptr" );
    ret_val = FALSE;
  }

  return ret_val;
} /* rfm_1x_ftm_set_rx_gain_state */

/*! @} */

/*============================================================================*/
/*!
  @name Tx parameter Configuration

  @brief
  Functions used to configure RF parameter in FTM Mode
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set Power Amplier Gain State

  @details
  This function allows direct control of the PA State.

  @param device
  RF Device for which PA State is to be overridden

  @param state
  Valid PA Gain States

  @return
  Status of execution completion of the function execution
*/
boolean
rfm_1x_ftm_set_pa_state
(
  const rfm_device_enum_type device,
  rfm_pa_gain_state_type state
)
{
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  boolean ret_val = TRUE; /* Return value */

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL Pointer check */
  if ( logical_dev != NULL )
  {
    /* validate parameters */
    if ( state < RFM_PA_GAIN_STATE_NUM )
    {
      ret_val = rf_1x_mdsp_pa_state_override( logical_dev->tx_mdsp_path, state,
                                              NULL, NULL, NULL );
    }
    else
    {
      RF_MSG_2( RF_ERROR, "rfm_1x_ftm_set_pa_state: Invalid Param: device=%d, "
                "PA State=%d", device, state);
      ret_val = FALSE;
    }
  }
  else
  {
    RF_MSG( RF_ERROR, "rfm_1x_ftm_set_pa_state : NULL Device Param Ptr" );
    ret_val = FALSE;
  }

  return ret_val;

} /* rfm_1x_ftm_set_pa_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set Power Amplier Range value

  @details
  This function allows direct control of the PA R1_R0 lines.

  @param device
  RF Device under consideration

  @param pa_range
  Value to which PA_range is to be set

  @return
  Status of execution completion of the function execution
*/
boolean
rfm_1x_ftm_set_pa_range
(
  const rfm_device_enum_type device,
  rfm_pa_range_type pa_range
)
{
  boolean ret_val = TRUE;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL pointer check */
  if ( logical_dev != NULL )
  {
    /* validate parameters */
    if ( pa_range < RFM_PA_GAIN_RANGE_NUM )
    {
      ret_val = rf_1x_mdsp_pa_range_override( logical_dev->tx_mdsp_path,
                                              (uint8)pa_range, NULL, NULL,
                                              NULL );
    }
    else
    {
      RF_MSG_2( RF_ERROR, "rfm_1x_ftm_set_pa_range: Invalid Paramater: "
                "device %d, pa_range %d", device, pa_range );
      ret_val = FALSE;
    }
  }
  else
  {
    RF_MSG( RF_ERROR, "rfm_1x_ftm_set_pa_range : NULL Device Param Ptr" );
    ret_val = FALSE;
  }

  return ret_val;
} /* rfm_1x_ftm_set_pa_range */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set Power Amplier Range value

  @details
  This function allows direct control of the PA R1_R0 lines.

  @param device
  RF Device under consideration

  @param pa_range
  Value to which PA_range is to be set

  @return
  Status of execution completion of the function execution
*/
boolean
rfm_1x_ftm_clear_pa_range_override
(
  const rfm_device_enum_type device
)
{
  boolean ret_val = TRUE;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL pointer check */
  if ( logical_dev != NULL )
  {
    ret_val = rf_1x_mdsp_pa_range_clear_override( logical_dev->tx_mdsp_path,
              NULL, NULL, NULL );
  }
  else
  {
    RF_MSG( RF_ERROR, "rfm_1x_ftm_clear_pa_range_override : "
            "NULL Device Param Ptr" );
    ret_val = FALSE;
  }

  return ret_val;
} /* rfm_1x_ftm_clear_pa_range_override */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure TxAGC Override Values

  @details
  This function is used to override the TxAGC with PA State, TxAVGA, BetaP,
  and Pa Bias Value.

  @param device
  RF Device associated with the Tx whose power is to be adjusted

  @param pa_state
  Current PA state

  @param rgi_idx
  Index of LUT table corresponding to the desired Power Level

  @param betap_gain
  BetaP gain for Tx override

  @param pa_bias
  The PA Bias PDM value to be overridden to in mV

  @param pa_q_current
  The PA Quiescent Current to be overriden 0~255

  @param pa_q_current_enable
  Flag to indicate whether PA Quiscent Current override is enabled

  @return
  TRUE on success and FALSE on failure
*/
boolean
rfm_1x_ftm_set_fw_tx_agc_override
(
  const rfm_device_enum_type device,
  const rfm_cdma_band_class_type band,
  const rfm_pa_gain_state_type pa_state,
  uint16 rgi,
  int16 betap_gain,
  uint16 pa_bias,
  uint16 pa_q_current,
  boolean pa_q_current_enable
)
{
  boolean status = TRUE;

  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  /* Variables for RGI override */
  int lut_table_name;
  rfdevice_cdma_lut_type tx_lut_tbl_type; /* high-power or lo-power */
  rfdevice_1x_set_lut_idx_parms_type set_lux_index_params;
  /* PA device */
  void* pa_device;
  rfm_pa_range_type pa_gain_range;
  /* QPOET device */
  void* qpoet_device;
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Write Pointer*/
  rfcom_band_type_u tech_band;
  rfdevice_pa_config_type pa_params_s;
  rfdevice_pa_txagc_type pa_txagc_s;
  tech_band.cdma_band = band;

  dev_status_r = rf_cdma_get_device_status( device );
  /*Perform NULL pointer check*/
  if ( dev_status_r  == NULL )
  {
    RF_MSG_1 ( RF_ERROR,
               "rfm_1x_ftm_set_fw_tx_agc_override: NULL Dev %d data" , device ) ;
    return FALSE;
  }

  if ( ( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_RX   ) &&
       ( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_RXTX ) )
  {
    RF_MSG_2 ( RF_ERROR ,
               "rfm_1x_ftm_set_fw_tx_agc_override: Dev %d , Device State: %d"
               "Device not in RF_CDMA_STATE_RX state" ,
               device ,  dev_status_r->data_1x.rf_state ) ;
    return FALSE ;
  }

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG( RF_ERROR, "rfm_1x_ftm_set_fw_tx_agc_override: "
            "Null pointer returned to logical_dev ");
    status &= FALSE;
  }



  if( status == TRUE )
  {
    if( logical_dev->pa[band] == NULL )
    {
      RF_MSG( RF_ERROR, "rfm_1x_ftm_set_fw_tx_agc_override: "
              "Null PA object pointer ");
      status &= FALSE;
    }
  }

  /* Stop FW Tx AGC */
  if( status == TRUE )
  {
    /* Stop FW Tx AGC completely, before executing RF override */
    status &= rf_1x_mdsp_stop_txagc_update( TRUE );
    if( status == FALSE )
    {
      RF_MSG( RF_ERROR, "rfm_1x_ftm_set_fw_tx_agc_override: "
              "Failed to Stop FW TxAGC update ");
    }
  } /* Stop FW Tx AGC */

  /* Overrides must be done in the following order to prevent Tx spikes on
  PA state change:
  1) QPOET setting
  2) PA settning
  3) WTR setting
  for example: In calibration if WTR RGI is set before switching the PA state
  or setting QPOET bias voltage, we might end up setting a very high RGI for the
  previous calibrated state, this can put the QPOET into bypass mode */

  /* QPOET override */
  if ( status == TRUE )
  {
    qpoet_device = logical_dev->qpoet_obj[band];

    if( qpoet_device != NULL )
    {
      /* Set QPOET Bias */
      status &= rfdevice_papm_set_mode_bias( qpoet_device, 
                                             device, 
                                             RFM_1X_MODE,
                                             tech_band,
                                             RFDEVICE_APT_MODE,
                                             pa_bias, // bias in mV
                                             NULL, RFDEVICE_EXECUTE_IMMEDIATE );
    }
    else
    {
      RF_MSG( RF_HIGH,  "rfm_1x_ftm_set_fw_tx_agc_override: "
              "No QPOET object found");
    }

    if( status == FALSE )
    {
      RF_MSG( RF_ERROR, "rfm_1x_ftm_set_fw_tx_agc_override: "
              "Failed to Override QPOET Bias ");
    }
  } /* QPOET Override */

  /* PA override */
  if( status == TRUE )
  {
    pa_device = get_cdma_pa_device ( logical_dev->pa[band] ) ;
    if( pa_device != NULL )
    {
      /* Map CDMA PA state to PA Gain Range*/
      pa_gain_range = rf_cdma_map_pa_state_to_gain_range( device, band,
                      pa_state );

      /* Set PA gain range */

      pa_params_s.mode = RFM_1X_MODE;
      pa_params_s.band = tech_band;
      pa_params_s.bw = RFDEVICE_PA_BW_NOT_APPLICABLE;
      pa_params_s.trigger_type = RFDEVICE_PA_TRIGGER_TXAGC;
      pa_params_s.rfm_device = device;

      pa_txagc_s.pa_gain_range = pa_gain_range;
      pa_txagc_s.is_mod_type_gmsk = 	FALSE;
      pa_txagc_s.pa_icq_array_count = 0;
      pa_txagc_s.pa_icq_array_p = NULL;
      pa_txagc_s.use_default_icq_pdm= TRUE;

      status &= rfdevice_set_pa_txagc( &pa_params_s, &pa_txagc_s, pa_device,
                                       NULL, RFDEVICE_EXECUTE_IMMEDIATE,
                                       RFCOM_INVALID_TIMING_OFFSET );

      if( status == FALSE )
      {
        RF_MSG( RF_ERROR, "rfm_1x_ftm_set_fw_tx_agc_override: "
                "Failed to Override PA State ");
      }

      if( pa_q_current_enable == TRUE )
      {
        /* Set PA Q Current Bias */
        uint16 pa_icq = pa_q_current;

        pa_params_s.mode = RFM_1X_MODE;
        pa_params_s.band = tech_band;
        pa_params_s.bw = RFDEVICE_PA_BW_NOT_APPLICABLE;
        pa_params_s.trigger_type = RFDEVICE_PA_TRIGGER_TXAGC;
        pa_params_s.rfm_device = device;


        pa_txagc_s.is_mod_type_gmsk = 	FALSE;
        pa_txagc_s.pa_gain_range = pa_gain_range;
        pa_txagc_s.pa_icq_array_count = 1;
        pa_txagc_s.pa_icq_array_p = &pa_icq;
        pa_txagc_s.use_default_icq_pdm= FALSE;

        status &= rfdevice_set_pa_txagc( &pa_params_s, &pa_txagc_s, pa_device,
                                         NULL,
                                         RFDEVICE_EXECUTE_IMMEDIATE,
                                         RFCOM_INVALID_TIMING_OFFSET );

        if( status == FALSE )
        {
          RF_MSG( RF_ERROR, "rfm_1x_ftm_set_fw_tx_agc_override: "
                  "Failed to Override PA Q Current ");
        }
      }
    }
    else
    {
      RF_MSG( RF_ERROR, "rfm_1x_ftm_set_fw_tx_agc_override: "
              "CDMA PA object not found");
      status &= FALSE;
    }
  } /* PA override */

  /* RGI override */
  if( status == TRUE )
  {

    /* Get the common device pointer for the Tx device */
    rfdevice_rxtx_common_class* device_ptr =
      rfdevice_cdma_tx_get_common_device_pointer(
        logical_dev->tx_device[band] );

    if ( NULL != device_ptr )
    {
      if ( rfdevice_cmn_is_cmn_tx_intf_supported( device_ptr )
           == FALSE )
      {
        /* Obtain RFC LUT information */
        status &=
          rfc_cdma_get_tx_lut_table_name( RFM_1X_MODE, device, band,
                                          pa_state,
                                          &lut_table_name,
                                          &tx_lut_tbl_type );

        /* Set WTR RGI */
        set_lux_index_params.tbl_type = tx_lut_tbl_type;
        set_lux_index_params.lut_table_name = lut_table_name;
        set_lux_index_params.lut_idx = (uint8)rgi;
        rfdevice_cdma_tx_cmd_dispatch( logical_dev->tx_device[band],
                                       RFDEVICE_SET_LUT_INDEX,
                                       &set_lux_index_params );
      }
      else
      {
        rfdevice_tx_lut_data_type lut_data;
        status &=
          rfdevice_cmn_get_tx_lut_scripts (
            device_ptr ,RFM_1X_MODE ,
            tech_band ,
            pa_state, rgi,
            RFDEVICE_EXECUTE_IMMEDIATE,
            &lut_data ) ;
      } /* else rfdevice_cmn_is_cmn_tx_intf_supported() */
    }
    else
    {
      RF_MSG ( RF_ERROR ,  "rfm_1x_ftm_set_rf_tx_agc_override(): "
               "device_ptr is NULL. Unable to get Tx lut table" ) ;
    } /* if ( NULL != device_ptr ) */



    if( status == FALSE )
    {
      RF_MSG( RF_ERROR, "rfm_1x_ftm_set_fw_tx_agc_override : "
              "Failed to Override RGI ");
    }
  }/* RGI override */

  RF_MSG_5( RF_HIGH, "rfm_1x_ftm_set_fw_tx_agc_override: RGI %d, SMPS %d, "
            "PA State %d, PA Curr %d, PA Curr Enabled %d",
            rgi, pa_bias, pa_state, pa_q_current, pa_q_current_enable);

  return status;

} /* rfm_1x_ftm_set_fw_tx_agc_override */

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
rfm_1x_ftm_set_rf_tx_agc_override
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
  int32  env_delay_val
)
{
  boolean status = TRUE;

  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  const rf_cdma_data_status_type *dev_status_r;/* Dev Status Read Ptr*/
  rfdevice_therm_read_type therm_read;
  int16 delay_comp_x10ns = 0;
  rfdevice_tx_lut_data_type lut_data;
  rfdevice_pa_config_type pa_params_s;
  rfdevice_pa_txagc_type pa_txagc_s;
  /* Get the device status pointer */
  dev_status_r = rf_cdma_get_device_status( device );

  /* Perform NULL Pointer check */
  if ( dev_status_r == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_ftm_set_rf_tx_agc_override: "
              "NULL data for device %d",
              device );
    status = FALSE;
  }
  else
  {
    if ( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_RXTX )
    {
      status = FALSE ;
      RF_MSG_2 ( RF_ERROR ,
                 "rfm_1x_ftm_set_rf_tx_agc_override: Dev %d , Device State: %d "
                 "Device not in RXTX state" ,
                 device ,  dev_status_r->data_1x.rf_state ) ;
    }
    else
    {
      /* Get Logical Device Param */
      logical_dev = rfc_cdma_get_logical_device_params( device );
      if ( logical_dev == NULL )
      {
        RF_MSG( RF_ERROR, "rfm_1x_ftm_set_rf_tx_agc_override: "
                "Null pointer returned to logical_dev ");
        status = FALSE;
      }
    }
    /* continue only if above checks passed */
    if ( status == TRUE )
    {
      void* pa_device;     /* PA device */
      void* qpoet_device;  /* QPOET device */
      rfdevice_papm_mode_type qpoet_mode;
      rfcom_band_type_u tech_band;
      rfm_cdma_band_class_type band;
      /* Pointer to 1x RF NV data */
      const rf_1x_config_type *rf_1x_config_ptr;

      /* Variables for RGI override */
      int lut_table_name;
      rfdevice_cdma_lut_type tx_lut_tbl_type; /* high-power or lo-power */
      rfdevice_1x_set_lut_idx_parms_type set_lux_index_params;
      rfcommon_mdsp_txagc_oride_params_type mdsp_ovr_params;
      rfm_pa_range_type pa_gain_range;

      memset( &mdsp_ovr_params, 0, sizeof(rfcommon_mdsp_txagc_oride_params_type) );

      /* Obtain CDMA Band Class information */
      band = dev_status_r->curr_band;
      tech_band.cdma_band = band;

      if ( logical_dev->pa[band] == NULL )
      {
        RF_MSG( RF_ERROR, "rfm_1x_ftm_set_rf_tx_agc_override: NULL PA for band" );
        return FALSE;
      }

      pa_device = get_cdma_pa_device ( logical_dev->pa[band] ) ;
      qpoet_device = logical_dev->qpoet_obj[band];
      if ( pa_device == NULL || qpoet_device == NULL )
      {
        RF_MSG( RF_ERROR, "rfm_1x_ftm_set_rf_tx_agc_override: "
                "Null PA/QPOET devices ");
        status = FALSE;
      }

      rf_1x_config_ptr = rf_cdma_nv_get_1x_data( logical_dev->tx_nv_path[band],
                         band );

      if ( rf_1x_config_ptr == NULL )
      {
        RF_MSG_1( RF_ERROR, "rfm_1x_ftm_set_rf_tx_agc_override: "
                  "Failed to retrieve valid Config data from "
                  "NV for band %d",
                  band );
        status = FALSE;
      }

      /* If initial processing passed */
      if ( status == TRUE )
      {
        rfdevice_trx_cdma_tx* tx_device = logical_dev->tx_device[band] ;
        if ( NULL != tx_device )
        {
          /* Stop FW Tx AGC completely, before executing RF override */
          status = rf_1x_mdsp_stop_txagc_update( TRUE );

          /* Check for RGI validity */
          if ( rgi != FTM_COMMON_XPT_TX_OVR_RGI_INVALID )
          {
            /* Get the common device pointer for the Tx device */
            rfdevice_rxtx_common_class* device_ptr =
              rfdevice_cdma_tx_get_common_device_pointer ( tx_device );

            if ( NULL != device_ptr )
            {
              if ( rfdevice_cmn_is_cmn_tx_intf_supported( device_ptr )
                   == FALSE )
              {
                status &= rfc_cdma_get_tx_lut_table_name( RFM_1X_MODE,
                          device, band,
                          pa_state,
                          &lut_table_name,
                          &tx_lut_tbl_type );

                /* Program RGI */
                set_lux_index_params.tbl_type = tx_lut_tbl_type;
                set_lux_index_params.lut_table_name = lut_table_name;
                set_lux_index_params.lut_idx = (uint8)rgi;
                rfdevice_cdma_tx_cmd_dispatch( tx_device ,
                                               RFDEVICE_SET_LUT_INDEX,
                                               &set_lux_index_params );
              }
              else
              {
                status &= rfdevice_cmn_get_tx_lut_scripts (
                            device_ptr ,RFM_1X_MODE ,
                            tech_band ,
                            pa_state, rgi,
                            RFDEVICE_EXECUTE_IMMEDIATE,
                            &lut_data );
                /* Also update IREF data for baseband override */
                mdsp_ovr_params.iref_uamp_x10 = lut_data.iref_uamp_x10;
              }
            }
            else
            {
              RF_MSG ( RF_ERROR ,  "rfm_1x_ftm_set_rf_tx_agc_override(): "
                       "device_ptr is NULL. Unable to get Tx lut table" ) ;
            }
          }
        }
        else
        {
          RF_MSG_2 ( RF_ERROR ,  "rfm_1x_ftm_set_rf_tx_agc_override(): "
                     "tx_device is NULL. Device:%d | Band:%d" ,
                     device , band ) ;
        }


        /* Obtain PA Range from PA state */
        pa_gain_range = rf_cdma_map_pa_state_to_gain_range(
                          device, band,
                          pa_state );

        /* Check for valid PA state */
        if ( pa_state != FTM_COMMON_XPT_TX_OVR_PA_STATE_INVALID )
        {
          /* Set PA gain range */
          pa_params_s.mode = RFM_1X_MODE;
          pa_params_s.band = tech_band;
          pa_params_s.bw = RFDEVICE_PA_BW_NOT_APPLICABLE;
          pa_params_s.trigger_type = RFDEVICE_PA_TRIGGER_TXAGC;
          pa_params_s.rfm_device = device;


          pa_txagc_s.is_mod_type_gmsk = 	FALSE;
          pa_txagc_s.pa_icq_array_p = NULL;
          pa_txagc_s.pa_gain_range = pa_gain_range;
          pa_txagc_s.pa_icq_array_count = 0;
          pa_txagc_s.use_default_icq_pdm= TRUE;



          status &= rfdevice_set_pa_txagc( &pa_params_s, &pa_txagc_s, pa_device,
                                           NULL, RFDEVICE_EXECUTE_IMMEDIATE,
                                           RFCOM_INVALID_TIMING_OFFSET );
        }

        /* Check for PA current validity */
        if ( pa_curr != FTM_COMMON_XPT_TX_OVR_PA_CURR_INVALID )
        {
          uint16 pa_icq = pa_curr;

          pa_params_s.mode = RFM_1X_MODE;
          pa_params_s.band = tech_band;
          pa_params_s.bw = RFDEVICE_PA_BW_NOT_APPLICABLE;
          pa_params_s.trigger_type = RFDEVICE_PA_TRIGGER_TXAGC;
          pa_params_s.rfm_device = device;


          pa_txagc_s.is_mod_type_gmsk = 	FALSE;
          pa_txagc_s.pa_gain_range = pa_gain_range;
          pa_txagc_s.pa_icq_array_count = 1;
          pa_txagc_s.pa_icq_array_p = &pa_icq;
          pa_txagc_s.use_default_icq_pdm= FALSE;

          status &= rfdevice_set_pa_txagc( &pa_params_s, &pa_txagc_s, pa_device,
                                           NULL,
                                           RFDEVICE_EXECUTE_IMMEDIATE,
                                           RFCOM_INVALID_TIMING_OFFSET );
        }

        /* Get Tx chain from RFC */
        mdsp_ovr_params.modem_chain = rfc_cdma_get_tx_modem_chain(device,
                                      dev_status_r->curr_band);

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
        mdsp_ovr_params.rflm_handle = dev_status_r->txlm_handle;

        /* Decide on IQ gain override type */
        switch ( iq_gain_action_type )
        {
        case FTM_COMMON_XPT_TX_OVERRIDE_VAL:
          mdsp_ovr_params.iq_gain = iq_gain;
          break;

        case FTM_COMMON_XPT_TX_OVERRIDE_DEFAULT:
          mdsp_ovr_params.iq_gain = rf_cdma_msm_get_default_txc_gain(
                                      rfc_cdma_get_tx_modem_chain( device,
                                          band ),
                                      dev_status_r->txlm_handle );
          break;

        case FTM_COMMON_XPT_TX_OVERRIDE_NV:
          mdsp_ovr_params.iq_gain = rf_cdma_xpt_get_cfg_param(
                                      device,
                                      EPT_CFG_CMN_IQ_GAIN_UNITY );
          break;

        case FTM_COMMON_XPT_TX_OVERRIDE_SKIP:
        default:
          mdsp_ovr_params.iq_gain = FTM_COMMON_XPT_TX_OVR_IQ_GAIN_INVALID;
          break;
        }
        /* Decide on Env Scale override type */
        switch ( env_scale_action_type )
        {
        case FTM_COMMON_XPT_TX_OVERRIDE_VAL:
          mdsp_ovr_params.env_scale = env_scale;
          break;

        case FTM_COMMON_XPT_TX_OVERRIDE_NV:
          if ( xpt_mode == RFCOMMON_MDSP_XPT_MODE_EPT )
          {
            mdsp_ovr_params.env_scale = rf_cdma_xpt_get_cfg_param(
                                          device,
                                          EPT_CFG_CMN_EVN_SCALE_UNITY );
          }
          else if ( xpt_mode == RFCOMMON_MDSP_XPT_MODE_ET )
          {
            mdsp_ovr_params.env_scale = rf_cdma_xpt_get_cfg_param(
                                          device,
                                          ET_CFG_CMN_ENV_SCALE_UNITY );
          }
          break;

        case FTM_COMMON_XPT_TX_OVERRIDE_DEFAULT:
        case FTM_COMMON_XPT_TX_OVERRIDE_SKIP:
        default:
          mdsp_ovr_params.env_scale = FTM_COMMON_XPT_TX_OVR_ENV_SCALE_INVALID;
          break;
        }

        /* Decide on delay value override type */
        switch ( delay_action_type )
        {
        case FTM_COMMON_XPT_TX_OVERRIDE_VAL:
          mdsp_ovr_params.delay = env_delay_val;
          mdsp_ovr_params.delay_txlm_handle = (uint32)dev_status_r->txlm_handle;
          break;

        case FTM_COMMON_XPT_TX_OVERRIDE_NV:
          mdsp_ovr_params.delay =
            rf_cdma_xpt_get_path_delay( tx_device ,
                                        &(rf_1x_config_ptr->cdma_common_ptr->tx_cal),
                                        dev_status_r->curr_band, dev_status_r->lo_tuned_to_chan,
                                        FALSE,
                                        FALSE,
                                        0 );
          mdsp_ovr_params.delay_txlm_handle = (uint32)dev_status_r->txlm_handle;
          break;

        case FTM_COMMON_XPT_TX_OVERRIDE_NV_WITH_TEMP_COMP:
          rfdevice_cdma_tx_cmd_dispatch ( tx_device ,
                                          RFDEVICE_GET_THERMISTER_VALUE,
                                          &therm_read );
          if( therm_read.valid_temp_read == FALSE )
          {
            RF_MSG( RF_ERROR, "rfm_1x_ftm_set_rf_tx_agc_override: Delay override with temperature failed. "
                    "Temperature reading invalid." );
            status = FALSE;
          }
          mdsp_ovr_params.delay =
            rf_cdma_xpt_get_path_delay( tx_device ,
                                        &(rf_1x_config_ptr->cdma_common_ptr->tx_cal),
                                        dev_status_r->curr_band, dev_status_r->lo_tuned_to_chan,
                                        TRUE,
                                        therm_read.valid_temp_read,
                                        therm_read.temperature );
          mdsp_ovr_params.delay_txlm_handle = (uint32)dev_status_r->txlm_handle;
          break;

        case FTM_COMMON_XPT_TX_OVERRIDE_W_RC:
          mdsp_ovr_params.delay = env_delay_val;
          status &= 
            rfdevice_cdma_tx_get_et_delay_compensation( tx_device ,
                    &delay_comp_x10ns);
          mdsp_ovr_params.delay += delay_comp_x10ns;
          mdsp_ovr_params.delay_txlm_handle = (uint32)dev_status_r->txlm_handle;
          break;

        case FTM_COMMON_XPT_TX_OVERRIDE_SKIP:
        default:
          mdsp_ovr_params.delay = FTM_COMMON_XPT_TX_OVR_DELAY_INVALID;
          mdsp_ovr_params.delay_txlm_handle = 0;
          break;
        }

        /* Program QPOET after FW programming is complete */
        /* Obtain the QPOET mode of operation, based on override params */
        switch ( xpt_mode )
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

        if ( qpoet_mode == RFDEVICE_ET_MODE )
        {

          /* Call the common MC interface to override MSM settings */
          status &= rfcommon_mc_xpt_txagc_override( device, &mdsp_ovr_params );

          DALSYS_BusyWait(10); /* wait for 10 us before configuring QPOET */


          /* Check validity of bias for APT_MODE */
          status &= rfdevice_papm_set_mode_bias( qpoet_device, device, RFM_1X_MODE,
                                                 tech_band, RFDEVICE_ET_MODE,
                                                 0, // bias in mV
                                                 NULL, RFDEVICE_EXECUTE_IMMEDIATE );

        }
        else
        {
          if ( qpoet_mode == RFDEVICE_APT_MODE &&
               smps_bias != FTM_COMMON_XPT_TX_OVR_SMPS_BIAS_INVALID )
          {
            status &= rfdevice_papm_set_mode_bias( qpoet_device, device, 
                                                   RFM_1X_MODE, tech_band,
                                                   RFDEVICE_APT_MODE,
                                                   smps_bias, // bias in mV
                                                   NULL, RFDEVICE_EXECUTE_IMMEDIATE );
          }

          DALSYS_BusyWait(10); /* wait for 10 us before configuring QPOET */
          /* Call the common MC interface to override MSM settings */
          status &= rfcommon_mc_xpt_txagc_override( device, &mdsp_ovr_params );
        }

        RF_MSG_2( RF_HIGH, "rfm_1x_ftm_set_rf_tx_agc_override: Dev %d [Status %d]",
                  device, status );

        RF_MSG_7( RF_HIGH, "rfm_1x_ftm_set_rf_tx_agc_override: XPT Mode %d, "
                  "IQ Gain Action %d, IQ Gain %d, Env Scale Action %d, "
                  "Env Scale %d, Delay Action %d, Delay %d",
                  xpt_mode, iq_gain_action_type, mdsp_ovr_params.iq_gain,
                  env_scale_action_type, mdsp_ovr_params.env_scale,
                  delay_action_type, mdsp_ovr_params.delay );

        RF_MSG_4( RF_HIGH, "rfm_1x_ftm_set_rf_tx_agc_override: RGI %d, SMPS %d, "
                  "PA State %d, PA Curr %d",
                  rgi, smps_bias, pa_state, pa_curr);

      }/* if( status == TRUE ) */
      else
      {
        RF_MSG_1( RF_ERROR, "rfm_1x_ftm_set_rf_tx_agc_override: [Status %d]",
                  status );
      }
    } /* if ( status == TRUE ) -- all checks passed */
  } /* else if ( dev_status_r == NULL )*/
  return status;
} /* rfm_1x_ftm_set_rf_tx_agc_override */

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
rfm_1x_ftm_process_tx_override_params
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
  int32  env_delay_val
)
{
  boolean status = TRUE;

  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  const rf_cdma_data_status_type *dev_status_r;/* Dev Status Read Ptr*/
  rfdevice_therm_read_type therm_read;
  int16 delay_comp_x10ns = 0;
  rfcommon_core_xpt_tx_override_type curr_tx_ovr;
  uint32 final_iq_gain;
  uint32 final_env_scale;
  int32 final_delay;

  /* Get the device status pointer */
  dev_status_r = rf_cdma_get_device_status( device );

  /* Perform NULL Pointer check */
  if ( dev_status_r == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_ftm_process_tx_override_params: "
              "NULL data for device %d",
              device );
    status = FALSE;
  }
  else
  {
    if ( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_RXTX )
    {
      status = FALSE ;
      RF_MSG_2 ( RF_ERROR ,
                 "rfm_1x_ftm_process_tx_override_params: "
                 "Dev %d , Device State: %d Device not in RXTX state",
                 device, dev_status_r->data_1x.rf_state ) ;
    }
    else
    {
      /* Get Logical Device Param */
      logical_dev = rfc_cdma_get_logical_device_params( device );
      if ( logical_dev == NULL )
      {
        RF_MSG( RF_ERROR, "rfm_1x_ftm_process_tx_override_params: "
                "Null pointer returned to logical_dev ");
        status = FALSE;
      }
    }
    /* continue only if above checks passed */
    if ( status == TRUE )
    {
      rfm_cdma_band_class_type band;
      /* Pointer to 1x RF NV data */
      const rf_1x_config_type *rf_1x_config_ptr;
      rfdevice_trx_cdma_tx* tx_device ;
      /* Obtain CDMA Band Class information */
      band = dev_status_r->curr_band;

      rf_1x_config_ptr = rf_cdma_nv_get_1x_data( logical_dev->tx_nv_path[band],
                         band );


      tx_device = logical_dev->tx_device[band] ;

      if ( rf_1x_config_ptr == NULL )
      {
        RF_MSG_1( RF_ERROR, "rfm_1x_ftm_process_tx_override_params: "
                  "Failed to retrieve valid Config data from "
                  "NV for band %d",
                  band );
        status = FALSE;
      }

      /* If initial processing passed */
      if ( status == TRUE )
      {
        /* Decide on IQ gain override type */
        switch ( iq_gain_action_type )
        {
        case FTM_COMMON_XPT_TX_OVERRIDE_VAL:
          final_iq_gain = iq_gain;
          break;

        case FTM_COMMON_XPT_TX_OVERRIDE_DEFAULT:
          final_iq_gain = rf_cdma_msm_get_default_txc_gain(
                            rfc_cdma_get_tx_modem_chain( device, band ),
                            dev_status_r->txlm_handle );
          break;

        case FTM_COMMON_XPT_TX_OVERRIDE_NV:
          final_iq_gain = rf_cdma_xpt_get_cfg_param(
                            device,
                            EPT_CFG_CMN_IQ_GAIN_UNITY );
          break;

        case FTM_COMMON_XPT_TX_OVERRIDE_SKIP:
        default:
          final_iq_gain = FTM_COMMON_XPT_TX_OVR_IQ_GAIN_INVALID;
          break;
        }
        /* Decide on Env Scale override type */
        switch ( env_scale_action_type )
        {
        case FTM_COMMON_XPT_TX_OVERRIDE_VAL:
          final_env_scale = env_scale;
          break;

        case FTM_COMMON_XPT_TX_OVERRIDE_NV:
          if ( xpt_mode == RFCOMMON_MDSP_XPT_MODE_EPT )
          {
            final_env_scale = rf_cdma_xpt_get_cfg_param(
                                device,
                                EPT_CFG_CMN_EVN_SCALE_UNITY );
          }
          else if ( xpt_mode == RFCOMMON_MDSP_XPT_MODE_ET )
          {
            final_env_scale = rf_cdma_xpt_get_cfg_param(
                                device,
                                ET_CFG_CMN_ENV_SCALE_UNITY );
          }
          else
          {
            final_env_scale = FTM_COMMON_XPT_TX_OVR_ENV_SCALE_INVALID;
          }
          break;

        case FTM_COMMON_XPT_TX_OVERRIDE_DEFAULT:
        case FTM_COMMON_XPT_TX_OVERRIDE_SKIP:
        default:
          final_env_scale = FTM_COMMON_XPT_TX_OVR_ENV_SCALE_INVALID;
          break;
        }

        /* Decide on delay value override type */
        switch ( delay_action_type )
        {

        case FTM_COMMON_XPT_TX_OVERRIDE_VAL:
          final_delay = env_delay_val;
          break;

        case FTM_COMMON_XPT_TX_OVERRIDE_NV:
          final_delay =
            rf_cdma_xpt_get_path_delay(
              tx_device ,
              &(rf_1x_config_ptr->cdma_common_ptr->tx_cal),
              dev_status_r->curr_band,
              dev_status_r->lo_tuned_to_chan,
              FALSE, // mod_type
              FALSE,
              0 );
          break;

        case FTM_COMMON_XPT_TX_OVERRIDE_NV_WITH_TEMP_COMP:
          rfdevice_cdma_tx_cmd_dispatch( tx_device ,
                                         RFDEVICE_GET_THERMISTER_VALUE,
                                         &therm_read );
          if( therm_read.valid_temp_read == FALSE )
          {
            RF_MSG( RF_ERROR, "rfm_1x_ftm_process_tx_override_params: "
                    "Delay override with temperature failed. "
                    "Temperature reading invalid." );
            status = FALSE;
          }
          final_delay =
            rf_cdma_xpt_get_path_delay(
              tx_device ,
              &(rf_1x_config_ptr->cdma_common_ptr->tx_cal),
              dev_status_r->curr_band,
              dev_status_r->lo_tuned_to_chan,
              TRUE,
              therm_read.valid_temp_read,
              therm_read.temperature );
          break;

        case FTM_COMMON_XPT_TX_OVERRIDE_W_RC:
          final_delay = env_delay_val;
          status &= rfdevice_cdma_tx_get_et_delay_compensation(
                      tx_device ,
                      &delay_comp_x10ns);
          final_delay += delay_comp_x10ns;
          break;

        case FTM_COMMON_XPT_TX_OVERRIDE_SKIP:
        default:
          final_delay = FTM_COMMON_XPT_TX_OVR_DELAY_INVALID;
          break;
        }

        /* Update the current set of overrides to FTM common data */
        curr_tx_ovr.rflm_handle = dev_status_r->txlm_handle;
        curr_tx_ovr.modem_chain = rfc_cdma_get_tx_modem_chain(
                                    device,
                                    dev_status_r->curr_band);
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
        curr_tx_ovr.pa_range =
          rf_cdma_map_pa_state_to_gain_range( device, band,
                                              pa_state );
        curr_tx_ovr.pa_curr = pa_curr;
        status &= ftm_common_xpt_set_curr_tx_override( device,
                  &curr_tx_ovr );

      }/* if( status == TRUE ) */
      else
      {
        RF_MSG_1( RF_ERROR, "rfm_1x_ftm_process_tx_override_params: "
                  "[Status %d]",
                  status );
      }
    } /* if ( status == TRUE ) -- all checks passed */
  } /* else if ( dev_status_r == NULL )*/

  return status;
} /* rfm_1x_ftm_process_tx_override_params */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Sets 1x Open Loop or Closed Loop Tx Power.

  @details
  This function sets Open Loop or Closed Loop Transmit Power Level depending
  on the parameter passed. This API takes OL or CL Tx Power in dBm.

  @param device
  RF Device under consideration

  @param tx_pwr_mode
  Defines whether to use Closed Loop Power Mode or open Loop Power Mode

  @param tx_pwr
  Open Loop or Closed Loop Power in dBm to be set.

  @return
  Flag indicating success or failure of the operation
*/
boolean
rfm_1x_ftm_set_tx_pwr
(
  const rfm_device_enum_type device,
  rfm_cdma_tx_pwr_mode_type tx_pwr_mode,
  int16 tx_pwr
)
{
  int16 closed_loop_val;
  boolean override_closedloop = FALSE;  /* CL TxGain adj value is set to 0 
             throughout the duration that the open loop override is active */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  boolean ret_val = TRUE; /* Return Value */

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL pointer check */
  if ( logical_dev != NULL )
  {
    if (tx_pwr_mode == RFM_CDMA_TX_PWR_OPEN_LOOP)
    {
      closed_loop_val = 0; /* force closed_loop val to 0 */
      ret_val &= rf_1x_mdsp_tx_open_loop_override( logical_dev->tx_mdsp_path,
                 tx_pwr, NULL, NULL, NULL );
      ret_val &= rf_1x_mdsp_tx_closed_loop_override( logical_dev->tx_mdsp_path,
                 closed_loop_val, override_closedloop, NULL, NULL,
                 NULL);
    }
    else
    {
      ret_val &= rf_1x_mdsp_tx_closed_loop_override( logical_dev->tx_mdsp_path,
                 tx_pwr, override_closedloop, NULL, NULL,
                 NULL);
    }
  }
  else
  {
    RF_MSG( RF_ERROR, "rfm_1x_ftm_set_tx_pwr : NULL Device Param Ptr" );
    ret_val = FALSE;
  }

  return ret_val;
} /* rfm_1x_ftm_set_tx_pwr */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the Tx Datapath IQ Gain

  @details
  This function will override the Tx data IQ gain to adjust the DAC set point

  @param device
  RF device on which the Tx IQ gain is to be set.

  @param tx_dp_iq_gain
  Tx datapath IQ gain to be programmed on the given device

  @return
  Status indicating, if the IQ gain was successfully programmed or not.
*/
boolean
rfm_1x_ftm_set_tx_iq_gain
(
  rfm_device_enum_type device,
  uint32 tx_dp_iq_gain
)
{
  boolean ret_val = TRUE; /* Return value */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Write Pointer*/
  rfm_cdma_band_class_type band = RFM_CDMA_MAX_BAND;

  if ( device >= RFM_MAX_DEVICES )
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_ftm_set_tx_iq_gain: Invalid Device %d",device );
    ret_val = FALSE;
  }
  else
  {
    /* Get the current mode */
    rfm_mode_enum_type curr_mode = rfm_get_current_mode(device);

    /* Check whether mode is valid */
    if ( curr_mode != RFM_1X_MODE )
    {
      RF_MSG_2( RF_ERROR,"rfm_1x_ftm_set_tx_iq_gain: Invalid Mode %d on Dev %d",
                curr_mode, device );
      ret_val = FALSE;
    }
  } /*  if !( device >= RFM_MAX_DEVICES ) */

  if ( ret_val == TRUE )
  {
    /* Get Current Device Status for the given Device and check whether the data
    returned is valid. */
    dev_status_r = rf_cdma_get_device_status( device );

    if ( dev_status_r == NULL )
    {
      RF_MSG_1( RF_ERROR, "rfm_1x_ftm_set_tx_iq_gain: Invalid Data ptr on Dev %d",
                device );
      ret_val = FALSE;
    }
    else
    {
      if ( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_RXTX )
      {
        RF_MSG_2 ( RF_ERROR ,
                   "rfm_1x_ftm_set_tx_iq_gain: Dev %d , Device State: %d "
                   "Device not in RF_CDMA_STATE RXTX state" ,
                   device ,  dev_status_r->data_1x.rf_state ) ;
      }
      else
      {
        band = dev_status_r->curr_band;
        if ( band >= RFM_CDMA_MAX_BAND )
        {
          RF_MSG_1 ( RF_ERROR,
                     "rfm_1x_ftm_set_tx_iq_gain: Invalid band %d .. rf_band %d",
                     band );
          ret_val = FALSE;
        }
      }
    }
  } /* if ( ret_val == TRUE ) */

  /* all checks passed */
  if ( ret_val == TRUE )
  {
    const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device*/

    logical_dev = rfc_cdma_get_logical_device_params( device );

    /* NULL Pointer check */
    if ( logical_dev != NULL )
    {
      uint32 modem_chain = rfc_cdma_get_tx_modem_chain ( device , band );
      ret_val &= rf_cdma_msm_set_txc_gain( modem_chain ,
                                           dev_status_r->txlm_handle, tx_dp_iq_gain );

      ret_val &= rf_1x_mdsp_set_tx_iq_gain( logical_dev->rx_mdsp_path ,
                                            dev_status_r->txlm_handle ,
                                            modem_chain , NULL, NULL,
                                            NULL ) ;

      RF_MSG_2( RF_HIGH, "rfm_1x_ftm_set_tx_iq_gain : Dev %d - TXC "
                "Gain 0x%x set", device, tx_dp_iq_gain );
    }
    else
    {
      RF_MSG(RF_ERROR,"rfm_1x_ftm_set_tx_iq_gain : NULL Device Param Pointer");
      ret_val = FALSE;
    }

  } /* if ( ret_val == TRUE ) */

  return ret_val;

} /* rfm_1x_ftm_set_tx_iq_gain */

/*! @} */


/*============================================================================*/
/*!
  @name Rx Query Functions

  @brief
  Functions to query 1x Data
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Current Rx AGC data.

  @details
  This function returns the current Rx AGC value, LNA state and LNA gain word
  for 1x.

  @param device
  The receive device whose Rx AGC data is queried.

  @param rx_agc_data
  Container for Rx AGC (in FW units and in requested format), gain state and RTR
  gain word.

  @param format
  The units that the Rx AGC data should be returned in.
  RFM_CDMA_AGC_FORMAT__RAW, RFM_CDMA_AGC_FORMAT__DBM256, and
  RFM_CDMA_AGC_FORMAT__DBM10 units are supported.

  @return
  RxAGC data on success
*/
boolean
rfm_1x_ftm_get_rx_agc_data
(
  const rfm_device_enum_type device,
  rfm_cdma_ftm_rx_agc_data_type *rx_agc_data,
  const rf_cdma_agc_format_type format
)
{
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  boolean api_result; /* final result of API */
  /* Get Device Status for band info */
  const rf_cdma_data_status_type* dev_status_r =
    rf_cdma_get_device_status( device );

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_1x_ftm_get_rx_agc_data: Invalid logical_dev for "
               "device %d ", device );
    api_result = FALSE;
  }
  else if ( dev_status_r == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_ftm_get_rx_agc_data: Null pointer returned "
              "for device status - device %d", device );
    api_result = FALSE;
  }
  else
  {

    if ( ( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_RX ) &&
         ( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_RXTX  ) &&
         ( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_RXDIV ) )
    {
      api_result = FALSE ;
      RF_MSG_2 ( RF_ERROR ,
                 "rfm_1x_ftm_get_rx_agc_data: Dev %d , Device State: %d "
                 "Device not in RF_CDMA_STATE_RX or RXTX state" ,
                 device ,  dev_status_r->data_1x.rf_state ) ;
    }
    else /* all checks passed */
    {
      rf_1x_mdsp_rx_agc_read_type rx_agc_read; /* Rx AGC Data */

      /* Get RxAGC Data */
      rf_1x_mdsp_get_rx_agc_data( logical_dev->rx_mdsp_path, &rx_agc_read );

      /* save the Raw RxAGC value */
      rx_agc_data->raw_rxagc = rx_agc_read.rx_agc;

      /* Copy Data to return structure */
      rx_agc_data->gain_state = rx_agc_read.lna_state;

      /* convert Raw rxAGC to requested format */
      rx_agc_data->rx_agc[RFM_CDMA_CARRIER_0] = rf_cdma_convert_rxagc(format,
          rx_agc_data->raw_rxagc);

      /* ACQ Down Counter */
      rx_agc_data->acq_down_ctr = rx_agc_read.acq_down_ctr;

      /* Get LNA Gain word */
      rfdevice_cdma_rx_cmd_dispatch(
        logical_dev->rx_device[dev_status_r->curr_band],RFDEVICE_GET_LNA_STATE,
        &(rx_agc_data->gain_word) );

      api_result = TRUE;
    }
  }

  return api_result; /* return final result of API */

} /* rfm_1x_ftm_get_rx_agc_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Current Rx AGC.

  @details
  This function returns the current Rx AGC value

  @param device
  The receive device whose Rx AGC data is queried.

  @return
  RxAGC in 1/640th unit
*/
int16
rfm_1x_ftm_get_rx_agc
(
  const rfm_device_enum_type device
)
{
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_1x_ftm_get_rx_agc_data: Invalid logical_dev for "
               "device %d ", device );
    return FALSE;
  }
  else
  {
    rf_1x_mdsp_rx_agc_read_type rx_agc_data;

    /* Get RxAGC Data */
    rf_1x_mdsp_get_rx_agc_data( logical_dev->rx_mdsp_path, &rx_agc_data );

    return ( rx_agc_data.rx_agc );
  }

} /* rfm_1x_ftm_get_rx_agc */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get TxAGC data for device.

  @details
  This function returns the current TxAGC Data.

  @param device
  Transmit device whose TxAGC data is queried.

  @param tx_agc_data
  Container for current TxAGC data.

  @param format
  The units that the Rx AGC data should be returned in.
  RFM_CDMA_AGC_FORMAT__RAW, RFM_CDMA_AGC_FORMAT__DBM256, and
  RFM_CDMA_AGC_FORMAT__DBM10 units are supported.
*/
void
rfm_1x_ftm_get_tx_agc_data
(
  const rfm_device_enum_type device,
  rfm_cdma_ftm_tx_agc_data_type *tx_agc_data,
  const rf_cdma_agc_format_type format
)
{
  rf_cdma_mdsp_tx_agc_logical_read_type tx_agc_logical= {0}; /* TxAGC Data */
  rf_cdma_mdsp_tx_agc_physical_read_type tx_agc_physical; /* TxAGC Data */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  /* Linearizer logging data */
  const rfcommon_core_txlin_log_data_type* lin_log_data = NULL;

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* get read pointer to device - need this to find out current band */
  dev_status_r = rf_cdma_get_device_status( device );

  /* Get the linearizer data logging pointer */
  lin_log_data = rfcommon_core_txlin_get_log_data( RFM_1X_MODE, device );

  /* NULL Pointer check */
  if ( ( logical_dev != NULL ) && ( dev_status_r != NULL ) &&
       ( lin_log_data != NULL ) )
  {
    if ( dev_status_r->data_1x.rf_state != RF_CDMA_STATE_RXTX )
    {
      RF_MSG_2 ( RF_ERROR ,
                 "rfm_1x_ftm_get_tx_agc_data: Dev %d , Device State: %d "
                 "Device not in RF_CDMA_STATE_RXTX state" ,
                 device ,  dev_status_r->data_1x.rf_state ) ;
    }
    else
    {
      int16 K; /* turn-around const */
      uint16 temp_idx = 0 , temp_avga_idx = 0 ;
      rfm_pa_gain_state_type pa_state = RFM_PA_GAIN_STATE_0;

      /* Get Logical Side Data of TxAGC */
      rf_1x_mdsp_get_tx_agc_logical_data( logical_dev->tx_mdsp_path ,
                                          rf_cdma_data_get_txlm_handle (device) ,
                                          &tx_agc_logical ) ;

      /* Get Logical Side Data of TxAGC */
      rf_1x_mdsp_get_tx_agc_physical_data( logical_dev->tx_mdsp_path ,
                                           rf_cdma_data_get_txlm_handle (device) ,
                                           &tx_agc_physical ) ;

      /* convert TxAGC read from FW to requested format */
      K = rf_cdma_get_turnaround_constant(dev_status_r->curr_band);
      tx_agc_data->open_loop_power =
        rf_cdma_convert_txagc(format, K, tx_agc_logical.open_loop_power);

      tx_agc_data->closed_loop_power = tx_agc_logical.closed_loop_power;
      tx_agc_data->rate_adjust = tx_agc_logical.rate_adjust;
      tx_agc_data->total_power= tx_agc_logical.total_power;
      tx_agc_data->carrier_total_power= tx_agc_logical.total_carrier_power;
      tx_agc_data->pilot_power = tx_agc_logical.pilot_power;
      tx_agc_data->pa_state = tx_agc_logical.pa_state;
      tx_agc_data->tx_min = tx_agc_logical.tx_min;
      tx_agc_data->tx_max = tx_agc_logical.tx_max;

      /* extract PA state specific data */
      pa_state = (rfm_pa_gain_state_type)tx_agc_logical.pa_state;
      if ( pa_state < RFM_PA_GAIN_STATE_NUM )
      {
        /* Determine PA bias by looking up lin table based on FW RGI index*/
        tx_agc_data->apt_val = 
          lin_log_data->pa_state_data[pa_state].
                        rgi_data[tx_agc_physical.cal_bias_rgi_index].papm_bias;

        /* Determine the TxLUT for the given Tx AVGA index from FW */
        tx_agc_data->avga_index = 
          lin_log_data->pa_state_data[pa_state].
                        rgi_data[tx_agc_physical.full_bias_rgi_index].rgi;

        tx_agc_data->avga_val = tx_agc_physical.tx_avga;
        tx_agc_data->betap_gain = tx_agc_physical.betap;  
        tx_agc_data->xpt_mode_val = tx_agc_physical.xpt_mode;
        tx_agc_data->env_gain_val = tx_agc_physical.env_scale_gain;
        tx_agc_data->iq_gain_val =  tx_agc_physical.iq_gain;
        tx_agc_data->xpt_delay_val = tx_agc_physical.xpt_delay;

        /* Determine the pa icq for the given TX AVGA index from FW*/
        tx_agc_data->pa_q_current = 
          lin_log_data->pa_state_data[pa_state].
                        rgi_data[tx_agc_physical.cal_bias_rgi_index].pa_icq;

        /* Determine Pout compensation value */
        tx_agc_data->pout_comp =  rf_cdma_tx_agc_get_pout_comp_val( 
                                          device,
                                          tx_agc_logical.pa_state,
                                          tx_agc_physical.cal_bias_rgi_index );
        /* Determine Pin compensation value */
        tx_agc_data->pin_comp = rf_cdma_tx_agc_get_pin_comp_val( 
                                          device, tx_agc_logical.pa_state );
        /* Determine DPD index value */
        tx_agc_data->dpd_idx = rf_cdma_tx_agc_get_dpd_index( device,
                                     tx_agc_logical.pa_state,
                                     tx_agc_physical.cal_bias_rgi_index );
      }
      else
      {
        /* Invalid PA state. Set to default 0 */
        tx_agc_data->apt_val = 0;
        tx_agc_data->avga_index = 0;
        tx_agc_data->avga_val = 0;
        tx_agc_data->betap_gain = 0;  
        tx_agc_data->xpt_mode_val = 0;
        tx_agc_data->env_gain_val = 0;
        tx_agc_data->iq_gain_val = 0;
        tx_agc_data->xpt_delay_val = 0;
        tx_agc_data->pa_q_current = 0;
        tx_agc_data->pout_comp = 0;
        tx_agc_data->pin_comp = 0;
        tx_agc_data->dpd_idx = 0;
      }
    }
  }
  else
  {
    RF_MSG( RF_ERROR, "rfm_1x_ftm_get_tx_agc_data : NULL Device Param Ptr" );
    tx_agc_data = NULL;
  }
} /* rfm_1x_ftm_get_tx_agc_data */

/*! @} */

/*============================================================================*/
/*!
  @name Firmware App Control

  @brief
  This section contains functions to control FW App.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure 1x firmware state

  @details
  This function will enable FW App while running under FTM task context by
  calling RF-MDSP Interface.

  @param fw_state
  The state in which to configure firmware

  @return
  smem_addr from fw

*/
void*
rfm_1x_ftm_config_fw_state
(
  const rfm_device_enum_type device,
  rfm_cdma_fw_state_type fw_state
)
{
  void* smem_addr;
  rf_1x_fw_response_data_type *fwrsp_data = NULL;
  rf_cdma_mdsp_response_cb_param_type rsp_cb_param;
  rf_cdma_fwrsp_data_type *fwrsp_hk = NULL;

  /* Get FW Response Data */
  fwrsp_hk = rf_cdma_get_fwrsp_data( device );

  /* NULL Pointer Check */
  if ( fwrsp_hk == NULL )
  {
    RF_MSG( RF_ERROR, "rfm_1x_ftm_config_fw_state: NULL FWRSP Data" );
    return FALSE;
  } /* if ( fwrsp_data == NULL ) */

  /* Get FW Response Data */
  fwrsp_data = rf_cdma_get_1x_fw_response_data();

  /* NULL Pointer Check */
  if ( fwrsp_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfm_1x_ftm_config_fw_state: NULL FWRSP Data" );
    return FALSE;
  } /* if ( fwrsp_data == NULL ) */

  /* Enable FW Application */
  fwrsp_hk->fws_cfg_rsp.device = device;
  fwrsp_hk->fws_cfg_rsp.sema_token = rfcommon_semaphore_pop_item();
  rsp_cb_param.rsp_handler = rf_1x_fws_config_rsp_cb;
  rsp_cb_param.handler_data = &fwrsp_hk->fws_cfg_rsp;

  rf_1x_mdsp_ftm_configure_fw_state( fw_state, &rsp_cb_param, NULL, NULL );

  RF_MSG_2( RF_LOW, "[1X][RF-FW Sync] [D%d] FWS_STATE_CFG : Start Wait Sem @ "
            "0x%x", device, fwrsp_hk->fws_cfg_rsp.sema_token );
  rfcommon_semaphore_wait( fwrsp_hk->fws_cfg_rsp.sema_token );
  rfcommon_semaphore_push_item( fwrsp_hk->fws_cfg_rsp.sema_token );

  /* After configuring fw state, FW passes shared memory adddr in fws_cfg_rsp,
  and it returns the shared memory addr here*/
  smem_addr = fwrsp_hk->fws_cfg_rsp.smem_addr_fw ;

  return smem_addr;

} /* rfm_1x_ftm_config_fw_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfm_1x_ftm API to set tx freq adjust

  @details
  This is rfm_1x_ftm API that is called in FTM layer to set tx freq adjust

  @param tx_freq_adjust
  The tx freq adjust in HZ

  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean
rfm_1x_ftm_set_tx_freq_adjust
(
  int16 tx_freq_adjust
)
{
  boolean ret = TRUE;

  /*Called internal RF driver API to set tx freq adjust*/
  ret &= rf_1x_mdsp_tx_freq_offset_adjust( tx_freq_adjust, NULL, NULL, NULL );

  return ret;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  rfm_1x_ftm_set_tx_pwr_limit will override tx pwr limit

  @details
  This API is called in FTM layer to override tx pwr limit.
  Once the tx pwr_override is true, the new tx power limit
  will be set to tx_pwr_limit_dbm. Otherwise, NV based Tx
  pwr limit will be used ( This is default scenario).

  @param device
  The device that the tx pwr limit is set

  @param tx_pwr_override
  TRUE means to to tx pwr limit override;
  FALSE measn not to do tx pwr limit override;

  @tx_pwr_limit_dbm
  The new tx_pwr_limit in dbm10

  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean
rfm_1x_ftm_set_tx_pwr_limit
(
  rfm_device_enum_type device,
  boolean tx_pwr_override,
  int16 tx_pwr_limit_dbm
)
{
  boolean ret = TRUE;

  /*Called internal RF driver API to set tx freq adjust*/
  ret &= rf_1x_plim_update_external_limit( device, tx_pwr_override, tx_pwr_limit_dbm );

  return ret;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Issue pilot measurement config message to FW

  @details
  This is a wrapper API, which calls the MDSP layer API to send message to FW
  to start pilot measurement

  @param device
  The logical device

  @return
  Flag indicating the function execution success
*/
boolean
rfm_1x_ftm_pilot_meas_cfg
(
  rfm_device_enum_type device
)
{
  boolean ret = TRUE;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev != NULL )
  {
    /* Enable FW Application */
    rf_cdma_mdsp_response_cb_param_type rsp_cb_param;
    rf_cdma_fwrsp_data_type *fwrsp_hk = NULL;

    /* Get pointer to FW Response Data */
    fwrsp_hk = rf_cdma_get_fwrsp_data( device );

    /* NULL pointer check */
    if ( fwrsp_hk != NULL )
    {
      fwrsp_hk->pilot_meas_cfg.device = device;
      rsp_cb_param.rsp_handler = rf_1x_pilot_meas_config_rsp_cb;
      rsp_cb_param.handler_data = &fwrsp_hk->pilot_meas_cfg;
      /* send pilot meas config mssg to FW*/
      ret &= rf_1x_mdsp_pilot_meas_cfg( logical_dev->rx_mdsp_path,
                                        &rsp_cb_param,
                                        NULL, NULL );
    }/*if ( fwrsp_hk != NULL )*/
    else
    {
      RF_MSG_1( RF_ERROR, "rfm_1x_ftm_pilot_meas_cfg: NULL FWRSP"
                "Housekeeping data for device: %d",device );
      ret = FALSE;
    }/*if ( fwrsp_hk == NULL )*/
  }/* if ( logical_dev != NULL )*/
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_ftm_pilot_meas_cfg: NULL data for device:%d",
              device );
    ret = FALSE;
  }

  return ret;

}/*rfm_1x_ftm_pilot_meas_cfg*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Issue pilot measurement stop stream message to FW

  @details
  This is a wrapper API, which calls the MDSP layer API to send message to FW
  to stop pilot measurement

  @param device
  The logical device

  @return
  Flag indicating the function execution success
*/
boolean
rfm_1x_ftm_pilot_meas_stop_stream
(
  rfm_device_enum_type device
)
{
  rf_1x_fw_response_data_type *fwrsp_data = NULL;
  boolean ret = TRUE;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* Get FW Response Data */
  fwrsp_data = rf_cdma_get_1x_fw_response_data();

  /* NULL Pointer Check */
  if ( fwrsp_data == NULL )
  {
    RF_MSG( RF_ERROR, "rfm_1x_ftm_pilot_meas_stop_stream: NULL FWRSP Data" );
    return FALSE;
  } /* if ( fwrsp_data == NULL ) */

  if ( logical_dev != NULL )
  {
    /* Enable FW Application */
    rf_cdma_mdsp_response_cb_param_type rsp_cb_param;
    rf_cdma_fwrsp_data_type *fwrsp_hk = NULL;
    /* Get pointer to FW Response Data */
    fwrsp_hk = rf_cdma_get_fwrsp_data( device );

    /* NUll pointer check */
    if ( fwrsp_hk != NULL )
    {
      fwrsp_hk->pilot_meas_stop_stream.sema_token = rfcommon_semaphore_pop_item();
      fwrsp_hk->pilot_meas_stop_stream.device = device;
      rsp_cb_param.rsp_handler = rf_1x_pilot_meas_stop_stream_rsp_cb;
      rsp_cb_param.handler_data = &fwrsp_hk->pilot_meas_stop_stream;
      /* send pilot meas stop stream mssg to FW*/
      ret &= rf_1x_mdsp_pilot_meas_stop_stream( logical_dev->rx_mdsp_path,
             &rsp_cb_param,
             NULL,
             NULL );

      RF_MSG_2( RF_LOW, "[1X][RF-FW Sync] [D%d] PILOT_MEAS_STOP_STREAM :"
                "Start Wait Sem @ 0x%x", device,
                fwrsp_hk->pilot_meas_stop_stream.sema_token );
      rfcommon_semaphore_wait( fwrsp_hk->pilot_meas_stop_stream.sema_token );
      rfcommon_semaphore_push_item(
        fwrsp_hk->pilot_meas_stop_stream.sema_token );
    }/*if ( fwrsp_hk != NULL )*/
    else
    {
      RF_MSG_1( RF_ERROR, "rfm_1x_ftm_pilot_meas_stop_stream: NULL FWRSP"
                "Housekeeping data for device: %d",device );
      ret = FALSE;
    }/*if ( fwrsp_hk == NULL )*/
  }/*if ( logical_dev != NULL )*/
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_ftm_pilot_meas_stop_stream: NULL data for "
              "device:%d", device );
    ret = FALSE;
  }

  return ret;

}/*rfm_1x_ftm_pilot_meas_stop_stream*/

/*! @} */

/*============================================================================*/
/*!
  @name xPT

  @brief
  This section contains functions to control xPT
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configuration for XPT calibration

  @details
  EPT configuration. Message to indicate that new dpdDataInfo and/or dpdData
  are avl on shared memory. Does not allocate SW memory for IQ capture

  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean
rfm_1x_ftm_xpt_cal_configure
(
  rfm_device_enum_type device,
  rfm_device_enum_type device_fb
)
{
  boolean ret_val = TRUE;

  ret_val = rf_1x_xpt_cal_configure( device, device_fb );

  RF_MSG_3( RF_MED, "rfm_1x_ftm_xpt_cal_configure: Dev[ %d %d ] [Status %d]",
            device, device_fb, ret_val );

  return ret_val;

} /* rfm_1x_ftm_xpt_cal_configure */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Trigger IQ Capture on the FW

  @details
  This function will call RF-MDSP function to trigger the IQ Capture
  for EPT for 1x.

  @param device
  Primary device for which IQ capture is to be done

  @param device_fb
  Feedback device for which IQ capture is to be done

  @param capture_param
  Variable to store IQ capture parameter

  @param samp_rate
  Uint32 pointer to query sampling rate of FBRx

  @return
  TRUE is set successfully; FALSE otherwise
*/
boolean
rfm_1x_ftm_xpt_iq_capture
(
  rfm_device_enum_type device,
  rfm_device_enum_type device_fb,
  rfm_1x_iq_capture_param_type capture_param,
  uint32 * samp_rate
)
{
  boolean ret_val = FALSE;

  ret_val = rf_1x_xpt_iq_capture( device, device_fb, capture_param, samp_rate );

  RF_MSG_2( RF_MED, "rfm_1x_ftm_xpt_iq_capture : Dev[ %d %d ] [Status %d]",
            device, device_fb );

  /* return */
  return ret_val;

} /* rfm_1x_ftm_xpt_iq_capture */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure RxFe feedback path

  @details
  This function will configure the digital Rx front end to be capable
  of doing EPT IQ Capture

  @param device_fb
  RFM Device on which EPT IQ Capture is to be done

  @param rxlm_fb_handle
  RxLM handle pertaining to teh device for which the IQ capture is to be done

  @param band
  RFM CDMA band for which the Feeback path is to be configured

  @param band
  RFM CDMA channel for which the Feeback path is to be configured

  @return
  Flag indicating the status of the EPT capture initialition
*/
boolean
rfm_1x_ftm_config_fb_rxlm
(
  rfm_device_enum_type device,
  rfm_device_enum_type device_fb,
  lm_handle_type rxlm_fb_handle,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan
)
{
  boolean ret_val;

  ret_val = rf_1x_xpt_configure_fb_rxlm( device,
                                         device_fb, rxlm_fb_handle,
                                         band, chan );

  RF_MSG_3( RF_MED, "rfm_1x_ftm_config_fb_rxlm: Device %d RxLM %d "
            "[Status %d]", device_fb, rxlm_fb_handle, ret_val );

  return ret_val;

} /* rfm_1x_ftm_config_fb_rxlm */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Stop Tx AGC Update

  @param stop_tx_agc_update
  Flag that indicates whether to stop updating tx agc.

  @return
  Flag indicating the function execution success
*/
boolean
rfm_1x_ftm_stop_txagc_update
(
  boolean stop_tx_agc_update
)
{
  boolean api_status = TRUE;

  /* Send command to mdsp */
  api_status = rf_1x_mdsp_stop_txagc_update( stop_tx_agc_update );

  return api_status;
} /* rfm_1x_ftm_stop_txagc_update */

/*! @} */

/*============================================================================*/
/*!
  @name RFM teardown API

  @brief
  This section contains functions to execute radio teardown.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Teardown 1x radio

  @details
  This function will Tear Down RF to Sleep State, no matter what the current
  state is, using the rf_1x_mc_tear_down() function.

  @param device
  The device to tear down

  @return
  Return if tear down was required on the device or not
*/
boolean
rfm_1x_ftm_tear_down_radio
(
  rfm_device_enum_type device
)
{
  boolean was_tear_down_req = TRUE; /* Flag indicating if tear down is needed */

  was_tear_down_req = rf_1x_mc_tear_down( device );

  RF_MSG_2( RF_HIGH, "rfm_1x_ftm_tear_down_radio: Dev %d Tear down Status %d",
            device, was_tear_down_req );

  return was_tear_down_req;

} /* rfm_1x_ftm_tear_down_radio */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Register FTM related FW MSG responses with FWRSP Task

  @details
  This function is called in ftm and it calls mdsp layer to register some MSG
  1x Message headers to be sent to FW.

  @return
  TRUE if registration is successful
*/
boolean
rfm_1x_ftm_register_fw_msgs
(
  void
)
{
  return( rf_1x_mdsp_register_fw_msgs());
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  De-register FTM related FW MSG responses with FWRSP Task

  @details
  This function is called in ftm and it calls mdsp layer to de-register some MSG
  1x Message headers to be sent to FW.

  @return
  TRUE if de-registration is successful
*/
boolean
rfm_1x_ftm_deregister_fw_msgs
(
  void
)
{
  return( rf_1x_mdsp_deregister_fw_msgs());
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Antenna Tuner update tune code in ftm mode

  @details
  This function shall be used in FTM mode to dynamically re-tune the antenna
  tuner for peridiocally correcting the tune code when ever FTM provides RF
  an opportunity to do the same. FTM will schedule the dynamic antenna tuner
  update to FW on all devices which are active in 1x mode.

  @return
  Error code indicating the status of the execution of the antenna tuner tune code
  update. Negative return value indicates a failure, while non-negative indicates
  success.

*/
int32
rfm_1x_ftm_antenna_tuner_update_tune_code
(
  void
)
{
  int32 ret_val = RFM_CDMA_HEALTHY_STATE;
  rfm_device_enum_type device;     /* Device for Looping */
  const rf_cdma_data_status_type *m_dev_r;/* Dev Status Read Ptr */
  rfm_cdma_band_class_type band;   /* Current band */
  const rfc_cdma_logical_device_params_type *m_logical_dev;  /* Logic Device Ptr */
  rfcom_band_type_u rf_band;   /* rf band */
  boolean is_rx_mode;         /* Rx state Flag */

  /* Loop all device to check if any of them has rfmode in 1x, */
  for ( device = RFM_DEVICE_0; device < RFM_MAX_DEVICES; device++ )
  {
    /* Get Device Status */
    m_dev_r = rf_cdma_get_device_status( device );
    m_logical_dev = rfc_cdma_get_logical_device_params( device );

    /*Perform NULL pointer check*/
    if(  ( m_dev_r  == NULL ) || ( m_logical_dev  == NULL ) )
    {
      /* it is not an error if a device has an invalid pointer - an example:
      in case of PRx only, the device-0 has a valid pointer - all other devices
      will have NULL. Since such cases are valid, do not throw an error - just
      skip and move to the next device. */
      continue;
    }

    /* Check if it's in 1x mode and active*/
    if ( (m_dev_r->rf_mode == RFM_1X_MODE) &&
         (m_dev_r->data_1x.rf_state == RF_CDMA_STATE_RX ||
          m_dev_r->data_1x.rf_state == RF_CDMA_STATE_RXTX ||
          m_dev_r->data_1x.rf_state == RF_CDMA_STATE_RXDIV)
       )
    {
      /* get band */
      band = m_dev_r->curr_band;
      rf_band.cdma_band = band;

      /* Update tune code */
      if ( m_logical_dev->ant_tuner[band] != NULL )
      {
        /* Determin is_rx_mode */
        if ( m_dev_r->data_1x.rf_state == RF_CDMA_STATE_RX ||
             m_dev_r->data_1x.rf_state == RF_CDMA_STATE_RXDIV )
        {
          is_rx_mode = TRUE;
        }
        else
        {
          is_rx_mode = FALSE;
        }

        /* Obtain tuner update RF script on this Device */
        if( rf_cdma_atuner_update_ftm_tune_code(m_logical_dev->ant_tuner[band],
                                                RFM_1X_MODE,
                                                rf_band,
                                                m_dev_r->lo_tuned_to_chan,
                                                is_rx_mode
                                               ) == FALSE )
        {
          RF_MSG_1( RF_ERROR, "rfm_1x_antenna_tuner_update_tune_code: "
                    "Failed in tuner update tune code for Dev %d", device );
          ret_val = RFM_CDMA_ERROR_FAILURE;
        }
      } /* if(m_logical_dev->ant_tuner[band] != NULL) */
      else
      {
        RF_MSG_1( RF_ERROR, "rfm_1x_antenna_tuner_update_tune_code: "
                  "NULL logical dev pointer for Dev %d", device );
        ret_val = RFM_CDMA_ERROR_FAILURE;
      } /* if(m_logical_dev->ant_tuner[band] == NULL) */
    }/* If it's in 1x mode and active*/
  } /* for ( device = RFM_DEVICE_0; device < RFM_MAX_DEVICES; device++ ) */

  /* If API failed on any device */
  if ( ret_val == RFM_CDMA_ERROR_FAILURE )
  {
    RF_MSG( RF_ERROR, "rfm_1x_antenna_tuner_update_tune_code: API FAILED" );
  }

  return ret_val;
} /* rfm_1x_antenna_tuner_update_tune_code */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Send OQPCH_SEARCH_DONE message to FW

  @details
  This is a wrapper API, which calls the MDSP layer API to send message to FW
  to stop OQPCH_SEARCH

  @param device
  The logical device

  @return
  Flag indicating the function execution success
*/
boolean
rfm_1x_ftm_stop_oqpch_srch
(
  rfm_device_enum_type device
)
{
  boolean ret = TRUE;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev != NULL )
  {
    ret &= rf_1x_mdsp_stop_oqpch_search (logical_dev->rx_mdsp_path,
                                         NULL, NULL );
  }/* if ( logical_dev != NULL )*/
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_1x_ftm_stop_oqpch_srch: NULL data for device:%d",
              device );
    ret = FALSE;
  }

  return ret;

}/*rfm_1x_ftm_stop_oqpch_srch*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API is to configure rxlm for re-enable fbrx
 
  @details
  This API calls rfcommon_fbrx_enable to re-enable fbrx after IQ capture during 
  internal calibration. it's only called in ftm_1x_calibration_feedback_setup

  @return
  Status of LM allocation
*/

boolean
rfm_1x_ftm_config_rxlm_for_fbagc( void )
{
  boolean ret_value = TRUE;
  lm_handle_type fbrx_lm_handle;
  
  /* Get FBRx rxlm handle */
  fbrx_lm_handle = rfcommon_fbrx_mc_get_tech_handle( RFM_1X_MODE );

    /* Update FBRx static settings for rxlm */
  ret_value = rfcommon_fbrx_msm_update_static_settings(fbrx_lm_handle,
                                                       RFCOMMON_FBRX_1X);

    /* Enable FBRx Chain if all rxlm settings are updated successfully */
    if (ret_value != TRUE) 
  {
      RF_MSG( RF_ERROR, "rfm_1x_ftm_config_rxlm_for_fbagc: FBRx rxlm settings "
                          " are not re-enabled successfully!");
  }
  else
  {
    ret_value = rfcommon_fbrx_enable( fbrx_lm_handle );

    if (ret_value != RFCOMMON_FBRX_SUCCESS) 
    {
      RF_MSG( RF_ERROR, "rfm_1x_ftm_config_rxlm_for_fbagc: "
                        " rfcommon_fbrx_enable api failed!");
    }  
  }

  return ret_value;

}/*rfm_1x_ftm_config_rxlm_for_fbagc*/

#endif /* FEATURE_CDMA1X */
/*! @} */
/*! @} */
