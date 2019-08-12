#ifndef FTM_CDMA_DATA_H
#define FTM_CDMA_DATA_H

/*! 
  @file
  ftm_cdma_data.h
 
  @brief
  This file contains prototypes and definitions to be used by Centralized 
  FTM Global Data.
*/

/*==============================================================================

  Copyright (c) 2010 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/ftm/inc/ftm_cdma_data.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/10/15   vr      Use unique RxLM handle for XPT FB capture
11/07/14   dbz     [selftest] Add support for VSWR V2 capture and processing 
08/05/14   jmf     [SelfTest] Port Dime Defs for VSWR result return
07/07/14   zhw     Compiler fix for non-CDMA configuration
04/10/14   JJ      Changed ftm_cdma_data_1x_get_fw_smem_addr() return type
04/08/14   JJ      Add a new FTM command for timed sleep rx
12/26/13   sar/hdz Remove et_txlm_hanlde from Bolt c2k code, and use one single 
                   TxLm handle allocation.
11/11/13   JJ      Removed funcs to set/get fw smem addr
10/21/13   JJ      Added two funcs to set/get fw smem addr
09/06/13   ars     Added FTM interface for FTM_GET_MULTIPLE_RX_LEVEL_DBM command
08/27/13   aka     Added support for 1x self-test commands 
06/19/13   JJ      Removed two functions to check/update fw msg registration status
06/18/13   JJ      Added two functions to check/update fw msg registration status
05/31/13   JJ      Re-define FTM_CDMA_INVALID_BAND to RFM_CDMA_MAX_BAND
05/14/13   cc      Added data structures for holding IQ capture data and
                   RSP packet 
03/14/13   aro     Deprecated Composite Calibration and TxSweep Commands
01/21/13   nrk     Added aggregated multi synth lock status check and updates functions
12/19/12   cd      Add ET TxLM configuration support in FTM
12/04/12   zhw     FTM APT PA Q Current support  (interface cleanup)
08/06/12   hdz     Reverted adding ftm_cdma_tx_pwr_limit_data_type
07/24/12   hdz     Added ftm_cdma_tx_pwr_limit_data_type
07/02/12   cvd     Dynamically allocate memory to rf_cdma_data.rf_device_status
05/07/12   aa      Changes for Code Maintainability and data access 
03/22/12   aro     Deleted cdma_rf_task file
03/22/12   aro     Removed unsed functions and types
03/09/12   spa     Changed ftm_cdma_data_update_band_chan arguments to cdma band     
                   and chan type
03/02/12   aro     Added CDMA calibration debug mode variable
12/06/11   aro     Added infrastructure in CDMA Cal v3 to perform one PM cal
11/17/11   aro     Added new MCPM configure function with voting mechanism
11/16/11   aro     Added function to vote and unvote FW disabe
11/16/11   aro     Added centralized infrastruture to manage FW state change
11/15/11   aro     Updated cal v3 data structure with LNA offset param
11/15/11   aro     Renamed CDMA cal v3 data structure
10/31/11   aro     Added data structure for RF task handlers
09/09/11   shb     Use different macro for # of hdet reads (128) vs returns(32)
09/07/11   aro     Added data structure for HDR logging
09/01/11   cd      Add APT support
                   - Removed PA range functionality, replace with PA state
                   - Added variable to store APT bias in FTM data
08/24/11   aro     Added data accessor function to query CDMA log
06/13/11   aro     Documentation Update
06/09/11   aro     Added Calibration Sweep V3 Data structure and Accessor func
05/16/11   aro     Added MCPM state and accessor function
05/06/11   aro     Deprecated linearizer sweep functions
04/27/11   sty     Added prototypes for ftm_cdma_resolve_master_device() 
                   and ftm_cdma_data_update_chain_config()
04/14/11   sty     Added default band and chan #define to be used in OFS mode
04/07/11   aro     Re-sequenced the TxLinearizer Data fields
04/07/11   aro     Updated TxLinearizer Data Structure and Accessor function
04/07/11   aro     Fixed TxLinearizer data types
04/06/11   aro     Added Tx Linearizer Cal Data structure/ accessor func
03/29/11   sty     Added prototype for ftm_cdma_is_device_state_valid
03/24/11   aro     Renamed the structure for Q6 clock Speed voting
03/23/11   aro     Added Cal Instance variable for TxSweep
03/22/11   aro     Added Debug flag and cal instance to data structure
03/18/11   aro     Added CompositeCalibration debug flag
03/09/11   sty     Added #defines for min and max power
03/08/11   sty     Added #defines
03/07/11   sty     Added NPA-related timer vars  into ftm_cdma_timer_type
03/04/11   aro     Added device field in TxSweep Data structure to remove
                   the hardcoding of Device from TxSweep
03/04/11   aro     Doxygen warning fixed
03/03/11   aro     Added Composite Calibration Data Structure and Accessor func
03/03/11   aro     Removed device parameter for Tx Cal Data to reuse same
                   structure for multiple devices
03/03/11   sty     Doxygen comment
03/01/11   sty     Added pulse_created_for_tx_sweep to keep track if TX pulse 
                   was created during Tx Cal sweep
02/24/11   sty     Changed data type and size of hdet_read[]
                   Deleted unused FTM_CDMA_HDET_ARRAY_SIZE
02/22/11   sty     Added ftm_cdma_timer_type for time profiling
                   Renamed rfm_pa_r1_r0_type to rfm_pa_range_type
                   Added member tx_avga_idx to represent the TX LUT index
                   Added accessor API ftm_cdma_get_ftm_timer_ptr
02/17/11   sty     Use rfm_pa_r1_r0_type instead of rfm_pa_gain_state_type
02/17/11   sty     Renamed ftm_cdma_data_get_tx_sweep_adr to 
                   ftm_cdma_data_get_tx_sweep_ptr
02/16/11   sty     Added ftm_cdma_override_data_type as part of 
                   ftm_cdma_data_status_type
02/16/11   aro     Moved function to appropriate doxygen group
02/16/11   aro     Renamed function to ftm_cdma_data_invalidate_lm_buffers()
02/14/11   sty     Renamed ftm_rxlm_handle to rxlm_handle and ftm_txlm_handle 
                   to txlm_handle 
02/09/11   sty     [1] Added RxLM and TxLM buffer indices in 
                       ftm_cdma_data_status_type
                   [2] Added C++ protection
                   [3] Added ftm_cdma_data_get_device_ptr()
02/09/11   aro     Added per device field for RFCal Data
02/09/11   aro     Added Accessor function for RF Cal Data structure
02/09/11   aro     Added Calibration Data Structure
02/07/11   aro     Removed Dispatch Command Logging
02/02/11   aro     Added Device Status Init function
02/02/11   aro     Added FTM Command History Logging
02/01/11   aro     Promoted Power Mode types to RFM
01/31/11   aro     [1] Added Accessor Functions 
                   [2] Added FTM State Variables
01/15/11   aro     Updated FTM Data Structure to include RF Device Status
12/13/10   aro     Initial Revision.

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#else
#include "sys.h"
#endif
#include "ftm.h"
#include "rfm_device_types.h"
#include "rfm_gain_state_types.h"
#include "rfm_cdma_power_mode_types.h"
#include "ftm_cdma_state_types.h"
#include "rxlm_intf.h"
#include "txlm_intf.h"
#include "ftm_1x_log.h"
#include "ftm_hdr_log.h"
#include "ftm_1x_control.h"
#include "ftm_hdr_control.h"
#include "rfm_cdma_band_types.h"
#include "rfm_cdma_carrier_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Default Chan (-1) to be used for CDMA.*/
#define FTM_CDMA_INVALID_CHAN 65535

