/*!
  @file rfdevice_wcdma_type_defs.h

  @brief
  Provides type definitions needed by the rfdevice_interface.
 
  @details
  This file contains the type definitions used by the rfdevice interface
  functions.

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
   
                        EDIT HISTORY FOR MODULE 
   
This section contains comments describing changes made to the module. 
Notice that changes are listed in reverse chronological order. 
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_wcdma_type_defs.h#1 $ 
   
when       who     what, where, why 
--------   ---     ------------------------------------------------------------- 
05/15/15   ry      Device interface updates for RFC re-design for split Rx/Tx
02/27/15   aks     Device driver updates to support tuning Tx chain to Rx freq
12/12/14   dps     Added support for NV input to RxRSB calibration
11/06/14  aks/tks  Updates to format of WCDMA CB API
10/06/14   tks     Changed the notch params structure 
09/19/14   tks     Added support for db-dc spur mitigation
07/16/14   sma     Added Rx RSB temp comp per gain state support
05/15/14   aro     Interface to hold WTR ADC read data
03/07/14   hm      FW Decoupling
02/11/13   ak      Add cmd RFDEVICE_WCDMA_GET_RX_CSR_CORRECTION_PARAMS
11/28/13   tks     Added WCDMA tx and rx tune consolidated params structure
11/28/13   tks     Added WCDMA bw enumeration type
11/11/13   tks     Added extra parameter to determine meas scenario
07/08/13   sma     Added API for rsb cal and rsb temp comp support
06/24/13   shb     Added API to get ET delay compensation
05/21/13   rmb     Added device API to get Tx PLL lock status and  
                   add RF band information to Rx PLL Lock status API. 
03/14/13   tks     Added API to disable single tone generator
03/13/13   shb     Added API to pre-configure device with common SSBI setttings 
                   for IRAT across different physical devices
03/08/13   shb     Implemented tune_to_default_port_fp for ET FB capture support 
                   on SV cards
03/02/13   tks     Added support for getting stg config channel info 
03/02/13   tks     Added support for configuring the single tone generator
02/06/13   rmb     Added support for autoA/autoB optimization in building scripts
12/18/12   ka      Initial support for DCHSUPA
12/12/12   tks     Added new interface function for get rsb coefficients 
12/12/12   tks     Added new interface function for get lna offset 
12/12/12   tks     Added new interface function for get pll lock detect
11/01/12   rmb/swb Add support for autoA/autoB optimization in building scripts
10/11/12   ac      Configure RF_ON signal by SSBI
10/03/12   shb     Added rfdevice_wcdma_rx_build_meas_script_v2_type to RX 
                   function pointer table
09/28/12   swb     Add support for RFC mapped Virtual PA
09/24/12   spa     Added "extern C" check
09/20/12    aj     Fixed compiler warnings
09/19/12   tks     Added default functions for wcdma rx and tx device
09/18/12   tks     Added lna parameter type variable to support lna scripts
08/21/12   shb     Fixed offtarget compiler warnings due to type mismatch
08/16/12   shb     Added rfdevice_wcdma_txrx_device_type
08/16/12   shb     Renamed rfdevice_enum_type to rfdevice_id_enum_type
08/15/12   shb     Fixed compiler errors
08/15/12   shb     Moved all WCDMA data structs from rfdevice_wcdma_intf.h to 
                   rfdevice_wcdma_type_defs.h   
09/28/11   dw/kguo Add support for turn on/off Tx PLL in meas
08/25/11   sar     Removed obsolete file bbrx.h.
09/23/10   kguo    Added support for diversity for W2L IRAT 
07/29/10   kguo    Removed FW dependency
06/29/10   pl      Use MDSP layer to abstract FW from RF IRAT module
06/09/10   kguo    Initial version.
==============================================================================*/ 

#ifndef RFDEVICE_WCDMA_TYPE_DEFS_H
#define RFDEVICE_WCDMA_TYPE_DEFS_H

/*============================================================================== 
 
                                 INCLUDE FILES
 
==============================================================================*/

#include "rfcom.h" 
#include "rfwcdma_core.h"
#include "rfdevice_meas_type_defs.h"
#include "rf_buffer_intf.h"
#include "rfdevice_cmn_type_defs.h"
#include "rfm_path_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================== 
                               MACROS AND TYPEDEFS 
==============================================================================*/

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enum to distinguish what CSR correction algo to use

  @details
  Following is a list of currently supported CSR correction algos:
    RFDEVICE_WCDMA_CSR_ALGO_LINEAR_CORRECTION
      This algo type applies CSR correction linearly scaled based on
      characterization
      e.g. at fc = 2142.5 MHz the characterized CSR correction needed is -5 Hz
           then -2.5 Hz will be applied if fc = 1071.25 MHz
*/
typedef enum
{
  RFDEVICE_WCDMA_CSR_ALGO_NONE,
  RFDEVICE_WCDMA_CSR_ALGO_LINEAR_CORRECTION,
  RFDEVICE_WCDMA_CSR_ALGO_MAX,
  RFDEVICE_WCDMA_CSR_ALGO_INVALID = RFDEVICE_WCDMA_CSR_ALGO_MAX,
} rfdevice_wcdma_csr_correction_algo_type;



