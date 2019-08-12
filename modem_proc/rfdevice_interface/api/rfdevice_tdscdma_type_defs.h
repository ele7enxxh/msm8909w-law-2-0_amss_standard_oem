/*!
  @file rfdevice_tdscdma_type_defs.h

  @brief
  Provides type definitions needed by the rfdevice_interface.
 
  @details
  This file contains the type definitions used by the rfdevice interface
  functions.

*/

/*============================================================================== 
   
  Copyright (c) 2012 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved 
   
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
   
                        EDIT HISTORY FOR MODULE 
   
This section contains comments describing changes made to the module. 
Notice that changes are listed in reverse chronological order. 
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_tdscdma_type_defs.h#1 $ 
   
when       who     what, where, why 
--------   ---     ------------------------------------------------------------- 
01/23/15   dps     Updated support for new API for NV input to RxRSB calibration
12/15/14   dps     Added support for NV input to RxRSB calibration
07/17/14   ms      TDS Sawless RX support
05/05/14   sd      Support for Legacy drivers
04/30/14   xsh     Added the fbrx rsb cal func pointer
04/15/14   jyu     Added support to configure DC cal
03/15/14   jyu     Added support for double LNA latch
03/13/14   hm      Featurizing FW Decoupling for TDS to get it compiled on DimePM
03/07/14   hm      FW Decoupling
12/10/13   jyu     Updated rfdevice_tdscdma_set_lna_range_type to support 
                   building LNA direct write scripts
11/07/13   nsh     FED RxAGC Feature
09/06/13   jyu     Changed the data type for Rx LNA update
09/06/13   jyu     Added support to build TX_COMM_BSP_BW script
08/01/13   jyu     Added support to build Rx/Tx comm/bsp scripts in advance 
                   for wakeup timeline saving
07/25/13   sma     Added API to return if tx band uses saw or sawless port 
07/19/13   ms      Remove dependency on rfnv_tdscdma.h for max lna gain state
06/03/13   nsh	   Support Get Multi Synth Status in Dime
05/02/13   zg      Made rsb coefficient type consistent with autogen.
04/20/13   jyu     Added RFDEVICE_TDSCDMA_RX_SCRIPT_PRE_BUILD_LNA_SCRIPT
04/14/13   jyu     Added rfdevice_tdscdma_init_device_debug_flag_data type
04/02/13   jyu     Added rfdevice_tdscdma_build_special_bsp_for_irat_data type
03/24/13   jyu     Added support for IFREQ_TO_TX_SCRIPT
03/21/13   ms      Tuner TDET support
03/20/13   zg      Added support for disabling STG
03/15/13   jyu     Added support to put LNA writes into Rx tuning script and 
                   IFREQ meas script
02/13/13   zg      Added STG support on WTR1625
02/13/13   jyu     Added support for WTR2605 where one register control RX_ON 
                   and RF_ON
01/31/13   cd      Added support for FW interface changes needed on WTR2605
01/14/13   cd      Added script enums used by 2605
01/03/13   jyu     Added support to get timed SSBI script from device driver
01/02/13   jyu     Added more script support
01/02/13   jyu     Added support to fetch RX_ON/RF_ON timing from device driver
                   when they use SSBI control
12/05/12   jyu     Added support to return Tx ON/OFF/FAST OFF scripts
                   that incorporates with SSBI control of RF_ON
11/05/12   jyu     Added support to return Tx script fast off script
11/05/12   ms      Added rfdevice_tdscdma_get_irat_buffer_and_script_type
10/03/12   shb     Added rfdevice_tdscdma_rx_build_meas_script_type to RX 
                   function pointer table
09/25/12   jyu     Added rfdevice_tdscdma_set_rf_on and rfdevice_tdscdma_set_rx_on 
                   types 
09/24/12   spa     Added "extern C" check
08/29/12   jyu     Added support to return start_time of Rx BSP 
08/21/12   shb     Fixed offtarget compiler warnings due to type mismatch
08/16/12   shb     Added rfdevice_tdscdma_devices_type
08/16/12   shb     Renamed rfdevice_enum_type to rfdevice_id_enum_type
08/15/12   jyu     Added support to return script buffer  
08/16/12   shb     Include rf_buffer_intf.h for compilation
08/15/12   shb     Fixed compiler errors
08/15/12   shb     Moved all TDSCDMA data structs from rfdevice_tdscdma_intf.h 
                   to rfdevice_tdscdma_type_defs.h  
03/14/12   jyu     Added support to get RF chip information
03/12/12   jyu     Added support to calculate HDET/THERM measurement
                   from raw readings 
03/07/12   jyu     Created new API to obtain TxLUT per band per PA_state 
03/05/12   jyu     Increased the TDSCDMA RF script size to 85
12/15/11   jyu     Added support to more RF scripts for TDSCDMA standalone operation 
10/11/11   jyu     Added device API to query RX gain state register value and 
                   TX slot mode control register 
10/10/11   jyu     Updated RFDEVICE-TDSCDMA framework to support 
                   generic script building 
09/26/11   jyu     Added framework support to build TDSCDMA SSBI scripts 
08/22/11   jyu     Corrected type mismatch 
08/02/11   jyu     Initial version.
==============================================================================*/ 

#ifndef RFDEVICE_TDSCDMA_TYPE_DEFS_H
#define RFDEVICE_TDSCDMA_TYPE_DEFS_H
#ifdef FEATURE_TDSCDMA
#define RF_TDSCDMA_MAX_LNA_STATES 4 /* = RF_TDSCDMA_MAX_LNA_GAIN_STATES */
#endif