/*----------------------------------------------------------------------------*/
/*! Default Band to be used for CDMA.*/
#define FTM_CDMA_INVALID_BAND RFM_CDMA_MAX_BAND

/*----------------------------------------------------------------------------*/
#define FTM_CDMA_Q6SW_REQUEST_FREQ             595000 /*!< NPA Req freq (MHz) */

/*----------------------------------------------------------------------------*/
#define FTM_CDMA_TXAGC_RESOLUTION              (102.4/65536)
/*!< Conversion factor used when converting TxAGC(mdsp units) <=> TxAGC(dBm)  */

/*----------------------------------------------------------------------------*/
/*! Invalid power */
#define FTM_CDMA_INVALID_TX_MIN_PWR                                     -32768

/*----------------------------------------------------------------------------*/
/*! Invalid Max power */
#define FTM_CDMA_INVALID_TX_MAX_PWR                                      32767

/*----------------------------------------------------------------------------*/
/*! Default band to tune to in OFS mode */
#define FTM_CDMA_DEFAULT_OFS_BAND                                            0

/*----------------------------------------------------------------------------*/
/*! Default channel to tune to in OFS mode */
#define FTM_CDMA_DEFAULT_OFS_CHAN                                          300

/* ------------------------------------------------------------------------*/
/*! FTM IQ capture  macros */
/*! Maximum CDMA IQ Samples that can be captured  */
#define FTM_CDMA_IQ_MAX_SAMPLES 8192

