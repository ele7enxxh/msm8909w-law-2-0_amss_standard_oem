/*!
  @file
  ftm_1x_calibration.c

  @brief
  This module contains common 1x RF calibration code for.

  @addtogroup RF_FTM_1X_CAL
  @{
*/

/*==============================================================================

  Copyright (c) 2010 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/ftm/src/ftm_1x_calibration.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/08/15   rs      KW error fix
09/17/15   kma     Added auto pin support for Pin cal
04/10/15   vr      Add support for XPT cal when Drx path not present
04/10/15   vr      Use unique RxLM handle for XPT FB capture
02/04/15   vc      Updated the Copyright information
01/24/15   vr      Init Tx Freq Error to zero during FTM wakeup Tx
11/19/14   spa     Added FTM concurrecny manager check,done in single enable API
11/12/14   spa     Change tx_nv_path to be per band per logical device
10/08/14    ka     Replace rfc with device api for retrieving device pointer
09/29/14   zhw     Compiler warning fix
09/23/14   pk      compiler warning fix
09/13/14   dj      Fixed fb power read for TX_MEAS_FB_PWR
09/03/14   cd      Remove legacy Tx override fields from data module
08/26/14   cd      Use correct override value during Pin calibration
08/07/14   JJ      Added PM override for slave dev during radio setup
08/01/14   aa      Move fbrx rsb comp during ftm_common_fbrx_capture_and_process()
07/29/14   JJ      Re-do FBRx capture if the first is invalud due to FW timeline
07/26/14   aro/fh  Added support for TX_MEAS_FB_PWR action in TxMeas
07/03/14   JJ      Pass band to rfm_1x_ftm_config_rxlm_for_fbagc()
07/02/14   kr/kai  Non-0dB Pin SW implementation to support -ve pin offsets values
06/19/14   vs/kai  Non-0dB Pin SW implementation
06/18/14   JJ      Added support to reduce Cal time
06/09/14   JJ      Added support for CONFIGURE_RXLM_FOR_FBAGC
06/09/14   JJ      Re-capture FBRx if first one's max mag LSE exceeds 0x2000
05/20/14   JJ      Remove the hack of discarding first FBRX capture
04/10/14   JJ      Pass fw_smem_ptr to enable modulator api
04/08/13   sty     KW fix - potential null pointer deref
03/26/14   JJ      Discard first capture for fbrx calibration
03/24/14   aa      Remove hardcoded value for fbrx status check
03/06/14   zhw     Normalized Pin Cal support
02/20/14   spa     Calculate LNA offsets relative to previos gain state
02/13/14    id     Updates needed for FBRx Cal API to match FBRx capture API
02/04/14   jmf     Feedback Setup updated with support for configuring fb gain state
01/31/14   jmf     Added support for FBRx Type IQ capture
01/23/14   zhw     Pin Cal vs Freq support
01/21/14   spa     Use int32 math for offset calculation in Cal V4 serial
01/13/14    id     Add support for FB Gain Meas in TX Meas Opcode
01/08/14   cac     Update apply_pin_alignment to reflect change in the ftm common API.
12/12/13   spa     Use int32 math for offset calculation (fixes wraparound bug)
12/11/13   cd      Pin alignment uses RFLM Tx override info for xPT cal
12/06/13   spa     Calculate offsets relative to LNA state 0
12/06/13   JJ      Add check for enable modulator
11/14/13   kma     Added pin calibration support
11/19/13   cd      Add support for APT cal with new RFLM Tx override
10/28/13   zhw     Fix uninitialized band assignment in pin apply API
09/17/13   dj      If feedback action contains only WTR config command, call it
                   in ISR context
08/21/13   dj      Moved feedback_setup to be run from the cdma command dispatch
08/16/13   jmf     [xPT] Update internal data structure for xpt capture info
08/08/13   dj      Added error messages for unsupported fb_set_up actions
08/08/13   zhw     KW error fix
08/02/13   dj      Updated feedback setup to use fb_state instead of pa_state
07/22/13   APU     Removed a Redundant IC update flag.
07/18/13   dj      Handle disable FB path case in Feedback Setup
07/17/13   jmf     [xPT] New Delay Table separate from DPD data module
06/21/13   jmf     [xPT] ET delay implementation for Calv4
06/11/13   pk      Replaced memcpy with memscpy fucntion
06/07/13   zhw     Reduce number of RxAGC sampling from 4 to 1
06/05/13   cd      Add support to clear DPD map in radio set-up
05/24/13   rsr     Cal V4 not applying Delay value. Hardcode to use from NV for now. 
05/23/13   cd      Tx override by power should return if row is not found
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
05/20/13   cd      Fixed Calv4 sequence to allow Mdev retune and Sdev enable in 
                   the same segment
05/08/13   spa     Use #def from QPOET driver for min Tx power in TxAGC override
05/07/13   cd      Fixes for apply pin alignment
05/01/13   jmf     Implement xPT IQ Capture SWI change
04/29/13   cd      Apply Pin alignment during calibration
04/29/13   cd      Send Therm read to data module for current Tx channel
04/24/13   cd      Add support for Therm read as part of Tx Meas Opcode
04/24/13   sty     removed redundnat calls to enc_tx_enable()
04/24/13    dj     Added support for Feedback Setup
04/23/13   cri     Add reference channel to tx override
04/22/13   jmf     Implement xPT processing Enum Type
04/18/13   aro     Optimized while loop in Rx config and Rx meas
04/18/13   spa/fh  Call enc_tx_enable() during retune
04/17/13   aro     RFM api to perform RF and FW Tx param override
04/12/13   aro     Optimized Rx Config and measure commands
04/11/13   cri     Use XPT mode in power lookup for uniqueness
04/09/13   cri     Add init and deinit for cal data module
04/09/13   aro     Fixed mismatch in ftm_cal_data_set_current_override()
04/09/13   aro     Added Data Module hookup
03/29/13   aro     Updated Tx Override OpCode to support modular payload
03/19/13   APU     Replaced memcpy with memscpy.
03/14/13   aro     Deprecated Composite Calibration and TxSweep Commands
03/19/13   aro     Deleted ET config path
03/19/13   aro     Modularized FB config into transceiver, asm, and rxlm
03/19/13   aro     Added XPT CDMA cal deconfig
03/19/13   aro     Added call to XPT cal config from protocal IQ capture
03/19/20   aro     Added Data module hookup
02/28/13   spa     Fix indentation
02/20/13   zhw     Remove 5ms delay as FW has stopped sending RF OFF at start
02/15/13   spa     Remove code under featurization FEATURE_RF_HAS_QFE1510_HDET
02/08/13   sty     KW fix
02/05/13   cd      Implicit xPT configuration is valid for radio set-up as it 
                   does not affect APT cal right now
02/05/13   cd      Remove implicit FTM xPT configuration during wakeup Tx
01/22/13   aro     Code to use unified IQ capture param
01/22/13   aro     Added common XPT capture type
01/22/13   aro     Updated IQ capture and XPT config functions to perform
                   buffer management
01/22/13   aro     Removed DPD index from IQ capture interface
01/22/13   aro     Renamed IQ capture and config interfaces
01/22/13   zhw     Added 5ms delay between tx_start and modulator enable
01/18/13   aro     Renamed function name to have XPT
01/18/13   aro     Deprecated XPT polling code
01/18/13   aro     Deleted nikel-XPT specific code
01/16/13   cd      Fixed Tx opcode execution as per changes to Tx override 
                   interface
01/14/13   adk     Updated HDET code
12/21/12   aro     KW Fix
12/19/12   cd      Program ET Tx configuration only in FTM mode
12/07/12   aki     Moved dev flag conversion to common file
12/06/12   aki     Added RX threshold aka RX wait functionality
12/05/12   aro     Off-target compiler warning fix
12/04/12   zhw     APT PA Q Current interface cleanup
12/03/12   aro     Added 1x handler function for Tx measure opcode
11/30/12   aro     KW fix in tx override opcode
11/30/12   aro     KW fix in rx measure opcode
11/30/12   aro     1x handler function for rx measure opcode
11/19/12   zhw     Cal mode interface change support for APT in Dime
11/21/12   aro     F3 message update
11/16/12   aro     Doxygen Update
11/16/12   Saul    Sample Capture. Use common response.
11/15/12   aro     Tx Override 1x handler function
11/15/12   aro     Added FTM data update in Radio Setup
11/14/12   cri     Move radio setup request to common 
11/09/12   zhw     Brakets for operation precedence fix
11/08/12   cd      Radio set-up opcode functionality calls FW Tx AGC override
11/08/12   cd      xPT Tx AGC override added. Hence renamed legacy override to 
                   FW override
11/07/12   aro     Migrated Radio Setup operation to FTM Task
11/06/12   aro     Added 1x handler function to perform cal v3 radio setup
11/05/12   aro     Renamed polling event enum
10/17/12   kai     Changed calling to QFE1510 HDET APIs
10/31/12   adk     Merged antenna tuner code from Nikel.
10/30/12   spa     Removed refs to RFM_DEVICE_4 (not used in CDMA)
09/19/12   Saul    CDMA Sample Capture. Use PA state 1.
09/04/12   aro     Added band and channel to EPT FB config interface
08/31/12   Saul    CDMA. Added support for EPT_PROCESSING_DONE FW message
08/29/12   sbo     Return correct response for 1x intelliceiver cal failure
08/23/12   aro     Removed usage of coarse field from RF time profiler struct
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
08/17/12   aro     Removed FB config from ftm_1x_ept_configure
08/16/12   aro     Compiler warning fix
08/08/12   vs      Added support to query EPT config params
08/08/12   cvd     Replace timetick with rfcommon_time_profile interface
08/07/12   vs      Modifications to Cal V3 Tx config data packet
08/04/12   vs      Add IQ gain setpoint in Cal V3 Tx config data packet
08/03/12   Saul    OFF-Target. Added test for XPT sample capture.
08/01/12   aro     Added EPT override Test function
07/31/12   aro     Updated Single IQ capture interface
07/31/12   aro     Updated EPT configure
07/30/12   aro     Added ET path enable in signle instance IQ capture
07/26/12   aro     Renamed XPT filename
07/25/12   aro     DPD index in IQ Capture command
07/25/12   aro     Added time profiling for EPT
07/25/12   aro     Added payload for each event
07/24/12   aro     KW Warning
07/24/12   aro     Compiler Warning
07/20/12   aro     Added support to do polling for EPT expansion
07/19/12   aro     Documentation update
07/18/12   aro     Renamed the XPT type definitions
07/18/12   aro     Moved the XPT result pushing to common XPT framework
07/18/12   aro     Added Error Code and SubCopde fields in XPT results
07/17/12   aro     Updated XPT Opcode payload
07/17/12   aro     Enhanced support of configure feedback path
07/06/12   aro     Fixed the configuring Feedback path
07/02/12   cvd     Add Null pointer checks for ftm_cdma_data_get_device_ptr()
07/02/12   spa     Compiler warning fix
06/29/12   spa     Compiler warning fix 
06/26/12   aro     Deleted unused parameter from EPT configure function
06/26/12   aro     Deleted IQ Capture init function
06/25/12   aro     Hooked EPT Configure command
06/14/12   aro     Added common function to populate EPT results buffer
06/13/12   zhw     Fix OFT compiler error
06/13/12   aro     Fixed single instance IQ Capture
06/13/12   aro     Implemented DPD coefficient interface
06/13/12   aro     Update EPT configure, iq capture, and override functions
                   to have devices
06/13/12   aro     Removed unrelevant fields from EPT payloads
06/11/12   aro     Compiler Warning Fix
06/11/12   aro     Added MDSP path/device to EPT configure function
06/08/12   aro     Deleted FTM wrapper function for EPT IQ capture
06/05/12   anr     Initial EPT Support
06/04/12   ks      Add FTM-PC interface for XPT
03/18/12   jfc     Remove tramp.h references. Use void * func pointer.
03/09/12   spa     Changed rfm_1x_ftm_get_calibration_compensation arguments to 
                   cdma band and chan type
03/02/12   aro     Mainlined Enhanced Rx Calibration
03/02/12   aro     F3 message Update to flag Cal v3
02/02/12   cd      Added LPM HDET reads to cal v3 sequence
01/16/12   aro     Added ET Path disable action
01/13/12   aro     F3 message update
01/13/12   aro     Converted Tx actions to Mask
01/13/12   aro     Added support for ET path enable for self calibration
01/12/12   aro     Converted Tx flag and Rx flag to device flag
12/06/11   aro     Added infrastructure in CDMA Cal v3 to perform one PM cal
12/06/11   aro     Added interface to sample and average HDET reads
12/05/11   aro     Reduced wait between RxAGC samples to be 130 uS
11/30/11   aro     KW fix
11/29/11   aro     Deleted unused Cal v3 Rx Config and Rx Measure functions
11/21/11   aro     Added fixes to enable DRx Composite Calibration
11/16/11   aro     Renamed CDMA Rx config and measure functions
11/15/11   aro     Removed redundant 200 ms wait at the end of Rx
                   measurement
11/15/11   aro     Added new optimized Cal v3 Config Rx function with
                   reduced execution time
11/15/11   aro     Added new optimized Cal v3 Measure Rx function with
                   reduced execution time
11/15/11   aro     Added Cal v3 1x time profiling
11/04/11   cd      Add 50us delay before triggering HDET reads in cal v3
11/03/11   APU     Changed the order in ftm_1x_composite_cal_sweep() such that
                   such that we set tx to lowest power before CAL starts. 
                   Previously because of a state gain index and PA state a 
                   spike is observed while doing CAL.  
10/20/11   adk     Added support for SV diversity.
10/10/11   sty     removed redundant debug messages and hard-coding of waits
10/08/11   sty     Enabled HDET after retune API is called
09/20/11   aro     Updated CompositeCal to use HW Timer
09/20/11   aro     Updated CompositeCal debug messages
09/20/11   aro     Fixed the Rx calibration issue in CompCal caused by
                   bad expected RxAGC
09/20/11   aro     Added new function to perform clear PA Range
                   override action
09/20/11   aro     Added Debug messages in Composite calibration
09/20/11   aro     Changed CompCal implementation to perform Tx actions based on
                   PA State override (Not PA Range override)
09/20/11   aro     Fixed units of expected RxAGC for composite calibration
09/09/11   shb     Use different #define for HDET reads vs returns during cal
09/07/11   shb     Fixed non-monotonicity in HDET vs power curve by correcting
                   hdet_read index in serial cal linearizer sweep
09/07/11   sar     Clockworks error fix for (System: 7890).
09/01/11   cd      Add APT support
                   - Set fixed max bias value for calv2 support
                   - Remove dependence on PA range, use PA state
08/25/11   aro     Renamed functions to better match the functionality
08/23/11   aro     Added Tx Calibration (v3) for SV Chain
08/19/11   aro     Klocworks error fixes
08/18/11   aro     Moved to new timer function
08/17/11   cd      Fixed KW warnings
08/11/11   aro     Moved Cal v3 configure rx to be done under ISR context
08/09/11   aro     Moved send FTM command from CDMA to FTM common
08/08/11   aro     Updated cal v3 configure Rx function to perform operation
                   in FTM task context
08/08/11   aro     Updated Measure Rx function to have expected RxAGC in
                   1/10dB resolution
08/03/11   aro     Added support for radio tear down for cal v3
08/03/11   aro     [1] Added support for retune for cal v3
                   [2] Enabled HDET Read for HDET
07/27/11   aro     [1] Updated configure radio function to send single command
                   to FTM to configure cal v3 radio / retune radio
                   [2] Added tear down radio functions
07/26/11   aro     Added HDET settling time query to RF Device
07/21/11   aro     Added Enable and Disable HDET around Tx Sweep
07/21/11   aro     Compiler Warning Fix
07/21/11   aro     Improved the efficiency of the Loops used for RxAction, Rx
                   Flag, Tx Action and Tx flag analysis
07/20/11   aro     Added Rx Measurement OpCode
07/20/11   aro     Renamed Cal v3 Opcode/payload type names for consistency
07/20/11   aro     Renamed variable to max_tx_buffer_len and max_rx_buffer_len
07/19/11   aro     [1] Updated Config Tx and Measure Tx functions to handle multiple
                   Tx Data
                   [2] Updated Rx/Tx Flag, Rx/Tx Action enums
07/19/11   aro     Changed the segment result buffer pointer to byte ptr
07/19/11   aro     Updated Config Rx and Measure Rx functions to handle multiple
                   Device for a given segment
07/14/11   aro     Fixed crash during iC cal failure case
07/13/11   sty     Added null pointer checks
07/08/11   aro     Updated Rx measurement interface to pass pointer to result
                   container
07/08/11   aro     Compiler warning Fix
07/08/11   aro     Added support to return multiple Rx measurement data
07/07/11   sty     Added band info in arg list when API 
                   rfm_1x_ftm_set_tx_agc_override is invoked 
07/06/11   aro     Filled in success status for iC Calibration
07/01/11   aro     Added interface to perform Intelliceiver Calibration
06/29/11   aro     Renamed intelliceiver function name
06/28/11   aro     Renamed header file to ftm_common_calibration_v3
06/24/11   aro     Compiler Warning Fix
06/24/11   aro     Fixed Rx and Tx measurement to return Cal result
06/21/11   aro     Moved ftm_sys_control functionality to ftm_common_control
06/21/11   aro     Deleted tech specific ISR func to use Common ISR
06/14/11   bmg     Handle memory allocation error conditions in DVGA and LNA
                   cal
06/14/11   aro     Created helper functions for each Cal Sweep OpCode
06/14/11   aro     Compiler Warning Fix
06/14/11   aro     Renamed TxAGC Override function + Signature to use PA State
06/14/11   aro     Added Time profiling for Calibration Sweep
06/13/11   aro     Moved RF Tuning to FTM Task Context
06/13/11   aro     [1] Added code to move the Tx to lowest power before
                       doing TX_START
                   [2] Mainlined Debug Flags
06/09/11   aro     Added debug flag for Calibration Sweep V3
06/08/11   aro     Added retune opcode implementation for CalSweep ISR
06/07/11   aro     Removed SV debug flag
05/31/11   aro     Added Error return handling for RFM APIs
05/27/11   aro     Moved SV debug flag to main dispatch
05/27/11   aro     Added SV Chain Calibration temp support
05/18/11   aro     Removed CDMA timer to use common timer
05/16/11   aro     Removed bit shift operation to mathematical operation
05/16/11   aro     [1] Updated DVGA Cal Intf to use LNA calibration routine
                   [2] Added proper unit conversion for exp RxAGC and Offset val
05/16/11   aro     Updated 1x Cal Sweep ISR
05/16/11   aro     [1] Updated Rx Measurement configuration
                   [2] Added Rx Measurement query fucntion
05/16/11   aro     Updated 1x Cal Sweep ISR
05/09/11   sar     Included ftm_rf_cmd.h for rfa/api cleanup.
05/09/11   aro     Added support for 1x Calibration Sweep
05/06/11   aro     Deprecated linearizer sweep functions
05/02/11   cd      Moved null pointer checks outside of loop
04/26/11   cd      Calibration compensation across all gain states for 
                   composite cal
04/26/11   sty     Renamed ftm_state_enum_type to ftm_rfstate_enum_type
04/25/11   aro     Renamed FTM RFMODE to FTM State
04/22/11   cd      KW fixes
04/18/11   shb     1x TX linearizer calibration fix  - HDET reads should be 
                   stored in global array hdet_read[32] only for the first
                   32 PDMs in the linearizer. Since hdet read array size is 
                   FTM_NUM_HDET_TO_RETURN (32), hdet_read[] accesses for 
                   instances > FTM_NUM_HDET_TO_RETURN corrupt other variables
04/11/11   aro     Doxygen Update
04/07/11   aro     [1] Removed return types for ISR functions
                   [2] Renamed HDET Read function
                   [3] Doxygen update
                   [4] Added Memory cleanup for TxLinearizer Sweep
04/07/11   aro     [1] Re-sequenced the TxLinearizer argument
                   [2] Deleted unnecessary function
                   [3] Renamed Trigger functions for Sweep
04/07/11   aro     [1] Renamed Override TxAGC inteface
                   [2] Added TxLinearizer ISR
04/07/11   aro     Fixed TxLinearizer data types
04/06/11   aro     Compiler Warning Fix
04/06/11   aro     Added initial implementation of Tx Linearizer Calibration
03/31/11   aro     Updated Debug Messages for TxSweep Calibration
03/30/11   aro     Updated Debug Messages for CompCal
03/29/11   aro     Added support to disable Tx and LogCommit for CompCal under
                   FTM Task Context
03/29/11   sty     Use ftm_1x_disable_tx() to disable Tx
03/28/11   aro     Changed the Composite Cal Tune segment to be done under
                   FTM Task context
03/23/11   aro     Compiler Warning Fix
03/23/11   aro     Made TxSweep and CompCal trigger functions static
03/22/11   aro     [1] Replaced TxSweep and CompCal debug feature with a flag
                   [2] Replaced memcpy() with element by element copy
                   [3] Moved CompCal instance to FTM Data structrue
03/18/11   aro     Replaced CompositeCal Debug feature with debug flag
03/18/11   aro     Added call to query Calibration Compensation Value
03/16/11   aro     Cleaned up Override Interface + added device argument
03/08/11   aro     Compiler Warning Fix
03/07/11   aro     [1] Added Composite Calibration Support
                   [2] Removed unnecessary access of FTM timer Data structure
03/04/11   aro     Removed the hardcoding of Device from TxSweep
03/04/11   aro     [1] Moved Install/Install Timer ISR to CDMA FTM Hal
                   [2] Added AVGA index Query function
                   [3] Added Turnaround Query function
03/03/11   aro     Implemented initial version of Composite Calibration
03/03/11   aro     [1] Removed device parameter for Tx Cal Data to reuse same
                   structure for multiple devices
                   [2] Compiler Warning fix
03/04/11   aro     Added trigger interface for Composite calibration
03/03/11   sty     Fixed warnings
03/03/11   sty     Replaced clk API with DALSys API
                   Removed get_min_max flag - this functionality already 
                   provided by "tx_sweep_ptr->sweep_state == ON"
03/01/11   sty     Deleted artificial check that capped number of Tx Cal steps
03/01/11   sty     Invoked rfm_1x_ftm_get_min_max_avga_idx
03/01/11   sty     [1] Query min and max AVGA index from the RF device
                   [2] Use the global var pulse_created_for_tx_sweep to indicate
                   if the Tx pulse is created or not
                   [3] Initialize above variable in ftm_1x_tx_sweep_cal
02/28/11   sty     Added temp check FTM_1X_TEMP_MAX_STEP_COUNTS
02/24/11   sty     Populate correct HDET array in ftm_1x_get_all_hdet_readings
                   ftm_1x_tx_sweep_cal_isr(): Read and store HDET only for the 
                   first FTM_NUM_HDET_TO_RETURN samples
02/23/11   sty     Added implementation for ftm_1x_get_all_hdet_readings
02/23/11   sty     Renamed FTM_1X_TX_SWEEP_POWER_ADJ_MAX to 
                   FTM_1X_AVGA_IDX_FOR_LOWEST_TX_PWR
                   Fixed issue where the same txAVGA index was being used for 
                   step_1 and step_2
02/22/11   sty     Do not update tx_avga_idx in ftm_cdma_data during Tx sweep
02/22/11   sty     Added support for Tx Sweep Cal
02/17/11   aro     Removed RFM Device Check to replace with RFM Mode check
02/17/11   sty     Added debug messages, minor changes to variable names
02/17/11   aro     Moved LNA Gain Step definition to RFM
02/14/11   aro     Cleaned up Dispatch
02/09/11   aro     Added TxSweep functions
02/09/11   aro     Added Configure TxSweep Interfaces
02/02/11   aro     F3 Update
02/01/11   aro     Added support for NV Commit
02/01/11   aro     Added LNA Calibration Support
02/01/11   aro     Renamed function name
02/01/11   aro     Renamed function name
02/01/11   aro     Promoted Power Mode types to RFM
02/01/11   aro     Added DVGA Calibrate and Store
01/08/11   aro     Changed @ingroup to @addtogroup
01/05/11   aro     Moved @defgroup to rf_cdma_documentation.h
12/16/10   aro     Initial Revision

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#ifdef FEATURE_CDMA1X
/* memscpy */
#include "stringl.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "rfm_cdma_ftm.h"
#include "rf_cdma_mc.h"
#include "rfm_1x_ftm.h"
#include "rfm_1x_cal_data_types.h"
#include "ftm_cdma_control.h"
#include "ftm_cdma_data.h"
#include "ftm_1x_calibration.h"
#include "rf_test_task_v.h"
#include "ftm_msg.h"
#include "ftm_cdma_data.h"
#include "ftm_1x_control.h"
#include "ftm_1x_msm.h"
#include "ftm_common_msm.h"
#include "ftm_1x_log.h"
#include "ftm_common_control.h"
#include "ftm_cdma_packet_types.h"
#include "ftm_cdma_msm.h"
#include "ftm_rf_cmd.h"
#include "rfm_internal.h"
#include "rfcommon_math.h"
#include "ftm_cdma_cmd.h"
#include "DALSys.h"
#include "diagcmd.h"
#include "rf_cdma_utils_freq.h"
#include "ftm_calibration_v3_xpt.h"
#include "ftm_calibration_v3_radio.h"
#include "ftm_common_xpt.h"
#include "rfcommon_core_xpt.h"
#include "rfm.h"
#include "rfm_ftm.h"
#include "rfm_common.h"
#include "rfc_cdma.h"
#include "rf_cdma_data.h"
#include "rfcommon_time_profile.h"
#include "rfcommon_core_xpt_buffer.h"
#include "rfcommon_core_txlin.h"
#include "rfcommon_core_txlin_types.h"
#include "ftm_common_xpt.h"
#include "rfcommon_mc.h"
#include "ftm_calibration_data_module.h"
#include "rfcommon_data.h"
#include "ftm_1x_xpt.h"
#include "rfcommon_autopin_api.h"
/* This is included to access QPOET #def for min bias voltage */
#include "rfdevice_papm_intf.h"
#include "ftm_common_fbrx.h"
#include "rfcommon_fbrx_types.h"
#include "rfdevice_hdet_cmn_intf.h"
#include "rfdevice_hdet_cdma_intf.h"
#include "math.h"