/*============================================================================== 
 
                                 INCLUDE FILES
 
==============================================================================*/

#include "rfa_variation.h"
#include "rfcom.h" 
#include "rfdevice_meas_type_defs.h"
#include "rfdevice_cmn_type_defs.h"
#ifdef FEATURE_TDSCDMA
#include "rfmeas_tdscdma.h"
#endif
#include "rfm_path_types.h"
#include "rf_buffer_intf.h"
#include "rfcommon_locks.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FEATURE_TDSCDMA

/*============================================================================== 
 
                               MACROS AND TYPEDEFS 
 
==============================================================================*/

#define RFDEVICE_TDSCDMA_MAX_NUM_OF_SBI 85


/*----------------------------------------------------------------------------*/
typedef enum
{
  RFDEVICE_TDSCDMA_RX_SCRIPT_FOR_IDLE_TO_F1,
  RFDEVICE_TDSCDMA_RX_SCRIPT_FOR_IDLE_TO_F1_FOR_TX_TO_RX,
  RFDEVICE_TDSCDMA_RX_SCRIPT_FOR_IDLE_TO_F1_W_LNA,
  RFDEVICE_TDSCDMA_RX_SCRIPT_FOR_IDLE_TO_F1_FOR_TX_TO_RX_W_LNA,
  RFDEVICE_TDSCDMA_RX_SCRIPT_FOR_IDLE_TO_F2,
  RFDEVICE_TDSCDMA_RX_SCRIPT_FOR_IDLE_TO_F2_W_LNA,
  RFDEVICE_TDSCDMA_RX_SCRIPT_FOR_IDLE_TO_F3,
  RFDEVICE_TDSCDMA_RX_SCRIPT_FOR_IDLE_TO_F3_W_LNA,
  RFDEVICE_TDSCDMA_RX_SCRIPT_OFF_SBI_MODE_SCRIPT, 
  RFDEVICE_TDSCDMA_RX_SCRIPT_ON_SBI_MODE_SCRIPT,
  RFDEVICE_TDSCDMA_RX_SCRIPT_OFF_SBI_MODE_SCRIPT_FOR_RX_TO_TX,
  RFDEVICE_TDSCDMA_RX_SCRIPT_OFF_SCRIPT, 
  RFDEVICE_TDSCDMA_RX_SCRIPT_ENABLE_RX,
  RFDEVICE_TDSCDMA_RX_SCRIPT_RX_TO_OFF_SCRIPT,
  RFDEVICE_TDSCDMA_RX_SCRIPT_IFREQ_BANK_2_TO_1_SCRIPT,
  RFDEVICE_TDSCDMA_RX_SCRIPT_OFF_TO_RX_SCRIPT,
  RFDEVICE_TDSCDMA_RX_SCRIPT_IFREQ_CLEANUP_SCRIPT,
  RFDEVICE_TDSCDMA_RX_SCRIPT_PRE_BUILD_LNA_SCRIPT,
  RFDEVICE_TDSCDMA_RX_SCRIPT_RX_COMM_SCRIPT,
  RFDEVICE_TDSCDMA_RX_SCRIPT_RX_BSP_SCRIPT,
  
  RFDEVICE_TDSCDMA_TX_SCRIPT_FOR_IDLE_TO_F1=30,
  RFDEVICE_TDSCDMA_TX_SCRIPT_FOR_IDLE_TO_F1_FOR_RX_TO_TX,
  RFDEVICE_TDSCDMA_TX_SCRIPT_FOR_IDLE_TO_F3,
  RFDEVICE_TDSCDMA_TX_SCRIPT_OFF_SCRIPT, 
  RFDEVICE_TDSCDMA_TX_SCRIPT_TX_LO_ON_SCRIPT,
  RFDEVICE_TDSCDMA_TX_SCRIPT_FAST_OFF_SCRIPT,
  RFDEVICE_TDSCDMA_TX_SCRIPT_OFF_SBI_MODE_SCRIPT, 
  RFDEVICE_TDSCDMA_TX_SCRIPT_FAST_OFF_SBI_MODE_SCRIPT,
  RFDEVICE_TDSCDMA_TX_SCRIPT_ON_SBI_MODE_SCRIPT,
  RFDEVICE_TDSCDMA_TX_SCRIPT_TXAGC_LATCH_SBI_MODE_SCRIPT,
  RFDEVICE_TDSCDMA_TX_SCRIPT_OFF_SBI_MODE_SCRIPT_FOR_TX_TO_RX,
  RFDEVICE_TDSCDMA_TX_SCRIPT_TX_TO_OFF_SCRIPT,
  RFDEVICE_TDSCDMA_TX_SCRIPT_TX_TO_RX_SCRIPT,
  RFDEVICE_TDSCDMA_TX_SCRIPT_RX_TO_TX_SCRIPT,
  RFDEVICE_TDSCDMA_TX_SCRIPT_TX_TO_TX_SCRIPT,
  RFDEVICE_TDSCDMA_TX_SCRIPT_OFF_TO_TX_SCRIPT,
  RFDEVICE_TDSCDMA_TX_SCRIPT_IFREQ_TO_TX_SCRIPT,
  RFDEVICE_TDSCDMA_TX_SCRIPT_TX_COMM_SCRIPT,
  RFDEVICE_TDSCDMA_TX_SCRIPT_TX_BSP_SCRIPT,
  RFDEVICE_TDSCDMA_TX_SCRIPT_TX_COMM_BSP_BW_SCRIPT,

  RFDEVICE_TDSCDMA_HDET_SCRIPT_HDET_WRITE=60,  
  RFDEVICE_TDSCDMA_HDET_SCRIPT_HDET_READ,

  RFDEVICE_TDSCDMA_THERM_SCRIPT_THERM_WRITE=70,
  RFDEVICE_TDSCDMA_THERM_SCRIPT_THERM_READ,

  RFDEVICE_TDSCDMA_BHO_SCRIPT_TX_F1_TO_TX_F3=80,

  RFDEVICE_TDSCDMA_IFREQ_SCRIPT_FOR_IFREQ_MEAS=90,  
  RFDEVICE_TDSCDMA_IFREQ_SCRIPT_FOR_IFREQ_MEAS_W_LNA,
  RFDEVICE_TDSCDMA_IFREQ_SCRIPT_RX_F2_TO_TX_F1,

  RFDEVICE_IRAT_STARTUP_SCRIPT=100,
  RFDEVICE_IRAT_CLEANUP_SCRIPT,

  RFDEVICE_GET_RX_PLL_STATUS_SCRIPT=110,
  RFDEVICE_GET_TX_PLL_STATUS_SCRIPT,

  RFDEVICE_TDSCDMA_GET_RX_LNA_TRIGGER_SCRIPT,
  RFDEVICE_TDSCDMA_GET_RX_LNA_TRIGGER_SCRIPT_WXE,
  RFDEVICE_TDSCDMA_CONFIG_DC_CAL_SCRIPT,

  RFDEVICE_TDSCDMA_JDET_SCRIPT_JDET_READ=130,
  RFDEVICE_TDSCDMA_IP2_CAL_CODE_WRITE,

  RFDEVICE_TDSCDMA_INVALID_SCRIPT_TYPE
} rfdevice_tdscdma_ssbi_script_type;