/*! CDMA Sample server IQ format */
#define FTM_CDMA_IQ_SS_DATA_FORMAT FTM_IQ_8_BIT_FORMAT 

/*! CHIPX2 CDMA frequency */
#define FTM_CDMA_IQ_SAMPLE_FREQ_CHIPX2 (1.2288e6*2)

/*! This is the maximum number of CDMA IQ samples that can be sent */
/*! through the diag. */
/*! If diag requires more, it has to get it through multiple iterations */
/*! Through experimentation it is found to be 512 bytes: 256 samples */
/*! I (1 byte) + Q(1 byte)  */
#define FTM_CDMA_IQ_DIAG_MAX_SAMPLES (FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG/2)

/* *************************************************************/
/* ***************FTM FBRx IQ capture macros*****************/
/* *************************************************************/

// The CDMA FBRx IQ capture structure which stores all the relevent data when acquiring samples.

/*! Maximum FBRx CDMA IQ Samples that can be captured  */
#define FTM_CDMA_FBRX_IQ_SAMPLES_MAX 9216

/*! FBRx CDMA Sample server IQ format - Tx*/
#define FTM_CDMA_FBRX_IQ_SAMPLE_DATA_FORMAT_TX FTM_IQ_8_BIT_FORMAT 

/*! FBRx CDMA Sample server IQ format - Rx*/
#define FTM_CDMA_FBRX_IQ_SAMPLE_DATA_FORMAT_RX FTM_IQ_32_BIT_FORMAT

/*! CDMA FBRx IQ capture sampling frequency */
#define FTM_CDMA_FBRX_IQ_SAMPLE_FREQ (9.8304e6) /* specification */

/*! This is the maximum number of CDMA FBRx IQ samples that can be sent through the diag. */
/*! If diag requires more, it has to get it through multiple iterations */
#define FTM_CDMA_FBRX_IQ_DIAG_MAX_SAMPLES_TX (FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG/2)
#define FTM_CDMA_FBRX_IQ_DIAG_MAX_SAMPLES_RX ((FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG/2)/4)

/*----------------------------------------------------------------------------*/
/*! Typecasting since this type could be used by primary device as well - it 
    is not intiutive to use ftm_sec_chain_ctl_enum_type for device 0          */
