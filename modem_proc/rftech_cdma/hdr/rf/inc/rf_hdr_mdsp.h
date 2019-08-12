#ifndef RFHDR_MDSP_H
#define RFHDR_MDSP_H
/*!
  @file
  rfhdr_mdsp.h

  @brief
  This module defines the function prototypes and defintions used by HDR RF-MDSP
  Internal(RF) interface.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/rf/inc/rf_hdr_mdsp.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/17/15   vr      Added dynamic txr_iq_dac setting update support
10/07/14   vr      ftm iq capture changes
05/02/14   spa     Get IQ offset based on MDSP chain and not device
04/11/14   spa     Move jammer detect algo support to CDMA MDSP 
04/01/14   APU     PA params update hook-up for different RC/data rate configs.
03/28/14   APU     Hook up Tx logging interfaces in SW driver to log data 
                   provided by RFLM.
03/20/14   spa     Initial support to test Rx on/off on RFLM side
03/10/14   spa     Add Stop TxAGC MDSP API to freeze/unfreeze TxAGC on RFLM side
02/25/14   JJ/fh   Updated DAC bit length to support both DIMEPM and BOLT
02/18/14   cd      Provide baseband gain values to RFLM for APT and ET modes
01/15/14   JJ      Added api tp get mempool addr from FW shared memory for MCDO
01/08/14   cd      Provide xPT type to RFLM per PA state
12/13/13   sty     deleted refs to template_id
12/09/13   JJ      Pass dac_rate to rf_hdr_mdsp_configure_tx_freq_offsets()
11/08/13   cd      Remove support for legacy FW interfaces
08/16/13   ndb     Clean-up the SPI based antenna tuner support
07/19/13   Saul    XPT. ET delay vs freq vs temp support.
06/26/13   zhw     Tuner CL support. Removed unused HDET config msg
06/18/13   JJ      Added functions to register/de-register FW MSG responses
06/13/13   zhw     Added Tuner CL meas trigger API using HDET request msg
06/03/13   spa     Migrate to new RF dispatch for XPT FW response mssgs 
04/23/13   APU     Removed old unused iqgain_params and incoporated CC comments.
04/19/13   APU     Removed:
                   1>  the older unused _configure_tx_linearizer() function.
                   2>  the older unused _get_iqgain_params() function.
04/14/13   APU     Changed the signature of  get_filtered_tx_power API to be 
                   able to query different kind of filters.
04/05/13   APU     Hooked up the get_filtered APIs to FW interface.           
04/05/13   spa     Use RF dispatch for HDR Rxagc tracking and logging response
04/01/13   spa     Migrated to RF Dispatch for RF-FW synchronization
03/27/13   ry      Added interface for new WBAGC JD APIs
03/21/13   zhw     Move to Antenna Tuner Open Loop common module
03/21/13   shb     Updated to supported gated(WTR1625) & non-gated(WTR1605) HDET
03/14/13   zhw     Ant tuner token FW interface support
02/26/13   Saul    CDMA. Set EPT switch-point to 42 dBm when ET is enabled
02/22/13   Saul    CDMA. Config xpt mode based on dpd info in linearizer.
02/21/13   spa     Add API to send pilot meas stop stream to FW (FTM IRAT)
02/19/13   spa     Added API to send FLL XO config mssg (FTM IRAT)
02/08/13   Saul    CDMA. Set XPT mode with EPT/ET only when such mode is enabled.
09/17/12   APU     Take neq power in deciding the HDET power mode. 
01/30/13   zhw     Pass in high/low band info for FW TxAGC logging
01/28/13   APU     Implemented rfm_hdr_get_filtered_rx\tx_power() APIs.
11/30/12   spa     Updated HDET API for Dime
12/19/12   Saul    CDMA. TX AGC XPT Dev. Use xpt cfg msg in online.
12/06/12   APU     Replaced LM chains with Modem chains.
10/16/12   sty     Removed all refs to qpa_params
10/02/12   zhw     Support for SSBI / GRFC based PA
09/28/12   zhw     Remove unused config_rf_on_off API
09/26/12   zhw     Configure TxAGC with Template ID from MC layer
09/25/12   zhw     Refactor configure tx agc API
09/21/12   zhw     Support for PA RFFE Scripting
09/12/12   spa     Removed featurization flag RF_HAS_CDMA_DIME_SUPPORT 
09/12/12   spa     Removed unused API rf_hdr_mdsp_configure_analog_script
09/10/12   aro     Added HDR RxAGC logging using RF-FW Sync
09/07/12   aro     Added individual dispatcher function for each UMID
09/06/12   aro     FWRSP registration for IRAT_RX_START response
09/06/12   aro     FWRSP registration for ACQ_2_TRK_IND response
09/05/12   aro     Added RF-FW Sync callback parameter in HDR MDSP Interface
08/24/12   cd      CDMA EPT Temp and HDET feature support
08/22/12   cri     EPT support for CDMA FW interfaces
08/16/12   aro     Generic FW response handler function
08/16/12   aro     Added common function to extract sequence number from
                   MSGR Payload
08/15/12   zhw     Added RF event handle in rf_cdma_data, which are used to 
                   control CCS event flow
08/08/12   adk     Added support for scripting buffer interface.
07/11/12   shb     Updated script_buffer to be of rf_buffer_intf* type 
                   instead of void ptr
07/09/12   spa     Compile error fix in rf_hdr_mdsp_configure_analog_script()
07/06/12   spa     Update rf_hdr_mdsp_configure_analog_script for rf_hal_buffer
06/12/12   Saul    CDMA. Made static events non-atomic.
06/12/12   Saul    CDMA. Providing RF ONOFF event info to FW.
06/06/12   Saul    CDMA. Sending scripts as Dime shared mem format.
                   MC now using mdsp events.
04/18/12   aro     Moved FWS transition logic in upper layer
04/18/12   aro     Callback function for Rx/Tx Start/Stop interfaces
04/18/12   aro     Added callback function as parameter to override functions
04/11/12   Saul    QFE. Initial support.
04/04/12   aro     Added callback functions to HDR Tx interfaces
04/03/12   aro     callback function for HDR MDSP Rx interfaces
04/02/12   aro     Migrated RX_START synchronization using callback function
04/02/12   aro     Moved RxAGC convergence to a separate function to be called
                   from RX_START callback function
03/29/12   sty     Deleted rf_hdr_mdsp_get_device_buffer_index
03/28/12   aro     Merged the ic_update and rx_agc_resume interfaces
03/28/12   aro     Added callback function as a parameter of RxAGC resume
                   function
03/21/12   cd      Functionality to extract Rx AGC logging data from SMEM
03/08/12   cd      Provide FW with GTR Thresh NV and GRFC information
02/29/12   cd      Added rf_hdr_mdsp_config_tx_pa_params()
02/15/12   spa     Changed rf_hdr_mdsp_configure_rx_agc return type to boolean 
                   From void  
02/06/12   hdz     Added rf_hdr_mdsp_resume_rx_agc()
01/20/12   sty     Added prototype for rf_hdr_mdsp_dyn_notch_update()
01/20/12   shb     Updated CDMA device interface to use generic 
                   rfdevice_ssbi_write_type and rfdevice_script_type instead of
                   CDMA specific structures
12/01/11   cd      HDR FW interface data structure for APT adjust values  
12/02/11   hdz     Added rf_hdr_mdsp_get_therm_read_data()
11/16/11   aro     Renamed FW state change function
11/10/11   aro     Upadated RF-MDSP interfaces to pass semaphore to 
                   wait on for blocking calls
10/28/11   Saul    IRAT update. Antenna Tuner support.
10/20/11   adk     Updated MDSP buffer index assignments.
09/27/11   aro     Added MSGR message to update TXC DP IQ Gain
09/12/11   cd      Updated support to set/clear Tx open loop override in FW
09/06/11   aro     Added interfaces Stop RxAGC and Update Intelliceiver
08/30/11   shb     Added rf_hdr_mdsp_configure_tx_power_limit_db640()
08/05/11   shb     Added MDSP interface APIs for HDET algorithm
07/28/11   Saul    IRAT updates. 
                   - Now sending rx start config message to fw.
                   - Re-store AGC ACCUM
07/25/11   Saul    IRAT updates. 
                   - Now sending pilot_meas_cfg message.
                   - GRFC start delta sent in cx8 units.
                   - Override of debug flags done in FTM meas APIs.
07/23/11   Saul    Converted rx wake up APIs to use GRFC generate script.
07/19/11   Saul    Combined FW enable/disable APIs into one FW config state API.
07/05/11   bmg     Changed rf_hdr_mdsp_configure_tx_power_limit() to take
                   turnaround constant and min/max power in dBm10.
06/17/11   Saul    Now configuring Tx carrier freq offsets and mask for MCDO
06/16/11   adk     Updates to provide buffer_index to FW with SBI scripts.
06/16/11   sar     Included internal rf hdr interface.
06/10/11   aro     Added return val for HDR MDSP init
06/07/11   Saul    Updates for multi-carrier driver.
05/24/11   aro     Added Missing function prototype
04/26/11   aro     Removed Enable/Disable Tx interfaces
04/25/11   aro     Added Tx Start and Tx Stop Messages
04/12/11   aro     Renamed Enable/Disable FW Interface
04/11/11   aro     [1] Added Rx Stop interface
                   [2] Deleted Stop RxAGC Interface
04/11/11   aro     Renamed Start rx interface
04/08/11   aro     Added interface for RxAGC Start
04/01/11   aro     Removed TEMP Interface
03/31/11   aro     Re-designed MDSP override Interfaces
03/31/11   aro     Added TxAVGA Index Read Interface
03/24/11   aro     Unified the RxAGC Read Interface
03/24/11   aro     Added Analog script Programming script and re-arranged
                   doxygen block
03/24/11   aro     [1] Re-designed Configure TxAGC Interface
                   [2] Deleted unused interfaces
03/24/11   aro     [1] Re-factored Enable FW function
                   [2] Re-designed COnfigure RxAGC Interface
                   [3] Compiler Warning Removal
03/24/11   sty     [1] Replaced rfhdr_mdsp_config_lut and rfhdr_mdsp_latch_lut 
                       with rf_hdr_mdsp_configure_tx_linearizer()
                   [2] C++ protection added
03/16/11   aro     Renamed MDSP types Header file
03/14/11   aro     Renamed RxAGC structure
03/09/11   aro     Renamed Config RxAGC function name
03/02/11   aro     Re-structured the RxAGC Param type
03/02/11   sty     Renamed rf1xhdr_* to rf_cdma_*
03/02/11   sty     Renamed rf1xhdr_mdsp_pa_param_type to 
                   rf_cdma_mdsp_pa_param_type
02/04/11   sty     Added rf_hdr_mdsp_push_rfdevice_script instead of 
                   rf_hdr_mdsp_activate_device_rx_tune and 
                   rf_hdr_mdsp_activate_device_tx_tune
02/04/11   sty     Added rf_hdr_mdsp_activate_device_tx_tune
02/03/11   sty     Added rfhdr_mdsp_config_lut_temp
02/03/11   aro     Moved new Device interface to separate file
01/24/11   aro     Added LNA Range Query Interface
01/19/11   aro     Removed MDSP Push Interface for Device
01/19/11   aro     Added LNA Config and RxAGC activate interfaces
01/19/11   aro     Renamed RF-MDSP filename
01/18/11   aro     Added RxAGC, Device Tune, and GRFC Script functions
01/18/11   sty     Renamed rf1xhdr_mdsp_tx_sbi_param_type to 
                   rf_cdma_mdsp_tx_sbi_param_type
12/10/10   aro     [1] Replaced with RFM layer Enums for AGC Modes
                   [2] Enabling SVDO Flavor
11/17/10   zg      Updated arguments in rfhdr_mdsp_enter_rx_agc_acq_mode(). 
08/21/10   aro     Removed unused function
08/19/10   pl      Added Support for IRAT RxAGC
07/30/10   hp      Added support for HDR Tx Open Loop Correction
07/16/10   zg      Added support for SSMA.
07/07/10   aro     Added Interface to set RxAGC Gain
06/28/10   aro     Added rfhdr_mdsp_config_notch_filter_freq()
06/15/10   aro     Added Interface to configure FLL
05/24/10   aro     Changed the Param Definition for the fucntion
                   rfhdr_mdsp_config_cxn_to_fadc_ratio()
05/13/10   aro     Changed Argument type + include file
05/11/10   aro     [1] Separated Set/CLear Override Interfaces
                   [2] Changes LUT Interface to operate in Loop
                   [3] Minor Argument Changes
05/07/10   aro     Renamed funtion argument names
05/07/10   aro     Renamed funtion argument names
04/12/10   aro     RF-MDSP Interface : Corrected Function name
04/01/10   aro     [1] Added New HDR RF-MDSP Interface - Framework Only
                   [2] Doxygen Documentation Update
01/24/10   aro     Initial Revision : Added the Call to L1 to enable HDR FW App

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_cdma_mdsp_types.h"
#include "rf_cdma_mdsp.h"
#include "rfdevice_cdma_interface.h"
#include "rfm_cdma_carrier_types.h"
#include "rfm_hdr.h"
#include "rfm_cdma_fw_types.h"
#include "rfc_common.h"
#include "rf_cdma_mdsp_ccs_events.h"
#include "rf_fwrsp_task.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to used to return HDR RxAGC Data read
*/
typedef struct
{

  int16 rx_agc[ RFM_CDMA_CARRIER_NUM + 1 ];
  /*!< Current RxAGC value for all carriers. Index 0,1,2 are used for
  Carrier RxAGC and Index 3 is used for Cumulative RxAGC */

  int16 rx_agc_min;
  /*!< RxAGC Min Value */

  int16 rx_agc_max;
  /*!< RxAGC Max Value */

  uint8 lna_state;
  /*!< Current LNA State */

} rf_hdr_mdsp_rx_agc_read_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold HDR APT Adjust data to program to FW
*/
typedef struct
{
  /*!
    @brief
    Gain adjustments for each reverse rate configuration.

    @details
    The rate adjustments are specified for the following rates in the
    respective array entries.
      -# DOr0 Pilot only
      -# DOr0 RRI0
      -# DOr0 RRI1
      -# DOr0 RRI2
      -# DOr0 RRI3
      -# DOr0 RRI4
      -# DOrA PS0
      -# DOrA PS1
      -# DOrA PS2
      -# DOrA PS3
      -# DOrA PS4
      -# DOrA PS5
      -# DOrA PS6
      -# DOrA PS7
      -# DOrA PS8
      -# DOrA PS9
      -# DOrA PS10
      -# DOrA PS11
      -# DOrA PS12
  */
  rf_cdma_mdsp_tx_iq_gain_data_type rc[RF_CDMA_MDSP_TX_IQGAIN_HDR_NUM];
} rf_hdr_mdsp_tx_apt_adj_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  rf_cdma_mdsp_chain_enum_type mdsp_chain ;
  int16 min_pwr_db640 ;
  int16 max_pwr_db640 ; 
  int16 hdet_temp_correction_db640 ;
  boolean is_enhanced_power_limiting_enabled ;
  boolean freq_sorted_lut_indicator[RFM_CDMA_CARRIER_NUM] ;
  const rf_cdma_tx_pwr_limit_data_type* hdr_max_power_limits ;
  rf_cdma_mdsp_cb_handler_type callback ;
  void* userdata ;
} rf_hdr_mdsp_max_power_capsule_t ;

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_init
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_ftm_configure_fw_state
(
  rfm_cdma_fw_state_type fw_state,
  const rf_cdma_mdsp_response_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_pilot_meas_cfg
( 
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_response_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback, 
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_fll_xo_cfg_msg
( 
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_pilot_meas_stop_stream
( 
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_response_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_configure_rx_agc
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_carrier_mode_type carrier_mode,
  rf_cdma_mdsp_rx_agc_config_type *rxagc_data,
  const rfm_hdr_agc_acq_type* const acq_mode_data,
  const rf_cdma_mdsp_response_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_start_rx
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_response_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_start_rx_irat
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_response_cb_param_type *cb_param
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_start_rx_config
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rf_cdma_mdsp_rx_start_type *cfg_start,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_stop_rx
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rf_cdma_mdsp_rx_stop_type *cfg_stop,
  const rf_cdma_mdsp_response_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_stop_rx_agc
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_resume_rx_agc
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rf_cdma_mdsp_ic_update_type *cfg_start,
  const rf_cdma_mdsp_response_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_configure_tx_agc
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rf_cdma_mdsp_pa_stm_config_type *pa_stm_data,
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
rf_hdr_mdsp_update_tx_linearizer
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_configure_tx_power_limit_db640
(
  const rf_hdr_mdsp_max_power_capsule_t max_pwr_capsule 
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_start_tx
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rf_cdma_mdsp_tx_start_type *cfg_start,
  const rf_cdma_mdsp_response_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_stop_tx
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rf_cdma_mdsp_tx_stop_type *cfg_stop,
  const rf_cdma_mdsp_response_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_configure_tx_freq_offsets
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  uint32 num_carriers,
  rfm_cdma_carrier_en_mask_type carrier_mask,
  const int32 * const freq_offset_hz,
  const uint32 dac_sampling_rate,
  const uint8 dac_bit_length,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_lna_state_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rfm_lna_gain_state_type lna_state,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_lna_state_clear_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_tx_closed_loop_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rfm_cdma_carrier_type carrier,
  int16 tx_cl_val,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean 
rf_hdr_mdsp_tx_closed_loop_clear_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rfm_cdma_carrier_type carrier,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_tx_open_loop_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rfm_cdma_carrier_type carrier,
  int16 tx_ol_val,
  int16 K10,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_tx_open_loop_clear_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rfm_cdma_carrier_type carrier,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_tx_agc_output_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  uint32 tx_agc_adj_val,
  int16 betap_gain_val,
  uint16 pa_state_val,
  uint32 apt_val,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_tx_agc_output_clear_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_pa_state_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  uint8 pa_state_val,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_pa_state_clear_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_pa_range_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  uint8 pa_range_val,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_pa_range_clear_override
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_set_tx_iq_gain
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain ,
  lm_handle_type txlm_handle  ,
  uint32 modem_chain ,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback ,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_configure_xpt
(
  rf_cdma_mdsp_xpt_config_type *xpt_config_data,
  const rf_cdma_mdsp_response_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_xpt_delay_update
( 
   int32 et_delay
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_mdsp_get_rx_agc_data
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rf_hdr_mdsp_rx_agc_read_type *rx_agc_data
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_mdsp_read_rx_log_buffer
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rf_hdr_mdsp_rx_agc_read_type *rx_agc_data,
  int16 index
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_mdsp_get_tx_agc_logical_data
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  lm_handle_type txlm_handle ,
  rfm_cdma_carrier_type carrier,
  rf_cdma_mdsp_tx_agc_logical_read_type *tx_agc_data
);

/*----------------------------------------------------------------------------*/
boolean 
rf_hdr_mdsp_get_funneling_metrics 
(
  rf_cdma_mdsp_chain_enum_type  mdsp_chain ,
  int16*                        funnel_max_power ,
  boolean*                      is_funneling_enabled
) ;

/*----------------------------------------------------------------------------*/
void
rf_hdr_mdsp_get_tx_agc_physical_data
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  lm_handle_type rflm_handle,
  rfcom_multi_carrier_id_type carrier,
  rf_cdma_mdsp_tx_agc_physical_read_type *tx_agc_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_get_hdet_read_data
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rf_cdma_mdsp_hdet_read_data_type *hdet_data
);

/*----------------------------------------------------------------------------*/
uint32 
rf_hdr_mdsp_get_mem_pool_base_addr
(
  const rfm_cdma_carrier_type carrier,
  const rf_cdma_mdsp_chain_enum_type rx_mdsp_chain
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_get_therm_read_data
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rf_cdma_mdsp_therm_read_data_type *temp_comp_data
);


/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_trigger_hdet_read
(
  uint16 txagc_freeze_t_us,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_trigger_tuner_cl_read
(
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_trigger_dyn_notch_update
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  lm_handle_type rxlm_handle,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_trigger_dyn_txr_iq_dac_update
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  lm_handle_type txlm_handle,
  const rf_cdma_mdsp_cb_param_type *cb_param,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_mdsp_handle_rx_start_rsp
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_mdsp_handle_rx_stop_rsp
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_mdsp_handle_tx_start_rsp
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_mdsp_handle_tx_stop_rsp
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_mdsp_handle_fws_config
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_mdsp_handle_rxagc_track_ind
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_mdsp_handle_rxagc_logging
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_mdsp_handle_ic_update_rsp
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_mdsp_handle_irat_rx_start_rsp
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_mdsp_handle_pilot_meas_config_rsp
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_mdsp_handle_pilot_meas_stop_stream
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_mdsp_handle_firmware_response
(
  rf_fwrsp_cmd_type* req_ptr,
  struct rf_fwrsp_umid_info_type_t *umid_info
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_mdsp_handle_xpt_config
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_mdsp_handle_xpt_trans_complete
(
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_mdsp_get_filtered_rx_power
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  rfm_cdma_carrier_type carrier,
  int16 filtered_rx_power[]
) ;

/*----------------------------------------------------------------------------*/
void
rf_hdr_mdsp_get_filtered_tx_power
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  int16* filtered_tx_power ,
  int16* coex_tx_power 
) ;

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_enable_rxagc_logging
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain,
  const rf_cdma_mdsp_response_cb_param_type *cb_param
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_disable_rxagc_logging
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_register_fw_msgs
( 
  void
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_deregister_fw_msgs
( 
  void
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_query_iq_sample_line_offset
(
  const rfm_cdma_carrier_type carrier,
  const rf_cdma_mdsp_chain_enum_type mdsp_chain,
  uint32 *mempool_addr_line_offset
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_stop_txagc_update
(
  boolean stop_tx_agc_update
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_test_rx_on_off
( 
  uint32 force_microsleep_enable_disable,
  uint32 rf_off_ustmr_delta,
  uint32 rf_on_ustmr_delta,
  uint32 duration_half_slots,
  uint32 sleep_mode,
  const rf_cdma_mdsp_cb_handler_type callback,
  void* const userdata
);

#endif /* FEATURE_CDMA1X */

#ifdef __cplusplus
}
#endif

#endif /* RFHDR_MDSP_H */