/*----------------------------------------------------------------------------*/
#define FTM_1X_CAL_V3_RXAGC_WAIT_US 130 /*!< Wait time in microseconds to be
applied before taking two consecutive RxAGC samples during calibration */

/*----------------------------------------------------------------------------*/
#define FTM_1X_RXAGC_RD_SAMPLES 1 /*!< Number of RxAGC samples to be taken
during RF calibration */

/*----------------------------------------------------------------------------*/
#define FTM_1X_FW_LATENCY_AFTER_RGI_UPDATE_US 50 /*!< Time taken by firmware in 
microseconds to apply the overriden RFI value */

/*============================================================================*/
/*!
  @name Data Packet Structure Definition

  @brief
  List of Typedefs for Request and Response Packets used for Rx Calibration.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  DVGA Calibration Request Packet
*/
typedef PACK(struct)
{

  uint8 cmd;
  /*!< Command ID */

  uint8 sub_sys;
  /*!< Sub-system ID. Ex - 11 for FTM */

  uint16 mode;
  /*!< RF Mode ID. Ex-18 for SVDO Cmd, FTM_1X_CAL_V2_C */

  uint16 ftm_cmd;
  /*!< FTM Sub command. Example, FTM_1X_CAL_DVGA */

  uint16 req_len;
  /*!< Request Length */

  uint16 rsp_len;
  /*!< Response Length */

  uint8  device;
  /*!< RF Device */

  int16  power;
  /*!< Expected Power DVGA Calibration */

} ftm_1x_calibrate_dvga_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  DVGA Calibration Response Packet
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command ID */

  uint8 sub_sys;
  /*!< Sub-system ID. Ex - 11 for FTM */

  uint16 mode;
  /*!< RF Mode ID. Ex-28 for SVDO Cmd, FTM_1X_CAL_V2_C */

  uint16 ftm_cmd;
  /*!< FTM Sub command. Example, FTM_1X_CAL_DVGA */

  uint16 req_len;
  /*!< Request Length */

  uint16 rsp_len;
  /*!< Response Length */

  uint8 power_mode_num;
  /*!< Num of RF Power Modes */

  int16 calibration[RFM_CDMA_POWER_MODE_MAX];
  /*!< Cal value for each Mode */

} ftm_1x_calibrate_dvga_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  LNA Calibration Request Packet
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command ID */

  uint8 sub_sys;
  /*!< Sub-system ID. Ex - 11 for FTM */

  uint16 mode;
  /*!< RF Mode ID. Ex-18 for SVDO Cmd, FTM_1X_CAL_V2_C */

  uint16 ftm_cmd;
  /*!< FTM Sub command. Example, FTM_1X_CAL_DVGA */

  uint16 req_len;
  /*!< Request Length */

  uint16 rsp_len;
  /*!< Response Length */

  uint8 path;
  /*!< RF Path */

  uint8 lna_step;
  /*!< LNA Step */

  int16 power;
  /*!< Expected Power DVGA Calibration */

} ftm_1x_calibrate_lna_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Intelliceiver Calibration Request Packet
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command ID */

  uint8 sub_sys;
  /*!< Sub-system ID. Ex - 11 for FTM */

  uint16 mode;
  /*!< RF Mode ID. Ex-18 for SVDO Cmd, FTM_1X_CAL_V2_C */

  uint16 ftm_cmd;
  /*!< FTM Sub command. Example, FTM_1X_CAL_DVGA */

  uint16 req_len;
  /*!< Request Length */

  uint16 rsp_len;
  /*!< Response Length */

  uint8 device;
  /*!< Calibration Device */

  int16 power;
  /*!< Cell IOR during calibration */

} ftm_1x_calibrate_intelliceiver_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  LNA Calibration Response Packet
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command ID */

  uint8 sub_sys;
  /*!< Sub-system ID. Ex - 11 for FTM */

  uint16 mode;
  /*!< RF Mode ID. Ex-28 for SVDO Cmd, FTM_1X_CAL_V2_C */

  uint16 ftm_cmd;
  /*!< FTM Sub command. Example, FTM_1X_CAL_DVGA */

  uint16 req_len;
  /*!< Request Length */

  uint16 rsp_len;
  /*!< Response Length */

  uint8 power_mode_num;
  /*!< Num of RF Power Modes */

  int16 calibration[RFM_CDMA_POWER_MODE_MAX];
  /*!< Cal value for each Mode */

} ftm_1x_calibrate_lna_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Intelliceiver Calibration Response Packet
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command ID */

  uint8 sub_sys;
  /*!< Sub-system ID. Ex - 11 for FTM */

  uint16 mode;
  /*!< RF Mode ID. Ex-18 for SVDO Cmd, FTM_1X_CAL_V2_C */

  uint16 ftm_cmd;
  /*!< FTM Sub command. Example, FTM_1X_CAL_DVGA */

  uint16 req_len;
  /*!< Request Length */

  uint16 rsp_len;
  /*!< Response Length */

  uint8 success;
  /*!< Status of Calibration */

  uint8 cal_data[128];
  /*!< Intelliciever Calibration Data */

} ftm_1x_calibrate_intelliceiver_rsp_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Rx Calibration NV Commit Request Packet
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command ID */

  uint8 sub_sys;
  /*!< Sub-system ID. Ex - 11 for FTM */

  uint16 mode;
  /*!< RF Mode ID. Ex-18 for SVDO Cmd, FTM_1X_CAL_V2_C */

  uint16 ftm_cmd;
  /*!< FTM Sub command. Example, FTM_CDMA_CAL_COMMIT_NV */

  uint16 req_len;
  /*!< Request Length */

  uint16 rsp_len;
  /*!< Response Length */

  uint8 band;
  /*!< RF Band whose NV is to be written */

} ftm_1x_commit_cal_nv_req_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Rx Calibration NV Commit Response Packet
*/
typedef PACK(struct)
{
  uint8 cmd;
  /*!< Command ID */

  uint8 sub_sys;
  /*!< Sub-system ID. Ex - 11 for FTM */

  uint16 mode;
  /*!< RF Mode ID. Ex-28 for SVDO Cmd, FTM_1X_CAL_V2_C */

  uint16 ftm_cmd;
  /*!< FTM Sub command. Example, FTM_CDMA_CAL_COMMIT_NV */

  uint16 req_len;
  /*!< Request Length */

  uint16 rsp_len;
  /*!< Response Length */

  uint8 status;
  /*!< Status of NV Write */

} ftm_1x_commit_cal_nv_rsp_type;


/*! \} */