typedef ftm_sec_chain_ctl_enum_type ftm_chain_config_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to keep track of overridden values (such as PA state, AGC, etc)
*/
typedef struct
{

  boolean pa_range_overriden;
  /*!< keeps track of whether R1_R0 lines are in override mode, in which 
  case a PA state override in FW will not take any effect */

  rfm_pa_range_type pa_range;
  /*!< keeps track of the current R1_R0 lines state */

  rfm_pa_gain_state_type pa_state;
  /*!< keeps track of the current FTM PA State */

  uint16 tx_avga_idx;
  /*!< The member stores the current Look-up-table index programmed in the RF 
  device. This will be updated when FTM_SET_PDM is called. This variable shall 
  NOT be updated when Tx Sweep is done.*/

  uint16 apt_smps_bias_val;
  /*!< keeps track of the current APT SMPS Bias override value. This 
  variable shall NOT be updated when Tx sweep is done */

  uint16 apt_pa_q_current;
  /*!< keeps track of the current APT PA Q Current override value. */

  boolean apt_pa_q_current_enable;
  /*!< keeps track of the current APT PA Q Current override status. */

} ftm_cdma_override_data_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to track the status of a device
*/
typedef struct
{
  ftm_cdma_state_type rf_state;
  /*!< Current CDMA RF State. The default state for this field will be 
  RF_CDMA_STATE_SLEEP. */

  rfm_cdma_band_class_type curr_band;
  /*!< CDMA Band to which the device is tuned to. The default band for CDMA 
  system will be the invalid Band*/

  rfm_cdma_chan_type curr_chan;
  /*!< CDMA RF channel to which the device is tuned to. The default value 
  for this field will be RF_CDMA_INVALID_CHAN indicating invalid Channel */

  rfm_device_enum_type assoc_dev;
  /*!<                                                             
  @brief
  This is used when diversity is enabled for this device
   
  @details 
  This variable indicates other associated device with this device. 
  When the system is operating in Rx Diversity Mode, this field for
  primary device will hold the the slave device number on which Rx diversity is 
  turned on. On the other hand, this field for secondary device will hold the 
  master device number to which the secondary device is associated with. The 
  determination of master or slave device during diversity mode will be 
  identified by checking the rf_state. If rf_state is FTM_CDMA_STATE_RX or
  FTM_CDMA_STATE_RXTX, then it is a master device; whereas if it is 
  FTM_CDMA_STATE_RXDIV, then it is a slave device. In case of non-diversity mode 
  of operation, this field will hold invalid device, RFM_INVALID_DEVICE. The 
  default value for this field will also be RFM_INVALID_DEVICE. */

  /*!< 
  @brief
  This flag indicates which mode (OFS or diversity) the chain is to be tuned in.
  The flag carries significance only for secondary devices.
  */
  ftm_chain_config_type chain_config;
  
  rfm_cdma_power_mode_type power_mode;
  /*!< Current Power mode. This variable will be updated only when
  FTM_SET_RF_POWER_MODE is called. When FTM_GET_LNA_OFFSET or
  FTM_GET_DVGA_OFFSET is called, current Power Mode will be queried (to this
  variable) and passed to the Calibration API as argument. */

  lm_handle_type rxlm_handle;
  /*!< This variable points to the Rx Link Manager buffer associated with this
       device. */

  lm_handle_type txlm_handle;
  /*!< This variable points to the Tx Link Manager buffer associated with this
       device */

  lm_handle_type xpt_fb_handle;
  /*!< This variable points to the Rx Link Manager buffer needed for the 
  feedback for xpt calibraion associated with this device. */

  ftm_cdma_override_data_type override_data;
  /*!< This data type is used to hold current values for overrides */

  ftm_get_multi_synth_lock_data_type aggregated_multi_synth_lock_status;
  /*!< This data type is used to hold current multi synth lock status */
  
} ftm_cdma_data_status_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to keep track of Calibration Sweep V3 Data.
*/
typedef struct
{

  int16 lna_offset[RFM_CDMA_POWER_MODE_MAX][RFM_LNA_GAIN_STATE_NUM ];
  /*!< Variable to store LNA offsets measured during LNA calibration
  for all LNA gain Steps.The value stored in this variable is fetched during 
  consecutive LNA offset calibration. The units is in 1/640th of dB. */ 

  int32 agc_acc_val;
  /*!< Variable to store the accumulated AGC value during Rx measurement */

  rfm_cdma_power_mode_type curr_power_mode;
  /*!< Current Power Mode of CDMA device during Calibration v3 */

  boolean debug_en;
  /*!< Flag indicating, of cal v3 CDMA debug mode is to be enabled */

} ftm_cdma_cal_v3_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to keep track of RF Calibration Data
*/
typedef struct
{

  ftm_cdma_cal_v3_data_type cal_v3[RFM_MAX_DEVICES];
  /*!< The member stores Calibration Sweep V3 Data. */

} ftm_cdma_cal_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to keep track of MCPM configuration
*/
typedef struct
{

  ftm_1x_mcpm_config_data_type onex_mcpm;
  /*!< Structure to store the 1x FW app state configuration data */

  ftm_hdr_mcpm_config_data_type hdr_mcpm;
  /*!< Structure to store the HDR FW app state configuration data */
  
} ftm_cdma_mcpm_config_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The CDMA IQ capture structure which stores all the relevent data when 
  acquiring samples.