/* rfdevice_tdscdma_ssbi_param_type needed only for legacy drivers */
typedef struct
{
  uint16  sbi_counter;
  rfmeas_sbi_type  sbi_buffer[ RFDEVICE_TDSCDMA_MAX_NUM_OF_SBI ];
} rfdevice_tdscdma_ssbi_param_type;

typedef struct
{
  rfcom_tdscdma_band_type band;
  uint16 channel;
  boolean drx_enable;
  uint8 path;
  uint8 gain_state;
  rfdevice_tdscdma_ssbi_script_type script_type;
  boolean scrip_built_success;  
  void* ssbi_tbl_ptr;
  boolean use_rf_buffer_intf_type; 
  uint16 last_write_pos;
  boolean calc_time_w_ccs_task_switch_delay;
  boolean enable_dc_cal;
  uint8 rx_lin_state;
  uint8 jdet_sample;
}rfdevice_tdscdma_get_ssbi_script_type;

typedef struct
{ 
  rfm_meas_tdscdma_setup_and_build_scripts_param_type* build_script_info_ptr;
  rfdevice_tdscdma_ssbi_script_type script_type;  
  boolean script_built_success;  
  rfdevice_meas_script_type *sbi_script_tbl; 
  uint16 last_write_pos;
}rfdevice_tdscdma_get_irat_script_type;


typedef struct
{ 
  rfcom_tdscdma_band_type band;
  uint8 lna_range;
  boolean w_latch;

  // Primary chain
  boolean program_pri_chain;
  rf_device_execution_type pri_chain_action;
  rf_buffer_intf* pri_chain_buf;

  // Secondary chain
  boolean program_sec_chain;
  rf_device_execution_type sec_chain_action;
  rf_buffer_intf* sec_chain_buf;

  uint8 lin_state;

  boolean is_irat;

}rfdevice_tdscdma_set_lna_range_type;


typedef struct
{
  rfdevice_tdscdma_get_irat_script_type rfdevice_get_irat_script_table;
  rf_buffer_intf* rftdscdma_mc_meas_script_buffer_ptr;
}rfdevice_tdscdma_get_irat_buffer_and_script_type;


typedef struct
{
  rfcom_tdscdma_band_type band;
  uint8 lut_idx;
  uint8 pa_range;
  uint8 slot_index;
} rfdevice_tdscdma_lut_idx_pa_range_type;

typedef struct
{
  rfcom_tdscdma_band_type band;
  uint32 val;
  uint8 slot_index;
} rfdevice_tdscdma_lut_addr_type;

typedef enum
{
  RFDEVICE_TDSCDMA_OFF_TO_RX_TRANSITION,
  RFDEVICE_TDSCDMA_RX_TO_OFF_TRANSITION,
  RFDEVICE_TDSCDMA_OFF_TO_TX_TRANSITION,
  RFDEVICE_TDSCDMA_TX_TO_OFF_TRANSITION,
  RFDEVICE_TDSCDMA_RX_TO_TX_TRANSITION,
  RFDEVICE_TDSCDMA_TX_TO_RX_TRANSITION,
  RFDEVICE_TDSCDMA_TX_TO_TX_TRANSITION,

  RFDEVICE_TDSCDMA_INVALID_EVENT_TRANSITION_TYPE
} rfdevice_tdscdma_event_transition_type;

typedef struct
{
  rfcom_tdscdma_band_type band;
  boolean rf_on;
  rf_device_execution_type execution_type;
  rf_buffer_intf* script_buffer;
  boolean w_time;
  rfdevice_tdscdma_event_transition_type transition_type;
  boolean is_rf_on_ssbi;
  boolean is_rx_on_ssbi;
} rfdevice_tdscdma_set_rf_on_type;