/*============================================================================*/
/*!
  @name Rx Calibration V2

  @brief
  This section contains the functions used by Cal v2. Cal v2 includes
  LNA, DVGA, IM2, and Intelliceiver Calibration.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform DVGA Calibration and Store

  @details
  This function performs DVGA calibration on all available power modes. If the
  intelliceiver is not enabled for the system, then DVA calibration will be
  done for Normal Power Mode only. The calibrated values are stores in NV
  table.
 
  Associated FTM Cal v2 Command(s) : <b>FTM_CDMA_CAL_DVGA</b>
 
  @param *ftm_req_data
  FTM Request Data including Device and Power

  @return
  Response Packet with Calibration Status and Data
*/
ftm_rsp_pkt_type
ftm_1x_calibrate_dvga
(
  void  * ftm_req_data
)
{
  /* Request Packet */
  ftm_1x_calibrate_dvga_req_type* header = 
    (ftm_1x_calibrate_dvga_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data; /* Packet returned to Diag */
  ftm_1x_calibrate_dvga_rsp_type *cal_rsp;   /* Packet Payload */
  rfm_1x_lna_cal_data_type *cal_data = 
    (void *) ftm_malloc(sizeof(rfm_1x_lna_cal_data_type) );/* Cal Data
    populated during DVGA Cal */

  /* Define : To Extract Useful Data */
  rfm_device_enum_type device;
  int16 power;

  /* Other Defines */
  uint8 power_idx; /* Power Index for LOOP */
  boolean dvga_cal_status = FALSE; /* Status of DVGA Cal Completion */

  if ( cal_data == NULL )
  {
    FTM_MSG( FTM_ERROR,
             "Error allocating dynamic data, aborting calibration." );
    ftm_rsp_data.pkt_len = 0;
    ftm_rsp_data.pkt_payload = NULL;
    ftm_rsp_data.delete_payload = FALSE;
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  power = (int16)header->power; /* This is in 1/10th dB units */

  /* Create a new reponse packet in the ftm heap.
  This will also update first 10-bytes field in Packet Payload in the FTM
  response packet. These fields are command(1), subsystem ID (1), Command
  Mode (2), FTM command (2), Request Packet Length (2), and Response packet
  Length (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data, 
                          sizeof(ftm_1x_calibrate_dvga_rsp_type) );

  /* Convert the power to 1/640th of dB units */
  power = power * 64 ;
  
  /* Call DVGA Calibration API */
  dvga_cal_status = rfm_1x_ftm_calibrate_lna( device, power, 
                   RFM_LNA_GAIN_STATE_0, cal_data,
                   sizeof( rfm_1x_lna_cal_data_type ) );

  if ( dvga_cal_status == TRUE ) /* DVGA Cal Successful */
  {
    /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
    cal_rsp = (ftm_1x_calibrate_dvga_rsp_type*)ftm_rsp_data.pkt_payload;

    if( cal_rsp != NULL )
    {
    /* Update the Response Packet Field */
    cal_rsp->power_mode_num = cal_data->power_mode_num;
    for ( power_idx = 0; power_idx < cal_rsp->power_mode_num; power_idx++  )
    {
      /* Convert to 1/10 of dB Units and return the absolute offset value */ 
      cal_rsp->calibration[power_idx] = (cal_data->calibration[power_idx])/64;
    }
  }
    else
    {
      FTM_MSG( FTM_ERROR, "DVGA Caliberation : NULL cal_rsp" );
      ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    }
  }
  else /* DVGA Cal Failure */
  {
    ftm_rsp_data.pkt_len = 0;
    ftm_rsp_data.delete_payload = FALSE;
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, "DVGA Calibration : Failed !" );
  }

  /* Free the Memory */
  ftm_free(cal_data);

  FTM_MSG( FTM_LOW, "CDMA Calv2 : DVGA Calibration Done" );

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_calibrate_dvga */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform LNA Calibration and Store

  @details
  This function performs LNA calibration on all available power modes. If the
  intelliceiver is not enabled for the system, then LNA calibration will be
  done for Normal Power Mode only. The calibrated values are stores in NV
  table.
 
  Associated FTM Cal v2 Command(s) : <b>FTM_CDMA_CAL_LNA</b>

  @param *ftm_req_data
  FTM Request Data including Device, Power, and LNA Step

  @return
  Response Packet with Calibration Status and Data
*/
ftm_rsp_pkt_type
ftm_1x_calibrate_lna
(
  void  * ftm_req_data
)
{
  /* Request Packet */
  ftm_1x_calibrate_lna_req_type* header = 
    (ftm_1x_calibrate_lna_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data; /* Packet returned to Diag */
  ftm_1x_calibrate_lna_rsp_type *cal_rsp;    /* Packet Payload */
  rfm_1x_lna_cal_data_type *cal_data = 
    (void *) ftm_malloc(sizeof(rfm_1x_lna_cal_data_type) );/* Cal Data
    populated during LNA Cal */

  /* Define : To Extract Useful Data */
  rfm_device_enum_type device;
  int16 power;
  rfm_lna_gain_step_type lna_step;

  /* Other Defines */
  uint8 power_idx; /* Power Index for LOOP */
  boolean lna_cal_status = FALSE; /* Status of DVGA Cal Completion */

  if ( cal_data == NULL )
  {
    FTM_MSG( FTM_ERROR,
             "Error allocating dynamic data, aborting calibration." );
    ftm_rsp_data.pkt_len = 0;
    ftm_rsp_data.pkt_payload = NULL;
    ftm_rsp_data.delete_payload = FALSE;
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->path;
  lna_step = (rfm_lna_gain_step_type)header->lna_step;
  power = (int16)header->power;

  /* Create a new reponse packet in the ftm heap.
  This will also update first 10-bytes field in Packet Payload in the FTM
  response packet. These fields are command(1), subsystem ID (1), Command
  Mode (2), FTM command (2), Request Packet Length (2), and Response packet
  Length (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                              sizeof(ftm_1x_calibrate_lna_rsp_type) );

  /* Convert to 1/640th dB units */
  power = power * 64;

  /* Call DVGA Calibration API */
  lna_cal_status = rfm_1x_ftm_calibrate_lna( device, power, \
                   (rfm_lna_gain_state_type)(lna_step + 1), cal_data,
                   sizeof( rfm_1x_lna_cal_data_type ) );

  if ( lna_cal_status == TRUE ) /* LNA Cal Successful */
  {
    /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
    cal_rsp = (ftm_1x_calibrate_lna_rsp_type*)ftm_rsp_data.pkt_payload;

    if(cal_rsp != NULL)
    {
    /* Update the Response Packet Field */
    cal_rsp->power_mode_num = cal_data->power_mode_num;
    for ( power_idx = 0; power_idx < cal_rsp->power_mode_num; power_idx++  )
    {
      /* Convert to 1/10 of dB Units and return the absolute offset value */ 
      cal_rsp->calibration[power_idx] = (cal_data->calibration[power_idx])/64;
    }
  }
    else
    {
      FTM_MSG( FTM_ERROR, "LNA Caliberation : NULL cal_rsp" );
      ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    }
  }
  else /* DVGA Cal Failure */
  {
    ftm_rsp_data.pkt_len = 0;
    ftm_rsp_data.delete_payload = FALSE;
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    FTM_MSG( FTM_ERROR, "LNA Calibration : Failed !" );
  }

  /* Free the Memory */
  ftm_free(cal_data);

  FTM_MSG( FTM_LOW, "CDMA Calv2 : LNA Calibration Done" );

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_calibrate_lna */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform Intelliceiver Calibration and Store

  @details
  This function performs iC calibration on all available power modes. The
  calibrated values are stores in NV table.
 
  Associated FTM Cal v2 Command(s) : <b>FTM_CDMA_CAL_INTELLICEIVER</b>

  @param *ftm_req_data
  FTM Request Data including Device and Power

  @return
  Response Packet with Calibration Status and Data
*/
ftm_rsp_pkt_type
ftm_1x_calibrate_intelliceiver
(
  void  * ftm_req_data
)
{
  /* Request Packet */
  ftm_1x_calibrate_intelliceiver_req_type* header = 
    (ftm_1x_calibrate_intelliceiver_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data; /* Packet returned to Diag */
  ftm_1x_calibrate_intelliceiver_rsp_type *cal_rsp;    /* Packet Payload */
  rfm_1x_ic_calibration_data_type *cal_data = 
    (void *) ftm_malloc(sizeof(rfm_1x_ic_calibration_data_type) );/* Cal Data
    populated during LNA Cal */

  /* Define : To Extract Useful Data */
  rfm_device_enum_type device;
  int16 power;

  /* Other Defines */
  boolean cal_status = FALSE; /* Status of iC Cal Completion */

  if ( cal_data == NULL )
  {
    FTM_MSG( FTM_ERROR,
             "Error allocating dynamic data, aborting calibration." );
    ftm_rsp_data.pkt_len = 0;
    ftm_rsp_data.pkt_payload = NULL;
    ftm_rsp_data.delete_payload = FALSE;
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
    return ftm_rsp_data;
  }

  /* Extract Useful Data */
  device = (rfm_device_enum_type)header->device;
  power = (int16)header->power;

  /* Create a new reponse packet in the ftm heap.
  This will also update first 10-bytes field in Packet Payload in the FTM
  response packet. These fields are command(1), subsystem ID (1), Command
  Mode (2), FTM command (2), Request Packet Length (2), and Response packet
  Length (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data,
                              sizeof(ftm_1x_calibrate_intelliceiver_rsp_type) );

  /* Call iC Calibration API */
  cal_status = rfm_1x_ftm_calibrate_intelliceiver( device, power, cal_data );

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cal_rsp = (ftm_1x_calibrate_intelliceiver_rsp_type*)ftm_rsp_data.pkt_payload;

  if( cal_rsp != NULL )
  {
  if ( cal_status == TRUE ) /* iC Cal Successful */
  {
      size_t data_length = sizeof(rfm_1x_ic_calibration_data_type) ; 
    /* Flag as success */
    cal_rsp->success = 1;
     (void) memscpy ( &(cal_rsp->cal_data) , data_length , 
                      cal_data , data_length ) ;

    }/*if ( cal_status == TRUE )*/
  else /* iC Cal Failure */
  {

      /* Return ftm_1x_calibrate_intelliceiver_rsp_type as response with 
         Flag set as failure */
    cal_rsp->success = 0;

    FTM_MSG( FTM_ERROR, "iC Calibration : Failed !" );

    }/* if ( cal_status != TRUE )*/
  }/* if( cal_rsp != NULL )*/
  else
  {
    FTM_MSG( FTM_ERROR, "iC Calibration : NULL cal_rsp" );
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }/* if( cal_rsp == NULL ) */

  /* Free the Memory */
  ftm_free(cal_data);

  FTM_MSG( FTM_LOW, "CDMA Calv2 : iC Calibration Done" );

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_calibrate_intelliceiver */


/*! \} */

/*============================================================================*/
/*!
  @name Common Calibration Sweep (v3)

  @brief
  This section contains functions for FTM to perform Common Calibration Sweep.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure Radio for Calibration Sweep
 
  @details
  Based on the RF Configuration paylaod, this function will configure the radio.
  All radio tuning actions will be done under FTM Task context by sending
  Radio tune command to FTM Task Q.
 
  @param rf_cfg
  Pointer to the data structure containing RF Configuration parameter
*/
boolean
ftm_1x_calibration_sweep_configure_radio
(
   ftm_cal_instr_payload_config_radio_type const *rf_cfg
)
{

  /* Config radio request */
  ftm_cdma_cal_sweep_configure_radio_req_type cfg_req;

  /* NULL pointer check */
  if ( rf_cfg == NULL )
  {
    return FALSE;
  }

  /* Populate the Request Packet to configure radio */
  cfg_req.cmd = DIAG_SUBSYS_CMD_F;
  cfg_req.sub_sys = FTM;
  cfg_req.mode = FTM_RFM_C;
  cfg_req.cdma_cmd = FTM_1X_CAL_V3_CONFIG_RADIO;
  cfg_req.req_len = 0;
  cfg_req.rsp_len = 0;
  cfg_req.device_master_mask = rf_cfg->device_master;
  cfg_req.device_slave = rf_cfg->device_slave;
  cfg_req.band = ftm_1x_resolve_bandclass( rf_cfg->rf_mode );
  cfg_req.channel = rf_cfg->channel;

  ftm_common_send_radio_command( &cfg_req,
                     sizeof(ftm_cdma_cal_sweep_configure_radio_req_type) );

  return TRUE;
} /* ftm_1x_calibration_sweep_configure_radio */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Retune Radio for Calibration Sweep
 
  @details
  Based on the RF retune paylaod, this function will retune the radio. This
  function will retune all radios associated with the Master that is passed
  with the Retune radio paylaod. All radio tuning actions will be done under FTM 
  Task context by sending Radio tune command to FTM Task Q.
 
  @param rf_retune
  Pointer to the data structure containing RF retune parameter
*/
boolean
ftm_1x_calibration_sweep_retune_radio
(
   ftm_cal_instr_payload_retune_radio_type const *rf_retune
)
{
  /* Config radio request */
  ftm_cdma_cal_sweep_retune_radio_req_type retune_req;

  /* NULL pointer check */
  if ( rf_retune == NULL )
  {
    return FALSE;
  }

  /* Populate the Request Packet to configure radio */
  retune_req.cmd = DIAG_SUBSYS_CMD_F;
  retune_req.sub_sys = FTM;
  retune_req.mode = FTM_RFM_C;
  retune_req.cdma_cmd = FTM_1X_CAL_V3_RETUNE_RADIO;
  retune_req.req_len = 0;
  retune_req.rsp_len = 0;
  retune_req.device_master_mask = rf_retune->device_master;
  retune_req.channel = rf_retune->channel;

  ftm_common_send_radio_command( &retune_req,
                     sizeof(ftm_cdma_cal_sweep_retune_radio_req_type) );

  return TRUE;

} /* ftm_1x_calibration_sweep_retune_radio */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Tear Down Radio for Calibration Sweep
 
  @details
  This function will tear down all radios that are tuned to CDMA mode to
  sleep state.
*/
boolean
ftm_1x_calibration_sweep_tear_down_radio
(
   void
)
{
  /* Config radio request */
  ftm_cdma_cal_sweep_tear_radio_req_type tear_down_req;

  /* Populate the Request Packet to configure radio */
  tear_down_req.cmd = DIAG_SUBSYS_CMD_F;
  tear_down_req.sub_sys = FTM;
  tear_down_req.mode = FTM_RFM_C;
  tear_down_req.cdma_cmd = FTM_1X_CAL_V3_TEAR_DOWN_RADIO;
  tear_down_req.req_len = 0;
  tear_down_req.rsp_len = 0;

  ftm_common_send_radio_command( &tear_down_req,
                     sizeof(ftm_cdma_cal_sweep_tear_radio_req_type) );

  return TRUE;

} /* ftm_1x_calibration_sweep_tear_down_radio */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure Rx for Calibration Sweep
 
  @details
  Based on the Rx measurement paylaod, this function will configure Rx
  measurement. This will call RFM function to override the LNA state and
  LAN gain value.
 
  This supports rx mesaurment configration on multiple devices; however the
  LNA state and gain is overriden to the value pertaining to one LNA gain
  state.
 
  Eventhough Rx action is a mask, CDMA supports only one Rx Action
  in a segment. Therefore determine the Rx Action. If there is more
  than one rx action, then this function will return failure. 
 
  @param rx_meas
  Pointer to the data structure containing Rx measurement parameter
 
  @return
  Status of Rx measurement configuration
*/
boolean
ftm_1x_calibration_sweep_configure_rx
(
  ftm_cal_instr_payload_config_rx_type const *rx_config
)
{

  rfm_device_enum_type device;
  rfm_lna_gain_state_type gain_state; /* gain state */
  boolean gain_state_determined = FALSE; /* Flag indicating, if the gain
  state is determined */
  boolean ret_val = TRUE;
  uint32 cal_action_mask = 0xFFFFFFFF; /* Variable to store CalAction Mask
  extracted */
  uint32 cal_action; /* Variable to store the CalAction for the segment. This
  will be modified in the while loop. */
  uint32 rx_flag_mask = 0xFFFFFFFF; /* Variable to store Rx Flag Mask
  extracted */
  uint32 rx_flag; /* Variable to store the Rx Flag for the segment. This
  will be modified in the while loop. */
  int16 lna_gain_prev_state; /* LNA Gain Value From previous State. */
  ftm_cdma_cal_v3_data_type *cal_data;
  rf_time_tick_type prof_cal_func; /* Function Profiling Variable */
  rf_time_type prof_cal_func_val = 0; /* Function Profiling Value Variable */

  prof_cal_func = rf_time_get_tick(); /* Get Time tick for Function Profile */

  /* Eventhough Rx action is a mask, CDMA supports only one Rx Action
  in a segment. Therefore determine the Rx Action. If there is more
  than one rx action, then this function will return failure. */

  /* Get the Cal Action for the current instruction */
  cal_action = (uint32)rx_config->cal_action;
  
    /* For the current unprocessed Cal Flag . The first bit will be from LSB. */
  cal_action_mask = inline_rf_extract_first_one_bit_mask( cal_action );
  gain_state = (rfm_lna_gain_state_type)\
                 inline_rf_extract_first_one_bit_lsb_location(cal_action_mask);

  /* Update Cal Action to set the processed bit to zero. */
  cal_action = cal_action & (~cal_action_mask);

  /* if Gain state is determined, determine if there are other RxCal actions.
  If there are more cal action, then it is a failure. Break the
  FOR loop */
  if ( cal_action != 0 )
  {
    ret_val = FALSE;
  }

  /* Perform configuration only for success case */
  if ( ret_val == TRUE )
  {

    /* Get the Cal Action for the current instruction */
    rx_flag = (uint32)rx_config->device_flag;
    
    /* For the current unprocessed Rx Flag. The first bit will be from LSB. */
    rx_flag_mask = inline_rf_extract_first_one_bit_mask( rx_flag );
        
    /* Loop until, the Cal Action Mask comes out to be zero */
    while ( rx_flag_mask != 0 )
    {

      /* Determine RFM device based on the given flag */
      device = ftm_calibration_dev_flag_to_rfm_dev(rx_flag_mask);

      /* If the device is not Invalid, perform Rx measurement configuration */
      if ( device != RFM_INVALID_DEVICE )
      {
        /* Get Calibration Data */
        cal_data = ftm_cdma_data_get_cal_v3_adr(device);

        /* Populate lna_offset_prev value reading the global variable  */
        if ( gain_state == RFM_LNA_GAIN_STATE_0 )
        {
          /* Force previous step LNA offset to be used for step index 0 to be 
          zero */
          lna_gain_prev_state = 0;
        }
        else
        {
          /* Otherwise use the offset from previous step. */
          lna_gain_prev_state = 
                    cal_data->lna_offset[rx_config->power_mode][gain_state-1];
        }

        if ( cal_data->curr_power_mode != rx_config->power_mode )
        {
          /* Set intelliceiver Power Mode */
          rfm_cdma_ftm_power_mode_override( device, rx_config->power_mode );
          cal_data->curr_power_mode = rx_config->power_mode;
        }
    
        /* Setup LNA to appropriate LNA State with given LNA offset. */
        rfm_1x_ftm_setup_lna_calibration( device,
                                          gain_state, 
                                          lna_gain_prev_state,
                                          RFM_1X_FTM_IC_ACQ_DURATION_US );

        /* Debug Message */
        if ( cal_data->debug_en == TRUE )
        {
          FTM_MSG_3( FTM_MED,"ftm_1x_calv3_configure_rx - Dev %d LNA %d "
                     "LNAOffset %d", device, gain_state, lna_gain_prev_state );
        } /* if ( cal_data->debug_en == TRUE ) */

      } /* if ( device != RFM_INVALID_DEVICE ) */
    
      /* Update Rx Flag to set the processed bit to zero. */
      rx_flag = rx_flag & (~rx_flag_mask);

      /* For the current unprocessed Rx Flag. The first bit will be from LSB. */
      rx_flag_mask = inline_rf_extract_first_one_bit_mask( rx_flag );
      
    } /* while ( rx_flag_mask != 0 ) */
  } /* if ( ret_val == TRUE ) */
  else
  {
    FTM_MSG_2( FTM_ERROR, "Cal v3 ftm_1x_calv3_configure_rx: Failed - "
               "ret_val = %d, Gain State Determined = %d", ret_val, 
               gain_state_determined );
  } /* if ! ( ret_val == TRUE ) */

  /* Get Time for this function */
  prof_cal_func_val = rf_time_get_elapsed( prof_cal_func, RF_USEC );

  FTM_MSG_1( FTM_MED, "Cal v3 ftm_1x_calibration_sweep_configure_rx - Cal V3 "
                      "Profile %d us", prof_cal_func_val );
  
  return TRUE;

} /* ftm_1x_calibration_sweep_configure_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure Tx for Calibration Sweep
 
  @details
  Based on the Tx measurement paylaod, this function will configure Tx radio
  power. For CDMA, there are no TX calibration actions defined and only
  one device can be calibrated per segment.
 
  @param tx_config
  Pointer to the data structure containing Tx measurement parameter
 
  @return
  Flag indicating if the Tx is configured successfully.
*/
boolean
ftm_1x_calibration_sweep_configure_tx
(
   ftm_cal_instr_payload_config_tx_type const *tx_config
)
{
  rfm_device_enum_type device = RFM_INVALID_DEVICE;
  boolean dev_determined = FALSE; /* Flag indicating, if the dev is determined*/
  boolean ret_val = TRUE; /* Return value */
  uint32 tx_flag_mask = 0xFFFFFFFF; /* Variable to store Tx Flag Mask
  extracted */
  uint32 tx_flag; /* Variable to store the Tx Flag for the segment. This
  will be modified in the while loop. */
  uint32 cal_action_mask = 0xFFFFFFFF; /* Variable to store CalAction Mask
  extracted */
  uint32 cal_action; /* Variable to store the CalAction for the segment. This
  will be modified in the while loop. */
  rf_time_tick_type prof_cal_func;   /* Function Profiling Variable */
  rf_time_type prof_cal_func_val = 0;  /* Function Profiling Value Variable */

  prof_cal_func = rf_time_get_tick(); /* Get Time tick for Function Profile */

  if ( tx_config == NULL )
  {
    FTM_MSG( FTM_ERROR, "Cal v3 ftm_1x_calibration_sweep_configure_tx: "
                        "NULL pointer" );
    return FALSE;
  }
  
  /* Eventhough Tx Flag is a mask, CDMA supports only one Tx Flag
  in a segment. Therefore determine the Tx Flags. If there is more
  than one Tx flag, then this function will return failure. */

  /* Get the Tx Flag for the current instruction */
  tx_flag = (uint32)tx_config->device_flag;
  
  /* Loop untill Tx Flag is zero */
  while ( tx_flag != 0 )
  {
    /* For the current unprocessed Tx Flag. The first bit will be from LSB. */
    tx_flag_mask = rf_extract_first_one_bit_mask( tx_flag );

    /* Based on Cal Flag, configure Tx  */
    switch( tx_flag_mask )
    {

    case DEVICE_FLAG_0:
      device = RFM_DEVICE_0;
      dev_determined = TRUE;
      break;

    case DEVICE_FLAG_1:
      device = RFM_DEVICE_1;
      dev_determined = TRUE;
      break;

    case DEVICE_FLAG_2:
      device = RFM_DEVICE_2;
      dev_determined = TRUE;
      break;

    default:
      device = RFM_INVALID_DEVICE;
      dev_determined = FALSE;
      break;

    }

    /* Update Tx Flag to set the processed bit to zero. */
    tx_flag = tx_flag & (~tx_flag_mask);

    /* if device is determined, determine if there is other TxCal device.
    If there are more cal action, then it is a failure. Break the
    FOR loop */
    if ( dev_determined == TRUE )
    {
      if ( tx_flag != 0 )
      {
        ret_val = FALSE;
      }

      break;
    } /* if ( dev_determined == TRUE ) */

  }

  /* Perform configuration only for success case */
  if ( ( ret_val == TRUE ) && ( dev_determined == TRUE ) )
  {
    /* Query the current RF State - need this to get the current band info */
    ftm_cdma_data_status_type *dev_ptr = ftm_cdma_data_get_device_ptr(device);

    /* check for valid device ptr */
    if (dev_ptr == NULL)
    {
      FTM_MSG_1( FTM_ERROR, "Cal v3 ftm_1x_calibration_sweep_configure_tx: "
                 "Failed to get a valid device pointer for device %d", device );
      ret_val = FALSE;
    }
    else
    {

      /* Perform Tx Override, only if the RGI is valid */
      if ( tx_config->rgi != FTM_CALIBRATION_INVALID_RGI )
      {

        ret_val &= rfm_1x_ftm_set_fw_tx_agc_override( 
                        device, dev_ptr->curr_band, 
                        tx_config->pa_state, 
                        tx_config->rgi, 0, 
                        tx_config->pa_ctl_struct.pa_bias,
                        tx_config->pa_ctl_struct.quiscent_current,
                        (boolean)tx_config->pa_ctl_struct.quiscent_current_en );  

      } /* if ( tx_config->rgi != FTM_CALIBRATION_INVALID_RGI ) */
      else
      {
        FTM_MSG_2( FTM_MED, "Cal v3 ftm_1x_calibration_sweep_configure_tx: "
                   "Dev %d skipped Tx RGI %d override", device, 
                   tx_config->rgi );
      } /* if ! ( tx_config->rgi != FTM_CALIBRATION_INVALID_RGI ) */

      /* Perform multiple Tx actions based on Tx Action */

      /* Get the Cal Action for the current instruction */
      cal_action = (uint32)tx_config->cal_action;

      /* Loop until, the Cal Action Mask comes out to be zero */
      while ( cal_action_mask != 0 )
      {
        /* For the current unprocessed Cal Flag . The first bit will be from
        LSB. */
        cal_action_mask = rf_extract_first_one_bit_mask( cal_action );

        /* Based on Cal Actions, configure Rx  */
        switch( cal_action_mask )
        {

        default:
          break;

        }

        /* Update Cal Action to set the processed bit to zero. */
        cal_action = cal_action & (~cal_action_mask);

      } /* while ( cal_action_mask != 0 ) */

    } /* if (dev_ptr == NULL) */

  } /* if ( ( ret_val == TRUE ) && ( dev_determined == TRUE ) ) */
  else
  {
    FTM_MSG_2( FTM_ERROR, "Cal v3 ftm_1x_calibration_sweep_configure_tx: "
               "Failed - ret_val = %d, device Determined = %d", ret_val,
               dev_determined );
    ret_val = FALSE;
  } /* if ( ( ret_val == TRUE ) && ( gain_state_determined == TRUE ) ) */

  /* Get Time for this function */
  prof_cal_func_val = rf_time_get_elapsed( prof_cal_func, RF_USEC );

  FTM_MSG_1( FTM_MED, "Cal v3 ftm_1x_calibration_sweep_configure_tx - Cal V3 "
                      "Profile %d us", prof_cal_func_val );
  
  return ret_val;

} /* ftm_1x_calibration_sweep_configure_tx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Measure Tx for Calibration Sweep
 
  @details
  Based on the HDET paylaod, this function will measure HDET and LPM HDET.
  For CDMA, only one device can be measured for HDET per segment.
 
  @param tx_meas
  Pointer to the data structure containing HDET parameter
 
  @return
  Flag indicating if the HDET is measured successfully.
*/
boolean
ftm_1x_calibration_sweep_measure_tx
(
   ftm_cal_instr_payload_meas_hdet_type const *hdet_meas,
   ftm_calibration_tx_result_type *tx_result
)
{
  rfm_device_enum_type device = RFM_INVALID_DEVICE;
  boolean dev_determined = FALSE; /* Flag indicating, if the dev is determined*/
  uint32 hdet_read = 0; /* HDET Read accumulator */
  uint8 tx_meas_count = 0; /* Tx measurement count */
  uint16 tx_meas_size_count = 0; /* Tx measurement size count */
  boolean ret_val = TRUE; /* Return Value */
  uint32 tx_flag_mask = 0xFFFFFFFF; /* Variable to store Tx Flag Mask
  extracted */
  uint32 tx_flag; /* Variable to store the Tx Flag for the segment. This
  will be modified in the while loop. */
  rf_time_tick_type prof_cal_func;     /* Function Profiling Variable */
  rf_time_type prof_cal_func_val = 0; /* Function Profiling Value Variable */
  ftm_cdma_cal_v3_data_type *cal_data;

  prof_cal_func = rf_time_get_tick(); /* Get Time tick for Function Profile */

  /* NULL Pointer Check */
  if ( ( hdet_meas == NULL ) || ( tx_result == NULL ) )
  {
    FTM_MSG( FTM_ERROR, "Cal v3 ftm_1x_calibration_sweep_measure_tx : "
                        "NULL Pointer" );
    return FALSE;
  }

  /* NULL Pointer Check */
  if ( tx_result->tx_buffer == NULL )
  {
    FTM_MSG( FTM_ERROR, "Cal v3 ftm_1x_calibration_sweep_measure_tx : "
                        "NULL Buffer" );
    return FALSE;
  }
  
  /* Eventhough Tx Flag is a mask, CDMA supports only one Tx Flag
  in a segment. Therefore determine the Tx Flags. If there is more
  than one Tx flag, then this function will return failure. */

  /* Get the Tx Flag for the current instruction */
  tx_flag = (uint32)hdet_meas->device_flag;

  /* Loop untill Tx Flag is zero */  
  while ( tx_flag != 0 )
  {
    /* For the current unprocessed Tx Flag. The first bit will be from LSB. */
    tx_flag_mask = rf_extract_first_one_bit_mask( tx_flag );

    /* Based on Cal Flag, configure Tx  */
    switch( tx_flag_mask )
    {

    case DEVICE_FLAG_0:
      device = RFM_DEVICE_0;
      dev_determined = TRUE;
      break;

    case DEVICE_FLAG_1:
      device = RFM_DEVICE_1;
      dev_determined = TRUE;
      break;

    case DEVICE_FLAG_2:
      device = RFM_DEVICE_2;
      dev_determined = TRUE;
      break;

    default:
      device = RFM_INVALID_DEVICE;
      dev_determined = FALSE;
      break;

    }

    /* Update Tx Flag to set the processed bit to zero. */
    tx_flag = tx_flag & (~tx_flag_mask);

    /* if device is determined, determine if there is other TxCal device.
    If there are more cal action, then it is a failure. Break the
    FOR loop */
    if ( dev_determined == TRUE )
    {
      if ( tx_flag != 0 )
      {
        ret_val = FALSE;
      }

      break;
    } /* if ( dev_determined == TRUE ) */
  } /* while ( tx_flag != 0 ) */

  /* Perform configuration only for success case */
  if ( ( ret_val == TRUE ) && ( dev_determined == TRUE ) )
  {
    /* Allow for FW latency to latch the required PDM value to the RTR.
    In cal override mode, FW operates with a 64-chip update interval,
    and so we provide an additional 50usec delay here */

    /* Take HDET reads, only if number if HDET samples are more than 0 */
    /* HDET needs time to settle. Time to settle varies based on HDET
    used. This time will need to be changed if any HDET used has longer 
    settling time. Before changing time make sure it doen not spil 
    over next ISR for composite cal. Wait is needed only for first
    reading. */
    DALSYS_BusyWait( FTM_1X_FW_LATENCY_AFTER_RGI_UPDATE_US );
  
    /* Take samples of HDET read  */
    if ( hdet_meas->hdet_sample > 0 )
    {
      /* Get Calibration Data */
      cal_data = ftm_cdma_data_get_cal_v3_adr(device);

      rfm_cdma_ftm_setup_hdet( device, RFDEVICE_CDMA_HDET_HPM );
      /* Allow for PDET settling time after setting up HDET */
      DALSYS_BusyWait( rfm_cdma_ftm_get_hdet_settling_time(device));

      hdet_read = rfm_cdma_ftm_multi_read_hdet( device,
                                                hdet_meas->hdet_sample );

      if ( cal_data->debug_en == TRUE )
      {
        FTM_MSG_1( FTM_HIGH, "Cal v3 ftm_1x_calibration_sweep_measure_tx: "
                             "HDET Read = %d", hdet_read );
      }

      /* Check the tx measurement counter against the maximum allowed
      number of measurements */
      if ( tx_result->max_tx_buffer_len >= 
           (tx_meas_size_count + sizeof(ftm_calibration_tx_meas_data_type)) )
      {
        /* Get measurement data */
        ((ftm_calibration_tx_meas_data_type*)(tx_result->tx_buffer))[tx_meas_count] = 
                                                                (uint16)hdet_read;

        /* Increment the tx_meas counter */
        tx_meas_size_count += sizeof(ftm_calibration_tx_meas_data_type);

        /* Increment the measurement counter */
        tx_meas_count ++;
      }
      else
      {
        /* Buffer Overflow. Break immediately */
        ret_val = FALSE;
      }
    }/*if ( hdet_meas->hdet_sample > 0 )*/

    /* Take samples of LPM HDET read*/
    if ( hdet_meas->lpm_hdet_sample > 0 )
    {
      rfm_cdma_ftm_setup_hdet( device, RFDEVICE_CDMA_HDET_LPM );
      /* Allow for PDET settling time after setting up HDET */
      DALSYS_BusyWait( rfm_cdma_ftm_get_hdet_settling_time(device));

      hdet_read = rfm_cdma_ftm_multi_read_hdet( device,
                                                hdet_meas->lpm_hdet_sample );

      /* Check the tx measurement counter against the maximum allowed
      number of measurements */
      if ( tx_result->max_tx_buffer_len >= 
           (tx_meas_size_count + sizeof(ftm_calibration_tx_meas_data_type)) )
      {
        /* Get measurement data */
        ((ftm_calibration_tx_meas_data_type*)(tx_result->tx_buffer))[tx_meas_count] = 
                                                             (uint16)hdet_read;

        /* Increment the tx_meas counter */
        tx_meas_size_count += sizeof(ftm_calibration_tx_meas_data_type);

        /* Increment the measurement counter */
        tx_meas_count ++;
      }/* if ( tx_result->max_tx_buffer_len...*/
      else
      {
        /* Buffer Overflow. Break immediately */
        ret_val = FALSE;
      }
    }/*if ( hdet_meas->lpm_hdet_sample > 0 )*/
  }/* if ( ( ret_val == TRUE ) && ( gain_state_determined == TRUE ) ) */
  else
  {
    FTM_MSG_2( FTM_ERROR, "Cal v3 ftm_1x_calibration_sweep_measure_tx: "
               "Failed - ret_val = %d, device Determined = %d", ret_val,
               dev_determined );
    ret_val = FALSE;
  } /* if ( ( ret_val == TRUE ) && ( gain_state_determined == TRUE ) ) */

  /* Update number of Tx measurements */
  tx_result->num_of_tx_reads = tx_meas_count;

  /* Get Time for this function */
  prof_cal_func_val = rf_time_get_elapsed( prof_cal_func, RF_USEC );

  FTM_MSG_1( FTM_MED, "Cal v3 ftm_1x_calibration_sweep_measure_tx - Cal V3 "
                      "Profile %d us", prof_cal_func_val );
  
  return ret_val;
  
} /* ftm_1x_calibration_sweep_measure_tx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Measure Rx for Calibration Sweep
 
  @details
  Based on the Rx Measurement paylaod, this function will measure Rx.
  All measurment data will be put in the segment result buffer. To avoid
  any buffer overflow, this function will check the current total rx results
  size with the maximum length of the results buffer.
 
  @param rx_meas
  Pointer to the data structure containing Rx Measurement parameter
 
  @param rx_result
  Pointer to the container to hold Rx calinration results
 
  @return
  Flag indicating the validity of the Rx results.
 
  @retval TRUE
  The Rx calibration results are valid
 
  @retval FALSE
  The Rx calibration results are invalid
*/
boolean
ftm_1x_calibration_sweep_measure_rx
(
  ftm_cal_instr_payload_meas_rx_type const *rx_meas,
  ftm_calibration_rx_result_type *rx_result
)
{
  rfm_device_enum_type device;
  rfm_lna_gain_state_type gain_state; /* gain state */
  boolean gain_state_determined = FALSE; /* Flag indicating, if the gain
  state is determined */
  boolean ret_val = TRUE;
  uint8 rx_meas_count = 0; /* Rx measurement count */
  uint16 rx_meas_size_count = 0; /* Rx measurement size count */
  uint32 cal_action_mask = 0xFFFFFFFF; /* Variable to store CalAction Mask
  extracted */
  uint32 cal_action; /* Variable to store the CalAction for the segment. This
  will be modified in the while loop. */
  uint32 rx_flag_mask = 0xFFFFFFFF; /* Variable to store Rx Flag Mask
  extracted */
  uint32 rx_flag; /* Variable to store the Rx Flag for the segment. This
  will be modified in the while loop. */
  ftm_cdma_cal_v3_data_type *cal_data = NULL;
  int16 agc_expected_val_1_640th = 0; /* Expected RxAGC in 1/640dB resolution */
  int16 lna_gain_prev_state; /* LNA Offset Value From previous Step. This
                                value will be used passed step_index */
  int16 lna_offset = 0; /* LNA Offset Value */
  int16 lna_offset_1_10th = 0; /* LNA Offset Value in 1/10th dB resolution*/
  boolean reset_agc_accum = TRUE;
  int16 agc_avg_val;
  uint8 readx; /* Counter variable for FOR Loop */
  int16 agc_read_val = 0; /* AGC Read Value */
  rf_time_tick_type prof_cal_func; /* Function Profiling Variable */
  rf_time_type prof_cal_func_val = 0; /* Function Profiling Value Variable */
  rfm_cdma_power_mode_type power_mode;

  /* Get Time tick for Function Profile */
  prof_cal_func = rf_time_get_tick();

  /* NULL Pointer Check */
  if ( ( rx_result == NULL ) || (rx_meas == NULL ) )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_calibration_sweep_measure_rx : "
                        "NULL Pointer" );
    return FALSE;
  }

  /* NULL Pointer Check */
  if ( rx_result->rx_buffer == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_calibration_sweep_measure_rx :"
                        " NULL Buffer" );
    return FALSE;
  }

  /* Eventhough Rx action is a mask, CDMA supports only one Rx Action
  in a segment. Therefore determine the Rx Action. If there is more
  than one rx action, then this function will return failure. */

  /* Get the Cal Action for the current instruction */
  cal_action = (uint32)rx_meas->cal_action;
  
    /* For the current unprocessed Cal Flag . The first bit will be from LSB. */
  cal_action_mask = inline_rf_extract_first_one_bit_mask( cal_action );
  gain_state = (rfm_lna_gain_state_type)\
                 inline_rf_extract_first_one_bit_lsb_location(cal_action_mask);

  /* Update Cal Action to set the processed bit to zero. */
  cal_action = cal_action & (~cal_action_mask);

  /* if Gain state is determined, determine if there is other RxCal actions
  if there are more cal action, then it is a failure.  */
  if ( cal_action != 0 )
  {
    ret_val = FALSE;
  }

  if( gain_state >= RFM_LNA_GAIN_STATE_NUM )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_1x_calibration_sweep_measure_rx :"
                          " Invalide gain_state %d", gain_state );
    ret_val = FALSE;
  }

  /* Proceed only if Gain State is determined successfully */
  if ( ret_val == TRUE )
  {
    /*------------------------------------------------------------------------*/
    /* Sample RxAGC from all devices and store the total accumulated          */
    /* RxAGC to a FTM CDMA Data to be processed in future.                    */
    /*------------------------------------------------------------------------*/
    for ( readx = 0; readx < FTM_1X_RXAGC_RD_SAMPLES; readx++ )
    {
      /* Get the Cal Action for the current instruction */
      rx_flag = (uint32)rx_meas->device_flag;
      rx_flag_mask = (uint32)(-1);
  
      /* For the current unprocessed Rx Flag. The first bit will
      be from LSB. */
      rx_flag_mask = inline_rf_extract_first_one_bit_mask( rx_flag );

      /* Loop until, the Cal Action Mask comes out to be zero. First sample
      of the RxAGC will be taken for all devices before proceeding to next
      sample. This is done in order to avoid serializing of RxAGC sampling
      resulting in extra wait (FTM_1X_CAL_V3_RXAGC_WAIT_US) between two
      devices. */
      while ( rx_flag_mask != 0 )
      {
  
        /* Determine RFM device based on the given flag */
        device = ftm_calibration_dev_flag_to_rfm_dev(rx_flag_mask);
    
        /* If the device is not Invalid, Get Rx measurement */
        if ( device != RFM_INVALID_DEVICE )
        {
          /* Get Calibration Data */
          cal_data = ftm_cdma_data_get_cal_v3_adr(device);

          /* Reset the AGC Accum only if the reset flag is set to TRUE. This
          flag will be set to FALSE, when all devices are sampled atleast 
          once */
          if ( reset_agc_accum == TRUE )
          {
            cal_data->agc_acc_val = 0;
          } /* if ( reset_agc_accum == TRUE ) */
   
          /* Get RxAGC reading for the current device in raw format */
          agc_read_val = rfm_1x_ftm_get_rx_agc( device );
   
          /* Perform Sign Bit Extension for Negative AGC value */
          /* 512 - 1023 correspond to Negative AGC Read. Thus need to sign
          extend. */
         // if ( agc_read_val > 32767 )
         // {
         //   agc_read_val = agc_read_val - 65536;
         // }

          /* Debug Message */
          if ( cal_data->debug_en == TRUE )
          {
            FTM_MSG_3( FTM_MED, "ftm_1x_calibration_sweep_measure_rx - "
                       "RxAGC Sampling Dev %d, AGC %d (for Itr %d)",
                       device, 
                       agc_read_val, readx );
          }/* if ( cal_data->debug_en == TRUE ) */
   
          /* Accumulate all AGC Samples */
          cal_data->agc_acc_val += agc_read_val;
  
          /* Compute the average lna offset if the last sampling is done */
          if ( readx == (FTM_1X_RXAGC_RD_SAMPLES - 1) )
          {
            /* Convert to 1/640th dB resolution */
            agc_expected_val_1_640th = (rx_meas->exp_rxagc)*64;
          
            /* Power Mode */
            power_mode = rx_meas->power_mode;
          
            /* Perform Sign Bit Extension for Negative AGC value */

            /* 32768 - 65536 correspond to Negative AGC Read. Thus need to 
            sign extend. */
           // if ( agc_expected_val_1_640th > 32767 )
           // {
           //   agc_expected_val_1_640th = agc_expected_val_1_640th - 65536;
           // }
                      
            /* Populate lna_offset_prev value reading the global variable  */
            if ( gain_state == RFM_LNA_GAIN_STATE_0 )
            {
              /* Force previous step LNA offset to be used for step index 0 to
              be zero */
              lna_gain_prev_state = 0;
            } /* if ( gain_state == RFM_LNA_GAIN_STATE_0 ) */
            else
            {
              /* Otherwise use the offset from previous step. */
              lna_gain_prev_state = 
                              cal_data->lna_offset[power_mode][gain_state-1];
            } /* if ! ( gain_state == RFM_LNA_GAIN_STATE_0 ) */

            /* Average Out the Accumulated AGC Read value */
            if( cal_data->agc_acc_val > 0 ) /* For proper rounding */
            {
              agc_avg_val = 
                (int16)((cal_data->agc_acc_val + FTM_1X_RXAGC_RD_SAMPLES/2) / 
                                                    FTM_1X_RXAGC_RD_SAMPLES );
            } /* if( agc_acc_val > 0 ) */
            else
            {
              agc_avg_val =
                (int16)((cal_data->agc_acc_val - FTM_1X_RXAGC_RD_SAMPLES/2) /
                                                    FTM_1X_RXAGC_RD_SAMPLES );
            } /* if ! ( agc_acc_val > 0 ) */
            
            /* LNA offset is the difference between the actual AGC value and
            expected AGC value with addition of LNA offset value corresponding
            to LNA gain state higher by 1 */
            lna_offset = agc_avg_val - agc_expected_val_1_640th + 
                                                    lna_gain_prev_state;
            
            /* Store LNA offset in rf_lna_offset_val[] for future GET LNA
            OFFSET commands */
              cal_data->lna_offset[power_mode][gain_state] = lna_offset;
            
            /* Convert to 1/10th dB resolution */
            lna_offset_1_10th = lna_offset / 64;
            
                  /* Populate device specific results */
            /* Get measurement data */
            ((ftm_calibration_rx_meas_data_type*)
                    (rx_result->rx_buffer))[rx_meas_count] = lna_offset_1_10th;
            
            /* Increment the rx_meas counter */
            rx_meas_size_count += sizeof(ftm_calibration_rx_meas_data_type);
            
            /* Increment the measurement counter */
            rx_meas_count ++;
            
            /* Debug Message */
            if ( cal_data->debug_en == TRUE )
            {
              FTM_MSG_7( FTM_MED,"ftm_1x_calibration_sweep_measure_rx - "
                     "Dev %d LNA %d (act - exp + prev) = (%d-%d+%d)=%d "
                     "(in 1/10th : %d)", device, gain_state, agc_avg_val, 
                         agc_expected_val_1_640th, lna_gain_prev_state, 
                         lna_offset, lna_offset_1_10th );
            } /* if ( cal_data->debug_en == TRUE ) */
          } /* if ( readx == (FTM_1X_RXAGC_RD_SAMPLES - 1) ) */

        } /* if ( device != RFM_INVALID_DEVICE ) */

        /* Update Rx Flag to set the processed bit to zero. */
        rx_flag = rx_flag & (~rx_flag_mask);

        /* For the current unprocessed Rx Flag. The first bit will
        be from LSB. */
        rx_flag_mask = inline_rf_extract_first_one_bit_mask( rx_flag );

      } /* while ( rx_flag_mask != 0 ) */

      /* Wait for FTM_1X_CAL_V3_RXAGC_WAIT_US mS before taking another read.
      There is no need to wait, if this is the last loop as there will be no
      more RxAGC sampling */
      if ( readx != ( FTM_1X_RXAGC_RD_SAMPLES - 1 ) )
      {
        DALSYS_BusyWait( FTM_1X_CAL_V3_RXAGC_WAIT_US );
      }

      /* Set the AGC Reset flag to FALSE, so that it is not reset for further
      AGC accumulation. */
      reset_agc_accum = FALSE;

    } /* for ( readx = 0; readx < FTM_1X_RXAGC_RD_SAMPLES; readx++ ) */

  } /* if ( ( ret_val == TRUE ) && ( gain_state_determined == TRUE ) ) */
  else
  {
    FTM_MSG_2( FTM_ERROR, "ftm_1x_calibration_sweep_measure_rx: "
               "Failed - ret_val = %d, Gain State Determined = %d", ret_val, 
               gain_state_determined );
    ret_val = FALSE;
  } /* if ( ( ret_val == TRUE ) && ( gain_state_determined == TRUE ) ) */

  /* Update the total number of rx measurements taken during this call. */
  rx_result->num_of_rx_reads = rx_meas_count;
  
  /* Get Time for this function */
  prof_cal_func_val = rf_time_get_elapsed( prof_cal_func, RF_USEC );

  FTM_MSG_1( FTM_MED, "ftm_1x_calibration_sweep_measure_rx - Profile %d us", 
             prof_cal_func_val );

  return ret_val; 
  
} /* ftm_1x_calibration_sweep_measure_rx */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform Radio Setup to given state
 
  @details
  This function will be called from cal v3 common framework to perform
  radio setup operation. Radio setup involves operation such as first
  radio setung, channel retuning, feedback setup, and tx waveform
  configuration. Thus all these cases should be handled by this function.
 
  @param radio_setup
  Pointer to the data structure containing radio setup unpacked parameter
 
  @return
  Flag indicating the status of radio setup operation
*/
boolean
ftm_1x_calibration_v3_radio_setup
(
  ftm_cal_radio_setup_parameter_type const *radio_setup
)
{
  /* Config radio request */
  ftm_cal_radio_setup_req_type rs_req;

  /* Parameter validation */
  if ( radio_setup == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_calibration_v3_radio_setup: NULL argument" );
    return FALSE;
  } /* if ( radio_setup == NULL ) */

  /* Populate the Request Packet to configure radio */
  rs_req.cmd = DIAG_SUBSYS_CMD_F;
  rs_req.sub_sys = FTM;
  rs_req.mode = FTM_1X_C0_C;
  rs_req.cdma_cmd = FTM_INT_CALV3_RADIO_SETUP;

  {
    size_t data_length = sizeof(ftm_cal_radio_setup_parameter_type) ; 
    memscpy ( &(rs_req.rs_param), data_length , 
              radio_setup , data_length );
  }
  ftm_common_send_radio_command( &rs_req,
                                 sizeof(ftm_cal_radio_setup_req_type) );

  return TRUE;

} /* ftm_1x_calibration_v3_radio_setup */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform Feedback Setup to given state
 
  @details
  This function will be called from cal v3 common framework to perform
  feedback setup operation.
 
  @param feedback_setup
  Pointer to the data structure containing radio setup unpacked parameter
 
  @return
  Flag indicating the status of feedback setup operation
*/
boolean
ftm_1x_calibration_v3_feedback_setup
(
  ftm_cal_feedback_setup_parameter_type const *feedback_setup
)
{
    /* Config radio request */
  ftm_cal_feedback_setup_req_type fb_req;

  /* Parameter validation */
  if ( feedback_setup == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_calibration_v3_feedback_setup: NULL argument" );
    return FALSE;
  } /* if ( feedback_setup == NULL ) */

  /* Populate the Request Packet to configure radio */
  fb_req.cmd = DIAG_SUBSYS_CMD_F;
  fb_req.sub_sys = FTM;
  fb_req.mode = FTM_1X_C0_C;
  fb_req.cdma_cmd = FTM_INT_CALV3_FEEDBACK_SETUP;

  {
    size_t data_length = sizeof(ftm_cal_feedback_setup_parameter_type) ; 
    memscpy ( &(fb_req.fb_param), data_length , 
              feedback_setup , data_length );
  }

  /* If this segment contains only a WTR config command, call it in ISR context */
  if ( feedback_setup->feedback_setup_action == FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_WTR )
  {
    (void)ftm_1x_calibration_feedback_setup( &fb_req );
  }
  else
  {
  ftm_common_send_radio_command( &fb_req,
                                 sizeof(ftm_cal_feedback_setup_req_type) );
  }

  return TRUE;
} /*ftm_1x_calibration_v3_feedback_setup*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform Feedback Setup
 
  @details
  This function will be called from cal v3 common framework to perform
  feedback setup operation.
 
  @param rs_req
  Pointer to the data structure containing feedback setup request data
 
  @return
  Flag indicating the status of feedback setup operation
*/
boolean
ftm_1x_calibration_feedback_setup
(
  ftm_cal_feedback_setup_req_type *fb_req
)
{
  boolean ret_val = TRUE;
  rfm_device_enum_type device;          /* Var to hold primary device id */
  ftm_cdma_data_status_type dev_state;  /* Var to hold current state */

  rfm_device_enum_type device_fb;       /* Var to hold feedback device id */
  ftm_cdma_data_status_type dev_fb_state; /* Var to hold current state */

  ftm_cal_feedback_setup_parameter_type const *feedback_setup = NULL; /* FB Setup param */
  
  /* Parameter validation */
  if ( fb_req == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_calibration_radio_setup: NULL argument" );
    return FALSE;
  } /* if ( fb_req == NULL ) */

  /* Get the pointer to radio setup param */
  feedback_setup = &(fb_req->fb_param);

  device = feedback_setup->primary_device;
  device_fb = feedback_setup->feedback_device;

  (void)ftm_cdma_data_get_device_status( device_fb, &dev_fb_state );
  (void)ftm_cdma_data_get_device_status( device, &dev_state );  

  if ( feedback_setup->feedback_setup_action & 
                          FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_WTR )
  {
      /* Configure feedback transceiver */
     if( feedback_setup->configure_wtr.enable_mode != 0 )
     {
        ret_val &= rfm_cdma_ftm_config_fb_transceiver( device,
                                                     device_fb, 
                                                     RFM_1X_MODE,
                                                     dev_state.curr_band,
                                                     dev_state.curr_chan,
                                                     feedback_setup->configure_wtr.fb_state );
     }
     else
     {
        ret_val &= rfm_cdma_ftm_deconfig_fb_transceiver( device,
                                                     device_fb, 
                                                     RFM_1X_MODE );
     }
  }

  if ( feedback_setup->feedback_setup_action & 
                          FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_RXLM_FOR_XPT )
  {
    /* FTM Enter to 1x mode for fedback path + Get FTM Status for FB device */
    ftm_rfmode_enter( device_fb, FTM_STATE_1X );

    /* Configure feedback RxLM */
    ret_val &= rfm_1x_ftm_config_fb_rxlm( device,
                                          device_fb, 
                                          dev_fb_state.xpt_fb_handle,
                                          dev_state.curr_band,
                                          dev_state.curr_chan );
  }

  if ( feedback_setup->feedback_setup_action & 
                          FEEDBACK_SETUP_ACTION_MASK_CONFIGURE_RXLM_FOR_FBAGC )
  {
    rfcommon_fbrx_error_type ret_value;

    ret_value = rfm_1x_ftm_config_rxlm_for_fbagc( );

    if (ret_value == RFCOMMON_FBRX_SUCCESS) 
    {
        FTM_MSG(FTM_LOW,"FEEDBACK_SETUP_ACTION re-enabled FBRX");
    }
  }

  FTM_MSG_1( FTM_LOW, "ftm_1x_cal_feedback_setup : [Status %d] ", ret_val );

  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform Radio Setup to given state
 
  @details
  This function will be called from cal v3 common framework to perform
  radio setup operation. Radio setup involves operation such as first
  radio setung, channel retuning, feedback setup, and tx waveform
  configuration. Thus all these cases should be handled by this function.
 
  @param rs_req
  Pointer to the data structure containing radio setup request data
 
  @return
  Flag indicating the status of radio setup operation
*/
boolean
ftm_1x_calibration_radio_setup
(
  ftm_cal_radio_setup_req_type *rs_req
)
{
  rfm_device_enum_type mdev; /* Master device */
  rfm_device_enum_type sdev; /* Slave device */
  ftm_cal_radio_setup_mdevice_state_type mdev_state; /* Master device State */
  ftm_cal_radio_setup_sdevice_state_type sdev_state; /* Slave device State */
  ftm_cdma_data_status_type *mdev_status = NULL; /* mDevice Status pointer */
  ftm_cdma_data_status_type *sdev_status = NULL; /* sDevice Status pointer */
  ftm_1x_rfm_callback_data_type cb_payload; /* Payload for callback */
  sys_channel_type band_chan; /* System band and channel */
  boolean rfm_ret = TRUE; /* flag indicating the RFM execution status */
  /* Pointer to cal v3 data structure for both master and slave devices*/
  ftm_cdma_cal_v3_data_type *cal_v3_data, *cal_v3_data_s; 
  uint8 rgi_min; /* Minimum RGI */
  uint8 rgi_max; /* MAximum RGI */
  rfm_cdma_band_class_type band; /* CDMA Band */
  uint8 num_of_chan;
  uint32 channel;
  boolean ret_val = TRUE; /* Status of radio setup */
  rf_time_tick_type rs_tick;  /* Radio Setup Time tick */
  rf_time_type rs_time; /* Radio setup time */
  ftm_cal_radio_setup_parameter_type const *radio_setup = NULL; /* RS param */
  
  /* Start Profiling of Rx Warm Up */
  rs_tick = rf_time_get_tick();
  
  /* Parameter validation */
  if ( rs_req == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_calibration_radio_setup: NULL argument" );
    return FALSE;
  } /* if ( radio_setup == NULL ) */

  /* Get the pointer to radio setup param */
  radio_setup = &(rs_req->rs_param);

  /* Extract Radio Setup parameter */
  mdev = radio_setup->mdevice;
  sdev = radio_setup->sdevice;
  mdev_state = radio_setup->mdevice_state;
  sdev_state = radio_setup->sdevice_state;
  band_chan.band = radio_setup->band;
  band_chan.chan_num = radio_setup->chan_list[0].channel;
  num_of_chan = radio_setup->num_of_chan;
  band = radio_setup->band;
  channel = radio_setup->chan_list[0].channel;

  /*==========================================================================*/
  /* MASTER DEVICE Tuning */
  /*==========================================================================*/

  /* Perform Master device tuning */
  if ( ( mdev_state != MASTER_DEVICE_NO_CHANGE ) &&
       ( mdev_state < MASTER_DEVICE_STATE_MAX) )
  {
    /* Query the current RF State Ptr for master device */
    mdev_status = ftm_cdma_data_get_device_ptr( mdev );

    /* Get Pointer to Cal v3 data */
    cal_v3_data = ftm_cdma_data_get_cal_v3_adr( mdev );

    /* Perform NULL pointer check */
    if( ( mdev_status == NULL ) || ( cal_v3_data == NULL ))
    {
      ret_val = FALSE;
      FTM_MSG_1( FTM_ERROR,"ftm_1x_calibration_radio_setup: NULL "
                           "data for mDevice %d", mdev );
    } /* if( mdev_status == NULL ) */
    else
    {

      /*----------------------------------------------------------------------*/
      /* MASTER_DEVICE_DISABLED */
      /*----------------------------------------------------------------------*/

      if ( mdev_state == MASTER_DEVICE_DISABLED )
      {

        /* Tear Down RF to enter 1x mode */  
        ftm_1x_tear_down(mdev);
        /* Tear down data module when master device is disabled */
        ftm_cal_data_teardown_data_module();

      } /* if ( radio_setup->mdevice_state == MASTER_DEVICE_DISABLED ) */

      /*----------------------------------------------------------------------*/
      /* MASTER_DEVICE_RX || MASTER_DEVICE_RXTX */
      /*----------------------------------------------------------------------*/

      else if ( ( mdev_state == MASTER_DEVICE_RX ) ||
                ( mdev_state == MASTER_DEVICE_RXTX ) )
      {
        /* Enter FTM to 1x Mode */
        rfm_ret &= ( ftm_rfmode_enter( mdev, FTM_STATE_1X ) == 1 );

        /* Tear Down RF to enter 1x mode */  
        ftm_1x_tear_down(mdev);

        /* Enter 1x Mode */
        rfm_ret &= ( rfm_enter_mode( mdev, RFM_1X_MODE, NULL, NULL,
                                     mdev_status->rxlm_handle) == 0 );

        cb_payload.device = mdev; /* Populate the CB Payload */

        rfm_ret &= ftm_cdma_control_enable_tech( RFM_1X_MODE,
                                               mdev, 
                                               band_chan,
                                               mdev_status->rxlm_handle, 
                                               &ftm_1x_rfm_callback, 
                                               &cb_payload );

        /* Make sure everything passed till this point before calling tune
        APIs */
        if ( rfm_ret != FALSE )
        {
          /* Perform Wakeup Rx preparation */
          cb_payload.device = mdev; /* Populate the CB Payload */
          rfm_ret &= ( rfm_1x_prep_wakeup_rx( mdev, mdev_status->rxlm_handle,
                                              band_chan, &ftm_1x_rfm_callback, 
                                              &cb_payload ) >= 0 );
      
          /* Perform wakeup Rx execution */
          cb_payload.device = mdev; /* Populate the CB Payload */
          rfm_ret &= ( rfm_1x_exec_wakeup_rx( mdev, mdev_status->rxlm_handle,
                                              &ftm_1x_rfm_callback,
                                              &cb_payload ) >= 0 );

          /* Override the Intelliciever Power Mode to HL: This is done as a part
          of Rx Calibration time optimization. During the RF Config segment,
          PM will always be set to HL.
          - For one PM calibration, calibration will be done on HL and static
          offset will be applied to LL. Thus since we are overriding PM to HL
          in RF Config OpCode, there would be no need to set PM in Rx Config
          segment. This gives us a savings to 2ms of calibration time per
          Rx Segment. PM override will be skipped in Rx Config segment, if tools
          always sends same PM (in this case HL) nin Rx Config segment. Thus
          for one PM calibration, tools will always send PM as HL.
          - However, if both PM calibration is to be done, then tools will send
            appropriate PM in Rx Config segment. Thus if there is difference
            between last PM and new PM, then Rx Config segment will set the new
            PM. Thus there is a need to keep track of current PM in cal v3. This
            will be done by a variable in FTM CDMA CAL v3 data.
          */    
          rfm_cdma_ftm_power_mode_override( mdev, 
                                            RFM_CDMA_POWER_MODE_NORMAL );
          
          /* Update cal v3 data structure with current power mode */
          cal_v3_data->curr_power_mode = RFM_CDMA_POWER_MODE_NORMAL;

          /* Update FTM CDMA data structure for successfull RFM execution */
          if ( rfm_ret == TRUE )
          {
            mdev_status->rf_state = FTM_CDMA_STATE_RX;
            mdev_status->curr_band = band;
            mdev_status->curr_chan = channel;
            mdev_status->assoc_dev = RFM_INVALID_DEVICE;
          } /* if ( rfm_ret == TRUE ) */
          else
          {
            FTM_MSG_1( FTM_ERROR, "ftm_1x_calibration_radio_setup: FTM data "
                                  "update skipped for mDev %d", mdev );
          } /* if ! ( rfm_ret == TRUE ) */

          /* Perform Tx Setup, if the state requested is RxTx. During this step,
          it needs to be made sure that the Tx power is overridden to the lowest
          possible level in order to avoid false triggering of call box */
          if ( mdev_state == MASTER_DEVICE_RXTX )
          {
            /* Get min and max AVGA table index - this is a one-time thing */
            rfm_ret &= rfm_cdma_ftm_get_min_max_avga_idx( mdev, &rgi_min,
                                                          &rgi_max);

            /* Set lowest Tx AGC - set lowest index for AVGA to get the lowest
            possible Tx power */ 
            /* For min Tx power the bias voltage that should be sent should come 
            from the PA driver, this is the minimum possible bias voltage that can 
            be set by the QPOET */ 
            rfm_ret &= rfm_1x_ftm_set_fw_tx_agc_override( mdev, band,
                                             RFM_PA_GAIN_STATE_0,
                                             rgi_min, 0, /*betaP*/
                                             RFDEVICE_PAPM_MIN_TX_BIAS_VOLTAGE, 
                                             0,/*Quiscent current*/ 
                                             FALSE);

            /* Set frequency adjustment parameters to default for FTM 
            Tx wakeup. Freq error is initialized to zero */
            rfm_ret &= rfm_1x_ftm_set_tx_freq_adjust( 0 );

            /* Clear the DPD map if we are starting calibration */
            ftm_cal_data_teardown_data_module();
            /* Clear the ET Delay Table if we are starting calibration */
            ftm_cal_data_teardown_delay_map();
            /* Clear the pin Table if we are starting calibration */
            ftm_cal_data_teardown_pin_map();

            /* Perform wakeup Tx */
            cb_payload.device = mdev; /* Populate the CB Payload */
            rfm_ret &= ( rfm_1x_wakeup_tx( mdev, mdev_status->txlm_handle, 
                                           &ftm_1x_rfm_callback, 
                                           &cb_payload ) >= 0 );

            if ( rfm_ret == TRUE )
            { 
              void* smem_addr = ftm_cdma_data_1x_get_fw_smem_addr();
              if (smem_addr != NULL) 
              {
            /* Configure and enable modulator for 1x */
                ftm_1x_enable_modulator( mdev, smem_addr );
              }
              else
              {
                rfm_ret = FALSE;
              FTM_MSG( FTM_ERROR, "ftm_1x_calibration_radio_setup: fw shared memory "
                                  "ptr is NULL" );
              }

            /* Enable HDET */
            rfm_ret &= ( rfm_cdma_ftm_enable_hdet( mdev ) == 1 );

            }
            else
            {
              FTM_MSG_2( FTM_ERROR, "ftm_1x_calibration_radio_setup: Both "
                            "enable_modulator and enable_hdet are skipped for "
                            "mDev %d", mdev , band);
            }  

            /* Update FTM CDMA data structure for successfull RFM execution */
            if ( rfm_ret == TRUE )
            {
              mdev_status->rf_state = FTM_CDMA_STATE_RXTX;
            } /* if ( rfm_ret == TRUE ) */
            else
            {
              FTM_MSG_1( FTM_ERROR, "ftm_1x_calibration_radio_setup: FTM data "
                                    "update skipped for mDev %d", mdev );
            } /* if ! ( rfm_ret == TRUE ) */

          } /* if ( mdev_state == MASTER_DEVICE_RXTX ) */

        }/* if ( rfm_ret != FALSE ) */
        else
        {
          FTM_MSG_2( FTM_ERROR, "ftm_1x_calibration_radio_setup: Enter mode/"
                                "enable mode failed for device:%d on band %d "
                                , mdev ,band);
        }/* if ( rfm_ret == FALSE ) */
      } /* else if ( ( mdev_state == MASTER_DEVICE_RX ) ||
                ( mdev_state == MASTER_DEVICE_RXTX ) ) */

      /*----------------------------------------------------------------------*/
      /* MASTER_DEVICE_RETUNE */
      /*----------------------------------------------------------------------*/

      else if ( mdev_state == MASTER_DEVICE_RETUNE )
      {
        /* Get min and max AVGA table index - this is a one-time thing */
        rfm_ret &= rfm_cdma_ftm_get_min_max_avga_idx( mdev, &rgi_min,
                                                      &rgi_max);

        /* Set lowest Tx AGC - set lowest index for AVGA to get the lowest
        possible Tx power */ 
        /* For min Tx power the bias voltage that should be sent should come 
        from the PA driver, this is the minimum possible bias voltage that can 
        be set by the QPOET */ 
        rfm_ret &= rfm_1x_ftm_set_fw_tx_agc_override( mdev,
                                                    mdev_status->curr_band,
                                                    RFM_PA_GAIN_STATE_0,
                                                    rgi_min, 0, /*betaP*/
                                                    RFDEVICE_PAPM_MIN_TX_BIAS_VOLTAGE, 
                                                    0,/*Quiscent current*/ 
                                                    FALSE );

        /* Disable HDET */
        rfm_ret &= ( rfm_cdma_ftm_disable_hdet( mdev ) == 1 );

        cb_payload.device = mdev; /* Populate the CB Payload */

        rfm_ret &= (rfm_1x_retune( mdev, mdev_status->rxlm_handle,
                                   band_chan, &ftm_1x_rfm_callback, 
                                   &cb_payload ) >= 0 );

        /* Override the Intelliciever Power Mode to HL: This is done as a part
        of Rx Calibration time optimization. During the RF Config segment,
        PM will always be set to HL.
        - For one PM calibration, calibration will be done on HL and static
          offset will be applied to LL. Thus since we are overriding PM to HL
          in RF Config OpCode, there would be no need to set PM in Rx Config
          segment. This gives us a savings to 2ms of calibration time per
          Rx Segment. PM override will be skipped in Rx Config segment, if tools
          always sends same PM (in this case HL) nin Rx Config segment. Thus
          for one PM calibration, tools will always send PM as HL.
        - However, if both PM calibration is to be done, then tools will send
          appropriate PM in Rx Config segment. Thus if there is difference
          between last PM and new PM, then Rx Config segment will set the new
          PM. Thus there is a need to keep track of current PM in cal v3. This
          will be done by a variable in FTM CDMA CAL v3 data.
        */    
         rfm_cdma_ftm_power_mode_override( mdev, 
                                           RFM_CDMA_POWER_MODE_NORMAL );

         /* Update cal v3 data structure with current power mode */
         cal_v3_data->curr_power_mode = RFM_CDMA_POWER_MODE_NORMAL;

        /* Enable HDET */
        rfm_ret &= ( rfm_cdma_ftm_enable_hdet( mdev ) == 1 );

        /* Update FTM CDMA data structure for successfull RFM execution */
        if ( rfm_ret == TRUE )
        {
          mdev_status->curr_band = band;
          mdev_status->curr_chan = channel;
        } /* if ( rfm_ret == TRUE ) */
        else
        {
          FTM_MSG_1( FTM_ERROR, "ftm_1x_calibration_radio_setup: FTM data "
                                "update skipped for mDev %d", mdev );
        } /* if ! ( rfm_ret == TRUE ) */
        
        /* Configure Tx Waveform, if needed : CDMA uses uses default
        CDMA waveform */

      } /* else if ( mdev_state == MASTER_DEVICE_RETUNE ) */

    } /* if( mdev_status == NULL ) */

  } /* if ( ( radio_setup->mdevice_state != MASTER_DEVICE_NO_CHANGE ) &&
       ( radio_setup->mdevice_state < MASTER_DEVICE_STATE_MAX) ) */

  /*==========================================================================*/
  /* SLAVE DEVICE */
  /*==========================================================================*/

  /* Perform slave device tuning */
  if ( ( sdev_state != SLAVE_DEVICE_NO_CHANGE ) &&
       ( sdev_state < SLAVE_DEVICE_STATE_MAX ) )   
  {
    /* Query the current RF State Ptr for master device */
    mdev_status = ftm_cdma_data_get_device_ptr( mdev );

      /* Query the current RF State Ptr for slave device */
    sdev_status = ftm_cdma_data_get_device_ptr( sdev );

    /* Get Pointer to Cal v3 data */                    
    cal_v3_data_s = ftm_cdma_data_get_cal_v3_adr( sdev ); 

    if( ( mdev_status == NULL ) || ( sdev_status == NULL ) 
        || ( cal_v3_data_s == NULL ))
    {
      FTM_MSG_2( FTM_ERROR,"ftm_1x_calibration_radio_setup: "
                           "NULL data for Dev [ %d %d ]", mdev, sdev );
    } /* if( sdev_status == NULL ) */
    else
    {

      /*----------------------------------------------------------------------*/
      /* SLAVE_DEVICE_DISABLED */
      /*----------------------------------------------------------------------*/

      if ( sdev_state == SLAVE_DEVICE_DISABLED )
      {
        /* Tear Down RF to enter 1x mode */  
        ftm_1x_tear_down(sdev);

      } /* if ( sdev_state == SLAVE_DEVICE_DISABLED ) */

      /*----------------------------------------------------------------------*/
      /* SLAVE_DEVICE_RXDIV */
      /*----------------------------------------------------------------------*/

      else if ( sdev_state == SLAVE_DEVICE_RXDIV )
      {
        /* Enter FTM to 1x mode for sloave device */
        rfm_ret &= ( ftm_rfmode_enter( sdev, FTM_STATE_1X ) == 1 );

        /* Tear Down RF to enter 1x mode */  
        ftm_1x_tear_down(sdev);

        /* Enter 1x Mode */
        rfm_ret &= ( rfm_enter_mode( sdev, RFM_1X_MODE, NULL, NULL,
                                     sdev_status->rxlm_handle ) == 0 );

        /* Enable diversity */
        cb_payload.device = sdev; /* Populate the CB Payload */
        rfm_ret &= ( rfm_1x_enable_diversity( mdev,
                                              mdev_status->rxlm_handle, 
                                              sdev,
                                              sdev_status->rxlm_handle,
                                              &ftm_1x_rfm_callback,
                                              &cb_payload ) >= 0 );

         rfm_cdma_ftm_power_mode_override( sdev, 
                                           RFM_CDMA_POWER_MODE_NORMAL );

         /* Update cal v3 data structure with current power mode */
         cal_v3_data_s->curr_power_mode = RFM_CDMA_POWER_MODE_NORMAL;

        /* Update FTM CDMA data structure for successfull RFM execution */
        if ( rfm_ret == TRUE )
        {
          sdev_status->rf_state = FTM_CDMA_STATE_RXDIV;
          sdev_status->curr_band = band;
          sdev_status->curr_chan = channel;
          sdev_status->assoc_dev = mdev;
          mdev_status->assoc_dev = sdev;
        } /* if ( rfm_ret == TRUE ) */
        else
        {
          FTM_MSG_1( FTM_ERROR, "ftm_1x_calibration_radio_setup: FTM data "
                                "update skipped for mDev %d", mdev );
        } /* if ! ( rfm_ret == TRUE ) */

      } /* else if ( sdev_state == SLAVE_DEVICE_RXDIV ) */
    } /* if( sdev_status == NULL ) */
  } /* if ( ( sdev_state != SLAVE_DEVICE_NO_CHANGE ) &&
       ( sdev_state < SLAVE_DEVICE_STATE_MAX ) )  */

  /* Update the radio setup status based on RFM status */
  ret_val &= rfm_ret;

  /* Record the execution time */
  rs_time = rf_time_get_elapsed( rs_tick, RF_USEC );

  FTM_MSG_9( FTM_HIGH, "ftm_1x_calibration_radio_setup: [Status %d] "
             "Dev[%d,%d] State[%d,%d] Num[%d] Band [%d %d] rsTime[%d]us", 
             ret_val, mdev, sdev, mdev_state, sdev_state, num_of_chan, band, 
             channel, rs_time );

  /* Return Status */
  return ret_val;

} /* ftm_1x_calibration_radio_setup */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Apply Pin alignment during xPT cal when requested

  @details
  This function applies Pin alignment to the RGIs that must be used during
  RF calibration. Based on the action flag to do Pin alignment, this function
  takes as input the override values that would be used for the current RF
  calibration measurement. These override values are adjusted based on static
  NV Pin compensation values to give new override values.
 
  The new override values are returned to the Tx override functionality,
  which includes RGI and IQ gain adjustment

  @param device
  Current device for which calibration is running
 
  @param ref_chan
  Reference linearizer channel with respect to which Pin alignment must be
  computed
 
  @param input_override_row
  Pointer to row containing input override values
 
  @param output_override_row
  Pointer to row containing Pin aligned override values
 
  @return
  Flag indicating the status of Pin alignment calculation result
*/
static
boolean
ftm_1x_calibration_apply_pin_alignment
(
  rfm_device_enum_type device,
  uint16 ref_chan,
  ftm_cal_data_dpd_map_row_type *input_override_row,
  ftm_cal_data_dpd_map_row_type *output_override_row
)
{
  /* Logical Device */
  const rfc_cdma_logical_device_params_type *logical_dev;
  /* pointer to cdma NV item*/
  const rf_1x_config_type* rf_1x_config_ptr;
  /* Var to hold current state */
  ftm_cdma_data_status_type dev_state;
  rfm_cdma_band_class_type band;
  rfm_cdma_chan_type curr_chan;
  boolean pin_offset_calibrated;

  /* Pin compensation */
  uint16 ref_lin_therm_adc;
  int16 pin_comp_offset;
  int16 pout_comp_offset;
  tx_lin_vs_temp_vs_freq_comp_param_type temp_vs_freq_params;

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_calibration_apply_pin_alignment: "
                        "Null pointer returned for logical_dev" );

    return FALSE;
  }

  /* Get current band chan information */
  (void)ftm_cdma_data_get_device_status( device, &dev_state );
  band = dev_state.curr_band;
  curr_chan = dev_state.curr_chan;
  
  /* Gets a modifiable pointer, but stores it as a pointer to const
  to prevent changing anything. */
  rf_1x_config_ptr = rf_cdma_nv_get_1x_data( logical_dev->tx_nv_path[band], 
                                             band );

  if ( rf_1x_config_ptr == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_calibration_apply_pin_alignment:  "
                        "Null pointer returned for rf_1x_config_ptr" );

    return FALSE;
  }

  pin_offset_calibrated = rfcommon_core_txlin_has_pin_cal_offset( 
    &(rf_1x_config_ptr->cdma_common_ptr->tx_static.lin_temp_comp_data),
    input_override_row->curr_tx_override.pa_state );

  if ( pin_offset_calibrated == TRUE )
  {
    /* Get the reference linearizer Therm ADC value from data module */
    ftm_cal_data_get_therm_adc( ref_chan, &ref_lin_therm_adc );

    pin_comp_offset = ftm_cal_data_get_pin_cal_offset(
      ref_chan,
      &(rf_1x_config_ptr->cdma_common_ptr->tx_static.lin_temp_comp_data),
      input_override_row->curr_tx_override.pa_state,      
      (uint32)curr_chan );
  }
  else
  {
  /* Get the reference linearizer Therm ADC value from data module */
  ftm_cal_data_get_therm_adc( ref_chan, &ref_lin_therm_adc );

  /* Initialize input data for the function to compute offset */
  temp_vs_freq_params.freq_params.tx_freq = 
                   rf_cdma_get_tx_carrier_freq( band,
                                                curr_chan );
  temp_vs_freq_params.freq_params.ref_lin_tx_freq = 
                   rf_cdma_get_tx_carrier_freq( band,
                                                ref_chan );

  temp_vs_freq_params.temp_params.temp_read = ref_lin_therm_adc;
  temp_vs_freq_params.temp_params.ref_lin_temp_adc = ref_lin_therm_adc;

  temp_vs_freq_params.pa_state = input_override_row->curr_tx_override.pa_state;
  temp_vs_freq_params.ref_xpt_pin_offset = 0;

  if( input_override_row->curr_tx_override.xpt_mode == RFCOMMON_MDSP_XPT_MODE_ET )
  {
   temp_vs_freq_params.ref_xpt_pin_offset = 
     (int32)rf_cdma_xpt_get_cfg_param( device, XPT_CFG_CMN_REF_ET_PIN_OFFSET );
  }
  else if( input_override_row->curr_tx_override.xpt_mode  == RFCOMMON_MDSP_XPT_MODE_EPT )
  {
   temp_vs_freq_params.ref_xpt_pin_offset = 
     (int32)rf_cdma_xpt_get_cfg_param( device, XPT_CFG_CMN_REF_EPT_PIN_OFFSET );
  }

  temp_vs_freq_params.auto_pin_enable = rfcommon_autopin_is_enabled(RFM_1X_MODE);

  /* Calculate Pin alignment */
  rfcommon_core_txlin_get_temp_compensation( 
         RFCOM_TXLIN_TEMP_COMP_ALGO_MULTILIN_V3,
         &pin_comp_offset,
         &pout_comp_offset,
         &temp_vs_freq_params,
         &rf_1x_config_ptr->cdma_common_ptr->tx_static.lin_temp_comp_data );
  }

  /* Apply Pin alignment to the override row */
  ftm_cal_data_apply_pin_alignment( ref_chan, 
                                    input_override_row->curr_tx_override.pa_state,
                                    input_override_row,
                                    output_override_row,
                                    pin_comp_offset );

  FTM_MSG_4( FTM_HIGH, "ftm_1x_calibration_apply_pin_alignment: "
                       "Ref Chan %d, Ref ADC %d, Curr Chan %d, PA State %d",
             ref_chan, ref_lin_therm_adc, curr_chan, 
             input_override_row->curr_tx_override.pa_state );
  FTM_MSG_4( FTM_HIGH, "ftm_1x_calibration_apply_pin_alignment: "
                       "Pin cal %d, Pin Comp %d dB10, Input RGI %d, Input IQ Gain %d",
             pin_offset_calibrated, pin_comp_offset, input_override_row->curr_tx_override.rgi,
             input_override_row->curr_tx_override.iq_gain );
  FTM_MSG_2( FTM_HIGH, "ftm_1x_calibration_apply_pin_alignment: "
                       "Output RGI %d, Output IQ Gain %d",
             output_override_row->curr_tx_override.rgi,
             output_override_row->curr_tx_override.iq_gain );

  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform Tx Override
 
  @details
  This function will be called from cal v3 common framework to perform
  Tx Override operation. Tx Override involves operation such as overriding
  several RF and MSM Tx parameters such as PA State, RGI, DPD, PA Bias,
  quiscent current and more.
 
  @image html ftm_cdma_tx_override.jpg 
  
  @param tx_override
  Pointer to the data structure containing Tx Override unpacked parameter
 
  @return
  Flag indicating the status of Tx Override operation
*/
boolean
ftm_1x_calibration_v3_tx_override
(
  ftm_cal_tx_override_parameter_type const *tx_override
)
{
  uint8 dev_mask = 0xFF; /*  */
  uint8 dev_temp = 0xFF; /* Variable to store the Device. This will be modified 
  in the while loop. */
  rfm_device_enum_type device; /* RFM Device */
  ftm_xpt_txagc_override_type ovr_params; /* Tx Override Params */
  rfcommon_core_xpt_tx_override_type curr_ovr_params;
  boolean ret_val = TRUE;
  rf_time_tick_type to_tick;  /* Tx Override Time tick */
  rf_time_type to_time; /* Tx Override time */
  ftm_cal_tx_override_type_mask_type override_type = 0;
  ftm_cal_tx_ovr_power_ovr_type power_ovr_type = 0;
  ftm_cal_tx_ovr_power_param_type param_mask = 0;
  ftm_cal_data_dpd_map_row_type override_row;
  ftm_cal_data_dpd_map_row_type override_row_before_pin;
  uint8 override_row_number = FTM_COMMON_XPT_INV_OVR_ROW;
  rfcom_band_type_u rfm_band;
  ftm_cdma_data_status_type *dev_ptr;

  /* Start Profiling of Rx Warm Up */
  to_tick = rf_time_get_tick();

  /* Parameter validation */
  if ( tx_override == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_calibration_v3_tx_override: NULL argument" );
    return FALSE;
  } /* if ( tx_override == NULL ) */

  /* Init value to 0 */
  memset( &ovr_params, 0x0, sizeof(ftm_xpt_txagc_override_type) );
  memset( &curr_ovr_params, 0, sizeof(rfcommon_core_xpt_tx_override_type) );
  memset( &override_row, 0, sizeof(ftm_cal_data_dpd_map_row_type) );

  /* Temporarily store the Device mask */
  dev_temp = tx_override->device_mask;

  /* For the current unprocessed Opcode, extract the bit mask for the first
  enabled cal action. The first bit will be from LSB. */
  dev_mask = rf_extract_first_one_bit_mask( dev_temp );
  
  /* Loop until, the OpCode Mask comes out to be zero */
  while ( dev_mask != 0 )
  {

    /* Find the bit location and this location will correspond to 
    RFM device */
    device = (rfm_device_enum_type)(
                        rf_extract_first_one_bit_lsb_location(dev_mask) );

    /* Check validity of TX Max device */
    if ( device < FTM_CAL_V3_TX_MAX_DEV )
    {
      dev_ptr = ftm_cdma_data_get_device_ptr( device );

      /* check for valid device ptr */
      if (dev_ptr == NULL)
      {
        FTM_MSG_1( FTM_ERROR, "ftm_1x_calibration_v3_tx_override: "
                              "Failed to get a valid "
                              "device pointer for device %d", 
                   device );
        return FALSE;
      }

      /* Populate Device */
      ovr_params.device = device;
      ovr_params.xpt_mode = tx_override->tx_param[device].xpt_mode;
      curr_ovr_params.xpt_mode = tx_override->tx_param[device].xpt_mode;
      override_type = tx_override->tx_param[device].override_type;

      if ( ( override_type & ( 1 << TX_OVR_POWER ) ) != 0 )
      {
        power_ovr_type = tx_override->tx_param[device].power_ovr.power_ovr_type;
        
        if ( power_ovr_type == TX_OVR_POWER_IND_PARAM )
        {
          param_mask = 
            tx_override->tx_param[device].power_ovr.individual_param.param_mask;

          /* Populate IQ Gain if the entry is valid */
          if ( ( param_mask & ( 1 << TX_OVR_IQ_GAIN ) ) != 0 )
          {
            ovr_params.iq_gain_action_type = 
               tx_override->tx_param[device].power_ovr.individual_param.iq_gain.ovr_action;
            ovr_params.iq_gain = 
              tx_override->tx_param[device].power_ovr.individual_param.iq_gain.iq_gain;
            curr_ovr_params.iq_gain_action_type = 
               tx_override->tx_param[device].power_ovr.individual_param.iq_gain.ovr_action;
            curr_ovr_params.iq_gain = 
              tx_override->tx_param[device].power_ovr.individual_param.iq_gain.iq_gain;
          } /* if ( ( param_mask & ( 1 << TX_OVR_IQ_GAIN ) ) != 0 ) */
          else
          {
            ovr_params.iq_gain_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
            ovr_params.iq_gain = 0;
            curr_ovr_params.iq_gain_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
            curr_ovr_params.iq_gain = 0;
          } /* if ! ( ( param_mask & ( 1 << TX_OVR_IQ_GAIN ) ) != 0 ) */

          /* Populate Envelope Scale if the entry is valid */
          if ( ( param_mask & ( 1 << TX_OVR_ENV_SCALE ) ) != 0 )
          {
            ovr_params.env_scale_action_type = 
              tx_override->tx_param[device].power_ovr.individual_param.env_scale.ovr_action;
            ovr_params.env_scale = 
              (uint16)tx_override->tx_param[device].power_ovr.individual_param.env_scale.env_scale;
            curr_ovr_params.env_scale_action_type = 
              tx_override->tx_param[device].power_ovr.individual_param.env_scale.ovr_action;
            curr_ovr_params.env_scale = 
              (uint16)tx_override->tx_param[device].power_ovr.individual_param.env_scale.env_scale;
          } /* if ( ( param_mask & ( 1 << TX_OVR_ENV_SCALE ) ) != 0 ) */
          else
          {
            ovr_params.env_scale_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
            ovr_params.env_scale = 0;
            curr_ovr_params.env_scale_action_type = FTM_COMMON_XPT_TX_OVERRIDE_SKIP;
            curr_ovr_params.env_scale = 0;
          } /* if ! ( ( param_mask & ( 1 << TX_OVR_ENV_SCALE ) ) != 0 ) */

          /* Populate PA Bias if the entry is valid */
          if ( ( param_mask & ( 1 << TX_OVR_RGI ) ) != 0 )
          {
            ovr_params.rgi = 
              tx_override->tx_param[device].power_ovr.individual_param.rgi.gain_index;
            curr_ovr_params.rgi = 
              tx_override->tx_param[device].power_ovr.individual_param.rgi.gain_index;
          } /* if ( ( param_mask & ( 1 << TX_OVR_RGI ) ) != 0 ) */
          else
          {
            ovr_params.rgi = -1;
            curr_ovr_params.rgi = FTM_COMMON_XPT_TX_OVR_RGI_INVALID;
          } /* if ! ( ( param_mask & ( 1 << TX_OVR_RGI ) ) != 0 ) */

          /* Populate XPT if the entry is valid */
          if ( ( param_mask & ( 1 << TX_OVR_PA_BIAS ) ) != 0 )
          {
            ovr_params.smps_bias = 
              tx_override->tx_param[device].power_ovr.individual_param.bias.pa_bias;
            curr_ovr_params.papm_bias = 
              tx_override->tx_param[device].power_ovr.individual_param.bias.pa_bias;
          } /* if ( ( param_mask & ( 1 << TX_OVR_PA_BIAS ) ) != 0 ) */
          else
          {
            ovr_params.smps_bias = -1;
            curr_ovr_params.papm_bias = FTM_COMMON_XPT_TX_OVR_SMPS_BIAS_INVALID;
          } /* if ! ( ( param_mask & ( 1 << TX_OVR_PA_BIAS ) ) != 0 ) */

          /* Populate PA State if the entry is valid */
          if ( ( param_mask & ( 1 << TX_OVR_PA_STATE ) ) != 0 )
          {
            ovr_params.pa_state = 
              tx_override->tx_param[device].power_ovr.individual_param.pa_state.gain_state;
            curr_ovr_params.pa_state = 
              tx_override->tx_param[device].power_ovr.individual_param.pa_state.gain_state;
          } /* if ( ( param_mask & ( 1 << TX_OVR_PA_STATE ) ) != 0 ) */
          else
          {
            ovr_params.pa_state = -1;
            ovr_params.pa_state = FTM_COMMON_XPT_TX_OVR_PA_STATE_INVALID;
          } /* if ! ( ( param_mask & ( 1 << TX_OVR_PA_STATE ) ) != 0 ) */

          /* Populate PA Current if the entry is valid */
          if ( ( param_mask & ( 1 << TX_OVR_QUISCENT_CURRENT ) ) != 0 )
          {
            ovr_params.pa_curr = 
               tx_override->tx_param[device].power_ovr.individual_param.current.quiscent_current;
            curr_ovr_params.pa_curr = 
               tx_override->tx_param[device].power_ovr.individual_param.current.quiscent_current;
          } /* if ( ( param_mask & ( 1 << TX_OVR_QUISCENT_CURRENT ) ) != 0 ) */
          else
          {
            ovr_params.pa_curr = -1;
            curr_ovr_params.pa_curr = FTM_COMMON_XPT_TX_OVR_PA_CURR_INVALID;
          } /* if !( ( param_mask & ( 1 << TX_OVR_QUISCENT_CURRENT ) ) != 0 ) */

          if ( ( param_mask & ( 1 << TX_OVR_DELAY ) ) != 0 )
          {
            ovr_params.delay_action_type = tx_override->tx_param[device].power_ovr.individual_param.delay.ovr_action;
            ovr_params.delay = tx_override->tx_param[device].power_ovr.individual_param.delay.delay_val;
            curr_ovr_params.delay_action_type = 
              tx_override->tx_param[device].power_ovr.
                                       individual_param.delay.ovr_action;
            curr_ovr_params.et_delay = 
              tx_override->tx_param[device].power_ovr.
                                       individual_param.delay.delay_val;
          }
          else
          {
            ovr_params.delay_action_type = FTM_COMMON_XPT_TX_OVERRIDE_NV;
            ovr_params.delay = 0;
            curr_ovr_params.delay_action_type = FTM_COMMON_XPT_TX_OVERRIDE_NV;
            curr_ovr_params.et_delay = 0;
          }

          /* Else override by current settings */
          curr_ovr_params.amam_addr.mem_bank_id = -1;
          curr_ovr_params.amam_addr.start_offset = -1;
          curr_ovr_params.amam_addr.tbl_size = 0;
          curr_ovr_params.ampm_addr.mem_bank_id = -1;
          curr_ovr_params.ampm_addr.start_offset = -1;
          curr_ovr_params.ampm_addr.tbl_size = 0;
          curr_ovr_params.detrough_addr.mem_bank_id = -1;
          curr_ovr_params.detrough_addr.start_offset = -1;
          curr_ovr_params.detrough_addr.tbl_size = 0;

          memscpy( (void*)&override_row.curr_tx_override,
                   sizeof(rfcommon_core_xpt_tx_override_type),
                   (void*)&curr_ovr_params,
                   sizeof(rfcommon_core_xpt_tx_override_type) );

          override_row_number = FTM_COMMON_XPT_INV_OVR_ROW;
        }
        else if ( power_ovr_type == TX_OVR_POWER_ABS )
        {
          ftm_cdma_data_status_type *mdev_status = NULL;

          /* Query the current RF State Ptr for master device */
          mdev_status = ftm_cdma_data_get_device_ptr( device );

          if ( mdev_status != NULL )
          {
            ret_val &= ftm_cal_data_get_row_data_by_power(
               tx_override->tx_param[device].power_ovr.abs_power.ref_channel, 
               &override_row_before_pin,
               tx_override->tx_param[device].power_ovr.abs_power.power_dbm10,
               curr_ovr_params.xpt_mode);

            /* Verify we found a row of data */
            if( ret_val == FALSE )
            {
              FTM_MSG_1( 
               FTM_ERROR, "ftm_1x_calibration_v3_tx_override: "
                                    "Could not find power %d in data module",
               tx_override->tx_param[device].power_ovr.abs_power.power_dbm10 );
              return FALSE;
            }

            /* Copy over the data module values to the final override row */
            memscpy( (void*)&override_row,sizeof(ftm_cal_data_dpd_map_row_type),
                    (void*)&override_row_before_pin, 
                    sizeof(ftm_cal_data_dpd_map_row_type) );
            FTM_MSG_2( FTM_HIGH,"Ovr by power row delay action %d, delay val %d", 
                       override_row.curr_tx_override.delay_action_type,
                       override_row.curr_tx_override.et_delay );

            /* If the Pin alignment flag is set, then apply Pin 
            alignment to the override values before proceeding. If Pin is 
            not required, the values obtained from the data module will 
            be used */
            if ( tx_override->flag == 
                                  FTM_CALIBRATION_TX_OVR_APPLY_PIN_ALIGNMENT )
            {
              ftm_1x_calibration_apply_pin_alignment( 
                     device,
                     tx_override->
                            tx_param[device].power_ovr.abs_power.ref_channel,
                     &override_row_before_pin,
                     &override_row );

              if ( tx_override->debug_mode_en == TRUE )
              {
                FTM_MSG_1( FTM_MED, "ftm_1x_calibration_v3_tx_override: "
                                    "Applied Pin alignment to Tx override for "
                                    "Power %d",
                           tx_override->
                            tx_param[device].power_ovr.abs_power.power_dbm10 );
              }
            }/* if( tx_override->flag == 
                                  FTM_CALIBRATION_TX_OVR_APPLY_PIN_ALIGNMENT */

            override_row_number = override_row.row_idx;
          }
          else
          {
            FTM_MSG_1( FTM_ERROR, "ftm_1x_calibration_v3_tx_override: "
                       "Abs Power Ovr - Null Device %d data", device );
            ret_val = FALSE;
          }
        }
        else
        {
          FTM_MSG_1( FTM_ERROR, "ftm_1x_calibration_v3_tx_override: "
                     "Invalid Power Override Type %d", power_ovr_type );
        }
      } /* if ( ( override_type & ( 1 << TX_OVR_POWER ) ) != 0 ) */

      if ( ( override_type & ( 1 << TX_OVR_APPLY_DPD ) ) != 0 )
      {
        FTM_MSG( FTM_ERROR, "Apply DPD to be implemented" );
      } /* if ( ( override_type & ( 1 << TX_OVR_APPLY_DPD ) ) != 0 ) */


      /* Set the current override */
      ftm_cal_data_set_current_override_row( override_row_number );

      /* Override TxAGC */
      if ( ret_val == TRUE )
      {
      if ( rfcommon_debug_flags.use_rflm_txagc_override )
      {
        /* Process override params */
        ret_val &= rfm_1x_ftm_process_tx_override_params( device,
                     override_row.curr_tx_override.xpt_mode,
                     override_row.curr_tx_override.amam_addr.mem_bank_id,
                     override_row.curr_tx_override.amam_addr.start_offset,
                     override_row.curr_tx_override.amam_addr.tbl_size,
                     override_row.curr_tx_override.ampm_addr.mem_bank_id,
                     override_row.curr_tx_override.ampm_addr.start_offset,
                     override_row.curr_tx_override.ampm_addr.tbl_size,
                     override_row.curr_tx_override.detrough_addr.mem_bank_id,
                     override_row.curr_tx_override.detrough_addr.start_offset,
                     override_row.curr_tx_override.detrough_addr.tbl_size,
                     override_row.curr_tx_override.iq_gain_action_type,
                     override_row.curr_tx_override.iq_gain,
                     override_row.curr_tx_override.env_scale_action_type,
                     override_row.curr_tx_override.env_scale,
                     override_row.curr_tx_override.rgi,
                     override_row.curr_tx_override.papm_bias,
                     override_row.curr_tx_override.pa_state,
                     override_row.curr_tx_override.pa_curr,
                     override_row.curr_tx_override.delay_action_type,
                     override_row.curr_tx_override.et_delay );

          /* Apply the Tx override through RFLM if the process_override_params() passed */
        rfm_band.cdma_band = dev_ptr->curr_band;
          if ( ret_val == TRUE )
          {
        ret_val &= rfm_ftm_apply_xpt_tx_agc_override( RFM_1X_MODE, device,
                                                      rfm_band );
      }
        }
      else
      {
      ret_val &= rfm_1x_ftm_set_rf_tx_agc_override( device,
                        override_row.curr_tx_override.xpt_mode,
                        override_row.curr_tx_override.amam_addr.mem_bank_id,
                        override_row.curr_tx_override.amam_addr.start_offset,
                        override_row.curr_tx_override.amam_addr.tbl_size,
                        override_row.curr_tx_override.ampm_addr.mem_bank_id,
                        override_row.curr_tx_override.ampm_addr.start_offset,
                        override_row.curr_tx_override.ampm_addr.tbl_size,
                        override_row.curr_tx_override.detrough_addr.mem_bank_id,
                        override_row.curr_tx_override.detrough_addr.start_offset,
                        override_row.curr_tx_override.detrough_addr.tbl_size,
                        override_row.curr_tx_override.iq_gain_action_type,
                        override_row.curr_tx_override.iq_gain,
                        override_row.curr_tx_override.env_scale_action_type,
                        override_row.curr_tx_override.env_scale,
                        override_row.curr_tx_override.rgi,
                        override_row.curr_tx_override.papm_bias,
                        override_row.curr_tx_override.pa_state,
                        override_row.curr_tx_override.pa_curr,
                        override_row.curr_tx_override.delay_action_type,
                        override_row.curr_tx_override.et_delay );
      }
      } /* if ( ret_val == TRUE )*/

      /* Update OpCode to set the processed bit to zero. */
      dev_temp = dev_temp & (~dev_mask);

      /* For the current unprocessed Opcode, extract the bit mask for the first
      enabled cal action. The first bit will be from LSB. */
      dev_mask = rf_extract_first_one_bit_mask( dev_temp );

      /* Print Extra Debug message */
      if ( tx_override->debug_mode_en == TRUE )
      {
        FTM_MSG_5( FTM_HIGH, "ftm_1x_calibration_v3_tx_override: Dev %d "
                   "Tx Param 0x%x, PA %d, RGI %d, Bias %d", 
                   device, 
                   param_mask,
                   curr_ovr_params.pa_state,
                   curr_ovr_params.rgi,
                   curr_ovr_params.papm_bias );
      } /* if ( tx_override->debug_mode_en == TRUE ) */
    } /* if ( device < FTM_CAL_V3_TX_MAX_DEV ) */
    else
    {
      ret_val = FALSE;
      FTM_MSG_1( FTM_ERROR, "ftm_1x_calibration_v3_tx_override: Invalid "
                            "Device %d", device );

    } /* if ! ( device < FTM_CAL_V3_TX_MAX_DEV ) */

  } /* while ( dev_mask != 0 ) */

  /* Record the execution time */
  to_time = rf_time_get_elapsed( to_tick, RF_USEC );
  
  FTM_MSG_3( FTM_HIGH, "ftm_1x_calibration_v3_tx_override: [Status %d] "
             "DevMask %d toTime[%d]us", 
             ret_val, tx_override->device_mask, to_time );

  return ret_val;

} /* ftm_1x_calibration_v3_tx_override */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform Rx Measurement
 
  @details
  This function will be called from cal v3 common framework to perform
  Rx measurement operation. Rx measurement involves operation of sampling RxAGC
  to compute the offset between the expected and actual Rx AGC
   
  @param rx_measure
  Pointer to the data structure containing Rx meas unpacked parameter
 
  @param cal_result
  Pointer to the data structure to which the results is supposed
  to be populated
 
  @return
  Flag indicating the status of Rx Measurement operation
*/
boolean
ftm_1x_calibration_v3_rx_measure
(
  ftm_cal_rx_meas_parameter_type const *rx_measure,
  ftm_cal_rx_meas_result_data_type *cal_result
)
{
  uint8 dev_mask = 0xFF; /*  */
  uint8 dev_temp = 0xFF; /* Variable to store the Device. This will be modified 
  in the while loop. */
  rfm_device_enum_type device; /* RFM Device */
  boolean ret_val = TRUE;
  rf_time_tick_type rm_tick;  /* Rx Meas Time tick */
  rf_time_type rm_time; /* Rx Meas time */
  uint8 readx = 0;
  rfm_cdma_ftm_rx_agc_data_type rx_agc_data;
  int32 agc_avg_val;
  int16 agc_read_val = 0; /* AGC Read Value */
  int32 agc_expected_val_1_640th = 0; /* Expected RxAGC in 1/640dB resolution */
  rfm_lna_gain_state_type gain_state; /* gain state */
  int32 lna_gain_prev_state; /* LNA Offset Value From previous Step.
                                    This value will be used passed step_index */
  rfm_cdma_power_mode_type power_mode; /* Analog transceievr power mode */
  int32 lna_offset = 0; /* LNA Offset Value */
  int16 lna_offset_1_10th = 0; /* LNA Offset Value in 1/10th dB resolution*/
  ftm_cdma_cal_v3_data_type *cal_data = NULL; /* Cal data */

  /* Start Profiling of Rx Warm Up */
  rm_tick = rf_time_get_tick();
  
  /* Parameter validation */
  if ( ( rx_measure == NULL ) || ( cal_result == NULL ) )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_calibration_v3_rx_measure: NULL argument" );
    return FALSE;
  } /* if ( ( rx_measure == NULL ) || ( cal_result == NULL ) ) */
  
  /*------------------------------------------------------------------------*/
  /* Sample RxAGC from all devices and store the total accumulated          */
  /* RxAGC to a FTM CDMA Data to be processed in future.                    */
  /*------------------------------------------------------------------------*/
  for ( readx = 0; readx < FTM_1X_RXAGC_RD_SAMPLES; readx++ )
  {
  
    /* Temporarily store the Device mask */
    dev_temp = rx_measure->device_mask;
  
    /* For the current unprocessed Opcode, extract the bit mask for the first
    enabled cal action. The first bit will be from LSB. */
    dev_mask = rf_extract_first_one_bit_mask( dev_temp );
  
    /* Loop until, the OpCode Mask comes out to be zero */
    while ( dev_mask != 0 )
    {
  
      /* Find the bit location and this location will correspond to 
      RFM device */
      device = (rfm_device_enum_type)(
                          rf_extract_first_one_bit_lsb_location(dev_mask) );

      /* Check if the device is valid */
      if ( device < FTM_CAL_V3_RX_MAX_DEV )
      {
        /* Get Calibration Data */
        cal_data = ftm_cdma_data_get_cal_v3_adr(device);

        /* Perform NULL pointer check */
        if ( cal_data == NULL )
        {
          FTM_MSG_1( FTM_ERROR, "ftm_1x_calibration_v3_rx_measure: NULL Cal"
                                "data for dev %d", device );
          ret_val = FALSE;
          break;
        } /* if ( cal_data == NULL ) */
        
        /* Perform LNA Offset measurment */
        if ( ( rx_measure->rx_meas_param[device].rx_meas_action & 
             ( 1 << RX_MEAS_LNA_OFFSET ) ) != 0 )
        {
    
          /* Reset the AGC Accum only if it is first sample. */
          if ( readx == 0 )
          {
            cal_data->agc_acc_val = 0;
          } /* if ( reset_agc_accum == TRUE ) */
        
          /* Get RxAGC reading for the current device in raw format */
          rfm_1x_ftm_get_rx_agc_data( device, &rx_agc_data,
                                      RFM_CDMA_AGC_FORMAT__RAW );
        
          /* Store the current RxAGC in local variabel */
          agc_read_val = rx_agc_data.rx_agc[RFM_CDMA_CARRIER_0];
        
          /* Perform Sign Bit Extension for Negative AGC value */
          /* 512 - 1023 correspond to Negative AGC Read. Thus need to sign
          extend. */
         // if ( agc_read_val > 32767 )
         // {
         //   agc_read_val = agc_read_val - 65536;
         // }
        
          /* Debug Message */
          if ( cal_data->debug_en == TRUE )
          {
            FTM_MSG_3( FTM_MED, "Cal v3 ftm_1x_calibration_sweep_measure_rx - "
                       "RxAGC Sampling Dev %d, AGC %d (for Itr %d)", device, 
                       agc_read_val, readx );
          } /* if ( cal_data->debug_en == TRUE ) */
        
          /* Accumulate all AGC Samples */
          cal_data->agc_acc_val += agc_read_val;
        
                /* Compute the average lna offset if the last sampling is done */
          if ( readx == (FTM_1X_RXAGC_RD_SAMPLES - 1) )
          {
            /* Convert to 1/640th dB resolution */
            agc_expected_val_1_640th = 
              (rx_measure->rx_meas_param[device].lna_offset.expected_exagc)*64;
          
            /* LNA Gain State */
            gain_state = 
              (rfm_lna_gain_state_type)rx_measure->rx_meas_param[device].lna_offset.lna_state;
          
            /* Power Mode */
            power_mode = 
              (rfm_cdma_power_mode_type)rx_measure->rx_meas_param[device].lna_offset.power_mode;

            /* Make sure that teh power mode and gain state are valid */
            if ( ( gain_state < RFM_LNA_GAIN_STATE_NUM ) && 
                 ( power_mode < RFM_CDMA_POWER_MODE_MAX ) )
            {

              /* Perform Sign Bit Extension for Negative AGC value */
              /* 32768 - 65536 correspond to Negative AGC Read. Thus need to 
              sign extend. */
              if ( agc_expected_val_1_640th > 32767 )
              {
                agc_expected_val_1_640th = agc_expected_val_1_640th - 65536;
              }
            
              /* Populate lna_offset_prev value reading the global variable  */
              if ( gain_state == RFM_LNA_GAIN_STATE_0 )
              {
                /* Force previous step LNA offset to be used for step index 0 to
                be zero */
                lna_gain_prev_state = 0;
              } /* if ( gain_state == RFM_LNA_GAIN_STATE_0 ) */
              else
              {
                /* Otherwise use the offset from previous step. */
                lna_gain_prev_state = 
                          cal_data->lna_offset[power_mode][gain_state-1];
              } /* if ! ( gain_state == RFM_LNA_GAIN_STATE_0 ) */
            
              /* Average Out the Accumulated AGC Read value */
              if( cal_data->agc_acc_val > 0 ) /* For proper rounding */
              {
                agc_avg_val = 
                  ((cal_data->agc_acc_val + FTM_1X_RXAGC_RD_SAMPLES/2) / 
                           FTM_1X_RXAGC_RD_SAMPLES);
              } /* if( agc_acc_val > 0 ) */
              else
              {
                agc_avg_val =
                  ((cal_data->agc_acc_val - FTM_1X_RXAGC_RD_SAMPLES/2) /
                           FTM_1X_RXAGC_RD_SAMPLES );
              } /* if ! ( agc_acc_val > 0 ) */
            
              /* LNA offset is the difference between the actual AGC value and
              expected AGC value with addition of LNA offset for previous LNA 
              gain state. LNA offset for previous gain state is applied on 
              RFLM side before making the measurement, so all computaitons are 
              relative to the previous LNA gain state */ 
              lna_offset = agc_avg_val - agc_expected_val_1_640th + 
                                                      lna_gain_prev_state;
            
              /* Store LNA offset in rf_lna_offset_val[] for future GET LNA
              OFFSET commands */
              cal_data->lna_offset[power_mode][gain_state] = lna_offset;
            
              /* Convert to 1/10th dB resolution */
              lna_offset_1_10th = lna_offset / 64;
            
              /* Populate device specific results */
              cal_result->rx_meas_param[device].rx_meas_action = 
                             rx_measure->rx_meas_param[device].rx_meas_action;
              cal_result->rx_meas_param[device].lna_offset.lna_state = 
                             gain_state;
              cal_result->rx_meas_param[device].lna_offset.power_mode = 
                             power_mode;
              cal_result->rx_meas_param[device].lna_offset.lna_offset = 
                             lna_offset_1_10th;
            
              /* Debug Message */
              FTM_MSG_7( FTM_MED,"Cal v3 ftm_1x_calibration_sweep_measure_rx - "
                         "Dev %d LNA %d (act - exp + prev) = (%d-%d+%d)=%d "
                         "(in 1/10th : %d)", device, gain_state, agc_avg_val, 
                         agc_expected_val_1_640th, lna_gain_prev_state, 
                         lna_offset, lna_offset_1_10th );
            } /* if ( ( gain_state < RFM_LNA_GAIN_STATE_NUM ) && 
                 ( power_mode < RFM_CDMA_POWER_MODE_MAX ) ) */
            else
            {
              ret_val = FALSE;
              FTM_MSG_3( FTM_ERROR, "ftm_1x_calibration_v3_rx_measure: "
                         "Bad GS or PM (Dev %d  Gain %d PM %d)", device, 
                         gain_state, power_mode );
              break;

            } /* if ! ( ( gain_state < RFM_LNA_GAIN_STATE_NUM ) && 
                 ( power_mode < RFM_CDMA_POWER_MODE_MAX ) ) */
          } /* if ( readx == (FTM_1X_RXAGC_RD_SAMPLES - 1) ) */
    
        } /* if ( ( rx_measure->rx_meas_param[device].rx_meas_action & 
             ( 1 << RX_MEAS_LNA_OFFSET ) ) != 0 ) */

        /* Update OpCode to set the processed bit to zero. */
        dev_temp = dev_temp & (~dev_mask);
        
        /* For the current unprocessed Opcode, extract the bit mask for the 
        first enabled cal action. The first bit will be from LSB. */
        dev_mask = (uint8)rf_extract_first_one_bit_mask( dev_temp );
        
        /* Print Extra Debug message */
        if ( rx_measure->debug_mode_en == TRUE )
        {
          FTM_MSG_2( FTM_HIGH, "ftm_1x_calibration_v3_rx_measure: Dev %d "
                     "Rx Meas mask 0x%x", device, 
                     rx_measure->rx_meas_param[device].rx_meas_action );        
        } /* if ( tx_override->debug_mode_en == TRUE ) */
      } /* if ( device < RFM_MAX_DEVICES ) */
      else
      {
        ret_val = FALSE;
        FTM_MSG_1( FTM_ERROR, "ftm_1x_calibration_v3_rx_measure: Dev %d "
                   "not valid", device );
        break;
      } /* if ! ( device < RFM_MAX_DEVICES ) */
    } /* while ( dev_mask != 0 ) */

    /* Wait for FTM_1X_CAL_V3_RXAGC_WAIT_US mS before taking another read.
    There is no need to wait, if this is the last loop as there will be no
    more RxAGC sampling */
    if ( readx != ( FTM_1X_RXAGC_RD_SAMPLES - 1 ) )
    {
      DALSYS_BusyWait( FTM_1X_CAL_V3_RXAGC_WAIT_US );
    }

  } /* for ( readx = 0; readx < FTM_1X_RXAGC_RD_SAMPLES; readx++ ) */

  /* Populate common results */
  cal_result->device_mask = rx_measure->device_mask;

  /* Record the execution time */
  rm_time = rf_time_get_elapsed( rm_tick, RF_USEC );
  
  FTM_MSG_3( FTM_HIGH, "ftm_1x_calibration_v3_rx_measure: [Status %d] "
             "DevMask %d rmTime[%d]us", 
             ret_val, rx_measure->device_mask, rm_time );
  
  return ret_val;

} /* ftm_1x_calibration_v3_rx_measure */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform Tx Measurement
 
  @details
  This function will be called from cal v3 common framework to perform
  Tx measurement operation. Tx measurement involves operation of sampling HDET
  reads
   
  @param tx_measure
  Pointer to the data structure containing Tx meas unpacked parameter
 
  @param cal_result
  Pointer to the data structure to which the results is supposed
  to be populated
 
  @return
  Flag indicating the status of Tx Measurement operation