*/
typedef struct
{
  rfm_device_enum_type device; 
  /*!< The device for which IQ samples have been acquired */

  rfm_cdma_carrier_type carrier; 
  /*!< The carrier for which IQ samples have been acquired */

  uint16 req_samples; 
  /*!< The number of samples requested to be acquired */

  uint16 *iq_buf;  
  /*!< The buffer which stores CDMA IQ samples that are captured. Each element 
      I(8-bit) Q(8-bit) */

  boolean data_available; 
  /*!< Flag to indicate that data capture has been sucessful */

} ftm_cdma_iq_capture_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure template for Measure Tx params payload.

  @details
*/
typedef struct
{
  int16 tx_power;
  /*!< Tx Power in dB10 units */

  uint16 evm_value;
  /*!< EVM in % * 100 units*/

  int16 aclr_value[6];
  /*!< Array of ACLR values */  

  int16 vswr_ratio_dB10;
  /*!< value of VSWR in dB10 */  

  int16 vswr_v2_return_loss;
  /*!< value of VSWR Return Loss in  dB100 */

  int16 vswr_v2_mag;
  /*!< value of VSWR Magnitude in dB100*/

  int16 vswr_v2_phase;
  /*!< value of VSWR phase in degree100 */

  int16 vswr_v2_fwd_pwr;
  /*!< Value of forward power in dbm100 */

  int16 vswr_v2_rev_pwr;
  /*!< Value of reverse power in dbm100 */

} ftm_rf_cdma_payload_tx_params_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This is the strcuture used to store the information regarding acquired IQ samples.
*/

typedef struct
{
   rfm_device_enum_type device; 
   /*!< The device for which IQ samples have been acquired */

   uint16 req_samples;          
   /*!< The number of Tx and Rx samples requested to be acquired */

   int32 *iq_buf_tx;            
   /*!< The buffer which stores CDMA Tx IQ samples that are captured. Each element is I(8-bit) +  Q(8-bit) */

   int32 *iq_buf_rx;         
   /*!< The buffer which stores CDMA IQ samples that are captured. Each element is either I(16-bit)+  Q(16-bit)  */

   int32 *iq_buf_tx_rsp_data;
   /*!< The buffer which stores CDMA Tx IQ samples that are captured. Each element is I(8-bit) +  Q(8-bit) */

   int32 *iq_buf_rx_rsp_data;         
   /*!< The buffer which stores CDMA IQ samples that are captured. Each element is either I(16-bit)+  Q(16-bit)  */

   uint16 tx_iq_data_siz_bytes;
   /*!< The buffer which stores CDMA Tx IQ samples that are captured. Each element is I(8-bit) +  Q(8-bit) */

   uint16 rx_iq_data_siz_bytes;
   /*!< The buffer which stores CDMA IQ samples that are captured. Each element is either I(16-bit)+  Q(16-bit)  */

   boolean data_available;      
   /*!< Flag to indicate that data capture has been successful */

   uint32 sampling_frequency;
   /*!< Sampling frequency that was used to acquire IQ samples */

   ftm_rf_cdma_payload_tx_params_type meas_results;

}ftm_rf_cdma_fbrx_iq_acquired_samples_data_type;

