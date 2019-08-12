#ifndef RFDEVICE_TRX_TDSCDMA_TX__H
#define RFDEVICE_TRX_TDSCDMA_TX__H
/*!
  @file
  rfdevice_trx_tdscdma_tx.h

  @brief
  Base class for TDSCDMA RF device interface 

*/

/*===========================================================================

Copyright (c) 2013-2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_trx_tdscdma_tx.h#1 $

when       who    what, where, why
--------   ---   ------------------------------------------------------------------- 
05/22/15   dps   Added support for power logging using rfdevice_wtr_config_bitmask
09/05/14   zg    WTR TDSCDMA API update for STx DSDA
04/30/14   xsh   Added the fbrx rsb cal API
04/10/14   zg    Added supporting to push band data from RFC.
12/09/13   rp    Added an interface to retrieve common device pointer associated with 
                 given tech trx class.
11/07/13   rp    Added and API to return common device class pointer.
08/20/13   zg    Initial version
============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "rf_buffer_intf.h"

#ifdef FEATURE_TDSCDMA
#include "rfdevice_tdscdma_type_defs.h"

#ifdef __cplusplus
#include "rfdevice_class.h"
#include "rfdevice_rxtx_common_class.h"


class rfdevice_trx_tdscdma_tx:public rfdevice_class 
{
  public:

  // Constructor
  rfdevice_trx_tdscdma_tx();

  // Destructor
  ~rfdevice_trx_tdscdma_tx();

  /*----------------------------------------------------------------------------*/
  virtual boolean tx_init
  ( 
    rfm_device_enum_type device,
    rfcom_mode_enum_type mode,
    rfcom_tdscdma_band_type rf_band,
    rf_device_execution_type execution_type, 
    rf_buffer_intf* script_buffer
   )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean tx_enter_mode
  ( 
    rfm_device_enum_type device, 
    rfcom_mode_enum_type mode, 
    rfcom_tdscdma_band_type rf_band,
    uint16 chan,
    rf_device_execution_type execution_type, 
    rfdevice_meas_script_data_type *script_data_ptr
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean tx_enable
  ( 
    rfm_device_enum_type device,
    rfcom_mode_enum_type mode,
    rfcom_tdscdma_band_type rf_band,
    uint16 chan,
    rf_device_execution_type execution_type, 
    rfdevice_meas_script_data_type *script_data_ptr
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean tx_tune_chan
  ( 
    rfm_device_enum_type device,
    rfcom_mode_enum_type mode,
    rfcom_tdscdma_band_type rf_band,
    uint16 chan,
    rf_device_execution_type execution_type, 
    rfdevice_meas_script_data_type *script_data_ptr,
    rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL
   )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean tx_disable
  ( 
    rfm_device_enum_type device,
    rfcom_mode_enum_type mode,
    rfcom_tdscdma_band_type rf_band,
    rf_device_execution_type execution_type, 
    rf_buffer_intf* script_buffer
   )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean tx_cmd_dispatch
  ( 
    rfm_device_enum_type device, 
    rfcom_mode_enum_type mode,
    rfcom_tdscdma_band_type rf_band,
    rfdevice_cmd_enum_type cmd, 
    void *data 
   )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean tx_exit
  ( 
    rfm_device_enum_type device,
    rfcom_mode_enum_type mode,
    rfcom_tdscdma_band_type rf_band,
    rf_device_execution_type execution_type, 
    rf_buffer_intf* script_buffer
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean tx_hdet_read
  ( 
    rfm_device_enum_type rfm_dev,
    rfcom_tdscdma_band_type rf_band,
    boolean mdsp_triggered_read,
    uint16 *hdet_value, 
    rf_device_execution_type execution_type,
    rf_buffer_intf* script_buffer
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean tx_hdet_setup
  (
    rfm_device_enum_type rfm_dev,
    rfcom_tdscdma_band_type rf_band,
    rf_device_execution_type execution_type,
    rf_buffer_intf* script_buffer,
    boolean* data
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean calc_hdet_measurement
  (
    rfm_device_enum_type rfm_dev,
    rfcom_tdscdma_band_type rf_band,
    rfdevice_tdscdma_calc_hdet_measurement_type*  calc_hdet_meas
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual rfdevice_tx_saw_port_type tx_get_port_type 
  (
    rfcom_tdscdma_band_type rf_band, 
    rfm_device_enum_type rfm_device 
  )=0;

  /*----------------------------------------------------------------------------*/
  virtual boolean calc_fbrx_rsb_nv_params ( rfm_device_enum_type rfm_dev,
                     rfcom_tdscdma_band_type rf_band,
                     uint8 tx_bw,  // not used for tdscdma fbrx rsb cal, it's for consitent with other tech api and for potential usuage in the future
                     rfdevice_rsb_cal_data_type *rsb_data )=0;


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
  virtual boolean enter_critical_section() = 0;

  /*----------------------------------------------------------------------------*/
  /*!
  @brief
  Disable Critical Section

  @details
  This function will release lock which was acquired for the preceecing code.
 
  @return
  Success/failure status flag
  */
  virtual boolean leave_critical_section() = 0;

  /*----------------------------------------------------------------------------*/
  /*!
  @brief
  Get rfdevice_rxtx_common_class pointer associated with this instance of device.
  
  @return 
  Pointer to rxtx common device associated with current instance of device.
  */
  virtual 
  rfdevice_rxtx_common_class* get_common_device_pointer() = 0;

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Push Tx band specific data to device

    @param band
    TDSCDMA band whose data is being pushed

    @param data_array
    Array of int32 containing device Tx data for the band

    @param array_size
    Size of array of int32 data to be pushed to device
  */
  virtual boolean set_tx_band_data
  (
    rfcom_tdscdma_band_type band,
    int32* data_array,  
    uint8 array_size
  )
  {
    RF_MSG( RF_ERROR, "set_tx_band_data: Derived class does not support this API!" );
    return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  virtual boolean tx_pwr_sleep ()
  {
    RF_MSG( RF_ERROR, "tx_pwr_sleep: Derived class does not support this API!" );
    return FALSE;
  }

  /*----------------------------------------------------------------------------*/
  virtual boolean tx_pwr_wakeup ()
  {
    RF_MSG( RF_ERROR, "tx_pwr_wakeup: Derived class does not support this API!" );
    return FALSE;
  }

};
#else

struct rfdev_tdscdma_tx;
typedef struct rfdev_tdscdma_tx rfdevice_trx_tdscdma_tx;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get rfdevice_rxtx_common_class pointer associated with this instance of device.
  
  @return 
  Pointer to rxtx common device associated with current instance of device.
*/
rfdevice_rxtx_common_class* 
rfdevice_tdscdma_tx_get_common_device_pointer(rfdevice_trx_tdscdma_tx* tx_device);

#endif /* __cplusplus */
#endif /* FEATURE_TDSCDMA */
#endif /* RFDEVICE_TRX_TDSCDMA_TX__H*/