typedef struct
{
  rfcom_tdscdma_band_type band;
  boolean rx_on;
  rf_device_execution_type execution_type;
  rf_buffer_intf* script_buffer;
  boolean w_time;
  rfdevice_tdscdma_event_transition_type transition_type;
  boolean is_rx_on_ssbi;
  boolean is_rf_on_ssbi;
} rfdevice_tdscdma_set_rx_on_type;

typedef struct
{
  uint8 sbi_bus;
  rfm_path_enum_type path;
  rfcom_tdscdma_band_type band;  
  uint32 data[RF_TDSCDMA_MAX_LNA_STATES];
  uint32 addr[RF_TDSCDMA_MAX_LNA_STATES]; 
}rfdevice_tdscdma_get_rx_lna_state_type; 

typedef struct
{
  uint8 slot_mode_ctrl_reg_bus;
  uint8 slot_mode_ctrl_reg_addr;
  uint8 number_of_slot_ctrl_modes;
  uint8 slot_mode_ctrl_reg_value[7];
} rfdevice_tdscdma_get_tx_slot_mode_ctrl_info_type;

typedef struct
{
  rfdevice_dac_iref_val iref_val;
  rfcom_tdscdma_band_type band; 
} rfdevice_tdscdma_dac_cal_iref_type;

typedef struct
{
  rfcom_tdscdma_band_type band;  
  uint8 pa_state;  
  uint8 number_of_tx_char_tbl_entry;
  const uint32 *tx_char_tbl_reg_values;
} 
rfdevice_tdscdma_get_tx_char_tbl_type;


typedef struct
{
  uint8   total_number;  
  uint8*  hdet_raw_reading;  
  boolean hdet_converg_flag;
  uint16  hdet_avg_measurement;    
} 
rfdevice_tdscdma_calc_hdet_measurement_type;

typedef struct
{
  uint8   total_number;  
  uint8*  therm_raw_reading;  
  boolean therm_converg_flag;
  uint16  temperature_measurement;    
} 
rfdevice_tdscdma_calc_therm_measurement_type;

typedef struct
{
  uint8  chip_revision;  
  uint8  chip_process;  
  uint16 rx_pll_lock_time_in_microsec;
  uint16 tx_pll_lock_time_in_microsec;
} 
rfdevice_tdscdma_get_rf_chip_info_type;

/*! 
  @brief
  Calibration data stored in NV item after RSB cal per band, path, bw
*/
typedef struct
{
  /*! Indicates if cal was performed successfully and hence NV
    for the band/path is active */
  boolean nv_active;

  /*! Sine theta estimate at room temp */
  int16 sin_theta;

  /*! Cosine theta estimate at room temp */
  int16 cos_theta;

  /*! Gain inv estimate at room temp */
  int16 gain_inv;

}rfdevice_tdscdma_rx_rsb_cal_data_type;

/*TDSCDMA structures to access device information*/

typedef struct
{
  int32 rsb_a;
  int32 rsb_b;
  rfcom_multi_carrier_hspa_id_type num_chan;
  rfcom_tdscdma_band_type band; 
  uint16 therm_value;
} 
rfdevice_tdscdma_rx_rsb_type;

typedef struct
{
  uint32 pll_val;
  rfcom_tdscdma_band_type band; 
} rfdevice_tdscdma_d_pll_data_type;


typedef struct
{
  uint8  lut_index;
  uint8  pa_range;
  rfcom_tdscdma_band_type band;
  uint32 lut_ssbi_data;
}rfdevice_tdscdma_lut_data_type;

typedef struct
{
  rfcom_tdscdma_band_type band;
  int16 txon_enable_exec_time;
  int16 txon_disable_exec_time;
  int16 rxon_enable_exec_time;
  int16 rxon_disable_exec_time;
  int16 tx_tx_txon_enable_exec_time;
  int16 tx_tx_txon_disable_exec_time;
  int16 tx_rx_txon_enable_exec_time;
  int16 tx_rx_txon_disable_exec_time;
}rfdevice_tdscdma_rx_on_rf_on_ssbi_timing_info_type;


typedef struct
{
  rfcom_tdscdma_band_type band;
  uint16 chan;
  void* ssbi_tbl_ptr;
  boolean use_rf_buffer_intf_type; 
}rfdevice_tdscdma_build_special_bsp_for_irat_data_type;

typedef struct
{
  boolean rfdevice_tdscdma_enable_ssbi_debug_flag;
  boolean rfdevice_tdscdma_enable_hdet_debug_flag;
  boolean rfdevice_tdscdma_disable_config_dc_cal_flag;
}rfdevice_tdscdma_init_device_debug_flag_data_type;

typedef struct
{
  rfcom_tdscdma_band_type band;
  boolean dc_cal_enable;
  rf_device_execution_type execution_type;
  rf_buffer_intf* script_buffer;
} rfdevice_tdscdma_config_dc_cal_data_type;



/*---------------------------------------------------------------------------*/
/*!
  @brief
  Forward declaration of device structure types to use in function pointers
  definitions
*/
typedef struct rftdscdma_rxdev rftdscdma_device_rx_type;
typedef struct rftdscdma_txdev rftdscdma_device_tx_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  It places the RF Rx Device to its default state.

  @details
  It resets the chip and initializes Rx device registers for the idle state.
  This function must be called at power up and at anytime the
  device needs to be placed in its default state.

  @param *rxdev 
  Pointer to rx device which needs to be initialized
  @param path
  Logical RF Path to be initialized.
  @param mode
  Specifies the mode(technology) of operation.
 
  @param execution_type 
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script. 
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally 

  @retval

  @pre
  SBI clock regime must be enabled.

  @post
  RTR will be in its default state.
