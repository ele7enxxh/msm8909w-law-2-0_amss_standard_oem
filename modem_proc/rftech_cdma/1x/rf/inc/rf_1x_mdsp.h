#ifndef RF_1X_MDSP_H
#define RF_1X_MDSP_H

/*! 
  @file
  rf_1x_mdsp.h
 
  @brief
  This module defines the function prototypes and defintions used by 1X RF-MDSP
  Internal(RF) interface.
*/

/*==============================================================================

  Copyright (c) 2009 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/inc/rf_1x_mdsp.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/17/15   vr      Added dynamic txr_iq_dac setting update support
10/07/14   vr      ftm iq capture changes
04/11/14   spa     Removed unused jammer detect API
04/08/14   JJ      Added an api to send oqpch_search_done msg to FW
04/01/14   APU     PA params update hook-up for different RC/data rate configs.
03/28/14   APU     Hook up Tx logging interfaces in SW driver to log data 
                   provided by RFLM.
02/20/14   spa     Update rx gain override/setup LNA cal interface (rx_handle)
12/06/13   cd      Load nominal baseband gains to RFLM
11/25/13   cd      Remove flag-based functionality for legacy Tx AGC
11/11/13   spa     Add rxlm handle to LNA override API
11/08/13   cd      Remove support for legacy FW interfaces
10/18/13   cd      Added flag to support RFLM TxAGC conditionally in FW
08/16/13   ndb     Clean-up the SPI based antenna tuner support
07/19/13   Saul    XPT. ET delay vs freq vs temp support.
06/26/13   zhw     Remove unused HDET message
06/18/13   JJ      Added functions to register/de-register FW MSG responses
06/13/13   zhw     Added Tuner CL meas trigger API using HDET request msg
06/03/13   spa     Migrate to new RF dispatch for XPT FW response mssgs 
05/09/13   spa     Added xpt config and DPD type as args to config_tx_pa_params
05/07/13   sty     Deleted rf_1x_mdsp_get_rx_agc()
05/01/13   jmf     Implement xPT IQ Capture SWI change
04/23/13   APU     Removed old unused iqgain_params and incoporated CC comments.
04/22/13   APU     Removed:
                   1>  the older unused _configure_tx_linearizer() function.
                   2>  the older unused _get_iqgain_params() function.
04/22/13   jmf     Implement xPT processing Enum Type
04/12/13   aro     Unified LNA calibration setup interface
04/12/13   aro     Added AGC ACQ down counter in RxAGC data
04/12/13   aro     Optimized RxAGC query function
04/01/13   spa     Migrated to RF Dispatch for RF-FW synchronization
03/28/13   zhw     Clean up obsolete pa_range and apt_val FW interface
03/21/13   zhw     Move to Antenna Tuner Open Loop common module
03/21/13   shb     Fixed compiler error due to merge issue
03/21/13   shb     Updated to supported gated(WTR1625) & non-gated(WTR1605) HDET
03/14/13   zhw     Ant tuner token FW interface support
02/26/13   Saul    CDMA. Set EPT switch-point to 42 dBm when ET is enabled
02/22/13   Saul    CDMA. Config xpt mode based on dpd info in linearizer.
02/21/13   spa     Add API to send pilot meas stop stream to FW (FTM IRAT)
02/08/13   Saul    CDMA. Set XPT mode with EPT/ET only when such mode is enabled.
01/30/13   zhw     Pass in high/low band info for FW TxAGC logging
01/28/13   APU     Implemented rfm_1x_get_filtered_rx\tx_power() APIs.
01/22/13   aro     Renamed IQ capture function to XPT
01/22/13   aro     Added common XPT capture type
01/18/13   aro     Deleted nikel-XPT specific code
11/30/12   spa     Updated configure HDET API for Dime
12/19/12   Saul    CDMA. TX AGC XPT Dev. Use xpt cfg msg in online.
12/06/12   APU     Replaced LM chains with Modem chains.
11/16/12   Saul    Sample Capture. Use common response.
11/08/12   cd      Added API to configure xPT
10/16/12   sty     Deleted refs to QPA
10/02/12   zhw     Support for PA state setting for SSBI/RFFE PA
09/28/12   zhw     Remove unused config_rf_on_off API
09/26/12   zhw     Configure TxAGC with Template ID from MC layer
09/25/12   zhw     Refactor configure tx agc API
09/20/12   zhw     PA scripting support
09/19/12   Saul    CDMA Sample Capture. Capture type, do leakage flag, 
                   stop tx agc update fw command
09/12/12   spa     Removed featurization flag RF_HAS_CDMA_DIME_SUPPORT 
09/12/12   spa     Removed unused API rf_1x_mdsp_configure_analog_script
09/07/12   aro     Renamed the DPD expansion function
09/07/12   aro     Added individual dispatcher function for each UMID
09/06/12   aro     FWRSP registration for IRAT_RX_START response
09/05/12   aro     FWRSP registration for IC_UPDATE and EPT IQ Capture command
09/05/12   aro     Interface to indicate DPD coeff expansion
09/05/12   aro     Added RF-FW Sync callback parameter in MDSP Interface
08/24/12   cd      CDMA EPT Temp and HDET feature support
08/22/12   cri     EPT support for CDMA FW interfaces
08/17/12   aro     Added RF-FW Sync support on FWS Config comamnd
08/16/12   aro     Generic FW response handler function
08/16/12   aro     Added common function to extract sequence number from
                   MSGR Payload
08/15/12   zhw     Added RF event handle in rf_cdma_data, which are used to 
                   control CCS event flow
08/16/12   aro     Handler functions to handle fw responses
08/08/12   adk     Added support for scripting buffer interface.
07/11/12   shb     Updated script_buffer to be of rf_buffer_intf* type 
                   instead of void ptr
07/06/12   spa     Update rf_1x_mdsp_configure_analog_script for rf_hal_buffer
06/26/12   aro     Deleted unused parameter from EPT configure function
06/26/12   aro     Deleted IQ Capture init function
06/13/12   aro     Implemented DPD coefficient interface
06/13/12   aro     Renamed function to read DPD coeff
06/13/12   aro     Update EPT configure, iq capture, and override functions
                   to have devices
06/13/12   aro     Removed unrelevant fields from EPT payloads
06/12/12   Saul    CDMA. Made static events non-atomic.
06/12/12   Saul    CDMA. Providing RF ONOFF event info to FW.
06/11/12   aro     Added MDSP path/device to EPT configure function
06/11/12   aro     Removed redundant EPT IQ Capture trigger function
06/08/12   aro     Updated EPT IQ capture interface to pass device
06/06/12   Saul    CDMA. Sending scripts as Dime shared mem format.
                   MC now using mdsp events.
06/05/12   anr     Initial EPT Support
04/11/12   Saul    QFE. Initial support.
04/03/12   aro     Updated FWS config function to perform SemWait in MC
04/03/12   aro     Updated FWS config function to manage state transition in
                   FTM
04/03/12   aro     CB functions in TX_START and TX_STOP interfaces
04/03/12   aro     CB functions in RX_STOP interface
04/03/12   aro     Deleted unused param from RX_START command
04/02/12   aro     Migrated RX_START synchronization using callback function
04/02/12   aro     Moved RxAGC convergence to a separate function to be called
                   from RX_START callback function
04/02/12   aro     Added CB as argument in Rx / Tx MDSP interfaces
04/02/12   aro     Added CB as argument in Rx MDSP interfaces
03/29/12   sty     deleted rf_1x_mdsp_get_device_buffer_index
03/28/12   aro     Added callback function as a parameter of RxAGC resume
                   function
03/28/12   aro     Merged the ic_update and rx_agc_resume interfaces
03/08/12   cd      Provide FW with GTR Thresh NV and GRFC information
03/08/12   hdz     Added rf_1x_mdsp_tx_freq_offset_adjust()
02/29/12   cd      Added rf_1x_mdsp_config_tx_pa_params()
02/15/12   spa     Changed rf_1x_mdsp_configure_rx_agc return type to boolean 
                   from void 
02/06/12   hdz     Added rf_1x_mdsp_resume_rx_agc()
01/23/12   shb     Added rf_1x_mdsp_reset_hdet_loop()
01/17/12   sty     Added prototype for rf_1x_mdsp_trigger_dyn_notch_update()
                   and rf_1x_mdsp_trigger_freq_offset_update()
01/20/12   shb     Updated CDMA device interface to use generic 
                   rfdevice_ssbi_write_type and rfdevice_script_type instead of
                   CDMA specific structures
12/22/11   adk     Implemented DAC_0 calibration in the 1x mode
12/05/11   aro     Added interface to reconfigure RxAGC from ACQ to TRK
12/01/11   cd      1x FW interface data structure for APT adjust values 
11/16/11   aro     Renamed FW state change function
11/10/11   aro     Upadated RF-MDSP interfaces to pass semaphore to 
                   wait on for blocking calls
11/10/11   aro     Defined CDMA semaphore as generic CDMA type
11/04/11   cd      Add FW flag to Tx AGC config message to indicate cal/non-cal
                   mode
11/03/11   cd      Removed unwanted header file
10/28/11   hdz     Added rf_1x_mdsp_get_therm_read_data 
10/28/11   Saul    IRAT update. Antenna Tuner support.
10/20/11   adk     Updated MDSP buffer index assignments.
10/03/11   sty     Added acq_down_ctr member to rf_1x_mdsp_rx_agc_read_type
09/27/11   aro     Updated Tx DP IQ Gain update function to have txlm_handle
                   and return value
09/07/11   aro     Updated RX_STOP interface to have offline QPCH flag
09/06/11   aro     Added interfaces Stop RxAGC and Update Intelliceiver
08/30/11   aro     Added function to set Tx datapath IQ Gain
08/24/11   shb     Added rf_1x_mdsp_configure_tx_power_limit_dbm640
08/05/11   shb     Added MDSP interface APIs for HDET algorithm
08/04/11   aro     Added interface to stop RxAGC
07/30/11   aro     Fixed the type of apt_val for Tx AGC override interface
07/28/11   Saul    IRAT updates. 
                   - Now sending rx start config message to fw.
                   - Re-store AGC ACCUM
07/25/11   Saul    IRAT updates. 
                   - Now sending pilot_meas_cfg message.
                   - GRFC start delta sent in cx8 units.
                   - Override of debug flags done in FTM meas APIs.
07/23/11   Saul    Converted rx wake up APIs to use GRFC generate script.
07/19/11   Saul    Combined FW enable/disable APIs into one FW config state API.
07/18/11   aro     Updated RxAGC Config interface to pass AGC Mode Param
07/05/11   bmg     Changed rf_1x_mdsp_configure_tx_power_limit() to take
                   turnaround constant and min/max power in dBm10.
06/16/11   adk     Updates to provide buffer_index to FW with SBI scripts.
06/10/11   aro     Added return val for 1x MDSP init
04/26/11   aro     Removed Enable/Disable Tx interfaces
04/25/11   aro     Added Tx Start and Tx Stop Messages
04/12/11   aro     Added Rx Start and Stop Interfaces
04/07/11   aro     Updated argument type for APT Value
03/31/11   aro     Added Clear LNA state override interface
03/24/11   aro     Defined unique RxAGC Read structure for 1x
03/16/11   aro     Changed Device Param to MDSP Chain
03/16/11   aro     Interface name update
03/16/11   aro     Renamed MDSP types Header file
03/16/11   aro     Removed unused typedef
03/16/11   aro     Doxygen Grouping Update
03/16/11   aro     [1] Added Analog Script Programming Interface
                   [2] Cleaned up Override Interface + added device argument
03/15/11   aro     Added consolidated TxAGC Read Interface
03/15/11   aro     Deleted unused interface
03/15/11   aro     Added support for consolidated Tx Linearizer
03/15/11   aro     Consolidated Tx configuration to PA STM and TX
                   Linearizer interfaces
03/14/11   aro     Consolidated RxAGC Reads to one data structure
03/14/11   aro     [1] Merged Set DVGA/LNA Offset to a unified interface
                   [2] Merged the DVGA and LNA offset to Gain Calibration Data
                   [3] Deleted unused interfaces
03/14/11   aro     [1] Renamed init interface
                   [2] Added FW disable interface
03/14/11   sty     Deleted rf_1x_mdsp_get_linearizer_data
03/13/11   sty     Changed args in rf_1x_mdsp_config_lut
03/10/11   sty     Deleted rf_1x_mdsp_config_lut_v2()
                   Added Turnaround constant as arg to rf_1x_mdsp_config_lut()
03/09/11   aro     Removed unused intefaces
03/08/11   sty     Added prototype for rf_1x_mdsp_config_lut_v2
                   Refactored few of rf1x_mdsp* APIs to rf_1x_mdsp*
03/07/11   sty     Refactored few of rf1x_mdsp* APIs to rf_1x_mdsp*
03/02/11   aro     Re-structured the RxAGC Param type
03/02/11   sty     Renamed rf1xhdr_* to rf_cdma_*
03/02/11   sty     Renamed rf1xhdr_mdsp_pa_param_type to 
                   rf_cdma_mdsp_pa_param_type
02/17/11   aro     Moved LNA Gain Step definition to RFM
02/15/11   aro     Moved set DVGA and LNA offset interface to MDSP
02/04/11   sty     [1] Renamed rf_1x_mdsp_activate_device_rx_tune to 
                   rf_1x_mdsp_push_rfdevice_script
                   [2] Removed rf_1x_mdsp_activate_rx_grfc_config
02/03/11   aro     Moved new Device interface to separate file
01/31/11   aro     Added LNA Override Interface
01/24/11   aro     Added LNA Range Query Interface
01/24/11   sty     Added temp API - rf1x_mdsp_config_lut_temp
                   Deleted rf_1x_mdsp_push_tx_agc_config
01/23/11   aro     Refined 1x wakeup Rx sequence
01/20/11   sty     Added rf1x_mdsp_config_sbi_param as arg to 
                   rf1x_mdsp_config_sbi_param and rf_1x_mdsp_push_tx_agc_config
                   Deleted rf_1x_mdsp_tx_agc_param_type
01/19/11   aro     Renamed RF-MDSP filename
01/18/11   sty     Added Tx Config Load and Activate functions
01/18/11   aro     Changed Typename
01/06/11   aro     Added RxAGC Query function
01/04/11   aro     Added RxAGC Config Load and Activate functions
12/30/10   aro     Added GRFC Script Push and Activate functions
12/29/10   aro     Updated RF-MDSP function to configure 1x RxAGC 
12/29/10   aro     Added interface to configur 1x RxAGC
12/23/10   aro     Added Device Script functions
07/16/10   zg      Added support for SSMA.
06/28/10   aro     Added rf1x_mdsp_config_notch_filter_freq()
06/16/10   aro     Clean up unused function
06/15/10   aro     Update Tx Override Function
06/10/10   aro     Cleaned up old RF-MDSP Interface
05/24/10   aro     Changed the Param Definition for the fucntion
                   rf1x_mdsp_config_cxn_to_fadc_ratio()
05/21/10   aro     Added SMEM Tx Limit Read Interfaces
05/16/10   aro     Fixed the type of betap override value
05/15/10   aro     Added Recover Normal Feature
05/13/10   aro     Changed the LUT update interface to use Loop/Latch
05/10/10   aro     Changed Function name
05/04/10   aro     Added Temporary Feature for transition to new Interface
05/04/10   aro     [1] Renamed PA SMPS Variable Name
                   [2] Added Extern C
                   [3] Added SMEM Read Interfaces
                   [4] Created Separate Override and Clear Override Functions
04/20/10   aro     Renamed funtion argument names
04/01/10   aro     [1] Added New 1x RF-MDSP Interface - Framework Only
                   [2] Doxygen Documentation Update
02/20/10   aro     [1] Deleted other rf1x_mdsp.h filr and Moved the MDSP
                   functionality to this file
                   [2] Cleaned up the file with unwanted commented code
01/28/10   aro     Added rf_cdma_mdsp_set_beta_scaling_pdm_pwr_val() func
01/04/10   anr     Remove dummy rf struct definitions
12/02/09   aro     [1] Added Functions to Set Port/Reg Address and Beta Scaling 
                   [2] Renamed the struct type
09/18/09   ans     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "rfcom.h"
#include "rf_cdma_mdsp_types.h"
#include "rf_cdma_mdsp.h"
#include "rf_cdma_lna.h"
#include "rfdevice_cdma_interface.h"
#include "rfm_gain_state_types.h"
#include "rfm_cdma_carrier_types.h"
#include "rfm_mode_types.h"
#include "rf_1x_agc_types.h"
#include "rfm_cdma_fw_types.h"
#include "rfc_common.h"
#include "rfcommon_core_xpt.h"
#include "rf_cdma_mdsp_ccs_events.h"
#include "rf_fwrsp_task.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to used to return 1x RxAGC Data read
*/
typedef struct
{

  int16 rx_agc;
  /*!< Current RxAGC value */

  uint8 lna_state;
  /*!< Current LNA State */

  int32 acq_down_ctr; 
  /*!< AGC acquisition down counter in units of 64 chips
     The AGC moves to track mode (i.e, RxAGC has settled)  when counter = 0 */

} rf_1x_mdsp_rx_agc_read_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold 1x APT Adjust data to program to FW
*/
typedef struct
{
  rf_cdma_mdsp_tx_iq_gain_data_type rc[RF_CDMA_MDSP_TX_IQGAIN_1X_NUM];

} rf_1x_mdsp_tx_apt_adj_type;