typedef enum
{
  RFDEV_WCDMA_BW_1X,
  RFDEV_WCDMA_BW_2X,
  RFDEV_WCDMA_BW_3X,
  RFDEV_WCDMA_BW_4X,
  RFDEV_WCDMA_BW_NUM,
  RFDEV_WCDMA_BW_INVALID
}rfdevice_wcdma_bw_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  uint16 channel;
  boolean drx_enable;
  boolean is_startup;
  rfdevice_meas_script_type *startup_tbl_ptr;
}rfdevice_wcdma_get_irat_meas_tbl_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief 
  WCDMA structures to access device information
 
*/
typedef struct
{
  int32 rsb_a;
  int32 rsb_b;
  rfcom_multi_carrier_hspa_id_type num_chan;
  rfcom_wcdma_band_type band; 
  uint8 lna_gain_state; 
} 
rfdevice_wcdma_rx_rsb_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  uint32 rxfe_lna_phaseoffset[6];
  rfcom_multi_carrier_hspa_id_type num_chan;
  rfcom_wcdma_band_type band; 
}rfdevice_wcdma_lna_phase_offset_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  uint32 tx_val;
  uint32 prx_val; 
  uint32 drx_val; 
  rfcom_wcdma_band_type band; 
} rfdevice_wcdma_d_pll_data_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  rfdevice_dac_iref_val iref_val;
  rfcom_wcdma_band_type band; 
} rfdevice_wcdma_dac_cal_iref_type;

/*----------------------------------------------------------------------------*/
typedef rfdevice_rsb_cal_data_type rfdevice_rx_wcdma_rsb_cal_data_type ; 

/*----------------------------------------------------------------------------*/
typedef struct
{
  uint8  lut_index;
  uint8  pa_range;
  rfcom_wcdma_band_type band;
  uint32 lut_ssbi_data;
  uint8 phase_comp_data; 
}rfdevice_wcdma_lut_data_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  rfcom_wcdma_band_type band;
  uint32 val;
}rfdevice_wcdma_lut_addr_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  rfm_path_enum_type path;
  rfcom_wcdma_band_type band;
  rfcom_multi_carrier_hspa_id_type carrier;
  uint32 data[6];
  uint32 addr[6]; 
}rfdevice_wcdma_rx_lna_state; 

/*----------------------------------------------------------------------------*/
typedef struct
{ 
  rfcom_device_enum_type device;  
  rfcom_device_enum_type paired_device;
  rfdevice_rx_mode_type rx_mode;
  rfcom_wcdma_band_type band;               
  rfcom_multi_carrier_hspa_id_type carrier; 
  uint8 current_state;                      
}rfdevice_wcdma_lna_param_type; 

/*----------------------------------------------------------------------------*/
typedef struct
{
  rfcom_wcdma_band_type band;
  boolean support;
  uint32 low_lin_data[6];
  uint32 high_lin_data[6];
  uint32 addr[6];
}rfdevice_wcdma_rx_linearity_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  /* Band to be tuned to */ 
  rfcom_wcdma_band_type band;
  
  /*! Paired Rx Device */
  rfcom_device_enum_type paired_device;

  /*! Enum to indicate if  Rx chain is 
      used for PRx/DRx operation */
  rfdevice_rx_mode_type rx_mode;

  /*! Channel list */
  uint16 channel[4]; 
  
  /* Bandwidth to be tuned to */ 
  rfdevice_wcdma_bw_type bw;
}rfdevice_wcdma_rx_tune_param_type; 

/*----------------------------------------------------------------------------*/
typedef struct
{
  rfcom_wcdma_band_type band; 
  rfdevice_dpd_enum_type dpd_state; 
  uint16 channel[4];
  rfdevice_wcdma_bw_type bw; 
  rfwcdma_core_uarfcn_ul_dl_type ul_dl;
}rfdevice_wcdma_tx_tune_param_type; 

/*----------------------------------------------------------------------------*/
typedef struct 
{
  rfcom_device_enum_type device; 
  /*!<  Device on which WCDMA will be tuned  */

  rfcom_device_enum_type paired_device;
  /*!< Paired device */

  rfdevice_rx_mode_type rx_mode;
  /*!< Enum to indicate if Rx chain is used for PRx/DRx operation */

  rfcom_mode_enum_type   source_tech;
  /*!<  Specify source technology for IRAT */

  uint32                 source_band;
  /*!< Specify source technology band*/

  rfcom_mode_enum_type   target_tech;
  /*!<  Specify target technology for IRAT */

  uint32                 target_band;
  /*!<  Specify target technology for IRAT */

  uint16                 channel[RFCOM_MAX_CARRIERS];
  /*!<  DL Channel on which WCDMA will be tuned  */

  rfcom_multi_carrier_hspa_id_type carrier_type;
  /*!<  Specify SC or DC mode  */

  boolean diversity_active;
  /*!< Specify if diversity is active or not */

  boolean enable_autob_optimization;
  /*!< enable or disable autob optimization */

  rfdevice_meas_scenario_type meas_scenario; 
  /*!< specify the irat scenario for which meas script is required */

} rfdevice_wcdma_meas_param_type;

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
} rfdevice_wcdma_hdet_read_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Data structure to hold spur mitigiation parameters to program baseband
  notches.
 
*/
#define RFDEVICE_WCDMA_MAX_SUPPORTED_NOTCHES 12 