*/
typedef void (*rfdevice_tdscdma_rx_init_func_ptr_type)
(
  rftdscdma_device_rx_type *rxdev,
  rf_path_enum_type path,
  rfcom_mode_enum_type mode,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  It configures the RF Rx Device for the requested mode of operation.

  @details
  It configures the RF Rx device chain for the specified mode or technology.
 
  @param *rxdev
  Pointer to rx device which needs to be enabled
  @param path
  Logical RF Path to be enabled
  @param mode
  Specifies the mode(technology) of operation
 
  @param execution_type 
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script. 
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally

  @retval 

  @pre
  SBI clock regime must be enabled.

  @post
  RTR will be in its default state.
*/
typedef void (*rfdevice_tdscdma_enter_mode_rx_func_ptr_type)
(
  rftdscdma_device_rx_type *rxdev,
  rf_path_enum_type path,
  rfcom_mode_enum_type mode,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer 
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Programs the RTR for Rx mode of operation.

  @details
  Places the RTR into Rx mode of operation.

  @param *rxdev
  Pointer to rx device to be enabled
  @param path
  Logical RF Path to be enabled
 
  @param execution_type 
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script. 
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally

  @retval 

  @pre
  RF device tune_to_chan() must be called.

  @post
  RF device will be placed in Rx mode of operation
*/
typedef void (*rfdevice_tdscdma_enable_rx_func_ptr_type)
(
  rftdscdma_device_rx_type *rxdev,
  rf_path_enum_type path,
  rfcom_tdscdma_band_type rf_band,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Programs the Rx PLL based on the band and channel.

  @details
  Programs the RF Rx synthesizer (PLL) as per the band and channel.
 
  @param *rxdev
  Pointer to rx device which needs to be tuned
  @param path
  Logical RF Path for which PLL to be programmed.
  @param band   
  Specifies the RF band the PLL is to be loaded for
  @param chan   
  Specifies the RF uplink channel within the specified RF band
 
  @param execution_type 
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script. 
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally

  @retval 

  @pre
  RF Device init() must have been called prior to calling this function.

  @post
  RF synthesizer will be tuned for the requested band and channel.
*/
typedef void (*rfdevice_tdscdma_tune_to_chan_rx_func_ptr_type)
(
  rftdscdma_device_rx_type *rxdev,
  rf_path_enum_type path,
  rfcom_mode_enum_type mode,
  rfcom_tdscdma_band_type rf_band,
  uint16 chan,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Program the PLL based on the band and channel.

  @details
  Programs the RF synthesizer (PLL) as per the band and channel.

  @param path 
  RF Path of the device for which PLL to be programmed.
  @param band
  Specifies the RF band the PLL is to be loaded for
  @param chan[]
  Channel list
 
  @param execution_type 
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script. 
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally 

  @retval None.

  @pre
  RF Device init() must have been called prior to calling this function.

  @post
  RF synthesizer will be tuned for the requested band and channle.
*/
typedef void (*rfdevice_tdscdma_tune_to_multi_chan_func_ptr_type)
(
  rftdscdma_device_rx_type *rxdev,
  rf_path_enum_type path,
  rfcom_mode_enum_type mode,
  rfcom_tdscdma_band_type rf_band,
  uint16 chan[],
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Provides generic get and set functionality for the Rx device specific
  parameters.

  @details
  It provides IOCTL kind of interface to get and set Rx device specific
  parameters.
 
  @param *rxdev
  Pointer to rx device for which cmd to be performed
  @param path
  Logical RF Path for which cmd to be performed
  @param cmd
  Specifies the command to be performed by the Rx device
  @param data
  Input and/or Output parameter for the command

  @retval ret_val
  generic return value which depends on the 'cmd' parameter.

  @pre
  It depends on the 'cmd' parameter. For some commands device specific init()
  function need to be called. For some there may not be any pre-condition prior
  to using this function.

  @post

*/
typedef int32 (*rfdevice_tdscdma_rx_cmd_dispatch_func_ptr_type)
(
  rftdscdma_device_rx_type *rxdev,
  rf_path_enum_type path,
  rfdevice_cmd_enum_type cmd,
  void *data
);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function puts the RTR into sleep.

  @details
  RTR will be placed into sleep mode.

  @param *rxdev
  Pointer to rx device to wakeup
  @param path
  Logical RF Path to be put into sleep mode.
 
  @param execution_type 
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script. 
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally
  @retval None

  @pre
  RF device init() must be called.

  @post
  Rf Device will be in sleep mode.
*/
typedef void (*rfdevice_tdscdma_sleep_rx_func_ptr_type) 
( 
  rftdscdma_device_rx_type *rxdev, 
  rf_path_enum_type path,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  It cleans up all rfdevice RX state variables.

  @details
  It cleans up all rfdevice RX state variables.
 
  @param *rxdev 
  Pointer to rx device which needs to exit.
  @param path
  Logical RF Path to exit.
  @param mode
  Specifies the mode(technology) of operation.
 
 
  @retval

  @pre  

  @post
  
*/
typedef void (*rfdevice_tdscdma_rx_exit_func_ptr_type)
(
  rftdscdma_device_rx_type *rxdev,
  rf_path_enum_type path,
  rfcom_mode_enum_type mode,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  It places the RF Tx Device to its default state.

  @details
  It resets the chip and initializes Tx device registers for the idle state.
  This function must be called at power up and at anytime the
  device needs to be placed in its default state.
 
  @param *txdev 
  Pointer to tx device which needs to be initialized
  @param path
  Logical RF Path to be initialized.
  @param mode
  Specifies the mode(technology) of operation.
 
  @param execution_type 
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script. 
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally
  @retval

  @pre
  SBI clock regime must be enabled.

  @post
  RTR will be in its default state.
*/
typedef void (*rfdevice_tdscdma_tx_init_func_ptr_type)
(
  rftdscdma_device_tx_type *txdev,
  rf_path_enum_type path,
  rfcom_mode_enum_type mode,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  It configures the RF Tx Device for the requested mode of operation.

  @details
  It configures the RF Tx device chain for the specified mode or technology.
 
  @param *txdev
  Pointer to tx device which needs to be enabled
  @param path
  Logical RF Path to be enabled
  @param mode
  Specifies the mode(technology) of operation

  @param execution_type 
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script. 
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internall 
 
  @retval 

  @pre
  SBI clock regime must be enabled.

  @post
  RTR will be in its default state.
*/
typedef void (*rfdevice_tdscdma_enter_mode_tx_func_ptr_type)
(
  rftdscdma_device_tx_type *txdev,
  rf_path_enum_type path,
  rfcom_mode_enum_type mode,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Programs the RTR for Tx mode of operation.

  @details
  Places the RTR into Tx mode of operation.
 
  @param *txdev
  Pointer to tx device to be enabled
  @param path
  Logical RF Path to be enabled
 
  @param execution_type 
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script. 
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internall
 
  @retval 

  @pre
  RF device tune_to_chan() must be called.

  @post
  RF device will be placed in Tx mode of operation
*/
typedef void (*rfdevice_tdscdma_enable_tx_func_ptr_type)
(
  rftdscdma_device_tx_type *txdev,
  rf_path_enum_type path,
  rfcom_tdscdma_band_type rf_band,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Programs the Tx PLL based on the band and channel.

  @details
  Programs the RF Tx synthesizer (PLL) as per the band and channel.

  @param *txdev
  Pointer to tx device which needs to be tuned
  @param path
  Logical RF Path for which PLL to be programmed.
  @param band   
  Specifies the RF band the PLL is to be loaded for
  @param chan   
  Specifies the RF uplink channel within the specified RF band
 
  @param execution_type 
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script. 
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally
 
  @retval 

  @pre
  RF Device init() must have been called prior to calling this function.

  @post
  RF synthesizer will be tuned for the requested band and channel.
*/
typedef void (*rfdevice_tdscdma_tune_to_chan_tx_func_ptr_type)
(
  rftdscdma_device_tx_type *txdev,
  rf_path_enum_type path,
  rfcom_mode_enum_type mode,
  rfcom_tdscdma_band_type rf_band,
  uint16 chan,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Provides generic get and set functionality for the Tx device specific
  parameters.

  @details
  It provides IOCTL kind of interface to get and set Tx device specific
  parameters.

  @param *txdev
  Pointer to tx device for which cmd to be performed
  @param path
  Logical RF Path for which cmd to be performed
  @param cmd
  Specifies the command to be performed by the Tx device
  @param data
  Input and/or Output parameter for the command

  @retval ret_val
  generic return value which depends on the 'cmd' parameter.

  @pre
  It depends on the 'cmd' parameter. For some commands device specific init()
  function need to be called. For some there may not be any pre-condition prior
  to using this function.

  @post

*/
typedef int32 (*rfdevice_tdscdma_tx_cmd_dispatch_func_ptr_type)
(
  rftdscdma_device_tx_type *txdev,
  rf_path_enum_type path,
  rfdevice_cmd_enum_type cmd,
  void *data
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disables the Rx or Tx mode of operation.

  @details
  When the RTR is in the Rx Mode, the disable operation is not applicable to
  primary chain.
  When the RTR is in Tx mode, Tx will be disabled and if the RTR is only used
  for Tx then the RTR will be placed into sleep mode.
  When the RTR is RxTx mode, first Tx disbale should be called before disabling
  the Rx.

  @param *txdev
  Pointer to rx device to wakeup
  @param path
  Logical RF Path to be disabled.
 
  @param execution_type 
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script. 
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally
 
  @retval None

  @pre
  RF device init() must be called.

  @post
  Rf Device will be placed either Rx or Tx or Sleep mode of operation
  depending on the use of RTR.
*/
typedef void (*rfdevice_tdscdma_disable_tx_func_ptr_type) 
(  
  rftdscdma_device_tx_type *txdev, 
  rf_path_enum_type path,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  TDSCDMA measurement script API for DIME using rf_buffer_intf

  @details
  This function prepares the sbi measurement script for tdscdma mode. The
  functionality provided by this API is similar to the command dispatch:
  RFDEVICE_GET_MEAS_START_SCRIPTS except that this function populates the
  script to a rf_buffer_intf object
 
  @param *rxdev
  "this" pointer to Rx device whose data is to be extracted

  @param ssbi_script_param
  struct provides the necessary parameters like current band, target band,
  etc for building meas script

  @param buf
  Pointer to rf_buffer_intf object that will be populated with meas script
*/
typedef void (*rfdevice_tdscdma_rx_build_meas_script_type)
(
  rftdscdma_device_rx_type *rxdev,
  rfdevice_tdscdma_get_irat_script_type * ssbi_script_param,
  rf_buffer_intf* buf
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  It cleans up all rfdevice TX state variables.

  @details
  It cleans up all rfdevice TX state variables.
 
  @param *txdev 
  Pointer to tx device which needs to exit.
  @param path
  Logical RF Path to exit.
  @param mode
  Specifies the mode(technology) of operation.

  @retval

  @pre  

  @post
  RFDEVICE will be in its default state.
*/
typedef void (*rfdevice_tdscdma_tx_exit_func_ptr_type)
(
  rftdscdma_device_tx_type *txdev,
  rf_path_enum_type path,
  rfcom_mode_enum_type mode,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
 
  @param rfm_dev
 
  @param *rx_device
  
  @param band
 
  @param *data
 
  @param exec_type
  enum to indicate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script
 
  @return
  NULL 
*/
typedef boolean (*rfdevice_tdscdma_rx_config_stg_func_ptr_type)
(
  rfm_device_enum_type rfm_dev, 
  rftdscdma_device_rx_type *rx_device,
  rfcom_tdscdma_band_type band, 
  void *data,
  rf_device_execution_type exec_type, 
  rf_buffer_intf* script_buffer 
); 

/*----------------------------------------------------------------------------*/
/*!
  @brief
 
  @param rfm_dev
 
  @param *rx_device
  
  @param band
 
  @param *channel
 
  @param *data_valid_status
 
  @param exec_type
  enum to indicate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script
 
  @return
  NULL 
*/
typedef boolean (*rfdevice_tdscdma_rx_get_stg_chan_func_ptr_type)
(
  rfm_device_enum_type rfm_dev, 
  rftdscdma_device_rx_type *rx_device,
  rfcom_tdscdma_band_type band, 
  uint16 *channel,
  boolean *data_valid_status, 
  rf_device_execution_type exec_type, 
  rf_buffer_intf* script_buffer 
); 

typedef boolean (*rfdevice_tdscdma_rx_disable_stg_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rftdscdma_device_rx_type *rx_device,
  rf_device_execution_type exec_type,
  rf_buffer_intf* script_buffer
);

typedef boolean (*rfdevice_tdscdma_rx_get_rsb_nv_params_func_ptr_type)
( 
  rfm_device_enum_type rfm_dev,
  rftdscdma_device_rx_type *rx_device,
  rfcom_tdscdma_band_type rf_band, 
  uint64   i_sq,
  uint64   q_sq,
  int64    i_q,
  uint16   therm_data, 
  int16*   sin_theta_final, 
  int16*   cos_theta_final, 
  int16*   gain_inv_final
//  boolean* data_valid_status 
);

/*----------------------------------------------------------------------------*/
/*!
  @brief: Get fbrx rsb nv params
 
  @param rfm_dev: RFM Device for which RSB params are desired
 
  @param *tx_device: "this" pointer to Tx Device pertaining to the current instance of device 
   driver, FB path is assocuated with that Tx device.
 
  @param rf_band: Band in use
 
   @param *rsb_data
  pointer to rsb data type, input I/Q, output the final mismathed sin_theta, cos_theta and gain_inv
  
*/
typedef boolean (*rfdevice_tdscdma_fbrx_calc_rsb_nv_params_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rftdscdma_device_tx_type *tx_device,
  rfcom_tdscdma_band_type rf_band,
  uint8 tx_bw, // not used for tds, but added it for consist with othet tech and for potential future use
  rfdevice_rsb_cal_data_type *rsb_data
);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Setup HDET for read 

  @details
 
  @param tx_device
  Pointer to the tdscdma device which needs to be programmed
 
  @param exec_type
  enum to indicate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script
 
  @return
  NULL 
*/
typedef boolean (*rfdevice_tdscdma_tx_hdet_setup_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rftdscdma_device_tx_type *tx_device,
  rf_device_execution_type exec_type,
  rf_buffer_intf* script_buffer, 
  void* data
); 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Read current HDET value 

  @details
  
  @param tx_device
  Pointer to the tdscdma device which needs to be programmed
 
  @param mdsp_triggered_read
  boolean to differentiate mdsp triggered read versus non-triggered read
 
  @param hdet_value
  uint16 variable to store the current hdet value 
 
  @param exec_type
  enum to indicate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script
 
  @return
  NULL
*/

//uint16 used so that wtr hdet read works
typedef uint16 (*rfdevice_tdscdma_tx_hdet_read_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rftdscdma_device_tx_type *tx_device,
  boolean mdsp_triggered_read, 
  uint16 *hdet_value,
  rf_device_execution_type exec_type,
  rf_buffer_intf* script_buffer 
);


/*----------------------------------------------------------------------------*/
typedef void (*rfdevice_tdscdma_calc_hdet_measurement_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rftdscdma_device_tx_type *tx_device,
  rfdevice_tdscdma_calc_hdet_measurement_type* calc_hdet_meas
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query API to indicate if a particular TDSCDMA band uses SAW or SAWless port on
  the device

  @param tx_device
  "this" pointer to tx device whose port info is queried
 
  @param rfm_device
  Logical device for which port info is queried
 
  @param band
  TDSCDMA rfcom band whose port info is needed
 
  @retval RFDEVICE_TX_INVALID_PORT
  Either the band is invalid or not configured in this device yet
 
  @retval RFDEVICE_TX_SAWLESS_PORT
  Band does not use Saw port in device
 
  @retval RFDEVICE_TX_SAW_PORT
  Band uses port with saw on this device
*/
typedef rfdevice_tx_saw_port_type (*rfdevice_tdscdma_tx_get_port_func_ptr_type)
(
  rftdscdma_device_tx_type* tx_device,
  rfm_device_enum_type rfm_device,
  rfcom_tdscdma_band_type band
);



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure of RX interface function pointer 
*/
typedef struct
{
  rfdevice_tdscdma_rx_init_func_ptr_type  init_fp;  /*!< to place the RTR into default state */
  rfdevice_tdscdma_enter_mode_rx_func_ptr_type enter_mode_fp; /*!< configure the RTR for the mode */
  rfdevice_tdscdma_enable_rx_func_ptr_type enable_fp;  /*!< to place the RTR into Rx mode */
  rfdevice_tdscdma_tune_to_chan_rx_func_ptr_type tune_to_chan_fp; /*!< Program the PLL for specific band and chan */
  rfdevice_tdscdma_tune_to_multi_chan_func_ptr_type tune_to_multi_chan_fp; /*!< Program the PLL for DC */
  rfdevice_tdscdma_sleep_rx_func_ptr_type sleep_fp;   /*!< place the RTR into sleep mode to save current */
  rfdevice_tdscdma_rx_cmd_dispatch_func_ptr_type cmd_dispatch_fp; /*!< generic IOCTL kind of function */
  rfdevice_tdscdma_rx_exit_func_ptr_type exit_fp; /*!< rx exit function */
  rfdevice_tdscdma_rx_build_meas_script_type build_meas_script_fp; /*!< Populates meas script in rf_buffer_intf */
  rfdevice_tdscdma_rx_get_stg_chan_func_ptr_type get_stg_chan_fp;  /*!< Used to get channel for RSB cal */
  rfdevice_tdscdma_rx_config_stg_func_ptr_type config_stg_fp;  /*!< Used to setup single tone generator for RSB cal */
  rfdevice_tdscdma_rx_disable_stg_func_ptr_type disable_stg_fp; /*!< Used to disable stg */  
  rfdevice_tdscdma_rx_get_rsb_nv_params_func_ptr_type get_rsb_nv_params_fp;  /*!< Used to get rsb NV params */
} rftdscdma_rxdev_func_tbl_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure of TX interface function pointer 
*/
typedef struct
{
  rfdevice_tdscdma_tx_init_func_ptr_type  init_fp;  /*!< to initialize the RF device for TDSCDMA Tx */
  rfdevice_tdscdma_enter_mode_tx_func_ptr_type enter_mode_fp; /*!< configure the RTR for the mode */
  rfdevice_tdscdma_enable_tx_func_ptr_type enable_fp;    /*!< to put the RF device into TDSCDMA Tx mode  */
  rfdevice_tdscdma_tune_to_chan_tx_func_ptr_type tune_to_chan_fp; /*!< to program the Tx PLL for a given TDSCDMA ARFCN */
  rfdevice_tdscdma_disable_tx_func_ptr_type disable_fp;           /*!< to disable TDSCDMA Tx */
  rfdevice_tdscdma_tx_hdet_setup_func_ptr_type setup_hdet_fp; /*!< to perform setup hdet */
  rfdevice_tdscdma_tx_hdet_read_func_ptr_type read_hdet_fp;   /*!< to perform hdet read */
  rfdevice_tdscdma_calc_hdet_measurement_func_ptr_type calc_hdet_measurement_fp; /*!< to perform calc hdet measurement */
  rfdevice_tdscdma_tx_cmd_dispatch_func_ptr_type cmd_dispatch_fp; /*!< generic IOCTL kind of function */
  rfdevice_tdscdma_tx_exit_func_ptr_type exit_fp; /*!< tx exit function */
  rfdevice_tdscdma_tx_get_port_func_ptr_type get_port_type_fp;/*!< Returns enum indicating 
                                                    whether the TX port used for the band on the 
                                                    device is saw or sawless */
  rfdevice_tdscdma_fbrx_calc_rsb_nv_params_func_ptr_type calc_rsb_nv_params_fp;
														/*!< Used to calculate and write FBRx RSB params */
                                                    
} rftdscdma_txdev_func_tbl_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF Device Rx related interface function pointers.

  @details
  A RF Device independent interface will be provided to Main Control, Core and
  Feature modules through these functions pointers. These function pointers
  will be initialized by the specific RF device whenever there is change in
  mode or band.
*/
struct rftdscdma_rxdev
{
  rf_lock_data_type *critical_sect;

  rfdevice_id_enum_type device_id;    /*!< RF device being used for rx  */

  const rftdscdma_rxdev_func_tbl_type *func_tbl;

  void *data; /*!< Void pointer to device data which can be device specific */
};

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF Device Rx related interface function pointers.

  @details
  A RF Device independent interface will be provided to Main Control, Core and
  Feature modules through these functions pointers. These function pointers
  will be initialized by the specific RF device whenever there is change in
  mode or band.
*/
struct rftdscdma_txdev
{
  rf_lock_data_type *critical_sect;

  rfdevice_id_enum_type device_id;  /*!< RF device being used for Tx */

  const rftdscdma_txdev_func_tbl_type *func_tbl;

  void *data;  /*!< Void pointer to device data which can be device specific */
} ;

typedef struct
{
  /*! Primary TDSCDMA receive device (on physical path 0) */
  rftdscdma_device_rx_type* rx0_device;

  /*! Secondary TDSCDMA receive device (on physical path 1) */
  rftdscdma_device_rx_type* rx1_device;

  /*! TDSCDMA transmit device (on physical path 0) */
  rftdscdma_device_tx_type* tx_device;

} rfdevice_tdscdma_devices_type;


/*----------------------------------------------------------------------------*/
extern rftdscdma_device_rx_type *rftdscdma_rx_device[RFDEVICE_RX_PATH_NUM];

/*----------------------------------------------------------------------------*/
extern rftdscdma_device_tx_type *rftdscdma_tx_device[RFDEVICE_TX_PATH_NUM];

#endif /* FEATURE_TDSCDMA */

#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_TDSCDMA_TYPE_DEFS_H */

