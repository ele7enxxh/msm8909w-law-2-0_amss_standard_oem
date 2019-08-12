#ifndef RFDEVICE_CDMA_TYPEDEF_H
#define RFDEVICE_CDMA_TYPEDEF_H

/*!
  @file
  rfdevice_cdma_typedef.h

  @brief
  Contains data and function types for CDMA device driver
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_cdma_type_defs.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/12/15   pk      Added support for cdma discontinuous rx
12/15/14   ry      Added rfdevice_cdma_tx_spectral_inversion_type
07/31/14   lmiao   Added new mcdo data structure rfdevice_cdma_multi_carrier_droop_compensations_type_v2
                   Added new const RFDEVICE_NUM_DROOP_COMP_ENTRIES, max data buffer size in the above struct 
06/19/14   bm      Changes to move SSMA to common spur mitigation state machine
03/09/14   pk      Added interface to query DLNA support
06/24/13   shb     Added API to get ET delay compensation
05/29/13   spa     Added function pointer to retrieve Rx ON/OFF scripts
03/21/12   shb     Updated HDET param to supported gated(WTR1625) & 
                   non-gated(WTR1605) HDET
03/11/13   sar     Updated file for APQ, GNSS only target.
03/08/13   shb     Implemented tune_to_default_port_fp for ET FB capture support 
                   on SV cards
02/21/13   bm      Added support for WBAGC jammer detection
01/10/13   spa     Support for RFC based PA LUT mapping
01/06/13   shb     Added function to return LNA gain script per gain state in 
                   rf_buffer_intf format
12/11/12   hdz     Enabled timing support for rf1x_tx_get_rf_on_off_script_func_ptr_type
11/08/12   cd      Support RF RGI LUT index override
10/26/12   spa     Added function ptr for SSBI based enabling/disabling Tx chain
10/18/12   spa     Added set RF on/off structure for SSBI on/off support 
08/16/12   shb     Renamed rfdevice_enum_type to rfdevice_id_enum_type
08/15/12   shb     Removed SHIM layer functions from rfdevice_cdma_typedef.h. 
                   This file contains data and func table types used by device
08/15/12   shb     Branched rfdevice_cdma_interface.h to rfdevice_cdma_typedef.h 
                   in rfdevice/interface/api. 
08/08/12   adk     Added support for scripting buffer interface.
08/02/12   spa     Added Dime specific API rfdevice_cdma_tx_get_therm_scripts_v2 
08/01/12   shb     Added rfdevice_cdma_rx/tx_set_band_data
06/26/12   cd      Added functionality to obtain power mode configuration 
                   from device
06/15/12   adk     Corrected BB spur mitigation for WTR1605
06/07/12   shb     Added DPD support in CDMA TX tune API
03/29/12   shb     Added LNA Phase Comp support on WTR1605. Required changes in 
                   device interface to include Channel, Bandwidth and CDMA State 
03/21/12   adk     Update rxf_get_iqmc_coef_type to retrieve IQMC coeff for SHDR
03/02/12   Saul    MCDO. Rx AGC freq comp and device droop compensation framework.
03/01/12   sty     Added debug_msgs_enabled to rfdevice_cdma_rx_reco_type
02/29/12   Saul    MCDO. WTR HDR RF Cal data adjust.
02/27/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan   
02/17/12   sty     Changes for SSMA in DORB mode
02/08/12   shb     Deleted unused function pointers from CDMA RX and TX device 
                   virtual function table
02/08/12   shb     Added argument in CDMA device rx tune api to distinguish 
                   between MEAS script generation and standalone wakeup
02/04/12   cd      - Added power mode argument to HDET set up functionality   
                   - Added HDET Power mode enum 
01/20/12   shb     Updated CDMA device interface to use generic 
                   rfdevice_ssbi_write_type and rfdevice_script_type instead of
                   CDMA specific structures
01/11/12   Saul    MCDO. Passing bandwidth for IQMC coefficients query.
                   Passing mode to Tx tune script.
01/09/12   Saul    MCDO. Added iqmc_coefs and cal_adj_for_hdr
01/04/12   aro     Added interface to query LNA phase offset
01/04/12   aro     Added interface to query LNA phase offsets for current
                   configuration
12/28/11   shb     Converted IQMC coefficients to int32 from uint32
12/22/11   shb     Added API to setup_hdet
12/21/11   sty     var name changes
12/19/11   sty     Deleted unused elements in 
                   rfdevice_1x_get_rx_recommendation_args_capsule_type - they 
                   will be needed for MCDO
12/13/11   Saul    MCDO. Passing bandwidth to rc tune per mode.
12/13/11   sty     Changed var name from rfdevice_1x_rx_recommendation_type to 
                   rfdevice_cdma_rx_recommendation_type
12/13/11   Saul    MCDO. Passing rx/tx bandwidth to device driver.
12/10/11   sty     var name changed
12/07/11   aro     Added interface to query Tx RGI address
12/06/11   aro     Added function to sample and average HDET reads
12/05/11   adk     Code cleanup
12/02/11   shb     Added band and power_mode as arg in set_ic_cal_data fp to 
                   avoid using internal device state variables that may not be 
                   correct at time of cal data application
12/01/11   sty     Use rfm_cdma_band_class_type intead of rf_card_band_type
11/29/11   sty     Added members to rfdevice_1x_get_rx_recommendation_args_capsule_type
11/17/11   shb     Updated rf1x_tx_get_therm_param_func_ptr_type to have a 
                   "therm_supported" flag to selectively enable therm reads on
                   a device
11/10/11   hdz     Added support for restoring HKADC setting back to HDET 
                   after thermistor read
10/24/11   hdz     Added support for temp comp   
                   - Added rfdevice_cdma_therm_param_type
                   - Added  rfdevice_cdma_therm_scripts_type
                   - Added  rfdevice_cdma_therm_read_type
10/24/11   hdz     Added new func ptr typs for temp comp   
                   - rf1x_1x_tx_get_therm_param_func_ptr_type
                   - rf1x_tx_get_therm_scripts_func_ptr_type
                   - rf1x_tx_read_therm_func_ptr_type
                   - rf1x_tx_setup_therm_func_ptr_type
10/24/11   hdz     Added members to rfdevice_cdma_func_tbl_type   
                   - get_therm_param
                   - get_therm_scripts
                   - read_therm
                   - setup_therm
10/10/11   aro     Increased maximum script size to 600
09/26/11   adk     Removed temporary code.
09/25/11   vb      Support for DAC cal using different Irefs
09/21/11   adk     Removed obsolete code.
09/15/11   adk     Enabled SBI scripts. Executed scripts in RF SW (for now).
09/11/11   aro     Merged CDMA device interface to delete
                   rfdevice_1x_interface.h
08/05/11   shb     Added API to get HDET scripts for MDSP
08/05/11   shb     Added arg in rfdevice_cdma_read_hdet() to indicate if HKADC
                   conversion has already been triggered by MDSP
08/03/11   shb     Converted rfdevice_cdma_get_tx_timing to 
                   rfdevice_cdma_get_tx_hdet_param
08/02/11   adk     Added prototype for rfdevice_cdma_append_script2_to_script1()
07/26/11   aro     Added interface to query HDET settling time
07/25/11   aro     [1] Created HDET enable/disable/read interfaces
                   [2] Pushed PDET LPF bandwidth to device driver
07/15/11   aro     Pushed JD code analysis inside RTR driver
07/12/11   aro     Added interface to reset JD status
07/12/11   sty     Renamed rfdevice_lut_data_type_new to rfdevice_lut_data_type
07/07/11   adk     Updated SBI script generation code.
07/07/11   sty     Added prototype for rfdevice_cdma_tx_get_lut_table
07/01/11   adk     Unified SBI script generation code and legacy code.
07/01/11   aro     Added interface to perform Intelliceiver Calibration
06/29/11   aro     Added interface to set intelliciever Cal data
06/20/11   aro     Added interface to get intelliceiver Power mode
06/16/11   aro     Added member interface to check intelliceiver availability
06/08/11   shb     Added rfdevice_cdma_rx_set_linearity()
06/07/11   shb     Updated RF CDMA device interface to accept RX/TX device
                   pointers (queried from RFC) instead of the logical path
05/05/11   adk     Fixed function pointer mismatch and other compiler warnings.
04/29/11   adk     Updated SBI script generation functions.
04/26/11   shb     Added CDMA device diversity enable/disable interface
04/14/11   sty     Added rfdevice_cdma_create_enable_rx_script()
03/09/11   aro     Renamed interface for consistency
02/04/11   sty     [1] Renamed rfdevice_cdma_script_tx_tune to  
                   rfdevice_cdma_create_tx_tune_script
                   [2] Added rfdevice_cdma_create_rx_sleep_script and
                       rfdevice_cdma_create_tx_sleep_script 
02/03/11   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"

#ifdef FEATURE_CDMA1X
#include "rfm_device_types.h"
#include "rfm_mode_types.h"
#include "rfm_cdma_power_mode_types.h"
#include "rfm_gain_state_types.h"
#include "rfm_cdma_band_types.h"
#include "rf_cdma_state_types.h"
#include "rfdevice_intf_cmd.h"
#include "rf_cdma_wb_rxf.h"
#include "rfdevice_meas_type_defs.h"
#include "rfcom.h"
#include "rfdevice_cmn_intf.h"
#include "rf_hal_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! @cond Doxygen_Suppress */
/*------------- FOR mDSP-based TX AGC -----------------------------*/
#define RF_1X_NUM_TX_LINEARIZERS                               4
/*!
  @brief
  Since RTR8600 CDMA supports BC0 (CELL) thru BC15 (AWS), 
  total number of CDMA bands is 16
*/
#define RFDEVICE_1X_TX_MAX_BAND                               16