*/
boolean
ftm_1x_calibration_v3_tx_measure
(
  ftm_cal_tx_meas_parameter_type const *tx_measure,
  ftm_cal_tx_meas_result_data_type *cal_result
)
{
  uint8 dev_mask = 0xFF; /*  */
  uint8 dev_temp = 0xFF; /* Variable to store the Device. This will be modified 
  in the while loop. */
  rfm_device_enum_type device; /* RFM Device */
  boolean ret_val = TRUE;
  rf_time_tick_type tm_tick;  /* Rx Meas Time tick */
  rf_time_type tm_time; /* Rx Meas time */
  ftm_cal_tx_meas_hdet_sample_type hdet_sample = 0;
  uint8 fb_power_samples = 0;
  uint16 hdet_read = 0;
  uint32  hdet_accm = 0; /* HDET read accumuulator */
  uint16  temp_hdet_val  = 0; /* temp HDET read*/
  uint16 therm_read = 0;
  uint8   fb_power_samples_idx = 0; /* read index */
  uint8   local_read_count; /* read counter */
  boolean therm_read_valid = FALSE;
  ftm_cdma_data_status_type dev_state;
  ftm_common_fbrx_capture_input_type fb_meas_inp;
  ftm_common_fbrx_capture_result_type fb_meas_res;
  uint8 log_ind = 0;
  rfdevice_cdma_hdet_read_type hdet_read_result = {0};

  /* Start Profiling of Rx Warm Up */
  tm_tick = rf_time_get_tick();
  
  /* Parameter validation */
  if ( ( tx_measure == NULL ) || ( cal_result == NULL ) )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_calibration_v3_tx_measure: NULL argument" );
    return FALSE;
  } /* if ( ( rx_measure == NULL ) || ( cal_result == NULL ) ) */
    
  /* Temporarily store the Device mask */
  dev_temp = tx_measure->device_mask;

  /* For the current unprocessed Opcode, extract the bit mask for the first
  enabled cal action. The first bit will be from LSB. */
  dev_mask = (uint8)rf_extract_first_one_bit_mask( dev_temp );

  /* Loop until, the OpCode Mask comes out to be zero */
  while ( dev_mask != 0 )
  {

    /* Find the bit location and this location will correspond to 
    RFM device */
    device = (rfm_device_enum_type)(
                        rf_extract_first_one_bit_lsb_location(dev_mask) );

    /* Check if the device is valid */
    if ( device < FTM_CAL_V3_TX_MAX_DEV )
    {
      /*----------------------------------------------------------------------*/
      /*  TX_MEAS_HDET */
      /*----------------------------------------------------------------------*/

      /* Perform HDET measurment */
      if ( ( tx_measure->tx_meas_param[device].tx_meas_action & 
           ( 1 << TX_MEAS_HDET ) ) != 0 )
      {
        hdet_sample = tx_measure->tx_meas_param[device].hdet.hdet_sample;
        
        /* Allow for FW latency to latch the required PDM value to the RTR.
        In cal override mode, FW operates with a 64-chip update interval,
        and so we provide an additional 50usec delay here */
        
        /* Take HDET reads, only if number if HDET samples are more than 0 */
        /* HDET needs time to settle. Time to settle varies based on HDET
        used. This time will need to be changed if any HDET used has longer 
        settling time. Before changing time make sure it doen not spil 
        over next ISR for composite cal. Wait is needed only for first
        reading. */
        DALSYS_BusyWait( FTM_1X_FW_LATENCY_AFTER_RGI_UPDATE_US );

        /* Take samples of HDET read  */
        if ( hdet_sample > 0 )
        {
        
          rfm_cdma_ftm_setup_hdet( device, RFDEVICE_CDMA_HDET_HPM );

          /* Allow for PDET settling time after setting up HDET */
          DALSYS_BusyWait( rfm_cdma_ftm_get_hdet_settling_time(device));
        
          hdet_read = rfm_cdma_ftm_multi_read_hdet( device, hdet_sample );
        
          if ( tx_measure->debug_mode_en == TRUE )
          {
            FTM_MSG_1( FTM_HIGH, "ftm_1x_calibration_v3_tx_measure: "
                                 "HDET Read %d", hdet_read );
          } /* if ( tx_measure->debug_mode_en == TRUE ) */

          /* Populate results data structure */ 
          cal_result->tx_meas_param[device].hdet.hdet_read = hdet_read;
        
        } /* if ( hdet_sample > 0 ) */
      } /* if ( ( tx_measure->tx_meas_param[device].tx_meas_action & 
           ( 1 << TX_MEAS_HDET ) ) != 0 ) */

      /*----------------------------------------------------------------------*/
      /*  TX_MEAS_LPM_HDET */
      /*----------------------------------------------------------------------*/

      /* Perform Low Power Mode HDET measurment */
      if ( ( tx_measure->tx_meas_param[device].tx_meas_action & 
           ( 1 << TX_MEAS_LPM_HDET ) ) != 0 )
      {

        hdet_sample = tx_measure->tx_meas_param[device].lpm_hdet.hdet_sample;

        /* Allow for FW latency to latch the required PDM value to the RTR.
        In cal override mode, FW operates with a 64-chip update interval,
        and so we provide an additional 50usec delay here */
        
        /* Take HDET reads, only if number if HDET samples are more than 0 */
        /* HDET needs time to settle. Time to settle varies based on HDET
        used. This time will need to be changed if any HDET used has longer 
        settling time. Before changing time make sure it doen not spil 
        over next ISR for composite cal. Wait is needed only for first
        reading. */
        DALSYS_BusyWait( FTM_1X_FW_LATENCY_AFTER_RGI_UPDATE_US );
  
        /* Take samples of HDET read  */
        if ( hdet_sample > 0 )
        {
        
          rfm_cdma_ftm_setup_hdet( device, RFDEVICE_CDMA_HDET_LPM );

          /* Allow for PDET settling time after setting up HDET */
          DALSYS_BusyWait( rfm_cdma_ftm_get_hdet_settling_time(device) );
        
          hdet_read = rfm_cdma_ftm_multi_read_hdet( device, hdet_sample );
        
          if ( tx_measure->debug_mode_en == TRUE )
          {
            FTM_MSG_1( FTM_HIGH, "ftm_1x_calibration_v3_tx_measure: "
                                 "LPM HDET Read %d", hdet_read );
          } /* if ( tx_measure->debug_mode_en == TRUE ) */

          /* Populate results data structure */ 
          cal_result->tx_meas_param[device].lpm_hdet.hdet_read = hdet_read;
          
        } /* if ( hdet_sample > 0 ) */
      } /* if ( ( tx_measure->tx_meas_param[device].tx_meas_action & 
           ( 1 << TX_MEAS_HDET ) ) != 0 ) */

      /*----------------------------------------------------------------------*/
      /*  TX_MEAS_LINEARIZER_THERM */
      /*----------------------------------------------------------------------*/

      /* Perform Therm ADC measurement */
      if ( ( tx_measure->tx_meas_param[device].tx_meas_action & 
           ( 1 << TX_MEAS_LINEARIZER_THERM ) ) != 0 )
      {
        /* Trigger Therm Read in FTM */
        therm_read_valid = rfm_cdma_ftm_get_therm_read_in_cal( device, 
                                                               &therm_read );

        /* Restore the HKADC to HDET */
        rfm_cdma_ftm_setup_hdet( device, RFDEVICE_CDMA_HDET_HPM );

        if ( therm_read_valid )
        {
          /* Populate results data structure */ 
          cal_result->tx_meas_param[device].lin_therm.therm_read = therm_read;
        }
        else
        {
          FTM_MSG_1( FTM_ERROR, "ftm_1x_calibration_v3_tx_measure: Dev %d "
                                "Therm Read Failed", device );
          cal_result->tx_meas_param[device].lin_therm.therm_read = 0;
        }

        /* Send the Therm value to the data module for storage with respect to 
        channel */
        (void)ftm_cdma_data_get_device_status( device, &dev_state );
        ftm_cal_data_set_therm_adc( 
                 (uint32)dev_state.curr_chan, 
                 &cal_result->tx_meas_param[device].lin_therm.therm_read );

        if ( tx_measure->debug_mode_en == TRUE )
        {
          FTM_MSG_1( FTM_MED, "ftm_1x_calibration_v3_tx_measure: "
                               "Therm Read %d", 
                     cal_result->tx_meas_param[device].lin_therm.therm_read );
        } /* if ( tx_measure->debug_mode_en == TRUE ) */

      } /* if ( ( tx_measure->tx_meas_param[device].tx_meas_action & 
           ( 1 << TX_MEAS_LINEARIZER_THERM ) ) != 0 ) */

      /*----------------------------------------------------------------------*/
      /*  TX_MEAS_FB_GAIN */
      /*----------------------------------------------------------------------*/

      /* Perform FB Gain measurement */
      if ( ( tx_measure->tx_meas_param[device].tx_meas_action & 
           ( 1 << TX_MEAS_FB_GAIN ) ) != 0 )
      {
        /* Fetch Input Params for FBRx capture */
        fb_meas_inp.tx_device_id = device;
        fb_meas_inp.xpt_mode = (rfcommon_mdsp_xpt_mode_enum_type)tx_measure->tx_meas_param[device].fb_gain.fb_gain_params.xpt_mode;
        fb_meas_inp.gain_comp = tx_measure->tx_meas_param[device].fb_gain.fb_gain_params.gain_comp;
        fb_meas_inp.fbrx_gain_state = tx_measure->tx_meas_param[device].fb_gain.fb_gain_params.fb_gain_state;
        fb_meas_inp.ubias = tx_measure->tx_meas_param[device].fb_gain.fb_gain_params.ubias;
        fb_meas_inp.scale_rx = tx_measure->tx_meas_param[device].fb_gain.fb_gain_params.rx_scale;
        fb_meas_inp.store_capture_data = (boolean)tx_measure->tx_meas_param[device].fb_gain.fb_gain_params.debug_mode_en;
        fb_meas_inp.tech = RFCOM_1X_MODE;
        fb_meas_inp.band.cdma_band = rf_cdma_data_get_curr_band((rfm_device_enum_type)device);
  
        /* Call common FB Gain Measurement function */
        fb_meas_inp.store_capture_data = (boolean)tx_measure->tx_meas_param[device].fb_gain.fb_gain_params.debug_mode_en;
        fb_meas_res = ftm_common_fbrx_capture_and_process(fb_meas_inp);

        if ( tx_measure->debug_mode_en == TRUE )
        {
          FTM_MSG_8( FTM_MED, "ftm_1x_calibration_v3_tx_measure - FB Gain Cal Res: Status %d , Gain = %d,"
                                  "LOFT = %d, RSB = %d, GI = %d, PI = %d, LS_ERR = %d, XCORR_LOG[0] = %d", 
                     fb_meas_res.status, fb_meas_res.gain, fb_meas_res.loft, fb_meas_res.rsb, 
                     fb_meas_res.gain_imbalance, fb_meas_res.phase_imbalance,
                     fb_meas_res.ls_err, fb_meas_res.xcorr_log[0] );
        }

        /*If the first capture is invalid (FW might abort FBRx capture if timeline is bad), re-do fbrx capture*/
        if ( fb_meas_res.status == RFCOMMON_FBRX_ERROR )
        {
          fb_meas_res = ftm_common_fbrx_capture_and_process(fb_meas_inp);
          FTM_MSG_1( FTM_HIGH, "ftm_1x_calibration_v3_tx_measure: Dev %d, FB Gain Measurement "
                                "returned from FW is invalid, re-do capture", device );
        }

        /* Check if FB Gain Measurement was a success */
        if ( fb_meas_res.status == RFCOMMON_FBRX_SUCCESS )
        {
          /* Populate results data structure */ 
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_gain_state = fb_meas_inp.fbrx_gain_state;
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fbrx_mode = ftm_common_fbrx_get_mode();
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_meas_stat = fb_meas_res.status;
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_gain_val = fb_meas_res.gain;
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_meas_loft = fb_meas_res.loft;
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_meas_rsb = fb_meas_res.rsb;
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_meas_gain_imb = fb_meas_res.gain_imbalance;
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_meas_phase_imb = fb_meas_res.phase_imbalance;
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_meas_ls_err = fb_meas_res.ls_err;
          for( log_ind = 0; log_ind < RFLM_FBRX_XCORR_LOG_SIZE; log_ind++ )
          {
            cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_meas_xcorr_log[log_ind] = fb_meas_res.xcorr_log[log_ind];
          }
        }
        else
        {
          FTM_MSG_1( FTM_ERROR, "ftm_1x_calibration_v3_tx_measure: Dev %d "
                                "FB Gain Measurement Failed", device );
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_meas_stat = 
            fb_meas_res.status;
        }

        if ( tx_measure->debug_mode_en == TRUE )
        {
          FTM_MSG_1( FTM_MED, "ftm_1x_calibration_v3_tx_measure: "
                               "FB Gain Measurement Status = %d", 
          cal_result->tx_meas_param[device].fb_gain_meas.fb_meas_res.fb_meas_stat );
        } /* if ( tx_measure->debug_mode_en == TRUE ) */

      } /* if ( ( tx_measure->tx_meas_param[device].tx_meas_action & 
         ( 1 << TX_MEAS_FB_GAIN ) ) != 0 ) */

      /*----------------------------------------------------------------------*/
      /*  TX_MEAS_FB_PWR */
      /*----------------------------------------------------------------------*/

      /* Perform FB Power measurment */
      if ( ( tx_measure->tx_meas_param[device].tx_meas_action & 
           ( 1 << TX_MEAS_FB_PWR ) ) != 0 )
      {
        fb_power_samples = 
            tx_measure->tx_meas_param[device].fb_power.fp_power_params.fb_power_samples;

        /* Allow for FW latency to latch the required PDM value to the RTR.
        In cal override mode, FW operates with a 64-chip update interval,
        and so we provide an additional 50usec delay here */

        /* Take HDET reads, only if number if HDET samples are more than 0 */
        /* HDET needs time to settle. Time to settle varies based on HDET
        used. This time will need to be changed if any HDET used has longer 
        settling time. Before changing time make sure it doen not spil 
        over next ISR for composite cal. Wait is needed only for first
        reading. */
        DALSYS_BusyWait( FTM_1X_FW_LATENCY_AFTER_RGI_UPDATE_US );

        /* Take samples of HDET read  */
        if ( fb_power_samples > 0 )
        {
          rfm_cdma_ftm_setup_hdet( device, 
                tx_measure->tx_meas_param[device].fb_power.fp_power_params.fb_gain_state );

          /* Allow for PDET settling time after setting up HDET */
          DALSYS_BusyWait( rfm_cdma_ftm_get_hdet_settling_time(device));

          hdet_read = rfm_cdma_ftm_multi_read_hdet( device, fb_power_samples );
      
          if ( tx_measure->debug_mode_en == TRUE )
          {
            FTM_MSG_1( FTM_HIGH, "ftm_1x_calibration_v3_tx_measure: "
                                 "FB Power Read %d", hdet_read );
          } /* if ( tx_measure->debug_mode_en == TRUE ) */
           
          /* Populate results data structure */ 
        cal_result->tx_meas_param[device].fb_power_meas.fb_pwr_meas_res = 
                                             (int16)(100 * log10((float)hdet_read));

        } /* if ( fb_power_samples > 0 ) */
      } /* if ( ( tx_measure->tx_meas_param[device].tx_meas_action & 
           ( 1 << TX_MEAS_FB_PWR ) ) != 0 ) */

      /* Populate Tx Meas action */
      cal_result->tx_meas_param[device].tx_meas_action = 
                          tx_measure->tx_meas_param[device].tx_meas_action;
      
      /* Update OpCode to set the processed bit to zero. */
      dev_temp = dev_temp & (~dev_mask);
      
      /* For the current unprocessed Opcode, extract the bit mask for the first
      enabled cal action. The first bit will be from LSB. */
      dev_mask = (uint8)rf_extract_first_one_bit_mask( dev_temp );
      
      /* Print Extra Debug message */
      if ( tx_measure->debug_mode_en == TRUE )
      {
        FTM_MSG_2( FTM_HIGH, "ftm_1x_calibration_v3_tx_measure: Dev %d "
                   "Rx Meas mask 0x%x", device, 
                   tx_measure->tx_meas_param[device].tx_meas_action );        
      } /* if ( tx_override->debug_mode_en == TRUE ) */
    } /* if ( device < RFM_MAX_DEVICES ) */
    else
    {
      ret_val = FALSE;
      FTM_MSG_1( FTM_ERROR, "ftm_1x_calibration_v3_tx_measure: Dev %d "
                 "not valid", device );
      break;
    } /* if ! ( device < RFM_MAX_DEVICES ) */
  } /* while ( dev_mask != 0 ) */

  /* Populate common results */
  cal_result->device_mask = tx_measure->device_mask;

  /* Record the execution time */
  tm_time = rf_time_get_elapsed( tm_tick, RF_USEC );
  
  FTM_MSG_3( FTM_HIGH, "ftm_1x_calibration_v3_tx_measure: [Status %d] "
             "DevMask %d tmTime[%d]us", 
             ret_val, tx_measure->device_mask, tm_time );
  
  return ret_val;

} /* ftm_1x_calibration_v3_tx_measure */

