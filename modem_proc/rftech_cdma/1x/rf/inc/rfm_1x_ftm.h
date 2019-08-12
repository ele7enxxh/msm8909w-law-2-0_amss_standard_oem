#ifndef RFM_1X_FTM_H
#define RFM_1X_FTM_H

/*!
   @file
   rfm_1x_ftm.h

   @brief
   RF Driver's 1x interface file for FTM.
*/

/*==============================================================================

  Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/inc/rfm_1x_ftm.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
04/10/15   vr      Add support for XPT cal when Drx path not present
08/01/14   aa      Move fbrx rsb comp during ftm_common_fbrx_capture_and_process()
07/03/14   JJ      Pass band to rfm_1x_ftm_config_rxlm_for_fbagc()
06/09/14   JJ      Added an api to re-enable fbrx during internal cal
04/10/14   JJ      Change rfm_1x_ftm_config_fw_state() return type
04/08/14   JJ      Added an api to send oqpch_search_done msg to FW
11/25/13   spa     Use int32 math for LNA offset calculation 
11/18/13   cd      Provide interface to process Tx override params for RFLM ovr
11/11/13   JJ      Added one input param in rfm_1x_ftm_config_fw_state
10/15/13   zhw     LLVM compiler error fix
06/28/13   JJ      Add API for antenna tuner to update tune code in ftm mode
06/18/13   JJ      Add func to register/de-register fw messages 
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
05/01/13   jmf     Implement xPT IQ Capture SWI change
04/22/13   jmf     Implement xPT processing Enum Type
04/18/13   aro     Inlined MSGR logging function
04/12/13   aro     Unified LNA calibration setup interface
04/12/13   aro     Added AGC ACQ down counter in RxAGC data
04/12/13   aro     Optimized RxAGC query function
03/19/13   aro     Deleted ET config path
03/19/13   aro     Modularized FB config into transceiver, asm, and rxlm
02/21/13   spa     Add API to send pilot meas stop stream to FW (FTM IRAT)
02/19/13   spa     Added rfm_1x_ftm_pilot_meas_cfg prototype 
01/22/13   aro     Code to use unified IQ capture param
01/22/13   aro     Added common XPT capture type
01/22/13   aro     Removed DPD index from IQ capture interface
01/18/13   aro     Renamed function name to have XPT
01/18/13   aro     Deprecated XPT polling code
01/18/13   aro     Deleted nikel-XPT deprecated FW responses
01/18/13   aro     Removed nikel-XPT specific code
01/18/13   aro     Deleted nikel-XPT specific code
12/27/12   cd      Added action types for IQ gain and Env scale overrides
12/21/12   cd      IQ gain and envelope scale overrides are 32 bit values
12/18/12   cd      Add support for delay value programming to common FW
11/19/12   zhw     Cal mode interface change support for APT in Dime
11/16/12   zhw     APT PA bias/current ftm support
11/16/12   Saul    Sample Capture. Use common response.
11/08/12   cd      - Added xPT Tx AGC override
                   - Added config EPT path override
10/25/12   cd/Saul Generalized load DPD table API
                   - Changed request packet type
                   - Added response packet type
                   - Made XPT IQ capture common.
10/03/12   Saul    CDMA Sample Capture. Capture done message.
09/28/12   aro     Added device to 1x FWS config interface
09/19/12   Saul    CDMA Sample Capture. Capture type, do leakage flag, 
                   stop tx agc update fw command
09/04/12   aro     Added band and channel to EPT FB config interface
08/31/12   Saul    CDMA. Added support for EPT_PROCESSING_DONE FW message
08/08/12   vs      Added support to query EPT config params
08/06/12   hdz     Added prototype of rfm_1x_ftm_set_tx_pwr_limit()
07/25/12   aro     DPD index in IQ Capture command
07/23/12   Saul    CDMA. Removed unnecessary header.
07/20/12   aro     Added support to do polling for EPT expansion
07/17/12   aro     Enhanced support of configure feedback path
06/26/12   aro     Deleted unused parameter from EPT configure function
06/26/12   aro     Deleted IQ Capture init function
06/13/12   aro     Fixed single instance IQ Capture
06/13/12   aro     Implemented DPD coefficient interface
06/13/12   aro     Update EPT configure, iq capture, and override functions
                   to have devices
06/13/12   aro     Removed unrelevant fields from EPT payloads
06/11/12   aro     Added MDSP path/device to EPT configure function
06/11/12   aro     Function ordering
06/11/12   aro     Removed redundant EPT IQ Capture trigger function
06/08/12   aro     Updated EPT IQ capture interface to pass device
06/05/12   anr     Initial EPT Support
05/31/12   spa     Added rfm_1x_ftm_tear_down_radio API prototype
04/03/12   aro     Updated FWS config function to perform SemWait in MC
04/03/12   aro     Updated FWS config function to manage state transition in
                   FTM
04/02/12   aro     Removed unused 1x Cal v3 functions
03/09/12   spa     Changed rfm_1x_ftm_get_calibration_compensation arguments to 
                   cdma band and chan type
03/08/12   hdz     Added rfm_1x_ftm_set_tx_freq_adjust
12/05/11   aro     Added interface to reconfigure RxAGC from ACQ to TRK
12/02/11   sty     Changed return type of rfm_1x_ftm_get_rx_agc_data()
11/10/11   aro     Updated RF-MDSP interfaces to pass semaphore to 
                   wait on for blocking calls
11/10/11   aro     Defined CDMA semaphore as generic CDMA type
09/01/11   cd      Provide RFM FTM interface to clear PA Range override. This 
                   is used when explicitly calling the set Tx override interface
                   in FTM
08/30/11   aro     Added support to set Tx DP IQ Gain from FTM
07/19/11   Saul    Combined FW enable/disable APIs into one FW config state API.
07/19/11   aro     Updated measure rx function to return status
07/07/11   sty     Added band info in rfm_1x_ftm_set_tx_agc_override
07/01/11   aro     Added interface to perform Intelliceiver Calibration
06/14/11   aro     Renamed TxAGC Override function + Signature to use PA State
06/14/11   sty     Added format arg to agc query APIs
05/16/11   aro     Removed DVGA Calibration interfaces
05/16/11   aro     Updated LNA Calibration interfaces to pas Gain State
05/16/11   aro     Added Rx Measurment config and query function
05/16/11   aro     PA Bias Type update
04/07/11   aro     Updated argument type for TxAGC Override
03/18/11   aro     Added interface to query Calibration Compensation Value
03/16/11   aro     Cleaned up Override Interface + added device argument
03/14/11   aro     Consolidated RxAGC Reads to one data structure
03/14/11   aro     Merged Set DVGA/LNA Offset to a unified interface
03/14/11   aro     Added FW enable and disable interface
03/04/11   aro     [1] Moved HDET Read and AVGA Idnex read function to Common
                   [2] Changed type for Set PA Range interface
03/04/11   sty     Added prototype for rfm_1x_ftm_get_tc()
03/04/11   sty     Changed return type of rfm_1x_read_hdet 
03/03/11   sty     Renamed rfm_1x_ftm_get_min_max_lut_idx to 
                   rfm_1x_ftm_get_min_max_avga_idx
03/01/11   sty     Renamed API to rfm_1x_ftm_get_min_max_lut_idx
03/01/11   sty     Added rfm_1x_ftm_get_min_max_lut
02/22/11   sty     Added rfm_1x_read_hdet() and renamed rfm_pa_r1_r0_type to 
                   rfm_pa_range_type
02/18/11   sty     Deleted redundant rfm_1x_ftm_set_freq_offset() and 
                   rfm_1x_ftm_set_cw
02/18/11   sty     Added rfm_1x_ftm_set_freq_offset()
02/17/11   sty     Use rfm_pa_r1_r0_type instead of rfm_pa_gain_state_type
02/17/11   aro     Moved LNA Gain Step definition to RFM
02/16/11   sty     Added support for Tx operations from FTM
02/15/11   sty     Deleted rfm_1x_ftm_enable_tx()
02/14/11   sty     Renamed rfm_1x_ftm_set_tx_on() to rfm_1x_ftm_enable_tx()
02/13/11   aro     Added Set Tx Power Interface
02/13/11   aro     [1] Updated LNA query interface
                   [2] Added support for Get TxAGC
02/12/11   aro     Changed the interface to return LNA Gain state and Gain word
02/11/11   sty     Added back rfm_1x_ftm_set_tx_on
02/09/11   sty     Deleted rfm_1x_ftm_set_tx_on
02/09/11   sty     Added rfm_1x_ftm_set_tx_on
02/09/11   aro     Added Interface to set Tx Power in dBm
02/09/11   aro     Added Set TxAVGA Interface 
02/09/11   aro     Added Set PA State interface
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
01/31/11   aro     Added DVGA Calibration Support
01/31/11   aro     Deleted unused function
01/24/11   aro     Added LNA Range Query Interface
01/15/11   aro     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfm_cdma_power_mode_types.h"
#include "rf_cdma_lna.h"
#include "rfm_1x_cal_data_types.h"
#include "rfm_gain_state_types.h"
#include "rfm_cdma_tx_power_types.h"
#include "rfm_cdma_ftm_types.h"
#include "rfm_cdma_agc_types.h"
#include "rfm_cdma_fw_types.h"
#include "rf_cdma_intelliceiver.h"
#include "rfcommon_core_xpt.h"
#include "rfcommon_core_xpt_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! 1x RxAGC ACQ duration to be used during intelliceievr update */
#define RFM_1X_FTM_IC_ACQ_DURATION_US RF_CDMA_IC_1X_CAL_MODE_ACQ_DURATION_US

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure holding the IQ capture configuration parameter. The data is used
  while perform IQ capture