/*----------------------------------------------------------------------------*/
/*! Maximum number of SBI that can be sent to device must be the same as
    MAX_NUM_OF_NON_TIME_SBI (= max( MAX_NUM_OF_SBI_BUF0, MAX_NUM_OF_SBI_BUF1)) */

/*! Maximum size of PDET ON, HKADC trigger and PDET OFF scripts in CDMA
TX devices */
#define RFDEVICE_CDMA_HDET_SCRIPTS_MAX_SIZE                    3

/*! Maximum size of Thermistor ON, HKADC trigger and Thermistor OFF scripts 
  in CDMA TX devices */
#define RFDEVICE_CDMA_THERM_SCRIPTS_MAX_SIZE                   3

/*----------------------------------------------------------------------------*/


/*maximun number of mcdo data could be stored in rfdevice_cdma_multi_carrier_droop_compensations_type_v2*/
#define  RFDEVICE_NUM_DROOP_COMP_ENTRIES            35
/*!
  @brief
  Enumeration of Tx look-up tables (LUT). Each PA-state for a given band may
  need to use the low-power or high-power table - this decision depends on the
  RF card being used
*/
typedef enum
{
  /*! Enum for low-power table */
  RFDEVICE_CDMA_LOW_POWER_TABLE,

  /*! Enum for mid-power table */
  RFDEVICE_CDMA_MID_POWER_TABLE,

  /*! for high-power table */
  RFDEVICE_CDMA_HIGH_POWER_TABLE,

  /*! use this enum if look-up table is invalid */
  RFC_CDMA_INVALID_TABLE

} rfdevice_cdma_lut_type;

/*----------------------------------------------------------------------------*/
/*! @brief
    This data structure holds all info pertaining to LUT tables. This data 
    structure will replace rfdevice_lut_data_type - which will be deprecated 
    soon    
*/ 
typedef struct 
{

  /*! var that holed the table-name */
  int lut_table_name;

  /*! pointer to the actual LUT table */
  const uint32* lut_table_ptr;

  /*! holds the size of the LUT table that lut_table_ptr points to */
  byte lut_table_size;

} rfdevice_lut_data_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  rfm_cdma_band_class_type band;
  boolean spectral_inversion;  
} 
rfdevice_cdma_tx_spectral_inversion_type;

/*---------------------------------------------------------------------------*/
/*!
  @brief
  Forward declaration of device structure types to use in function pointers
  definitions
*/
typedef struct rfdev_cdma_rx rf1x_device_rx_type;
typedef struct rfdev_cdma_tx rf1x_device_tx_type;


/*---------------------------------------------------------------------------*/
/*!
  @brief
  Structure that consists of band and iq mis a/a coefs
*/
typedef struct
{
  /* INPUTS: */
  rf_cdma_state_type sec_chain_rf_state;
  /*!< Current state of secondary chain */
  rfm_cdma_band_class_type band;
  /*!< Band class for which spectral inversion info is to be retrieved */
  rfm_cdma_chan_type chan;
  /*!< Channel for which spectral inversion info is to be retrieved */
  rfm_cdma_power_mode_type lin;  
  
  uint32 bw_khz;
  /*!< Input. Bandwidth in use. */

  /* OUTPUTS: */
  
  /*!< Current Intelliceiver Power Mode */
  int32 iqmc_a_coeff;
  /*!< I/Q MIS A COEFF in signed 32Q15 format */
  int32 iqmc_b_coeff;
  /*!< I/Q MIS B COEFF in signed 32Q15 format */
} rxf_get_iqmc_coef_type;

/*---------------------------------------------------------------------------*/
/*!
  @brief
  Structure to get TX LUT address for a particular band. use with tx command
  dispatch: RFDEVICE_GET_TX_LUT_TABLE
*/
typedef struct
{
  /*!< TX band whose LUT is needed */
  rfcom_1x_band_type band;
  /*! Double pointer will point to address of pointer to LUT for linearizer 0
  which can be used to accessed all linearizers for a given band */
  uint32** tx_lut_addr;
} rfdevice_1x_get_tx_lut_tbl_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Temperature reading from RTR thermistor

  @details
  It returns the temperature reading as well as if the reading is valid. 
 
*/
typedef struct
{
  uint16   temperature;
  boolean  valid_temp_read;    
} rfdevice_therm_read_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data capsule to hold the input (band) and output  
  (pdm_write_reg_address). This data structure is used when the TX AVGA gain
  addresse needs to be queried from the RF device 
*/
typedef struct
{
  /*! input: Band whose LUT SSBI Address is queried */
  rfm_cdma_band_class_type band;
  /*! output: TX AGC SSBI addresses  */
  uint32 pdm_reg_addr;
  /*! output: TX AGC SSBI Bus  */
  uint8 ssbi_bus;
}rfdevice_lut_addr_type;


typedef struct
{
   /*! input: Table type to pick (hi or lo power) */
  rfdevice_cdma_lut_type tbl_type;
  /*!  input: LUT table name to pick for required band */
  int lut_table_name;
  /*!  input: LUT index to program (RGI) */
  uint8 lut_idx;
}rfdevice_1x_set_lut_idx_parms_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data structure that holds HDET reading and a "quality indicator" that 
  signifies if the HDET reading is valid or not.