typedef struct
{

  /* Input parameter to be provided by rf wcdma mc */
  /*! Current logical device */ 
  rfm_device_enum_type curr_logical_dev; 

  /* Output parameter will be provided by device driver */
  /*! Spur count for first and second carrier in DB-DC */
  uint8 current_cell_spur_count;

  /*! Notch depth list for DB-DC carriers */
  uint16 current_cell_notch_depth_list[RFDEVICE_WCDMA_MAX_SUPPORTED_NOTCHES];

  /*! Notch frequency list for DB-DC carriers */ 
  int32 current_cell_notch_freq_list[RFDEVICE_WCDMA_MAX_SUPPORTED_NOTCHES];

  /*! Flag for apply notch immediate */ 
  boolean apply_notch_immediate;

  /*! Flag for clear notch immediate */ 
  boolean clear_notch;

} rfdevice_wcdma_spur_param_type; 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Callback function pointer type that is used to for notch programming.
  This callback would be invoked by device driver during tune when spurs are
  identified.
*/

typedef void (*rf_wcdma_spur_cb_type)(rfdevice_wcdma_spur_param_type* cb_data);


/*---------------------------------------------------------------------------*/
/*!
  @brief
  Forward declaration of device structure types to use in function pointers
  definitions
*/
typedef struct rfwcdma_rxdev rfwcdma_device_rx_type;
typedef struct rfwcdma_txdev rfwcdma_device_tx_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Configures WCDMA RX device to operate on the specified band/port 
   (rx_band_port) for the given band (rfcom_band)
   
   @details
   Ensures rfcom_band and rx_band_port are valid and updates instance data to
   store this band/port info for future use
   
   @param *rx_device
   RX device to be updated

   @param rfcom_band
   rfcom_band whose port is being set

   @param rx_band_port
   band/port enum will be typecast to wcdma_prx_band_type or
   wcdma_drx_band_type and will be used in all functions to apply
   autogenerated SSBI settings
*/
typedef boolean (*rfdevice_wcdma_rx_set_port_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_rx_type* rx_device,
  rfcom_wcdma_band_type rfcom_band,
  uint8 rx_band_port
);

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Configures WCDMA TX device to operate on the specified band/port 
   (tx_band_port) for the given band (rfcom_band)
   
   @details
   Ensures rfcom_band and tx_band_port are valid and updates instance data to
   store this band/port info for future use

   Device should be created before calling this function
   
   @param *tx_device
   TX device to be updated

   @param rfcom_band
   rfm_band whose port is being set

   @param tx_band_port
   band/port enum will be typecast to wcdma_tx_port_data_type and will
   be used in all functions to apply autogenerated SSBI settings
