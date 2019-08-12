#ifndef RFDEVICE_TRX_TDSCDMA_RX_ADAPTER_H
#define RFDEVICE_TRX_TDSCDMA_RX_ADAPTER_H
/*!
   @file
   rfdevice_trx_tdscdma_rx_adapter.h

   @brief

*/

/*===========================================================================

Copyright (c) 2013 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_trx_tdscdma_rx_adapter.h#1 $

when       who    what, where, why
--------   ---   -------------------------------------------------------------------
09/16/15   sd    Added support for Rx AGC reload
05/22/15   dps   Added support for power logging using rfdevice_wtr_config_bitmask
05/15/15   sd    Device driver updates for RFC re-design for Split Rx/Tx
01/23/15   dps   Added new API for Rx RSB calibration using NV input
09/18/14   lm    Added missing function set_rx_band_data() from rfdevice_trx_tdscdma_rx class
                 in rfdevice_trx_tdscdma_rx_adapter class
         Added parameter rx_lin_state in create_meas_script() to clean compiler warnings
08/28/14   shb     Deprecated API to return critical_section_ptr
07/17/14   ms    Sawless RX support
11/19/13   zg    Added stub for new iRAT API.
11/07/13   rp    Added and API to return common device class pointer.
08/27/13   zg    Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "comdef.h"
#include "rf_buffer_intf.h"
#include "rfdevice_class.h"

#ifdef FEATURE_TDSCDMA
#include "rfdevice_tdscdma_type_defs.h"
#include "rfdevice_trx_tdscdma_rx.h"

class rfdevice_trx_tdscdma_rx_adapter:public rfdevice_trx_tdscdma_rx
{
public:
  // Constructor
  rfdevice_trx_tdscdma_rx_adapter( rfdevice_rxtx_common_class *cmn_rxtx_dev,
                               rfdevice_trx_phy_path_enum_type phy_path);

  // Destructor
  ~rfdevice_trx_tdscdma_rx_adapter();

  /*----------------------------------------------------------------------------*/
  boolean rx_init
  ( 
    rfm_device_enum_type device,  
    rfcom_mode_enum_type mode,
    rfcom_tdscdma_band_type rf_band,
    rf_device_execution_type execution_type,
    rf_buffer_intf* script_buffer
  );

  /*----------------------------------------------------------------------------*/
  boolean rx_enter_mode
  ( 
    rfm_device_enum_type device, 
    rfcom_mode_enum_type mode,
    rfcom_tdscdma_band_type rf_band,
    rf_device_execution_type execution_type, 
    rf_buffer_intf* script_buffer,
    rfdevice_trx_tdscdma_rx* paired_device,
    rfdevice_rx_mode_type rx_mode
  );

  /*----------------------------------------------------------------------------*/
  boolean rx_enable
  ( 
    rfm_device_enum_type device, 
    rfcom_mode_enum_type mode,
    rfcom_tdscdma_band_type rf_band, 
    rf_device_execution_type execution_type, 
    rf_buffer_intf* script_buffer
   );

  /*----------------------------------------------------------------------------*/
  boolean rx_tune_chan
  ( 
    rfm_device_enum_type device, 
    rfcom_mode_enum_type mode,
    rfcom_tdscdma_band_type rf_band,
    uint16 chan,
    rf_device_execution_type execution_type, 
    rf_buffer_intf *script_buffer,
    uint8 rx_lin_state,
    rfdevice_trx_tdscdma_rx* paired_device,
    rfdevice_rx_mode_type rx_mode,
    rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL
  );

  /*----------------------------------------------------------------------------*/
  boolean rx_tune_multi_chan
  ( 
    rfm_device_enum_type device,  
    rfcom_mode_enum_type mode,
    rfcom_tdscdma_band_type rf_band,
    uint16 *chan,
    rf_device_execution_type execution_type, 
    rf_buffer_intf* script_buffer
  );

  /*----------------------------------------------------------------------------*/
  boolean rx_sleep
  ( 
    rfm_device_enum_type device, 
    rfcom_mode_enum_type mode,
    rfcom_tdscdma_band_type rf_band,
    rf_device_execution_type execution_type, 
    rf_buffer_intf* script_buffer,
    rfdevice_trx_tdscdma_rx* paired_device,
    rfdevice_rx_mode_type rx_mode
  );

  /*----------------------------------------------------------------------------*/
  boolean rx_cmd_dispatch
  ( 
    rfm_device_enum_type device,  
    rfcom_mode_enum_type mode,
    rfcom_tdscdma_band_type rf_band,
    rfdevice_cmd_enum_type cmd, 
    void *data,
    rfdevice_trx_tdscdma_rx* paired_device,
    rfdevice_rx_mode_type rx_mode,
    rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL,
    boolean use_alt_gain = FALSE
  );

  /*----------------------------------------------------------------------------*/
  boolean rx_exit
  ( 
    rfm_device_enum_type device,
    rfcom_mode_enum_type mode,
    rfcom_tdscdma_band_type rf_band,
    rf_device_execution_type execution_type, 
    rf_buffer_intf* script_buffer
  );

  /*----------------------------------------------------------------------------*/
  boolean rx_get_stg_channel
  (
    rfm_device_enum_type rfm_dev,
    rfcom_tdscdma_band_type rf_band,
    uint16 *channel,
    rf_device_execution_type execution_type, 
    rf_buffer_intf* script_buffer
  ); 

  /*----------------------------------------------------------------------------*/
  boolean rx_configure_stg
  (
    rfm_device_enum_type rfm_dev,
    rfcom_tdscdma_band_type rf_band,
    void *data,
    rf_device_execution_type execution_type, 
    rf_buffer_intf* script_buffer
  );

  /*----------------------------------------------------------------------------*/
  boolean rx_disable_stg
  (
    rfm_device_enum_type rfm_dev,
    rfcom_tdscdma_band_type rf_band,
    rf_device_execution_type execution_type, 
    rf_buffer_intf* script_buffer
  ); 

  /*----------------------------------------------------------------------------*/
  boolean rx_get_rsb_nv_params
  ( 
    rfm_device_enum_type rfm_dev,
    rfcom_tdscdma_band_type rf_band, 
    uint64   i_sq,
    uint64   q_sq,
    int64    i_q,
    int16    therm_data, 
    int16*   sin_theta_final, 
    int16*   cos_theta_final, 
    int16*   gain_inv_final,
    boolean  use_alt_gain = FALSE
  );

  /*----------------------------------------------------------------------------*/
  /*!
  @brief
  Interface to enter critical section

  @details
  This function will acquire lock to assign critical section for the successding 
  code. The code will be set to critical section  until the lock is released.
  Thus there should not be an attempt to re-acquire the same lock without 
  releasing it. This will result in Deadlock scenario.

  @return
  Success/failure status flag
  */
  boolean enter_critical_section();

  /*----------------------------------------------------------------------------*/
  /*!
  @brief
  Disable Critical Section

  @details
  This function will release lock which was acquired for the preceecing code.

  @return
  Success/failure status flag
  */
  boolean leave_critical_section();

  /*----------------------------------------------------------------------------*/
  /*!
  @brief
  Get rfdevice_rxtx_common_class pointer associated with this instance of device.
  
  @return 
  Pointer to rxtx common device associated with current instance of device.
  */
  rfdevice_rxtx_common_class* get_common_device_pointer(void); 

  /*----------------------------------------------------------------------------*/
  /*!
  @brief
  Create measurement (IRAT) script for tdscdma
  
  @details
  Generates preload and trigger scripts for tdscdma measurement - tune away or
  tune back.
  
  @param band
  Rx Band for which the script is to be generated
 
  @param chan
  Rx Channel for which the script is to be generated
 
  @param meas_type
  Indicates whether this is a startup (tune away) script or
  cleanup (tune back) script

  @param *script_data_ptr
  Structure with input/output parameters needed for IRAT scripts.
  
  script_data_ptr->preload_action
  is an input param indicating whether preload_script should be executed
  internally by device, returned in buffer provided or skipped
  altogether (preload was already done)
  
  script_data_ptr->preload_buf
  Buffer ptr where preload script should be populated IF
  script_data_ptr->preload_action == RFDEVICE_MEAS_CREATE_SCRIPT.
  Preload script can be executed before the measurement gap without
  disrupting the source RAT.
  
  script_data_ptr->meas_action
  is an input param indicating whether trigger_script should be executed
  internally by device, returned in buffer provided or skipped
  altogether (meas script will be generated later)
  
  script_data_ptr->meas_buf
  Buffer ptr where meas script should be populated if
  script_data_ptr->meas_action == RFDEVICE_MEAS_CREATE_SCRIPT.
  Meas script should be executed within the IRAT measurment gap as its
  execution will immediately disrupt source RAT and tune for away measurement.
 
  @param *paired_device
  Paired device object

  @param rx_mode
  Enum to indicate if tech is using Rx chain for PRx/DRx operation

  @return
  Success/Failure flag
  */
  boolean create_meas_script
  (
    rfm_mode_enum_type mode,
    rfcom_tdscdma_band_type band,
    uint16 chan,
    rfdevice_meas_scenario_type meas_type,
    rfdevice_meas_script_data_type *script_data_ptr,
    uint8 rx_lin_state,
    rfdevice_trx_tdscdma_rx* paired_device,
    rfdevice_rx_mode_type rx_mode
  );

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Push Rx band specific data to device
    
    @param band
    TDSCDMA band whose data is being pushed
   
    @param data_array
    Array of int32 containing device Rx data for the band
   
    @param array_size
    Size of array of int32 data to be pushed to device
  */
  boolean set_rx_band_data
  (
    rfcom_tdscdma_band_type band,
    int32* data_array,  
    uint8 array_size
  );

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Query Rx VCO path data from TRX reciever
    
    @param data_array
    Array of int32 containing device Rx data for band
   
    @param array_size
    Size of array of int32 data to be pushed to device
   
    @return
    VCO path number 
  */
  rfdevice_rx_vco_path_enum_type get_vco_path
  (
    int32* data_array,
    uint8 array_size
  );

  /*----------------------------------------------------------------------------*/
  /*! 
    @brief
    Program Rx RSB coefficients for TDSCDMA 

    @details
    RF MSM queries the WTR device to obtain the RSB coefficients from AG/EFS.
     
    @param - coeff
    RSB coefficients are stored depending upon the band information
    specified in this structure.
   
    @param *paired_device
    Paired device object

    @param rx_mode
    Enum to indicate if tech is using Rx chain for PRx/DRx operation

    @param - nv_rsb_data
    Rx RSB input data from NV item, if present.
    
    @return
    Status of success or not. 
  */
  boolean rx_get_rsb_coeff
  ( 
    rfm_device_enum_type rfm_dev, 
    rfdevice_tdscdma_rx_rsb_type *coeff,
    rfdevice_trx_tdscdma_rx* paired_device,
    rfdevice_rx_mode_type rx_mode,
    rfdevice_tdscdma_rx_rsb_cal_data_type *nv_rsb_data = NULL,
    boolean use_alt_gain = FALSE
  );

private:

  rftdscdma_device_rx_type *rx_device;

  rfdevice_rxtx_common_class* common_rxtx_dev;
};

#endif /*FEATURE_TDSCDMA*/
#endif /*RFDEVICE_TRX_TDSCDMA_TX_ADAPTER_H*/