/*---------------------------------------------------------------------------*/
/*!
  @brief
  Common data structure that can be used to send Response packet when we use 
  FTM_RSP_DO_NEW_PKT. All the new RSP packets have to be added here. 
  Based on need, user will select required packet.
*/
typedef union
{
  ftm_iq_capture_get_samples_rsp_pkt_type iq_capture_rsp; 
  /*!< RSP packet for IQ get samples */

  ftm_get_multiple_rx_level_dbm_rsp_pkt_type ftm_cdma_get_multiple_rx_level_dbm_rsp_pkt;
  /*!< Variable that is used as Response packet for FTM_GET_MULTIPLE_RX_LEVEL_DBM  */

  ftm_rf_fbrx_meas_tx_params_rsp_pkt_type ftm_rf_fbrx_meas_tx_params_rsp_pkt;
  /*!< RSP packet for FBRx Tx parameters */

  ftm_rf_fbrx_iq_capture_trigger_rsp_pkt_type ftm_rf_fbrx_iq_capture_trigger_rsp_pkt; 
  /*!< RSP packet for FBRx IQ capture trigger */

  ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt_type ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt_type;
  /*!< RSP packet for FBRx IQ capture get samples */

} ftm_cdma_rsp_pkt_type;


/*----------------------------------------------------------------------------*/
/*! Data structure containing CDMA logging information */
typedef struct
{
  
  ftm_1x_log_data_type onex_log;
  /*!< Data structure containing CDMA 1x logging information */

  ftm_hdr_log_data_type hdr_log;
  /*!< Data structure containing CDMA HDR logging information */

} ftm_cdma_log_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to store the CDMA FW app state configuration data */
typedef struct
{
  
  ftm_1x_fw_state_config_data_type onex_fw;
  /*!< Structure to store the 1x FW app state configuration data */

  ftm_hdr_fw_state_config_data_type hdr_fw;
  /*!< Structure to store the HDR FW app state configuration data */

} ftm_cdma_fw_state_config_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure Containing Common Global Data used by 1xHDR FTM. There should not
  be stray global variables. If global variable is required, it must be 
  contained within this structure. 
*/ 
typedef struct
{

  ftm_cdma_data_status_type *rf_device_status[RFM_MAX_DEVICES];
  /*!< The member sub-structure tracks the status of the device. */

  ftm_cdma_cal_data_type cal_data;
  /*!< The member sub-structure stores data used for RF Calibration. This
  field will be reused for different devices assuming that only one device
  will be calibrated at once. This structrue can still be shared when
  Rx and Tx cal are done at the same time on same device. */

  ftm_cdma_mcpm_config_data_type mcpm_state;
  /*!< The member sub-structure tracks the current configuration of MCPM
  for FTM CDMA */

  ftm_cdma_fw_state_config_data_type fw_state;
  /*!< Structure to store the CDMA FW app state configuration data */

  ftm_cdma_log_data_type cdma_log;
  /*!< Data structure containing CDMA logging information */

  ftm_cdma_iq_capture_data_type iq_capture_data;
  /*!< Data structure containing IQ capture data. This is common to both 1X and HDR*/
  
  ftm_cdma_rsp_pkt_type rsp_pkt;
  /*!< Data structure that is used for sending RSP pkt. This is common to both 1X and HDR*/

/* This varaible is used to maintain information of acquired IQ samples. */
  ftm_rf_cdma_fbrx_iq_acquired_samples_data_type ftm_rf_fbrx_iq_acquired_samples_data;

  boolean timed_sleep_success;
  /*!< Data structure that is used for store the status of timed sleep returned from FW */
  
} ftm_cdma_data_type;

/*----------------------------------------------------------------------------*/
boolean
ftm_cdma_data_init_device_status
(
  void
);