*/
typedef struct
{
  rfcommon_xpt_capture_type capture_type;
  /*!< Variable indicate the type of IQ capture */

  uint16 num_samples;
  /*!< Number of samples to be taken while doing IQ capture */

  boolean first_trig;
  /*!< Indicates that it is the first IQ capture in the sequence */

  boolean last_trig;
  /*!< Indicates that it is the last IQ capture in the sequence */

  rfcommon_xpt_processing_type do_processing;
  /*!< Flag to indicate firmware that the IQ capture processing needs to be 
  done after IQ capture */

  uint16 split_samp;
  /*!< Number of samples before split capture config change, 0 = no split capture*/

  rf_common_xpt_buffer_id_type buffer_id;
  /*!< Buffer ID to store IQ capture samples and results */

} rfm_1x_iq_capture_param_type;

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_setup_lna_calibration
(
  rfm_device_enum_type device,
  rfm_lna_gain_state_type gain_state,
  int16 gain_val,
  uint16 acq_duration_us
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_measure_lna_offset
(
  rfm_device_enum_type device,
  rfm_lna_gain_state_type gain_state,
  int32 agc_expected_val,
  int32 *lna_offset
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_calibrate_lna
(
  rfm_device_enum_type device,
  int16 power,
  rfm_lna_gain_state_type gain_state,
  rfm_1x_lna_cal_data_type *cal_data,
  int32 length
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_set_rx_gain
(
  rfm_device_enum_type device,
  rfm_lna_gain_state_type gain_state,
  int16 gain_val
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_configure_rxagc_acq
(
  rfm_device_enum_type device,
  uint16 acq_duration_us
);

/*----------------------------------------------------------------------------*/
int16
rfm_1x_ftm_get_calibration_compensation
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_calibrate_intelliceiver
(
  rfm_device_enum_type device,
  int16 power,
  rfm_1x_ic_calibration_data_type *cal_data
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_write_nv
(
  sys_band_class_e_type band
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_set_rx_gain_state
(
  rfm_device_enum_type device,
  boolean override_mode,
  rfm_lna_gain_state_type state
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_set_pa_state
(
  const rfm_device_enum_type device,
  rfm_pa_gain_state_type state
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_set_pa_range
(
  const rfm_device_enum_type device,
  rfm_pa_range_type pa_range
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_clear_pa_range_override
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_set_fw_tx_agc_override
(
  const rfm_device_enum_type device,
  const rfm_cdma_band_class_type band,
  const rfm_pa_gain_state_type pa_state,
  uint16 avga_idx,
  int16 betap_gain,
  uint16 pa_bias,
  uint16 pa_q_current,
  boolean pa_q_current_enable
);

/*----------------------------------------------------------------------------*/
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
);

/*----------------------------------------------------------------------------*/
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
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_get_rx_agc_data
(
  const rfm_device_enum_type device,
  rfm_cdma_ftm_rx_agc_data_type *rx_agc_data,
  const rf_cdma_agc_format_type format
);

/*----------------------------------------------------------------------------*/
int16
rfm_1x_ftm_get_rx_agc
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_set_tx_pwr
(
  const rfm_device_enum_type device,
  rfm_cdma_tx_pwr_mode_type tx_pwr_mode,
  int16 tx_pwr
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_set_tx_iq_gain
(
  rfm_device_enum_type device,
  uint32 tx_dp_iq_gain
);

/*----------------------------------------------------------------------------*/
void
rfm_1x_ftm_get_tx_agc_data
(
  const rfm_device_enum_type device,
  rfm_cdma_ftm_tx_agc_data_type *tx_agc_data,
  const rf_cdma_agc_format_type format
);

/*----------------------------------------------------------------------------*/
void*
rfm_1x_ftm_config_fw_state
(
  const rfm_device_enum_type device,
  rfm_cdma_fw_state_type fw_state
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_set_tx_freq_adjust
( 
  int16 tx_freq_adjust
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_xpt_cal_configure
(
  rfm_device_enum_type device,
  rfm_device_enum_type device_fb
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_xpt_iq_capture
(
  rfm_device_enum_type device,
  rfm_device_enum_type device_fb,
  rfm_1x_iq_capture_param_type capture_param,
  uint32 * samp_rate
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_config_fb_rxlm
(
  rfm_device_enum_type device,
  rfm_device_enum_type device_fb,
  lm_handle_type rxlm_fb_handle,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_tear_down_radio
( 
  rfm_device_enum_type device
);


/*----------------------------------------------------------------------------*/
boolean 
rfm_1x_ftm_stop_txagc_update
(
  boolean stop_tx_agc_update
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_set_tx_pwr_limit
(
  rfm_device_enum_type device,
  boolean tx_pwr_override, 
  int16 tx_pwr_limit_dbm
);

/*----------------------------------------------------------------------------*/
boolean 
rfm_1x_ftm_pilot_meas_cfg
(
   rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean 
rfm_1x_ftm_pilot_meas_stop_stream
(
   rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_register_fw_msgs
( 
  void
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_deregister_fw_msgs
( 
  void
);

/*----------------------------------------------------------------------------*/
int32 
rfm_1x_ftm_antenna_tuner_update_tune_code
(
   void
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_stop_oqpch_srch
(
   rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rfm_1x_ftm_config_rxlm_for_fbagc
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* RFM_1X_FTM_H */