/*! \} */

/*============================================================================*/
/*!
  @name Non Volatile

  @brief
  Functions related to NV items used with RF Calibration V2.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Write RX calibration to NV

  @details
  This function will write Write RX calibration to NV. All supported RX
  calibration NV items are written to NV.

  @param *ftm_req_data
  FTM Request Data
  
  @return
  FTM response structure, with FTM_RSP_DO_LEGACY set.
*/
ftm_rsp_pkt_type
ftm_1x_commit_cal_nv
(
  void  * ftm_req_data
)
{

  /* Request Packet */
  ftm_1x_commit_cal_nv_req_type* header = 
    (ftm_1x_commit_cal_nv_req_type*)ftm_req_data;

  /* Response Packets */
  ftm_rsp_pkt_type ftm_rsp_data; /* Packet returned to Diag */
  ftm_1x_commit_cal_nv_rsp_type *cal_rsp;    /* Packet Payload */

  /* Define : To Extract Useful Data */
  sys_band_class_e_type band; /* Band corresponding to NV Write */

  /* Other Defines */
  uint8 status; /* NV Write Status */

  /* Extract Useful Data */
  band = (sys_band_class_e_type)header->band;

  /* Create a new reponse packet in the ftm heap.
  This will also update first 10-bytes field in Packet Payload in the FTM
  response packet. These fields are command(1), subsystem ID (1), Command
  Mode (2), FTM command (2), Request Packet Length (2), and Response packet
  Length (2). */
  ftm_rsp_data = ftm_cdma_malloc_rsp_pkt( ftm_req_data, 
                          sizeof(ftm_1x_commit_cal_nv_rsp_type) );


  /* Perform NV Write */
  status = rfm_1x_ftm_write_nv( band );

  /* Extract Packet Payload from New Response Packet, ftm_rsp_data */
  cal_rsp = (ftm_1x_commit_cal_nv_rsp_type*)ftm_rsp_data.pkt_payload;

  if(cal_rsp != NULL)
  {
  /* Update the Response Packet Field */
  cal_rsp->status = status;
  }
  else
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_commit_cal_nv : NULL cal_rsp" );
    status = FALSE;
  }

  if ( status == FALSE )
  {
    ftm_rsp_data.cmd = FTM_RSP_BAD_PARM;
  }

  FTM_MSG( FTM_LOW, "CDMA Calv2 : Commit NV Done" );

  /* Return Response Packet */
  return ftm_rsp_data;

} /* ftm_1x_commit_cal_nv */