/*----------------------------------------------------------------------------*/
void
ftm_cdma_data_update_rf_state
(
  rfm_device_enum_type device,
  ftm_cdma_state_type rf_state
);

/*----------------------------------------------------------------------------*/
void
ftm_cdma_data_update_chain_config
(
  rfm_device_enum_type device,
  ftm_chain_config_type chain_config
);

/*----------------------------------------------------------------------------*/
void
ftm_cdma_data_update_assoc_device
(
  rfm_device_enum_type device,
  rfm_device_enum_type assoc_dev
);

/*----------------------------------------------------------------------------*/
void
ftm_cdma_data_update_band_chan
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan
);

/*----------------------------------------------------------------------------*/
void
ftm_1x_update_latest_aggregated_multisynth_status
(
  rfm_device_enum_type device,
  ftm_multi_synth_lock_status_enum_type rx_synth_lock,
  ftm_multi_synth_lock_status_enum_type tx_synth_lock
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_reset_aggregated_multisynth_lock_status
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void
ftm_cdma_data_set_power_mode
(
  rfm_device_enum_type device,
  rfm_cdma_power_mode_type power_mode
);

/*----------------------------------------------------------------------------*/
void
ftm_cdma_data_invalidate_lm_buffers
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
ftm_cdma_data_get_device_status
(
  rfm_device_enum_type device,
  ftm_cdma_data_status_type *dev_status
);

/*----------------------------------------------------------------------------*/
ftm_cdma_data_status_type *
ftm_cdma_data_get_device_ptr
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
ftm_cdma_cal_v3_data_type*
ftm_cdma_data_get_cal_v3_adr
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
rfm_cdma_power_mode_type
ftm_cdma_data_get_power_mode
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
ftm_cdma_mcpm_config_data_type*
ftm_cdma_get_mcpm_state_ptr
(
  void
);

/*----------------------------------------------------------------------------*/
ftm_cdma_log_data_type*
ftm_cdma_get_log_ptr
(
  void
);
/*----------------------------------------------------------------------------*/
ftm_cdma_iq_capture_data_type*
ftm_cdma_get_iq_capture_data_ptr
(
  void
);
/*----------------------------------------------------------------------------*/
ftm_rf_cdma_fbrx_iq_acquired_samples_data_type*
ftm_rf_fbrx_get_iq_acquired_samples_data_ptr
(
  void
);
/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_pkt_type*
ftm_cdma_get_rsp_pkt_data_ptr
(
  void
);

/*----------------------------------------------------------------------------*/
boolean 
ftm_cdma_is_device_state_valid
(
  rfm_device_enum_type device,
  ftm_cdma_state_type rf_state
);

/*----------------------------------------------------------------------------*/
rfm_device_enum_type
ftm_cdma_resolve_master_device
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
ftm_cdma_fw_state_config_data_type *
ftm_cdma_get_fw_state_config_data
(
  void
);
/*----------------------------------------------------------------------------*/
void
ftm_cdma_allocate_memory_for_iq_sample_buffer
(
  void
);
/*----------------------------------------------------------------------------*/
void
ftm_cdma_free_memory
(
  void
);
/*----------------------------------------------------------------------------*/
void*
ftm_cdma_data_1x_get_fw_smem_addr
(
  void
);
/*----------------------------------------------------------------------------*/
boolean
ftm_1x_get_aggregated_multi_synth_lock_status
(
  rfm_device_enum_type device,
  ftm_multi_synth_lock_status_enum_type *rx_synth_lock,
  ftm_multi_synth_lock_status_enum_type *tx_synth_lock 
);

/*----------------------------------------------------------------------------*/
rfm_cdma_band_class_type
ftm_cdma_data_get_band_class
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void
ftm_cdma_data_set_timed_sleep_state
(
  boolean state
);

/*----------------------------------------------------------------------------*/
boolean
ftm_cdma_data_get_timed_sleep_state
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* FTM_CDMA_DATA_H */