*/
typedef struct
{
  /*! Flag that signifies if the HDET reading is valid or not*/
  boolean hdet_quality;
  /*! place holder for HDET reading */
  uint16  hdet_reading;
} rfdevice_cdma_hdet_read_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure used to store HDET parameters that are device dependent
*/
typedef struct
{
  /*! Flag indicating if HDET is supported on a given device */
  boolean hdet_supported;

  /*! HDET needs time to settle. This variable defines the time in the units 
  of microseconds required to allow HDET to settle. This time will be used by RF 
  upper layer to perform clock busy wait before taking the first HDET sample. */
  uint32 pdet_lpf_settling_t;

  /*! Time for ADC output to settle after a HKADC conversion is triggered.
  Unit is microseconds */
  uint8 hkadc_conversion_t;

  /*! This is the time that TX AGC needs to be frozen for a HDET read. When HDET 
  is used in non-gated mode, this is the sum of pdet_lpf_settling_t and 
  hkadc_conversion_t. However, in gated mode the settling time is taken care of 
  by RFIC hardware and hence an explicit txagc freeze time needs to be provide 
  to freeze FW txagc for the appropriate duration */ 
  uint16 txagc_freeze_t;

  /*! Period in ms to perform HDET reads in Acquisition mode. Acq mode is used
  for fast convergence in TX power error at max powers. This polling period is
  used during initial operation and if the error drifts out of bounds */
  uint8 acq_read_period;

  /*! Period in ms to perform HDET reads in Tracking mode. Tracking mode is 
  used after the HDET error has converged to a reasonably small value to 
  maintain the error within bounds. Thus polling period required during 
  tracking mode will likely be smaller than acq_polling_period */
  uint16 trk_read_period;

  /*! HDET error filter coefficient in acquisition mode */
  uint8 acq_err_filter_coeff;

  /*! HDET error filter coefficient in tracking mode */
  uint8 trk_err_filter_coeff;

  /*! Minimum no of times HDET loop should operate in acquisition mode before
  any transitions */
  uint8 acq_mode_min_count;

  /*! If HDET error goes below this threshold, HDET loop may transition from
  acquisition to tracking mode if satisfying min count criteria. 
  Value in 1/10th of dB */
  uint8 acq2trk_err_thresh_x10;

  /*! If HDET error goes above this threshold, HDET loop may transition from
  tracking to acquisition mode. Value in 1/10th of dB */
  uint8 trk2acq_err_thresh_x10;
  
} rfdevice_cdma_hdet_param_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure used to store HDET scripts needed by MDSP
*/
typedef struct
{
  /*! Script of SSBI sequence to turn PDET ON */
  rfdevice_ssbi_write_type pdet_on_script[RFDEVICE_CDMA_HDET_SCRIPTS_MAX_SIZE];

  /*! Size of PDET on SSBI script */
  uint8 pdet_on_script_size;

  /*! Script of SSBI sequence to trigger HKADC conversion */
  rfdevice_ssbi_write_type hkadc_trig_script[RFDEVICE_CDMA_HDET_SCRIPTS_MAX_SIZE];

  /*! Size of HKADC trigger SSBI script */
  uint8 hkadc_trig_script_size;

  /*! Script of SSBI sequence to turn off PDET */
  rfdevice_ssbi_write_type pdet_off_script[RFDEVICE_CDMA_HDET_SCRIPTS_MAX_SIZE];

  /*! Size of PDET OFF SSBI script */
  uint8 pdet_off_script_size;

} rfdevice_cdma_hdet_scripts_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Define HDET power mode types for the device
*/
typedef enum
{
  /*! HDET is in high power mode */
  RFDEVICE_CDMA_HDET_HPM,

  /*! HDET is in low power mode */
  RFDEVICE_CDMA_HDET_LPM,

  /*! Number of power modes for HDET */
  RFDEVICE_CDMA_HDET_PM_NUM,

} rfdevice_cdma_hdet_power_mode_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure used to store Thermistor parameters that are device dependent
*/
typedef struct
{
  /*! Flag to indicate if a given device supports therm reads */
  boolean therm_supported;

  /*! Thermistor needs time to settle. This variable defines the time in the 
  units of microseconds required to allow thermistor to settle. This time will 
  be used by RF upper layer to perform clock busy wait before taking the first 
  thermistor sample. */
  uint32 therm_settling_t;

  /*! Time for ADC output to settle after a HKADC conversion is triggered.
  Unit is microseconds */
  uint8 hkadc_conversion_t;
  
} rfdevice_cdma_therm_param_type;


/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure used to store Thermistor scripts needed by MDSP
*/
typedef struct
{
  /*! Script of SSBI sequence to turn Thermistor ON */
  rfdevice_ssbi_write_type 
    therm_on_script[RFDEVICE_CDMA_THERM_SCRIPTS_MAX_SIZE];

  /*! Size of Thermistor on SSBI script */
  uint8 therm_on_script_size;

  /*! Script of SSBI sequence to trigger HKADC conversion */
  rfdevice_ssbi_write_type 
    hkadc_trig_script[RFDEVICE_CDMA_THERM_SCRIPTS_MAX_SIZE];

  /*! Size of HKADC trigger SSBI script */
  uint8 hkadc_trig_script_size;

  /*! Script of SSBI sequence to turn off Thermistor */
  rfdevice_ssbi_write_type 
    therm_off_script[RFDEVICE_CDMA_THERM_SCRIPTS_MAX_SIZE];

  /*! Size of Thermistor OFF SSBI script */
  uint8 therm_off_script_size;

    /*! Script of SSBI sequence to setup Thermistor  */
  rfdevice_ssbi_write_type 
    therm_setup_script[RFDEVICE_CDMA_THERM_SCRIPTS_MAX_SIZE];

  /*! Size of setup Thermistor on SSBI script */
  uint8 therm_setup_script_size;

} rfdevice_cdma_therm_scripts_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data structure that holds Therm reading and a "quality indicator" that 
  signifies if the Therm reading is valid or not.
*/
typedef struct
{
  /*! Flag that signifies if the Therm reading is valid or not*/
  boolean therm_quality;
  /*! place holder for Therm reading */
  uint16  therm_reading;
} rfdevice_cdma_therm_read_type;


/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure for getting Tx DAC Cal Iref from RF device based on the current
  Tx band
*/
typedef struct
{
  /*!< Iref to be used for the current Tx Band */
  rfdevice_dac_iref_val iref_val;

  /*!< current Tx band */
  rfm_cdma_band_class_type band; 

} rfdevice_cdma_dac_cal_iref_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enum defining the maximum number of jammer detectors available for the
  RTR Device.
*/
#define RFDEVICE_CDMA_IC_NUM_OF_DET 2

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to hold Intelliceiver Calibration Data
 
  @details
  This structure is used to push the intelliceiver calibration data 
  of the current device instance to device driver. These parameter will be used
  by the device driver to calucluate the Jammer Detect Threshold 
  value.
*/
typedef struct
{

  rfm_cdma_power_mode_type new_power_mode;
  /*!< New ower Mode for which the intellceiver Cal data is to be programmed */

  rfm_mode_enum_type curr_rf_mode;
  /*!< Current RF Mode for which the cal data is to be programmed. Based on Mode 
  information Device driver will choose the correct Jammer detector 
  switchpoint*/ 

  int8 detector_adjust[RFDEVICE_CDMA_IC_NUM_OF_DET][RFM_LNA_GAIN_STATE_NUM];
  /*!< IntelliCeiver detector threshold adjustments value in 1/10th dB 
  increments. The threshold can be adjusted +2dB to -10dB for each gain state*/

  int16 cal_value[RFDEVICE_CDMA_IC_NUM_OF_DET];
  /*!< IntelliCeiver calibration Data mesaured during calibration */

  int16 g0_gain_cal_delta;
  /*!< Delta between the LNA 0 Gain Calibration data of current channel and 
  intelliceiver reference calibration channel. This value will be used by 
  RTR driver to adjust the jammer switchpoint. */

}rfdevice_cdma_ic_thres_param_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold intelliceiver calibration data */
typedef struct intelliceiver_cal_data
{
  boolean cal_status;
  /*!< Flag indicating the calibration status */

  int16 cal_data;
  /*!< Intelliceiver calibration data */

} rfdevice_cdma_ic_cal_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enumeration for Power Mode Configuration that is possible for intelliceiver
  operation - based on RF device support
*/
typedef enum
{
  RFDEVICE_CDMA_IC_FIXED_PM_0,
  /*!< Indicates that device operation is fixed to Power Mode 0 */

  RFDEVICE_CDMA_IC_FIXED_PM_1,
  /*!< Indicates that device operation is fixed to Power Mode 1 */

  RFDEVICE_CDMA_IC_FIXED_PM_2,
  /*!< Indicates that device operation is fixed to Power Mode 2 */

  RFDEVICE_CDMA_IC_ADAPTIVE_PM,
  /*!< Indicates that device operation is adaptive */

  RFDEVICE_CDMA_IC_ADAPTIVE_PM_CHIP = RFDEVICE_CDMA_IC_ADAPTIVE_PM,
  /*!< Indicates that device operation is adaptive i.e. is permitted to 
  transition between power modes based on intelliceiver operation.
  JD is supported by the RF CHIP */

  RFDEVICE_CDMA_IC_ADAPTIVE_PM_WB_AGC,
  /*!< Indicates that device operation is adaptive i.e. is permitted to 
  transition between power modes based on intelliceiver operation.
  JD is supported by the firmware WB AGC algorithm */

  RFDEVICE_CDMA_IC_PM_CONFIG_INVALID
  /*!< Invalid power mode configuration from device */

} rfdevice_cdma_ic_power_mode_config_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold LNA phase offset data */
typedef struct
{

  uint16 offset_in_deg[RFM_LNA_GAIN_STATE_NUM];
  /*!< LNA Phase offsets in degrees for all LNA gain states */

} rfdevice_cdma_lna_phase_offset_type;