/*! \} */

/*============================================================================*/
/*!
  @name XPT Functions
  @brief
  Functions related to EPT Calibration
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure EPT

  @details
  This function will call RFM function in order to configure FW for
  EPT calibration Mode

  @param payload
  Payload associated with the EPT configuration
  
  @return
  Flag indicating the status of the EPT configure action
*/
boolean
ftm_1x_cal_xpt_configure
(
  ftm_cal_xpt_instr_payload_ept_config_type const *payload
)
{

  boolean ret_val = TRUE;
  rfm_device_enum_type device;
  rfm_device_enum_type device_fb;
  rf_time_tick_type prof_cal_func; /* Function Profiling Variable */
  rf_time_type prof_cal_func_val = 0; /* Function Profiling Value Variable */
  ftm_cdma_data_status_type dev_state; /* Var to hold current state */

  prof_cal_func = rf_time_get_tick(); /* Get Time tick for Function Profile */

  /* NULL pointer check */
  if ( payload == NULL )
  {
    FTM_MSG( FTM_LOW, "ftm_1x_cal_xpt_configure : NULL Payload" );
    ret_val = FALSE;
  } /* if ( payload == NULL ) */
  else
  {

    device = payload->primary_device;
    device_fb = payload->feedback_device;
    
    (void)ftm_cdma_data_get_device_status( device, &dev_state );  
        
    /* Initialize buffer pool for xpt captures */
    rfcommon_core_xpt_init_buffers();

    /* Create a new data module table using the current channel */
    ftm_cal_data_init_cur_channel_map( dev_state.curr_chan );

    /* Perform EPT Configure */
    ret_val = ftm_1x_xpt_cal_config( device, device_fb );

  } /* if ! ( payload == NULL ) */
  
  /* Get time taken for this function to execute */
  prof_cal_func_val = rf_time_get_elapsed( prof_cal_func, RF_USEC );

  FTM_MSG_2( FTM_LOW, "ftm_1x_cal_xpt_configure : [Status %d] Profile %d us ",
             ret_val, prof_cal_func_val );

  /* Return */
  return ret_val;

} /* ftm_1x_cal_xpt_configure */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  IQ Capture

  @details
  This function will call RFM function in order to send a command to FW to
  perform EPT IQ Capture

  @param payload
  Payload associated with the EPT IQ Capture
  
  @return
  Flag indicating the status of the EPT IQ Capture action