*/
typedef boolean (*rfdevice_wcdma_tx_set_port_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_tx_type* tx_device,
  rfcom_wcdma_band_type rfcom_band,
  uint8 tx_band_port,
  uint32 tx_band_pa_lut_map
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Push Rx band specific data to device
  
  @param *rx_device
  "this" pointer to RX device that needs to be configured
 
  @param band
  WCDMA band whose data is being pushed
 
  @param data_array
  Array of int32 containing device Rx data for band
 
  @param array_size
  Size of array of int32 data to be pushed to device
*/
typedef boolean (*rfdevice_wcdma_rx_set_band_data_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_rx_type* rx_device,
  rfcom_wcdma_band_type band,
  int32* data_array,
  uint8 array_size
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Push Tx band specific data to device
  
  @param *rx_device
  "this" pointer to TX device that needs to be configured
 
  @param band
  WCDMA band whose data is being pushed
 
  @param data_array
  Array of int32 containing device Tx data for band
 
  @param array_size
  Size of array of int32 data to be pushed to device
*/
typedef boolean (*rfdevice_wcdma_tx_set_band_data_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_tx_type* rx_device,
  rfcom_wcdma_band_type band,
  int32* data_array,
  uint8 array_size,
  uint32 tx_band_pa_lut_map
);

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
typedef void (*rfdevice_wcdma_rx_init_func_ptr_type)
(
  rfm_device_enum_type rfm_dev, 
  rfwcdma_device_rx_type *rxdev,
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
typedef void (*rfdevice_wcdma_enter_mode_rx_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_rx_type *rxdev,
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
typedef void (*rfdevice_wcdma_enable_rx_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_rx_type *rxdev,
  rf_path_enum_type path,
  rfcom_wcdma_band_type rf_band,
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
  Logical RF Path for which PLL to be programmed
 
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
typedef void (*rfdevice_wcdma_tune_to_chan_rx_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_rx_type *rxdev,
  rf_path_enum_type path,
  rfcom_mode_enum_type mode,
  rfcom_wcdma_band_type rf_band,
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
typedef void (*rfdevice_wcdma_tune_to_multi_chan_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_rx_type *rxdev,
  rf_path_enum_type path,
  rfcom_mode_enum_type mode,
  rfcom_wcdma_band_type rf_band,
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
typedef int32 (*rfdevice_wcdma_rx_cmd_dispatch_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_rx_type *rxdev,
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
typedef void (*rfdevice_wcdma_sleep_rx_func_ptr_type) 
( 
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_rx_type *rxdev, 
  rf_path_enum_type path,
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
typedef void (*rfdevice_wcdma_tx_init_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_tx_type *txdev,
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
  If NULL, script will have to be sent to hardware internally
 
  @retval 

  @pre
  SBI clock regime must be enabled.

  @post
  RTR will be in its default state.
*/
typedef void (*rfdevice_wcdma_enter_mode_tx_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_tx_type *txdev,
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
  If NULL, script will have to be sent to hardware internally  

  @retval 

  @pre
  RF device tune_to_chan() must be called.

  @post
  RF device will be placed in Tx mode of operation
*/
typedef void (*rfdevice_wcdma_enable_tx_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_tx_type *txdev,
  rf_path_enum_type path,
  rfcom_wcdma_band_type rf_band,
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
 
  @param dpd_state
  Indicates if we need to enable TX path for digital pre-distortion or without
  pre-distortion support. To support digital pre-distortion, RF device may need
  to choose different SSBI settings to widen bandwidth

  @param execution_type 
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script. 
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally
  @pre
  RF Device init() must have been called prior to calling this function.

  @post
  RF synthesizer will be tuned for the requested band and channel.
*/
typedef void (*rfdevice_wcdma_tune_to_chan_tx_func_ptr_type)
( 
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_tx_type *txdev,
  rfcom_mode_enum_type mode,
  rfcom_wcdma_band_type rf_band,
  uint16 chan,
  rfdevice_dpd_enum_type dpd_state,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer  
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Programs the Tx PLL based on the band and channel(s).

  @details
  Programs the RF Tx synthesizer (PLL) as per the band and channel(s).

  @param *txdev
  Pointer to tx device which needs to be tuned
 
  @param path
  Logical RF Path for which PLL to be programmed.
 
  @param band   
  Specifies the RF band the PLL is to be loaded for
 
  @param *chan   
  Specifies the RF uplink channel(s) within the specified RF band
 
  @param dpd_state
  Indicates if we need to enable TX path for digital pre-distortion or without
  pre-distortion support. To support digital pre-distortion, RF device may need
  to choose different SSBI settings to widen bandwidth

  @param execution_type 
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script. 
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally
  @pre
  RF Device init() must have been called prior to calling this function.

  @post
  RF synthesizer will be tuned for the requested band and channel.
*/
typedef void (*rfdevice_wcdma_tune_to_multi_chan_tx_func_ptr_type)
(
  rfwcdma_device_tx_type *txdev,
  rfcom_mode_enum_type mode,
  rfcom_wcdma_band_type rf_band,
  uint16 chan[],
  rfdevice_dpd_enum_type dpd_state,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer  
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure RX_ON signal by SSBI

  @details
  Configure RX_ON signal by SSBI
 
  @param *txdev
  Pointer to tx device to be enabled
 
  @param band
  band is set for the mode
 
  @param execution_type 
  Indicates whether API should execute SSBI internally or populate
  script_buffer pointer with SSBI script. 
 
  @param *script_buffer
  void Pointer to buffer_intf object where SSBI script needs to be appended.
  If NULL, script will have to be sent to hardware internally  

  @retval 

  @post
  Configure RX_ON signal by SSBI
*/

typedef void (*rfdevice_wcdma_tx_set_rf_on_ptr_type)
( 
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_tx_type *tx_device,
  rfcom_wcdma_band_type rf_band,
  boolean rf_on,
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
typedef int32 (*rfdevice_wcdma_tx_cmd_dispatch_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_tx_type *txdev,
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
typedef void (*rfdevice_wcdma_disable_tx_func_ptr_type) 
(
  rfm_device_enum_type rfm_dev,  
  rfwcdma_device_tx_type *txdev, 
  rf_path_enum_type path,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer     
);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Build WCDMA tune scripts for IRAT. That is XtoW startup script and WtoX
  cleanup scripts

  @details
  Build WCDMA tune scripts for IRAT. That is XtoW startup script and WtoX
  cleanup scripts
   
  @param *rxdev
  Pointer to WCDMA device that needs to be programmed

  @param param
  Parameters needed to generate the script
 
  @param *script
  Pointer to script to hold the startup/cleanup scripts
 
  @param *current_size
  Holds the current size of the script so that the script can be appended from *current_size,
  this function also updates *current_size after the scripts is updated
 
  @param *current_size_autob
  The current number of autob register writes in the script.  AutoB register writes must occur
  prior to PLL tuning.  current_size_autob <= current_size
 
  @param max_script_size
  Max script size that *script can hold

*/
typedef void (*rfdevice_wcdma_build_meas_script_type) 
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_rx_type *rxdev,  
  rfdevice_wcdma_meas_param_type param,
  void *script,
  uint16 *current_size,
  uint16 *current_size_rxd,
  uint16 *current_size_autob,
  uint16 max_script_size
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Obtain Rx LNA offset setup 
  
  @param *rxdev
  Pointer to wcdma device which needs to be programmed
 
  @param *offset
  Strucuture containing info like band, carrier type etc, needed to
  obtain the specific LNA settings.
 
  @param exec_type
  enum to indicate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script
 
  @return
  Success/failure status flag 
*/
typedef boolean (*rfdevice_wcdma_rx_set_lna_offset_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_rx_type *rxdev,
  rfdevice_wcdma_lna_phase_offset_type *offset, 
  rf_device_execution_type exec_type, 
  rf_buffer_intf* script_buffer 
); 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Obtain the RSB coefficients from the device 
  
  @param *rxdev
  Pointer to wcdma device which needs to be programmed
 
  @param *coeff
  Strucuture containing info like band, carrier type etc, needed to
  obtain the specific rsb coefficients 
 
  @param exec_type
  enum to indicate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script
 
  @return
  Success/failure status flag 
*/
typedef boolean (*rfdevice_wcdma_rx_compute_rsb_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_rx_type *rxdev,
  rfdevice_wcdma_rx_rsb_type *coeff, 
  boolean *data_valid_status,
  rf_device_execution_type exec_type, 
  rf_buffer_intf* script_buffer 
); 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Obtain the RX PLL Lock Status from the device
  
  @param rfm_dev
  Logical RFM Device
 
  @param *rxdev
  Pointer to wcdma device which needs to be programmed
 
  @param rf_band
  Current Band of RFM device
 
  @param *pll_lock_status
  Pointer to RX PLL Lock status
 
  @param *data_valid_status
  Pointer to status which says whether PLL is supported by this device
 
  @param exec_type
  enum to indicate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script
 
  @return
  Success/failure status flag 
*/
typedef boolean (*rfdevice_wcdma_rx_pll_lock_detect_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_rx_type *rxdev,
  rfcom_wcdma_band_type rf_band,
  boolean *pll_lock_status, 
  boolean *data_valid_status, 
  rf_device_execution_type exec_type, 
  rf_buffer_intf* script_buffer 
); 


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable Rx 
  
  @param *rxdev
  Pointer to wcdma device which needs to be programmed
 
  @param *coeff
  Strucuture containing info like band, carrier type etc, needed to
  obtain the specific rsb coefficients 
 
  @param exec_type
  enum to indicate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script
 
  @return
  Success/failure status flag 
*/
typedef void (*rfdevice_wcdma_rx_disable_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_rx_type *rxdev,
  rfcom_wcdma_band_type band, 
  rf_device_execution_type exec_type, 
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
  Success/failure status flag 
*/
typedef boolean (*rfdevice_wcdma_rx_config_stg_func_ptr_type)
(
  rfm_device_enum_type rfm_dev, 
  rfwcdma_device_rx_type *rx_device,
  rfcom_wcdma_band_type band, 
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
  Success/failure status flag 
*/
typedef boolean (*rfdevice_wcdma_rx_get_stg_chan_func_ptr_type)
(
  rfm_device_enum_type rfm_dev, 
  rfwcdma_device_rx_type *rx_device,
  rfcom_wcdma_band_type band, 
  uint16 *channel,
  boolean *data_valid_status, 
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
typedef boolean (*rfdevice_wcdma_rx_disable_stg_func_ptr_type)
(
  rfm_device_enum_type rfm_dev, 
  rfwcdma_device_rx_type *rx_device,
  rf_device_execution_type exec_type, 
  rf_buffer_intf* script_buffer 
); 

/*----------------------------------------------------------------------------*/
/*!
  @brief
 
  @param rfm_dev
 
  @param *rxdev
  
  @param band

  @param *rsb_data
  pointer to rsb data type
  
  @return
  Booelan flag indicating suuccess or failure 
*/

typedef boolean (*rfdevice_wcdma_rx_rsb_temp_comp_ptr_type)
(
  rfm_device_enum_type rfm_dev, 
  rfwcdma_device_rx_type *rxdev,
  rfcom_wcdma_band_type band,
  rfdevice_rx_wcdma_rsb_cal_data_type *rsb_data
); 
/*----------------------------------------------------------------------------*/
/*!
  @brief
 
  @param rfm_dev
 
  @param *rxdev
  
  @param band

  @param curr_band
  Number of carriers indicating single or dual carrier

  @param temperature
  thermistor reading

  @param *a_coeff 
  RSB coefficient A to be programmed in buffer

  @param *a_coeff 
  RSB coefficient A to be programmed in buffer
  
  @return
  Booelan flag indicating suuccess or failure 
*/
typedef boolean (*rfdevice_wcdma_rx_rsb_coeff_temp_comp_ptr_type)
(
 rfm_device_enum_type rfm_dev,
 rfwcdma_device_rx_type *rxdev,
 rfcom_wcdma_band_type band,
 rfcom_multi_carrier_hspa_id_type curr_bw,
 uint16 temperature,
 int32* a_coeff,
 int32* b_coeff
); 
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Obtain Rx LNA gain state settings 
  
  @param *rxdev
  Pointer to wcdma device which needs to be programmed
 
  @param *lna_state
  Strucuture containing info like band, carrier type etc, needed to
  obtain the specific LNA settings.
 
  @param exec_type
  enum to indicate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script
 
  @return
  Success/failure status flag 
*/
typedef boolean (*rfdevice_wcdma_rx_lna_setup_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_rx_type *rxdev,
  rfdevice_wcdma_lna_param_type *lna_state, 
  rf_device_execution_type exec_type, 
  rf_buffer_intf* script_buffer 
); 


/*----------------------------------------------------------------------------*/
/*!
  @brief
  WCDMA measurement script API for DIME using rf_buffer_intf

  @details
  This function prepares the sbi script for wcdma mode(single carrier or
  dual carrier), band & channel for measurement. This script is prepared
  one frame ahead of time as requested by WCDMA L1.
 
  This is the equivalent of wtr1605_wcdma_build_meas_script_v1 but returns the
  script in rf_buffer_intf* format
 
  @param *rxdev
  "this" pointer to Rx device whose data is to be extracted

  @param param
  struct provides the necessary parameters like current band, target band,
  carrier type etc for building meas script

  @param buf
  Pointer to rf_buffer_intf object that will be populated with meas script
*/
typedef boolean (*rfdevice_wcdma_rx_build_meas_script_v2_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_rx_type *rxdev,
  rfdevice_wcdma_meas_param_type param,
  rf_buffer_intf* buf,
  uint16 *current_size_autob
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface to just write WCDMA common table SSBI

  @details
  API to be used for IRAT to pre-configure device for measurement. The
  function will write all common table SSBI - both AutoA and AutoB.
 
  @param rfm_dev
  Logical device of operation
 
  @param rxdev
  "this" pointer to WCDMA RX device ptr
 
  @param rf_band
  Band for which SSBI is to be programmed. Use to determine port being used
  in conjunction with rfm_dev
 
  @param execution_type
  Indicates whether API should populate script_buffer or return a script
 
  @param script_buffer
  rf_buffer_intf object that will be populated with common settings if
  execution_type == RFDEVICE_CREATE_SCRIPT
 
  @return
  Success/failure status flag
*/
typedef boolean (*rfdevice_wcdma_rx_set_common_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_rx_type *rxdev,
  rfcom_wcdma_band_type rf_band,
  rf_device_execution_type execution_type,
  rf_buffer_intf* script_buffer 
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Read the current therm value from device 

  @details
   
  @param tx_device
  Pointer to the wcdma device which needs to be programmed
 
  @param therm_value
  uint16 value to obtain the current therm value
 
  @param exec_type
  enum to indicate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script 
 
  @return
  Success/failure status flag
*/
typedef boolean (*rfdevice_wcdma_tx_therm_read_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_tx_type *tx_device,
  uint16 *therm_value, 
  rf_device_execution_type exec_type,
  rf_buffer_intf* script_buffer 
);
 
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Setup HDET for read 

  @details
 
  @param tx_device
  Pointer to the wcdma device which needs to be programmed
 
  @param exec_type
  enum to indicate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script
 
  @return
  Success/failure status flag
*/
typedef boolean (*rfdevice_wcdma_tx_hdet_setup_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_tx_type *tx_device,
  rf_device_execution_type exec_type,
  rf_buffer_intf* script_buffer 
); 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Read current HDET value 

  @details
  
  @param tx_device
  Pointer to the wcdma device which needs to be programmed
 
  @param mdsp_triggered_read
  boolean to differentiate mdsp triggered read versus non-triggered read
 
  @param hdet_value
  uint16 variable to store the current hdet value 
 
  @param exec_type
  enum to indicate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script
 
  @return
  Success/failure status flag
*/
typedef boolean (*rfdevice_wcdma_tx_hdet_read_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_tx_type *tx_device,
  boolean mdsp_triggered_read, 
  uint16 *hdet_value,
  rf_device_execution_type exec_type,
  rf_buffer_intf* script_buffer 
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get script for CPC DTX 

  @details 
  
  @param tx_device
  pointer to the wcdma device which needs to be programmed
 
  @param band
  enum indicating the current wcdma band of operation
 
  @param tx_on_off
  boolean value to incdicate tx on and tx off separately 
 
  @param exec_type
  enum to incidate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script 
 
  @return
  Success/failure status flag
*/
typedef boolean (*rfdevice_wcdma_tx_get_cpc_dtx_val_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_tx_type *tx_device,
  rfcom_wcdma_band_type band,
  boolean tx_on_off, 
  rf_device_execution_type exec_type,
  rf_buffer_intf* script_buffer 
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the ET delay compensation to be applied for WCDMA

  @details
  Delay compensation value depends on TX BBF RC error, TX process
 
  @param tx_device
  "this" pointer to WCDMA Tx device of operation.
 
  @param delay_comp_x10ns
  Delay compensation to be applied in 1/10th of a ns unit
 
  @retval TRUE
  Indicates the API was successful and a correct delay comp value has been
  returned
 
  @retval FALSE
  Indicates API failed due to bad parameters
*/
typedef boolean (*rfdevice_wcdma_tx_get_et_delay_comp_func_ptr_type)
(
  rfwcdma_device_tx_type *txdev,
  int16* et_delay_comp_x10ns
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get script for CPC DRX 

  @details
  
  @param rx_device
  pointer to the wcdma device which needs to be programmed
 
  @param band
  enum indicating the current wcdma band of operation
 
  @param rx_on_off
  boolean value to incdicate rx on and rx off separately 
 
  @param exec_type
  enum to incidate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script 
 
  @return
  Success/failure status flag
*/
typedef boolean (*rfdevice_wcdma_rx_get_cpc_drx_val_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_rx_type *rx_device,
  rfcom_wcdma_band_type band,
  boolean rx_on_off, 
  rf_device_execution_type exec_type,
  rf_buffer_intf* script_buffer 
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Obtain the TX PLL Lock Status from the device 
  
  @param rfm_dev
  Logical RFM Device
 
  @param *rxdev
  Pointer to wcdma device which needs to be programmed
 
  @param *pll_lock_status
  Pointer to TX PLL Lock status
 
  @param *data_valid_status
  Pointer to status which says whether PLL is supported by this device
 
  @param exec_type
  enum to indicate direct hw writes or prepare script
 
  @param script_buffer
  populate the buffer with script
 
  @return
  Success/failure status flag 
*/
typedef boolean (*rfdevice_wcdma_tx_pll_lock_detect_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfwcdma_device_tx_type *txdev,
  boolean *pll_lock_status, 
  boolean *data_valid_status, 
  rf_device_execution_type exec_type, 
  rf_buffer_intf* script_buffer 
); 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Obtain the TX fbrx rsb cal data
  
  @param rfm_dev
  Logical RFM Device
 
  @param rf_band
  wcdma band for fbrx rsb cal
 
 @param tx_bw, bandwidth of the tx 

 @param rsb_data
   rsb data pointer that contains i/q measurement for computing rsb_coef, outputing rsb_sin_theta, cos_theta and gain_inv in rsb_data
   
  @return
  Success/failure status flag 
*/

typedef boolean (*calc_fbrx_rsb_nv_params_func_ptr_type)
(
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  rfdevice_wcdma_bw_type tx_bw,
  rfdevice_rsb_cal_data_type *rsb_data
);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure of RX interface function pointer 
*/
typedef struct
{
  rfdevice_wcdma_rx_init_func_ptr_type  init_fp;
  /*!< to place the RTR into default state */
  
  rfdevice_wcdma_enter_mode_rx_func_ptr_type enter_mode_fp; 
  /*!< configure the RTR for the mode */     

  rfdevice_wcdma_enable_rx_func_ptr_type enable_fp;  
  /*!< to place the RTR into Rx mode */

  rfdevice_wcdma_tune_to_chan_rx_func_ptr_type tune_to_chan_fp; 
  /*!< Program the PLL for specific band and chan */

  rfdevice_wcdma_tune_to_multi_chan_func_ptr_type tune_to_multi_chan_fp; 
  /*!< Program the PLL for DC */

  rfdevice_wcdma_sleep_rx_func_ptr_type sleep_fp;   
  /*!< place the RTR into sleep mode to save current */

  rfdevice_wcdma_build_meas_script_type build_meas_script_fp; 
  /*!< Generate XtoW startup or WtoX cleanup scripts */

  rfdevice_wcdma_rx_cmd_dispatch_func_ptr_type cmd_dispatch_fp; 
  /*!< generic IOCTL kind of function */

  rfdevice_wcdma_rx_set_port_func_ptr_type set_port_fp; 
  /*!< Sets RX port to be used for a WCDMA band */

  rfdevice_wcdma_rx_set_band_data_func_ptr_type set_band_data_fp; 
  /*!< Used to push RX band data to device */

  rfdevice_wcdma_rx_lna_setup_func_ptr_type get_lna_state_fp; 
  /*!< Used to get lna information from device */

  rfdevice_wcdma_rx_build_meas_script_v2_type build_meas_script_v2_fp; 
  /*!< Used to build W meas script in rf_buffer_intf */

  rfdevice_wcdma_rx_get_cpc_drx_val_func_ptr_type get_cpcdrx_val_fp; 
  /*!< Used to build script for CPC DRX operation */

  rfdevice_wcdma_rx_set_lna_offset_func_ptr_type set_lna_offset_fp; 
  /*!< Used to obtain the LNA offsets */
 
  rfdevice_wcdma_rx_compute_rsb_func_ptr_type get_rsb_coeff_fp; 
  /*!< Used to obtain the RSB coefficients */

  rfdevice_wcdma_rx_pll_lock_detect_func_ptr_type pll_lock_detect_fp; 
  /*!< Used to obtain the Rx PLL lock status */

  rfdevice_wcdma_rx_disable_func_ptr_type disable_fp; 
  /*!< Used to disable Rx */

  rfdevice_wcdma_rx_get_stg_chan_func_ptr_type get_stg_chan_fp;
  /*!< Used to get channel for RSB cal */

  rfdevice_wcdma_rx_config_stg_func_ptr_type config_stg_fp; 
  /*!< Used to setup single tone generator for RSB cal */

  rfdevice_wcdma_rx_set_common_func_ptr_type set_rx_common_fp;
  /*!< Used to pre-config RX common tables for IRAT */
  
  rfdevice_wcdma_rx_disable_stg_func_ptr_type disable_stg_fp; 
  /*!< Used to enable stg */  

  rfdevice_wcdma_rx_rsb_temp_comp_ptr_type rsb_comp_fp; 
  /*!< Used to adjust rsb cal data based on temperature */

  rfdevice_wcdma_rx_rsb_coeff_temp_comp_ptr_type rsb_coeff_comp_fp; 
  /*!< Used to adjust rsb coefficients based on temperature */ 

} rfwcdma_rxdev_func_tbl_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure of TX interface function pointer 
*/
typedef struct
{
  rfdevice_wcdma_tx_init_func_ptr_type  init_fp;  
  /*!< to initialize the RF device for WCDMA Tx */

  rfdevice_wcdma_enter_mode_tx_func_ptr_type enter_mode_fp; 
  /*!< configure the RTR for the mode */

  rfdevice_wcdma_enable_tx_func_ptr_type enable_fp;
  /*!< to put the RF device into WCDMA Tx mode  */
      
  rfdevice_wcdma_tune_to_chan_tx_func_ptr_type tune_to_chan_fp; 
  /*!< to program the Tx PLL for a given WCDMA ARFCN */      

  rfdevice_wcdma_disable_tx_func_ptr_type disable_fp;           
  /*!< to disable WCDMA Tx */

  rfdevice_wcdma_tx_cmd_dispatch_func_ptr_type cmd_dispatch_fp; 
  /*!< generic IOCTL kind of function */

  rfdevice_wcdma_tx_set_port_func_ptr_type set_port_fp; 
  /*!< Sets TX port to be used for a WCDMA band */

  rfdevice_wcdma_tx_set_band_data_func_ptr_type set_band_data_fp; 
  /*!< Used to push TX band data to device */

  rfdevice_wcdma_tx_therm_read_func_ptr_type read_therm_fp; 
  /*!< Use to perform thermistor read */

  rfdevice_wcdma_tx_hdet_setup_func_ptr_type setup_hdet_fp;
  /*!< Use to perform setup hdet */

  rfdevice_wcdma_tx_hdet_read_func_ptr_type read_hdet_fp; 
  /*!< Use to perform hdet read */

  rfdevice_wcdma_tx_set_rf_on_ptr_type set_rf_on_fp;  
  /*!< Use to perform rf_on/tx_on */

  rfdevice_wcdma_tx_get_cpc_dtx_val_func_ptr_type get_cpcdtx_val_fp; 
  /*!< Used to build script for CPC DTX operation */

  rfdevice_wcdma_tune_to_multi_chan_tx_func_ptr_type tune_to_multi_chan_fp; 
  /*!< Supports dual carrier */
  
 rfdevice_wcdma_tune_to_chan_tx_func_ptr_type tune_to_default_port_fp; 
 /*!< tune tx device to any port. Will be used for Feedback path calibration 
    where port is a dont care */

 rfdevice_wcdma_tx_pll_lock_detect_func_ptr_type pll_lock_detect_fp; 
 /*!< Used to obtain the Tx PLL lock status */

 rfdevice_wcdma_tx_get_et_delay_comp_func_ptr_type get_et_delay_comp_fp;
  /*!< Returns the ET delay compensation to be applied for WCDMA on a device */
 
 calc_fbrx_rsb_nv_params_func_ptr_type calc_fbrx_rsb_nv_params_fp;
} rfwcdma_txdev_func_tbl_type;

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
struct rfwcdma_rxdev
{
  rf_lock_data_type *critical_sect;

  rfdevice_id_enum_type device_id;    /*!< RF device being used for rx  */

  rfwcdma_rxdev_func_tbl_type *func_tbl;

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
struct rfwcdma_txdev
{
  rf_lock_data_type *critical_sect;

  rfdevice_id_enum_type device_id;  /*!< RF device being used for Tx */

  rfwcdma_txdev_func_tbl_type *func_tbl;

  void *data;  /*!< Void pointer to device data which can be device specific */
} ;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure contains pointers to the various WCDMA devices in a transceiver 

  @details
  The structure puts together pointers for all WCDMA devices in RF
  transceiver. Rx0 refers to primary path, Rx1 refers to diversity path
  and Tx refers to primary path. Structure of this type is stored in the
  common instance's WCDMA tech_instance pointer
*/
typedef struct
{
  /*! Const Pointer to Receive data on Primary path */
  rfwcdma_device_rx_type* const rx0_device;

  /*! Const Ptr to Receive data on Secondary path */
  rfwcdma_device_rx_type* const rx1_device;

  /*! Constant Pointer to Transmit data on Primary path */
  rfwcdma_device_tx_type* const tx_device;
} rfdevice_wcdma_txrx_device_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure used for querying device for Rx CSR correctio algo type and
  associated calculation parameters
*/
typedef struct
{
  /*! WCDMA band - Input */
  rfcom_wcdma_band_type                   band;
  /*! WCDMA DL channels - Input */
  uint16                                  chan[RFCOM_MAX_CARRIERS];
  /*! DL freqs - Input */
  uint32                                  freq[RFCOM_MAX_CARRIERS];
  /*! Number of channels - Input */
  rfcom_multi_carrier_hspa_id_type        num_chan;
  /*! Rx CSR Correction Algo - Output*/
  rfdevice_wcdma_csr_correction_algo_type algo;
  /*! Rx CSR correction Base for linear correction in KHz - Output*/
  int64                                   correction_base;
  /*! Rx CSR linear correction drift amount in Hz - Output */
  int64                                   drift;
} rfdevice_wcdma_rx_csr_correction_query_type;

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

  /*! WCDMA Bandwidth used for Rx RSB calibration */
  rfdevice_wcdma_bw_type cal_bw;

}rfdevice_wcdma_rx_rsb_cal_data_type;

/*----------------------------------------------------------------------------*/
extern rfwcdma_device_rx_type *rfwcdma_rx_device[RFDEVICE_RX_PATH_NUM];

/*----------------------------------------------------------------------------*/
extern rfwcdma_device_tx_type *rfwcdma_tx_device[RFDEVICE_TX_PATH_NUM];

#ifdef __cplusplus
}
#endif

#endif /* RFDEVICE_WCDMA_TYPE_DEFS_H */