/*----------------------------------------------------------------------------*/
/*! Enumeration used for communication from CDMA MC/Core to device whether it 
is requesting a script for IRAT or Standalone operation */
typedef enum
{
  /*! Current operation is MEAS or IRAT measurement. Device should not set 
  internal state variables or do anything that can perturb serving technology 
  - like reset SSBI in use by other technology */ 
  RFDEVICE_CDMA_MEAS_SCRIPT,

  /*! Current operation is in Standalone mode. Device CDMA state variables must 
  be set up accordingly */
  RFDEVICE_CDMA_STANDALONE_SCRIPT
} rfdevice_cdma_script_operation_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Retrieve the correct LUT table name for a given PA_state

  @param *txdev
  Pointer to tx device 

  @param pa_state
  The PA state for which LUT table is to be returned 

  @param *tx_lut_data
  Pointer that holds the input (table-name) and output (table size and pointer 
  to the LUT)

*/
typedef void (*rf1x_tx_get_lut_table_func_ptr_type) 
(
  rf1x_device_tx_type* tx_device,
  rfdevice_lut_data_type* tx_lut_data,
  rfdevice_cdma_lut_type table_type
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Setup TX device for HDET reads
*/
typedef boolean (*rf1x_tx_setup_hdet_func_ptr_type) 
(
  rf1x_device_tx_type* tx_device,
  rfm_mode_enum_type mode,
  rfdevice_cdma_hdet_power_mode_type power_mode
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable the HDET power detector circuit.

  @details
  This function is used if the driver wants to manually manage the HDET
  circuit to reduce overhead in readings or to insure the power detector
  has sufficient time to stabilize.
*/
typedef boolean (*rf1x_tx_enable_hdet_func_ptr_type) 
(
  rf1x_device_tx_type* tx_device
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Manually disable the HDET power detector circuit.

  @details
  This function is used to shut down the HDET circuit if it was manually
  enabled using the enable_hdet() function.
*/
typedef boolean (*rf1x_tx_disable_hdet_func_ptr_type) 
(
  rf1x_device_tx_type* tx_device
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return HDET reading and quality indicator

  @details
  This API reads the HDET, and also checks to see if the HDET is in a valid 
  state (i.e, if the HDET is enabled)
  
  @param *tx_device
  Pointer to current instance of RTR driver for which the HDET is to be 
  read.

  @param mdsp_triggered_hdet
  Flag indicating if MDSP has already triggered HDET read or not. If MDSP
  has triggered conversion, device driver will only read HDET results else
  it will trigger conversion within software
 
  @param *hdet_output
  Container to store HDET reading and quality indicator 
*/
typedef void (*rf1x_tx_read_hdet_func_ptr_type) 
(
  rf1x_device_tx_type* tx_device,
  boolean mdsp_triggered_read,
  rfdevice_cdma_hdet_read_type *hdet_output
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return average HDET reading for N samples of reads

  @details
  This API reads N samples of HDET and returns the average HDET value. This
  function should be called only after HDET is enabled. For the case when HDET
  is not enabled, this function will return invalid HDET read
  
  @param tx_device
  Pointer to current instance of RTR driver for which the HDET is to be 
  read.

  @param num_of_reads
  Number of HDET samples to be taken for averaging
 
  @return
  Average HDET read after sampling
*/
typedef uint16 (*rf1x_tx_multi_read_hdet_func_ptr_type) 
(
  rf1x_device_tx_type* tx_device,
  uint16 num_of_reads
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query Tx timing paramter
 
  @details
  This function is used to query all the timing parameter pertaining to the
  current Tx device.
 
  @param tx_device
  Pointer to Tx device for which HDET parameters are queried
 
  @param hdet_param
  Container where the HDET parameters will be populated by the device driver 
*/
typedef void (*rf1x_tx_get_hdet_param_func_ptr_type) 
(
  rf1x_device_tx_type* tx_device,
  rfdevice_cdma_hdet_param_type *hdet_param
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  
  @details

  @param tx_device
 
  @param 

*/
typedef void (*rf1x_tx_get_hdet_scripts_func_ptr_type) 
(
  rf1x_device_tx_type* tx_device,
  rfdevice_cdma_hdet_scripts_type* hdet_scripts
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query therm read timing parameters
 
  @details
  This function query the necessary timing parameter for therm FW read 
  operation. The timing parameters are device dependent and are provided by 
  system group
 
  @param tx_device
  Pointer to the tx_devide that the current therm operation is on
 
  @param therm_param
  Container of the timign parameters

*/
typedef void (*rf1x_tx_get_therm_param_func_ptr_type)
(
  rf1x_device_tx_type* txdev,
  rfdevice_cdma_therm_param_type *therm_param
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query therm read scripts
 
  @details
  This function query the necessary SBI scripts for therm FW read 
  operation, ie., Enable thermistor, Trigger reading and disable thermistor.
  The SBI scripts are device dependent and are provided by system group
 
  @param tx_device
  Pointer to the tx_devide that the current therm operation is on
 
  @param therm_scripts
  Container of the scripts

*/
typedef void (*rf1x_tx_get_therm_scripts_func_ptr_type)
(
  rf1x_device_tx_type* txdev,
  rfdevice_cdma_therm_scripts_type* therm_scripts
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query therm read scripts for DIME
 
  @details
  This function query the necessary SBI scripts for therm FW read 
  operation, ie., Enable thermistor, Trigger reading,disable thermistor and 
  setup therm. The SBI scripts are device dependent and are provided by system 
  group
 
  @param tx_device
  Pointer to the tx_devide that the current therm operation is on
 
  @param therm_scripts
  Container of the scripts
 
  @param therm_params
  Container of the thermistor params

*/
typedef void (*rf1x_tx_get_therm_scripts_v2_func_ptr_type)
(
  rf1x_device_tx_type* txdev,
  rf_buffer_intf *temp_comp_settings,
  rfdevice_cdma_therm_param_type *therm_param
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform rtr8600 thermistor read.
 
  @details
  This function reads the temperature value from rtr8600 registers. Note that 
  this function has to be called only after setup_therm() and mdsp has finished 
  reading operation.
 
  @param txdev
  Pointer to the tx_devide that the current therm operation is on
 
  @param mdsp_triggered_read
  Flag to indicate if the read is triggered by mdsp
 
  @param therm_output
  Container of the temperature value
*/
typedef void (*rf1x_tx_read_therm_func_ptr_type)
(
  rf1x_device_tx_type *txdev,                          
  boolean mdsp_triggered_read,
  rfdevice_cdma_therm_read_type *therm_output
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform thermistor setup.
 
  @details
  This function implements necessary SBI commands to aid for thermistor read.
 
  @param txdev
  Pointer to the tx_device that the current therm operation is on
 
  @param mode
  Mode of current operation
*/
typedef void (*rf1x_tx_setup_therm_func_ptr_type)
(
  rf1x_device_tx_type *txdev,
  rfcom_mode_enum_type mode
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
 
  @details

  @param tx_device
 
  @param 

*/
typedef void (*rf1x_tx_get_dac_cal_iref_func_ptr_type)
(
  rf1x_device_tx_type* tx_device,
  rfdevice_cdma_dac_cal_iref_type* iref_data
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set linearity (power mode) for a given RX device

  @param *rxdev
  Pointer to rx device whose linearity needs to be set

  @param path
  Logical RF path whose linearity needs to be set.

  @param power_mode
  Specifies the power mode to be set

  @param retune_pll
  boolean state variable indicating if PLL should be retuned after setting
  linearity

  @retval None.
*/
typedef void (*rf1x_set_linearity_func_ptr_type)
(
  rf1x_device_rx_type *rxdev,
  rfm_cdma_power_mode_type power_mode,
  boolean retune_pll
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Do post-PLL lock processing

  @param *rxdev
  Pointer to rx device for which post-PLL lock processing is to be done

  @retval None.
*/
typedef void (*rf1x_post_pll_lock_processing_func_ptr_type)
(
  rf1x_device_rx_type *rxdev
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Provides generic get and set functionality for the RX device specific
  parameters.

  @details
  It provides IOCTL kind of interface to get and set device specific
  parameters.

  @param *rxdev
  Pointer to rx device for which cmd to be performed.

  @param path
  Logical RF Path for which cmd to be performed.
  
  @param cmd
  Specifies the command to performed by the device.
  
  @param data
  Input and/or Output parameter for the command.

  @pre
  It depends on the 'cmd' parameter. For some commands device specific init()
  function need to be called. For some there may not be any pre-condition prior
  to using this function.
*/
typedef boolean (*rf1x_rx_cmd_dispatch_func_ptr_type) 
(
  rf1x_device_rx_type *rxdev,
  rfdevice_cmd_enum_type cmd,
  void *data
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query if intelliceiver is supported

  @details
  This function will check if the current active RTR driver supports
  intelliceiver on a given receive path.

  @param *rxdev
  Pointer to rx device for which the intelliceiver check to be performed.
*/
typedef boolean (*rf1x_rx_is_ic_supported_func_ptr_type) 
(
  rf1x_device_rx_type *rxdev
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Programs Intelliceiver to trip at the requested switch points
 
  @details
  This function is used to push the intelliceiver calibration data to the
  proper Device driver instance so that the driver can compute the final
  switchpoint to trip the intelliceiver.
 
  @param rx_device
  Pointer to rx device for which the intelliceiver Cal data is to be pushed
 
  @param cal_data
  Pointer to the intelliceiver calibration data to be pushed into the drivee
*/
typedef void (*rf1x_rx_set_ic_cal_data_func_ptr_type) 
(
 rf1x_device_rx_type *rxdev,
 rfdevice_cdma_ic_thres_param_type *cal_data,
 rfm_cdma_band_class_type band,
 rfm_cdma_power_mode_type power_mode
);

/*----------------------------------------------------------------------------*/
/*!
  @details
  Perform Intelliceiver Calibration
 
  @details
  This function will call the current instance of RTR driver to perform
  intelliceive calibration to estimate the the threshold gain at which the
  intelliceiver will trip.
 
  @param rx_device
  Pointer to the current instance of RTR driver.
 
  @param power
  Power at which intelliceiver is calibrated
 
  @return
  Structure containing the intellicever calibration data which contains the
  threshold gain at which the intelliceiver tripped.
*/
typedef rfdevice_cdma_ic_cal_data_type (*rf1x_rx_do_ic_cal_func_ptr_type) 
(
  rf1x_device_rx_type *rxdev,
  int16 power
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Reset Jammer Detector Status
 
  @details
  This function will reset the jammer detector status for the given device.
 
  @param rx_device
  Pointer to current instance of RTR driver for which the jammer detector is
  to be reset.
*/
typedef void (*rf1x_rx_reset_jd_func_ptr_type) 
(
  rf1x_device_rx_type *rxdev
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the LNA Phase offset to be applied for current device mode
 
  @details
  This function will return the appropriate LNA phase offset for the current
  mode of device. The LNA phase offsets returned by this function depends on
  the current band class, power mode, and SHDR mode.
 
  LNA Offsets returned by this function will be in the units of degrees with
  the range of 0 to 360. Thus the MC layer should convert this phase offsets to
  appropriate modem specific HW units before applying.
 
  @param rx_device
  Pointer to current instance of RTR driver for which the LNA phase offset is
  to be applied
 
  @param band
  RF CDMA band for which the LNA phase offset is queried
 
  @param chan
  Channel for which LNA phase offset is queried.
 
  @param power_mode
  Current power mode for which the LNA phase offsets are required.
 
  @param state
  RF CDMA state of receive device. Settings may be different for Diversity vs
  SHDR mode
 
  @param rx_bw_khz
  RX carrier Bandwidth in kHz. Used to select bandwidth specific phase offset
  for MCDO
 
  @param lna_phase
  Container to populate the LNA phase offsets for given configuration
  
  @return
  Flag indicating the validity of LNA phase offsets populated in lna_phase
*/
typedef boolean (*rf1x_rx_get_lna_phase_func_ptr_type) 
(
  rf1x_device_rx_type *rx_device,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan,
  rfm_cdma_power_mode_type power_mode,
  rf_cdma_state_type state,
  uint32 rx_bw_khz,
  rfdevice_cdma_lna_phase_offset_type *lna_phase

);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get intelliceiver State

  @details
  This function will query the current RTR instance to know the current
  intelliceiver state

  @param *rxdev
  Pointer to rx device for which the intelliceiver state to queried.
 
  @return
  Current State of intellicever for given RTR instance
*/
typedef rfm_cdma_power_mode_type (*rf1x_rx_get_power_mode_func_ptr_type) 
(
  rf1x_device_rx_type *rxdev
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get device specific Power Mode configuration

  @details
  This function will query the current RTR instance to know the power mode
  configuration supported by the RF device

  @param *rxdev
  Pointer to rx device for which the power mode config must be returned.
 
  @param band
  Band for which we're requesting power mode configuration
 
  @param rx_bw_khz
  Rx bandwidth for which driver is requesting power mode configuration
 
  @return
  Device Power mode configuration
*/
typedef rfdevice_cdma_ic_power_mode_config_type 
(*rf1x_rx_get_power_mode_config_func_ptr_type) 
(
  rf1x_device_rx_type *rxdev,
  rfm_cdma_band_class_type band,
  uint32 rx_bw_khz
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Provides generic get and set functionality for the TX device specific
  parameters.

  @details
  It provides IOCTL kind of interface to get and set device specific
  parameters.

  @param *txdev
  Pointer to tx device for which cmd to be performed.

  @param path
  Logical RF Path for which cmd to be performed.
  
  @param cmd
  Specifies the command to performed by the device.
  
  @param data
  Input and/or Output parameter for the command.

  @pre
  It depends on the 'cmd' parameter. For some commands device specific init()
  function need to be called. For some there may not be any pre-condition prior
  to using this function.
*/
typedef boolean (*rf1x_tx_cmd_dispatch_func_ptr_type) 
(
  rf1x_device_tx_type *txdev,
  rfdevice_cmd_enum_type cmd,
  void *data
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wake up the RTR from sleep and generate the set of SBI writes
  needed for this operation.

  @details
  Wake up the RTR from sleep and program the RF synthesizer (PLL) based on
  band, channel, and linearity.

  @param path
  RF Path of the device for which PLL to be programmed.

  @param mode
  RF mode of the device: WCDMA, 1X, 1XEVDO, GNSS, etc.

  @param rf_band
  Specifies the RF band the PLL is to be loaded for.

  @param chan
  Specifies the RF uplink channel within the specified RF band.

  @param power_mode
  Specifies the RF power mode: NORMAL, MODE_1, MODE_2, etc.

  @param rf_buffer_intf
  Ptr to the storage which holds generated SBI scripts.
 
  @param script_mode
  Input flag indicating whether the script being generated is for a
  temporary tune away for measurement (IRAT) or for a standalone RX tune.
  This is used to determine if device should set its internal state variables
  to a new state based on this function execution

  @retval None.

  @pre
  RF Device init() must have been called prior to calling this function.

  @post
  RF synthesizer will be tuned for the requested band, channle, and power mode.
*/
typedef void (*rf1x_wakeup_rx_script_func_ptr_type)
(
  rf1x_device_rx_type *rxdev,
  rfm_mode_enum_type mode,
  rfm_cdma_band_class_type rf_band,
  rfm_cdma_chan_type chan,
  uint32 rx_bw_khz,
  uint32 tx_bw_khz,
  rfm_cdma_power_mode_type power_mode,
  rf_buffer_intf *buf_obj_ptr,
  rfdevice_cdma_script_operation_type script_mode
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function put the RTR into sleep.

  @details
  RTR will be placed into sleep mode.

  @param *rxdev
  Pointer to rxdev to be put in sleep

  @param rf_buffer_intf
  Ptr to the storage which holds generated SBI scripts

  @param rx_bw_khz
  Bandwidth of RX baseband (in KHz)

  @param tx_bw_khz
  Bandwidth of TX baseband (in KHz)

  @pre
  RF device init() must be called.

  @post
  Rf Device will be in sleep mode.
*/
typedef void (*rf1x_sleep_script_func_ptr_type) 
(
  rf1x_device_rx_type *rxdev,
  rf_buffer_intf *buf_obj_ptr,
  uint32 rx_bw_khz,
  uint32 tx_bw_khz
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Program the TX PLL based on the band and channel.

  @details
  Programs the RF TX synthesizer (PLL) as per the band and channel.

  @param *txdev
  Pointer to tx device which needs to be tuned

  @param path
  Logical RF Path for which PLL to be programmed.

  @param mode
  RF mode of the device: WCDMA, 1X, 1XEVDO, GNSS, etc.

  @param rf_band
  Specifies the RF band the TX PLL is to be loaded for

  @param chan
  Specifies the RF channel within the specified RF band
 
  @param tx_bw_khz
  Bandwidth of TX baseband in kHz. Used to determine analog RF baseband settings
  of transmit path
 
  @param dpd_state
  Indicates if we need to enable TX path for digital pre-distortion or without
  pre-distortion support. To support digital pre-distortion, RF device may need
  to choose different SSBI settings to widen bandwidth

  @param buf_obj_ptr
  Ptr to the storage which holds generated SBI scripts.

  @retval None.

  @pre
  RF Device init() must have been called prior to calling this function.

  @post
  RF synthesizer will be tuned for the requested band and channel.
*/
typedef void (*rf1x_wakeup_tx_script_func_ptr_type)
(
  rf1x_device_tx_type *txdev,
  rfcom_mode_enum_type mode,
  rfm_cdma_band_class_type rf_band,
  rfm_cdma_chan_type chan,
  uint32 tx_bw_khz,
  rfdevice_dpd_enum_type dpd_state,
  rf_buffer_intf *buf_obj_ptr
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Uses default DA port to tune TX to a particular band/chan
 
  @details
  API to support tuning of TX PLL without any information about DA port to be
  used.
 
  This is an interface that can be used to simply tune TX PLL to a particular
  band/chan. Will be used for TX PLL tuning for FB Rx IQ capture in SV cards

  @param *txdev
  Pointer to tx device which needs to be tuned

  @param path
  Logical RF Path for which PLL to be programmed.

  @param mode
  RF mode of the device: WCDMA, 1X, 1XEVDO, GNSS, etc.

  @param rf_band
  Specifies the RF band the TX PLL is to be loaded for

  @param chan
  Specifies the RF channel within the specified RF band
 
  @param tx_bw_khz
  Bandwidth of TX baseband in kHz. Used to determine analog RF baseband settings
  of transmit path
 
  @param dpd_state
  Indicates if we need to enable TX path for digital pre-distortion or without
  pre-distortion support. To support digital pre-distortion, RF device may need
  to choose different SSBI settings to widen bandwidth

  @param buf_obj_ptr
  Ptr to the storage which holds generated SBI scripts.

  @return Indicates success or failure of the API
*/
typedef boolean (*rf1x_wakeup_tx_default_port_func_ptr_type)
(
  rf1x_device_tx_type *txdev,
  rfcom_mode_enum_type mode,
  rfm_cdma_band_class_type rf_band,
  rfm_cdma_chan_type chan,
  uint32 tx_bw_khz,
  rfdevice_dpd_enum_type dpd_state,
  rf_device_execution_type execution_type,
  rf_buffer_intf *buf_obj_ptr
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disables the Tx mode of operation.

  @param *txdev
  Pointer to txdev to be disabled

  @param path
  Logical RF Path to be disabled.

  @param buf_obj_ptr
  Ptr to the storage which holds generated SBI scripts.

  @pre
  RF device init() must be called.
*/
typedef void (*rf1x_disable_tx_script_func_ptr_type)
(
  rf1x_device_tx_type *txdev,
  rf_buffer_intf *buf_obj_ptr
);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the ET delay compensation to be applied for CDMA

  @details
  Delay compensation value depends on TX BBF RC error, TX process
 
  @param tx_device
  "this" pointer to CDMA Tx device of operation.
 
  @param delay_comp_x10ns
  Delay compensation to be applied in 1/10th of a ns unit
 
  @retval TRUE
  Indicates the API was successful and a correct delay comp value has been
  returned
 
  @retval FALSE
  Indicates API failed due to bad parameters
*/
typedef boolean (*rf1x_tx_get_et_delay_comp_func_ptr_type)
(
  rf1x_device_tx_type *txdev,
  int16* et_delay_comp_x10ns
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Push Rx band specific data to device
  
  @param *rx_device
  "this" pointer to RX device that needs to be configured
 
  @param band
  CDMA band whose data is being pushed
 
  @param data_array
  Array of int32 containing device Rx data for band
 
  @param array_size
  Size of array of int32 data to be pushed to device
*/
typedef boolean (*rf1x_rx_set_band_data_func_ptr_type)
(
  rf1x_device_rx_type* rx_device,
  rfm_cdma_band_class_type band,
  int32* data_array,
  uint8 array_size
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Push Tx band specific data to device
  
  @param *tx_device
  "this" pointer to TX device that needs to be configured
 
  @param band
  CDMA band whose data is being pushed
 
  @param data_array
  Array of int32 containing device Tx data for band
 
  @param array_size
  Size of array of int32 data to be pushed to device
*/
typedef boolean (*rf1x_tx_set_band_data_func_ptr_type)
(
  rf1x_device_tx_type* tx_device,
  rfm_cdma_band_class_type band,
  int32* data_array,
  uint8 array_size
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Push Tx band specific data to device
  
  @param *tx_device
  "this" pointer to TX device that needs to be configured
 
  @param band
  CDMA band whose data is being pushed
 
  @param data_array
  Array of int32 containing device Tx data for band
 
  @param array_size
  Size of array of int32 data to be pushed to device
 
  @param *cdma_tx_pa_lut
  Pointer to array for storing LUT information
*/
typedef boolean (*rf1x_tx_get_lut_data_func_ptr_type)
(
  rf1x_device_tx_type* tx_device,
  rfm_cdma_band_class_type band,
  int32* data_array,
  uint8 array_size,
  rfdevice_cdma_lut_type *cdma_tx_pa_lut
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Create SSBI script for enabling or disabling the Tx chain
  
  @param *tx_device
  "this" pointer to TX device chain which needs to be enabled/disabled
 
  @param rf_on
  TRUE for enabling Tx chain, FALSE for disabling Tx chain
 
  @param script_buffer
  Pointer to the buffer script for storing settings needed
 
*/
typedef boolean (*rf1x_tx_get_rf_on_off_script_func_ptr_type)
(
  rf1x_device_tx_type* tx_device,
  boolean rf_on,
  rf_buffer_intf* script_buffer,
  int16 delay
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Gets script to set LNA to a particular gain state
 
  @param *rx_device
  "this" pointer to receive device whose LNA gain word is queried
 
  @param band
  CDMA band whose LNA gain word is required.
 
  @param power_mode
  Linearity of operation
 
  @param rx_bw_khz
  BW in khz of operation. LNA gain word may be different for single carrier
  vs multi carrier
 
  @param gain_state
  LNA gain state whose script is needed
 
  @param buf
  Pointer to rf script buffer object where LNA gain script will be populated
 
  @retval TRUE
  LNA gain word script was successfully populated for the arguments provided
 
  @retval FALSE
  LNA gain word script population failed! buf ptr was not updated
*/
typedef boolean (*rf1x_rx_get_lna_gain_script_ptr_type)
(
  rf1x_device_rx_type* rx_device,
  rfm_cdma_band_class_type band,
  rfm_cdma_power_mode_type power_mode,
  uint32 rx_bw_khz,
  rfm_lna_gain_state_type gain_state,
  rf_buffer_intf* buf
);

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Create scripts for enabling or disabling Rx
  
  @param *rx_device
  "this" pointer to Rx device chain which needs to be enabled/disabled
 
  @param rx_on
  TRUE for enabling Rx chain, FALSE for disabling Rx chain
 
  @param script_buffer
  Pointer to the buffer script for storing settings needed
*/
typedef boolean (*rf1x_rx_get_rx_on_off_script_ptr_type)
(
  rf1x_device_rx_type* rx_device,
  boolean rx_on,
  rf_buffer_intf* script_buffer,
  int16 time_delay_us
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query if DLNA is supported

  @details
  This function will check if the current active WTR driver supports
  DLNA on a given receive path.

  @param *rxdev
  Pointer to rx device for which the intelliceiver check to be performed.
*/
typedef boolean (*rf1x_rx_is_dlna_supported_func_ptr_type) 
(
  rf1x_device_rx_type *rxdev
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Gets device script to enable or disable Rx chain for discontinuous Rx operation
 
  @details
  This API is not supported by legacy devices - hence adapter class returns
  FALSE

  @param rx_on
  TRUE indicates turning Rx back ON
  FALSE indicates disabling Rx for discontinuous Rx
 
  @param band
  CDMA Band of operation
  
  @param script_buffer
  RF script buffer where device writes have to be appended
 
  @return
  Success/Failure status of API
*/
typedef boolean (*rf1x_rx_discontinuous_rx_script_func_ptr_type) 
(
  rfm_cdma_band_class_type band, 
  boolean rx_on, 
  rf_buffer_intf* script_buffer
);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure of RX interface function pointer 
*/
typedef struct
{
  rf1x_wakeup_rx_script_func_ptr_type wakeup_script_fp;
  /*!< wakeup the RTR from sleep mode */

  rf1x_wakeup_rx_script_func_ptr_type enable_drx_script_fp;

  rf1x_set_linearity_func_ptr_type set_linearity_fp;

  rf1x_post_pll_lock_processing_func_ptr_type post_pll_lock_processing_fp;

  rf1x_sleep_script_func_ptr_type sleep_script_fp;   
  /*!< place the RTR into sleep mode */

  rf1x_sleep_script_func_ptr_type disable_drx_script_fp;
  /*!< Disable Diversity operation */

  rf1x_rx_is_ic_supported_func_ptr_type is_ic_supported_fp;
  /*!< Function to query if intelliceiver is supported  */

  rf1x_rx_get_power_mode_func_ptr_type get_power_mode_fp;
  /*!< Function to query current intelliciever Power Mode */

  rf1x_rx_set_ic_cal_data_func_ptr_type set_ic_cal_data_fp;
  /*!< Function to program intelliceiver calibration data */

  rf1x_rx_do_ic_cal_func_ptr_type do_ic_cal_fp;
  /*!< Function to program intelliceiver calibration data */

  rf1x_rx_get_power_mode_config_func_ptr_type get_power_mode_config_fp;
  /*!< Function to query the device Power Mode configuration */

  rf1x_rx_reset_jd_func_ptr_type reset_jd_fp;
  /*!< Function to reset Jammer Detect Status */

  rf1x_rx_get_lna_phase_func_ptr_type get_lna_phase_offset_fp;
  /*!< Function to query LNA Phase offset */

  rf1x_rx_set_band_data_func_ptr_type set_band_data_fp;
  /*!< Function to push RX band data to device */

  rf1x_rx_cmd_dispatch_func_ptr_type cmd_dispatch_fp;
  /*!< generic IOCTL kind of function */

  rf1x_rx_get_lna_gain_script_ptr_type get_lna_gain_word_fp;
  /*!< Gets LNA gain word from device for a particular band and gain state */

  rf1x_rx_get_rx_on_off_script_ptr_type get_rx_on_off_script;
  /*!< Function to create SSBI script for enabling/disabling Tx chain */

  rf1x_rx_is_dlna_supported_func_ptr_type is_dlna_supported_fp;
  /*!< Function to query if DLNA is supported  */

  rf1x_rx_discontinuous_rx_script_func_ptr_type discontinuous_rx_script_fp;
  /*!< Function to create discontinuous_rx script */

} rfdevice_cdma_rx_func_tbl_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure of TX interface function pointer 
*/
typedef struct
{
  rf1x_wakeup_tx_script_func_ptr_type wakeup_script_fp; /*!< wakeup the RTR from sleep mode */

  rf1x_disable_tx_script_func_ptr_type disable_script_fp; /*!< disable Tx mode */

  rf1x_tx_cmd_dispatch_func_ptr_type cmd_dispatch_fp;
  /*!< generic IOCTL kind of function */

  rf1x_tx_get_lut_table_func_ptr_type get_lut_table_fp;
  /*!< Get TX LUT table */

  rf1x_tx_setup_hdet_func_ptr_type setup_hdet;
  /*!< Execute SSBI to setup device for HDET read */

  rf1x_tx_enable_hdet_func_ptr_type enable_hdet;
  /*!< Enable HDET circuit */

  rf1x_tx_disable_hdet_func_ptr_type disable_hdet;
  /*!< Disable HDET circuit */

  rf1x_tx_read_hdet_func_ptr_type read_hdet;
  /*!< Interface to query current HDET value */

  rf1x_tx_multi_read_hdet_func_ptr_type multi_read_hdet;
  /*!< Interface to sample current HDET value */

  rf1x_tx_get_hdet_param_func_ptr_type get_hdet_param;
  /*!< Interface to query all Tx timing parameter */

  rf1x_tx_get_hdet_scripts_func_ptr_type get_hdet_scripts;
  /*!< Get PDET ON, HKADC trigger and PDET OFF scripts from device */

  rf1x_tx_get_dac_cal_iref_func_ptr_type get_dac_cal_iref;
  /*!< Get Iref for current Tx Band */

  rf1x_tx_get_therm_param_func_ptr_type get_therm_param;
  /*!< Get thermistor timing parameters from device */

  rf1x_tx_get_therm_scripts_func_ptr_type get_therm_scripts;
  /*!< Get thermistor scripts for on/off/trigger hkadc (used in NikeL) */

  rf1x_tx_get_therm_scripts_v2_func_ptr_type get_therm_scripts_v2;
  /*!< Get thermistor scripts for setup therm/on/off/hkadc trigger (Dime) */

  rf1x_tx_read_therm_func_ptr_type read_therm;
  /*!< Read thermistor */

  rf1x_tx_setup_therm_func_ptr_type setup_therm;
  /*!< Setup thermistor for temp comp */

  rf1x_tx_set_band_data_func_ptr_type set_band_data_fp;
  /*!< Function to push TX band data to device */

  rf1x_tx_get_lut_data_func_ptr_type get_lut_data_fp;
  /*!< Function to get PA LUT information from device  */

  rf1x_tx_get_rf_on_off_script_func_ptr_type get_rf_on_off_script;
  /*!< Function to create SSBI script for enabling/disabling Tx chain */

  rf1x_wakeup_tx_default_port_func_ptr_type tune_to_default_port_fp; 
  /*!< Tune tx device to any port. Will be used for Feedback path 
    calibration where port is a dont care */

  rf1x_tx_get_et_delay_comp_func_ptr_type get_et_delay_comp_fp;
  /*!< Returns the ET delay compensation to be applied for CDMA on a device */

} rfdevice_cdma_tx_func_tbl_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF Device Rx related interface function pointers and state variables are
  defined in this typedef

  @details
  A RF Device independent interface will ve provided to Main Control, Core and
  Feature modules through these functions pointers. These function pointers
  will be initialized by the specific RF device whenever there is change in
  mode or band.
*/
typedef struct rfdev_cdma_rx
{
  rf_lock_data_type *critical_sect;

  const rfdevice_id_enum_type device_id;    /*!< RF device being used for rx  */

  const rfdevice_cdma_rx_func_tbl_type* const func_tbl;

  void* const data; /*!< Void pointer to device data which can be device specific */
}rfdevice_cdma_rx_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF Device Tx interface function pointers and state variables are defined in 
  this typedef

  @details
  A RF Device independent interface will ve provided to Main Control, Core and
  Feature modules through these functions pointers. These function pointers
  will be initialized by the specific RF device whenever there is change in
  mode or band.
*/
typedef struct rfdev_cdma_tx
{
  rf_lock_data_type *critical_sect;

  const rfdevice_id_enum_type device_id;  /*!< RF device being used for rx*/

  const rfdevice_cdma_tx_func_tbl_type* const func_tbl;

  void* const data; /*!< Void pointer to device data which can be device specific */
}rfdevice_cdma_tx_type;

/*! 
  @brief
  Type for RF recommendation provided by RTR device
*/
typedef struct
{
  /*! Indicates if there is any recommendation */
  boolean has_reco;
  /*! Desired LO channel offset in KHz */
  int32  lo_offset_KHz;
  /*! Corresponding channel number */
  word   lo_chan;
} rfdevice_rf_reco_type;

/*! 
  @brief
  Type for baseband notch recommendation provided by RTR device
  for each carrier path
*/
typedef struct
{
  /*! Indicates if this structure holds valid information */
  boolean has_valid_info;

  /*! Carrier channel. Serves as an ID for clients to remap notches back to 
      the actually assigned HW carrier path */
  word carrier_chan;

  /*! Number of recommended notches */
  int32  filter_count;

  /*! The list of notch filter configurations that should be applied */
  rxf_notch_filter_config_type notch_filter[ RF_MAX_NUM_NOTCH_FILTERS_AVAIL];

} rfdevice_per_carrier_notch_reco_type;

/*! 
  @brief
  Type for baseband recommendation provided by RTR device
*/
typedef struct
{
  /*! Indicates if this structure holds valid information */
  boolean has_reco;

  /*! Notch recommendation for all carriers on a given path */
  rfdevice_per_carrier_notch_reco_type per_carrier_notch_reco[RF_MAX_NUM_CARRIERS_PER_PATH];

} rfdevice_bb_reco_type;

/*! 
  @brief
  Type for recommendation (for both RF and baseband) provided by RTR device
*/
typedef struct
{
  /*! RF recommendation */
  rfdevice_rf_reco_type       rf;

  /*! Baseband recommendation */
  rfdevice_bb_reco_type       bb;

  /*! flag to enable F3 messages */
  boolean                     debug_msgs_enabled;

} rfdevice_cdma_rx_reco_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold callback data for SSMA notch application */ 
typedef struct 
{
  /*! The device for which notch needs to be applied */
  rfm_device_enum_type device;

  /*! Number of notches that have been used up so far */
  uint8 num_notch_filters_used;

  /*! Clear the currently applied notches on this device */
  boolean clear_notch;

  /*! A flag to indicate when to apply notches 
  to rf cdma tech layer as to  */
  boolean apply_notch_immediate;

  /*! Recommendation from WTR */
  rfdevice_cdma_rx_reco_type ssma_reco;

} rfdevice_cdma_ssma_callback_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Callback function pointer type that is used to for notch programming.
  This callback would be invoked by device driver during tune when spurs are
  identified.
*/

typedef void (*rf_cdma_spur_cb_type)(rfdevice_cdma_ssma_callback_data_type* cb_data);

/*----------------------------------------------------------------------------*/
typedef struct
{
  rfcom_1x_band_type band;
  uint16 chan;
  uint32 freq;
  rfdevice_meas_script_type  *sbi_script_tbl;
} rfdevice_1xhdr_get_irat_meas_tbl_type;

typedef struct
{
  /* Inputs for the "Get Rx Reco" API */
  boolean                      prx_enabled;
  rfm_cdma_band_class_type     prx_band;
  word                         prx_chan;
  uint32                       num_prx_carriers;
  uint16 const                *prx_carrier_chan_list_ptr;
  int32                        prx_bw_khz;
  int32                        tx_bw_khz;
  boolean                      shdr_enabled;
  rfm_cdma_band_class_type     shdr_band;
  word                         shdr_chan;
  /* Outputs from the "Get Rx Reco" API returned through memory allocated by client
     and provided to this API as pointers */
  rfdevice_cdma_rx_reco_type  *prx_reco_ptr;
  rfdevice_cdma_rx_reco_type  *shdr_reco_ptr;
} rfdevice_cdma_get_recos_args_type;

typedef struct
{
  /* Inputs */
  rfcom_1x_band_type band;
  uint16 channel;

  /* Outputs */
  boolean band_chan_is_supported;
} rfdevice_1x_is_band_chan_supported_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Type for adjusting 1x RF Cal data for HDR operation.
*/
typedef struct
{
  /*! Inputs. */
  uint32 bw_khz;

  /*! Input/Output 
    As Input. The values to adjust
    As Output. The adjusted values */
  int16 lna_offset_adj[RFM_LNA_GAIN_STATE_NUM];

  /*! Output. Indicates data validity */
  boolean is_valid;
} rfdevice_hdr_adjust_rf_cal_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Type for droop compensation point.
 
  @details
  A point of this type is composed of (frequency, compensation).
*/
typedef struct
{
  uint32 freq;/*! Frequency */
  int16 compensation;/*! Compensation */
} rfdevice_cdma_droop_compensation_point_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Type for obtaining RF device droop compensation values.
*/
typedef struct
{
  /*! The number of available items */
  uint8 num_items;
  /*! Pointer to compensation items */
  const rfdevice_cdma_droop_compensation_point_type * compensations_ptr;
} rfdevice_cdma_multi_carrier_droop_compensations_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Newer Type for obtaining cdma multi-carrier droop compensation values
*/
typedef struct
{
  /* The number of available items */
  uint8 num_items;
  /* Array stores the mcdo droop data */
  rfdevice_cdma_droop_compensation_point_type data[RFDEVICE_NUM_DROOP_COMP_ENTRIES];
} rfdevice_cdma_multi_carrier_droop_compensations_type_v2;

/*----------------------------------------------------------------------------*/

typedef struct
{
  uint8 count;
  uint8 word[5];
} rfdevice_1x_rx_gain_word_type;

typedef struct
{
  uint8 addr;
  rfdevice_1x_rx_gain_word_type gain_word_info;
} rfdevice_1x_rx_gain_ctl_type;

typedef struct
{
  /* Inputs to "Get communication info" API */
  rfcom_1x_band_type band;
  rf_cdma_carrier_mode_type carrier_mode;
  uint32 bw_khz;
  rfm_cdma_power_mode_type power_mode;

  /* Outputs */
  boolean info_available;
  rfdevice_1x_rx_gain_ctl_type rx_gain_ctl_info;
  uint8 ssbi_bus;
} rfdevice_1x_get_comm_info_args_capsule_type;

typedef struct
{
  rfm_cdma_power_mode_type linearity_mode;
  rfcom_1x_band_type band;
  uint8  rtr_addr;
  uint8  gain_word[4];
} rfdevice_1x_get_rx_gain_word_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief In/out structure to query chipset LNA phase offsets
*/
typedef struct 
{
  /*! Input parameter: Band class desired */
  rfm_cdma_band_class_type     band;
  /*! Input parameter: Power mode desired */
  rfm_cdma_power_mode_type power_mode;
  /*! Ouput parameter: Array of LNA phase offsets for 5 gain states */
  uint16 lna_stage_phase_offset_v[5];
} rfdevice_1x_lna_phase_offset_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief Out structure to query Jammer Detector thresholds
*/
typedef struct 
{
  int32 jd_thresh;
  /*! Jammer detector threshold values in linear units */

  int32 jd_thresh_minus_hyst;
  /*! (Threshold - Hystersis) value in linear units */

  boolean is_valid;	
  /*! Output. Indicates data validity */
} rfdevice_1x_jd_thresh_type;

/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif /* FEATURE_CDMA1X */
#endif /* RFDEVICE_CDMA_INTERFACE_H */