*/
boolean
ftm_1x_cal_xpt_iq_capture
(
  ftm_cal_xpt_instr_payload_iq_capture_type const *payload
)
{
  /* IQ capture radio request */
  ftm_cal_xpt_instr_req_type xpt_req;
  boolean api_status;

  /* Parameter validation */
  if ( payload == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_cal_xpt_iq_capture: NULL argument" );
    api_status = FALSE;
  } /* if ( payload == NULL ) */
  else
  {
    /* Populate the Request Packet to configure radio */
    xpt_req.cmd = DIAG_SUBSYS_CMD_F;
    xpt_req.sub_sys = FTM;
    xpt_req.mode = FTM_1X_C0_C;
    xpt_req.cdma_cmd = FTM_INT_CALV3_XPT_IQ_CAPTURE;

    {
      size_t data_length = sizeof(ftm_cal_xpt_instr_payload_iq_capture_type) ; 
      memscpy ( &(xpt_req.xpt_param), data_length , 
                payload , data_length );
    }

    ftm_common_send_radio_command( &xpt_req,
                                   sizeof(ftm_cal_xpt_instr_req_type) );
    api_status = TRUE;
  }

  return api_status;
} /* ftm_1x_cal_xpt_iq_capture */

/*! \} */

/*============================================================================*/
/*!
  @name RX AGC Threshold
  @brief
  Functions related to RX AGC Threshold Triggering
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RX AGC Threshold Reached event callback

  @details
  This function will call RFM function to check if RX AGC is reached given
  threshold

  @param polling_payload
  Pointer to the polling payload
 
  @return
  Flag indicating the if the event is met or not,
 
  @retval TRUE
  Flag indicating the event is met
  
  @retval FALSE
  Flag indicating the event is not yet met
*/
static boolean
ftm_1x_rx_agc_threshold_reached
(
  ftm_cal_polling_instr_payload_type const *polling_payload,
  void const *event_payload
)
{
  ftm_cal_polling_event_1_payload_type *event_pl = 
            (ftm_cal_polling_event_1_payload_type*)event_payload;

  /* Validation */
  if ( polling_payload == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_rx_agc_threshold_reached: NULL Payload" );
    return FALSE;
  } /* if ( polling_payload == NULL ) */

  /* Validation */
  if ( event_payload == NULL )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_rx_agc_threshold_reached: NULL Event Payload" );
    return FALSE;
  } /* if ( event_payload == NULL ) */

  /* Validation */
  if (    SIGNAL_RISING  != event_pl->direction  
       && SIGNAL_FALLING != event_pl->direction )
  {
    FTM_MSG( FTM_ERROR, "ftm_1x_rx_agc_threshold_reached: "
                        "Invalid direction argument" );
    return FALSE;
  } /* if ( SIGNAL_RISING ... ) */

  {
    rfm_cdma_ftm_rx_agc_data_type rx_agc_data_dbm10;  

    FTM_MSG_3( FTM_LOW, "ftm_1x_rx_agc_threshold_reached: Reading AGC from "
               "device %d (RFM %d), threshold %d dBm10", 
               event_pl->primary_device, 
               ftm_calibration_dev_flag_to_rfm_dev(event_pl->primary_device), 
               event_pl->threshold );

    /* Call RFM function to get current RX AGC*/  
    if( FALSE == rfm_1x_ftm_get_rx_agc_data(
       ftm_calibration_dev_flag_to_rfm_dev(event_pl->primary_device), 
                                           &rx_agc_data_dbm10, 
                                           RFM_CDMA_AGC_FORMAT__DBM10))
    {
      FTM_MSG( FTM_ERROR, "ftm_1x_rx_agc_threshold_reached: "
                          "Failed to read RX AGC" );
      return FALSE;
    }

    FTM_MSG_1( FTM_LOW, "ftm_1x_rx_agc_threshold_reached: Current RX reading: "
               "%d dBm10", rx_agc_data_dbm10.rx_agc[0]);

    if( SIGNAL_RISING == event_pl->direction )
    {
      FTM_MSG_1( FTM_LOW, "ftm_1x_rx_agc_threshold_reached: Waiting signal to "
                 "rise above or equal %d dBm10", event_pl->threshold);
      if( event_pl->threshold <= rx_agc_data_dbm10.rx_agc[0] )
      {      
        FTM_MSG_1( FTM_LOW, "ftm_1x_rx_agc_threshold_reached: Rising signal "
                   "threshold %d dBm10 reached.", event_pl->threshold);
        return TRUE;
      }
    }/*if( SIGNAL_RISING == event_pl->direction )*/
    else 
    {
      FTM_MSG_1( FTM_LOW, "ftm_1x_rx_agc_threshold_reached: Waiting signal to "
                 "fall below or equal %d dBm10", event_pl->threshold);
      if( event_pl->threshold >= rx_agc_data_dbm10.rx_agc[0] )
      {
        FTM_MSG_1( FTM_LOW, "ftm_1x_rx_agc_threshold_reached: Falling signal "
                   "threshold %d dBm10 reached", event_pl->threshold);
        return TRUE;
      }
    }
  }

  return FALSE;
} /* ftm_1x_rx_agc_threshold_reached */