/*----------------------------------------------------------------------------*/
boolean 
rf_1x_mdsp_init
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_ftm_configure_fw_state
(
  rfm_cdma_fw_state_type fw_state,
  const rf_cdma_mdsp_response_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_pilot_meas_cfg
( 
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_response_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_pilot_meas_stop_stream
( 
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_response_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_configure_rx_agc
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rf_cdma_mdsp_rx_agc_config_type *rxagc_data,
  const rf_1x_agc_param_type* const acq_mode_data,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_configure_rx_agc_acq
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  uint16 acq_duration,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_start_rx
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_response_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_start_rx_irat
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_response_cb_param_type *cb_param
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_start_rx_config
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rf_cdma_mdsp_rx_start_type *cfg_start,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_stop_rx
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rf_cdma_mdsp_rx_stop_type *cfg_stop,
  boolean is_oqpch_mode,
  const rf_cdma_mdsp_response_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_stop_rx_agc
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_resume_rx_agc
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rf_cdma_mdsp_ic_update_type *cfg_start,
  const rf_cdma_mdsp_response_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_configure_tx_agc
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rf_cdma_mdsp_pa_stm_config_type *pa_stm_data,
  rf_cdma_mdsp_tx_mode_type tx_agc_mode,
  rf_cdma_mdsp_tx_gtr_thresh_type tx_thresh_params,
  boolean is_high_band,
  xpt_cfg_type xpt_cfg,
  int16 K,
  rf_cdma_mdsp_xpt_config_type *xpt_config_params,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_update_tx_linearizer
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_configure_tx_power_limit_db640
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  int16 min_pwr_db640,
  int16 max_pwr_db640,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_start_tx
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rf_cdma_mdsp_tx_start_type *cfg_start,
  const rf_cdma_mdsp_response_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_stop_tx
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rf_cdma_mdsp_tx_stop_type *cfg_stop,
  const rf_cdma_mdsp_response_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_lna_state_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  lm_handle_type rxlm_handle,
  rfm_lna_gain_state_type lna_state,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_lna_state_clear_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_setup_lna_calibration
(
  lm_handle_type rx_handle,
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rfm_lna_gain_state_type gain_state,
  int16 gain_val,
  uint16 acq_duration_us
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_rx_gain_override
(
  lm_handle_type rx_handle,
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rfm_lna_gain_state_type gain_state,
  int16 gain_val,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_tx_closed_loop_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  int16 tx_cl_val,
  boolean recover_normal,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_tx_closed_loop_clear_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_tx_open_loop_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  int16 tx_ol_val,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_tx_open_loop_clear_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_tx_agc_output_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  uint32 tx_agc_adj_val,
  int16 betap_gain_val,
  uint16 pa_state_val,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_tx_agc_output_clear_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_pa_state_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  uint8 pa_state_val,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_pa_state_clear_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_pa_range_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  uint8 pa_range_val,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_pa_range_clear_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_set_tx_iq_gain
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain ,
  lm_handle_type txlm_handle ,
  uint32 modem_chain ,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback ,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
void
rf_1x_mdsp_get_rx_agc_data
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rf_1x_mdsp_rx_agc_read_type *rx_agc_data
);


void
rf_1x_mdsp_get_tx_agc_logical_data
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  lm_handle_type rflm_handle ,
  rf_cdma_mdsp_tx_agc_logical_read_type *tx_agc_data
);

/*----------------------------------------------------------------------------*/
void
rf_1x_mdsp_get_tx_agc_physical_data
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  lm_handle_type rflm_handle,
  rf_cdma_mdsp_tx_agc_physical_read_type *tx_agc_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_get_hdet_read_data
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rf_cdma_mdsp_hdet_read_data_type *hdet_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_get_therm_read_data
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rf_cdma_mdsp_therm_read_data_type *therm_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_trigger_hdet_read
(
  uint16 txagc_freeze_t_us,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
uint32 
rf_1x_mdsp_get_mem_pool_base_addr
(
	rf_cdma_mdsp_chain_enum_type rx_mdsp_path
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_trigger_tuner_cl_read
(
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_reset_hdet_loop
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_configure_dac_cal
(
  uint16 dac_cal_ant0,
  uint16 dac_cal_ant1,
  uint32 txlm_buffer_idx,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);



boolean
rf_1x_mdsp_trigger_dyn_txr_iq_dac_update
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  lm_handle_type txlm_handle,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);


/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_trigger_dyn_notch_update
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  lm_handle_type rxlm_handle,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_trigger_freq_offset_update
(
  uint32 freq_offset,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_tx_freq_offset_adjust
(
  int16 freq_offset,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_xpt_capture
(
  boolean  first_trig,
  boolean  last_trig,
  uint8 capt_buf_id,
  uint8 proc_buf_id,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
void
rf_1x_mdsp_handle_firmware_response
(
  rf_fwrsp_cmd_type* req_ptr,
  struct rf_fwrsp_umid_info_type_t *umid_info
);

/*----------------------------------------------------------------------------*/
dpd_poly_coeff_type
rf_1x_mdsp_get_dpd_cal_data
(
  uint16 dpd_index
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_stop_txagc_update
(
  boolean stop_tx_agc_update
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_configure_xpt
(
  rf_cdma_mdsp_xpt_config_type *xpt_config_data,
  boolean is_cal_mode,
  const rf_cdma_mdsp_response_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_xpt_delay_update
( 
   int32 et_delay
);

/*----------------------------------------------------------------------------*/
void
rf_1x_mdsp_get_filtered_tx_power
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain ,
  int16* filtered_tx_power ,
  int16* coex_tx_power 
) ;

/*----------------------------------------------------------------------------*/
void
rf_1x_mdsp_get_filtered_rx_power
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  int16* filtered_rx_power
) ;

/*----------------------------------------------------------------------------*/
void
rf_1x_mdsp_handle_rx_start_rsp
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_1x_mdsp_handle_rx_stop_rsp
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_1x_mdsp_handle_tx_start_rsp
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_1x_mdsp_handle_tx_stop_rsp
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_1x_mdsp_handle_fws_config
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_1x_mdsp_handle_ic_update_rsp
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_1x_mdsp_handle_irat_rx_start_rsp
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);
/*----------------------------------------------------------------------------*/
void
rf_1x_mdsp_handle_pilot_meas_config_rsp
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_1x_mdsp_handle_pilot_meas_stop_stream
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_1x_mdsp_handle_xpt_config
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_1x_mdsp_handle_xpt_trans_complete
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_1x_mdsp_handle_iq_cap_rsp
(
  rf_fwrsp_cmd_type* req_ptr,
  struct rf_fwrsp_umid_info_type_t *umid_info
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_register_fw_msgs
( 
  void
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_deregister_fw_msgs
( 
  void
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_stop_oqpch_search
( 
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RF_1X_MDSP_H */