/*! \} */

/*============================================================================*/
/*!
  @name
  Cal v3 Polling
 
  @brief
  Functions related to Polling segment implementaion in cal v3
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Intialize 1x polling events
 
  @details
  This function will register the callbacks for the event to be used in
  cal v3 polling segment.
 
  To increase the registration efficiency register for the event with
  higher event_ID is registered first. This prevents multiple re-allocs in
  ftm_calibration_polling_register_event() function
 
  @return
  Status of 1x polling event registration.
*/
boolean
ftm_1x_polling_event_init
(
  void
)
{
  boolean ret_val = TRUE;

  ret_val &= ftm_calibration_polling_register_event( RFM_1X_MODE,
                                           POLLING_EVENT_RXAGC_THRES_REACHED,
                                           ftm_1x_rx_agc_threshold_reached );

  FTM_MSG( FTM_MED, "ftm_1x_polling_event_init: Events registered" );

  return ret_val;

} /* ftm_1x_polling_event_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  XPT IQ capture

  @details
  This function handles requests for IQ captures during XPT cal.

  @param *data
  Request Data Packet 

  @return
  Flag indicating the status of the XPT IQ Capture action
*/
boolean
ftm_1x_calv3_xpt_iq_capture
(
  ftm_cal_xpt_instr_req_type *ftm_req_data
)
{
  ftm_cal_xpt_instr_payload_iq_capture_type const *payload = NULL;
  const rf_cdma_data_status_type *dev_status_r;
  boolean ret_val = TRUE;
  rf_time_tick_type prof_cal_func; /* Function Profiling Variable */
  rf_time_type prof_cal_func_val = 0; /* Function Profiling Value Variable */
  rf_common_xpt_buffer_id_type buffer_ids; /* buffer ids for capture and dpd 
                                              results */
  boolean buff_alloc = TRUE; /* flag indicating if the buffer is allocated */
  rfm_1x_iq_capture_param_type capture_param; /* Capture param */
  ftm_cdma_data_status_type dev_state; /* Var to hold current state */
  rfcommon_xpt_cal_iq_capture_data_type iq_cap_data;
  boolean iq_cap_status;
  uint32 samp_rate = 0;

  prof_cal_func = rf_time_get_tick(); /* Get Time tick for Function Profile */
   
  /* Request Packet */
  payload = &(ftm_req_data->xpt_param);

  /* NULL pointer check */
  if ( payload == NULL )
  {
    FTM_MSG( FTM_LOW, "ftm_1x_calv3_xpt_iq_capture : NULL Payload" );
    return FALSE;
  } /* if ( payload == NULL ) */
  else
  {
    /* Init iq_cap_data value to 0 */
    memset( &iq_cap_data, 0, sizeof(rfcommon_xpt_cal_iq_capture_data_type) );
  
    dev_status_r = rf_cdma_get_device_status( payload->feedback_device );
    /*Perform NULL pointer check*/
    if ( dev_status_r  == NULL )
    {
      FTM_MSG_1( FTM_ERROR, "ftm_1x_calv3_xpt_iq_capture: dev_status_r [ %d ] "
                            "NULL Data",
                payload->feedback_device );
      ret_val = FALSE;
    } /* if( dev_status_r  == NULL ) */
    /* Send IQ Capture command only if previous capture is done */
    if ( ( rfcommon_xpt_is_capture_done() == TRUE ) && ( ret_val == TRUE ) )
    {
      (void)ftm_cdma_data_get_device_status( payload->primary_device,
                                             &dev_state );  

      /* Indicate that the IQ capture is in process */
      ret_val &= rfcommon_xpt_set_capture_in_progress();

      /* Set capture and processing flags */
      ret_val &= rfcommon_xpt_set_processing_in_progress();
      
      /* Get buffer and results container ID */
      buff_alloc = rfcommon_core_xpt_get_buffers( &buffer_ids );

      /* Start IQ capture */
      if ( buff_alloc == TRUE )
      {
        /* Populate capture params */
        capture_param.capture_type = 
             ftm_common_xpt_map_capture_type( payload->capture_type );
        capture_param.num_samples = payload->num_samples;
        capture_param.first_trig = payload->first_trigger;
        capture_param.last_trig = payload->last_trigger;
        capture_param.do_processing = 
                         ftm_calibration_xpt_map_proc_type(payload->proc_flag);
        capture_param.buffer_id = buffer_ids;
        capture_param.split_samp = 0;

        /* Only update DPD map for DPD captures */
        if (capture_param.capture_type < XPT_CAPTURE_DELAY)
        {
          FTM_MSG_4(FTM_MED,"ftm_1x_calv3_xpt_iq_capture: Push to DPD Map - "
                            "Chan: %d, CapType: %d, CaptBuf: %d, ResBuf: %d",
                                                 dev_state.curr_chan,
                                                 iq_cap_data.delay_capture_type,
                                                 buffer_ids.capt_buf_id,
                                                 buffer_ids.res_buf_id );

          /* Create a new data module table using the current channel */
          ftm_cal_data_push_cur_override_to_dpd_map( dev_state.curr_chan,
                                                     buffer_ids.capt_buf_id,
                                                     buffer_ids.res_buf_id );
        }

          /* Store the IQ Caprure Data */
        iq_cap_data.device = payload->primary_device;
        iq_cap_data.device_fb = payload->feedback_device;
        iq_cap_data.channel = dev_state.curr_chan;
        iq_cap_data.is_last_trig = payload->last_trigger;
        iq_cap_data.buffer_id = buffer_ids;
        /* RxLM buffer must have been set up properly before this */
          samp_rate = rf_cdma_mc_get_samp_rate((uint8)dev_status_r->rxlm_handle);
        iq_cap_data.samp_rate = samp_rate;
        iq_cap_data.delay_capture_type = RF_COMMON_XPT_DELAY_CAP_INVALID;

        /* only update the delay map for correct capture type */
        if (capture_param.capture_type == XPT_CAPTURE_DELAY)
        {
          iq_cap_data.delay_capture_type = RF_COMMON_XPT_DELAY_CAP_NONLTE;
        }

        ret_val &= rfcommon_xpt_store_iq_capture_data( &iq_cap_data );

        /* Send command to perform IQ capture */
        iq_cap_status = rfm_1x_ftm_xpt_iq_capture( payload->primary_device,
                                              payload->feedback_device,
                                              capture_param, &samp_rate );

        ret_val &= iq_cap_status;
        /* if xpt_iq_capture failed, unvote modem bus clk immediately;
           otherwise, modem bus clk will be unvoted at capture done response
           when last_trig is true*/
        if ( iq_cap_status == FALSE )
        {
            if (capture_param.capture_type == XPT_CAPTURE_FBRX)
            {
              // do nothing
              FTM_MSG(FTM_HIGH, "ftm_1x_calv3_xpt_iq_capture: FBRx type IQ "
                                "Capture failed,"
                                " use feedback_setup api to turn off FB device");
            }
            else
            {
          ftm_1x_xpt_cal_deconfig( payload->primary_device, 
                                   payload->feedback_device );
        }
          }
      } /* if ( buff_alloc == TRUE ) */
      else
      {
        ret_val = FALSE;
        FTM_MSG( FTM_ERROR, "ftm_1x_calv3_xpt_iq_capture: Buffer could not "
                            "be allocated" );
      } /* if ! ( buff_alloc == TRUE ) */

    } /* if ( rfcommon_xpt_is_capture_done() == TRUE ) */
    else
    {
      FTM_MSG( FTM_ERROR, "ftm_1x_calv3_xpt_iq_capture: Previous IQ capture"
                          "is not complete" );
    } /* if ! ( rfcommon_xpt_is_capture_done() == TRUE ) */
  } /* if ! ( payload == NULL ) */

  /* Get time taken for this function to execute */
  prof_cal_func_val = rf_time_get_elapsed( prof_cal_func, RF_USEC );

  FTM_MSG_2( FTM_LOW, "ftm_1x_calv3_xpt_iq_capture : [Status %d] Profile %d us",
             ret_val, prof_cal_func_val );

  /* Return */
  return ret_val;

}/* ftm_1x_calv3_xpt_iq_capture */

#endif /* FEATURE_CDMA1X */
/*! \} */

#endif /* FEATURE_FACTORY_TESTMODE */

/*! @} */
