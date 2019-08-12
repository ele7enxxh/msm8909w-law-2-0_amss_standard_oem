#ifndef RFDEVICE_TRX_TDSCDMA_TX_ADAPTER_H
#define RFDEVICE_TRX_TDSCDMA_TX_ADAPTER_H
/*!
   @file
   rfdevice_trx_tdscdma_tx_adapter.h

   @brief

*/

/*===========================================================================

Copyright (c) 2013 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_trx_tdscdma_tx_adapter.h#1 $

when       who    what, where, why
--------   ---   ------------------------------------------------------------------- 
05/22/15   dps   Added support for power logging using rfdevice_wtr_config_bitmask
09/05/14   zg    WTR TDSCDMA API update for STx DSDA
04/30/14   xsh   Added the fbrx rsb cal Implementation API
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
#include "rfdevice_trx_tdscdma_tx.h"

class rfdevice_trx_tdscdma_tx_adapter:public rfdevice_trx_tdscdma_tx
{
public:
  // Constructor
  rfdevice_trx_tdscdma_tx_adapter(rfdevice_rxtx_common_class *cmn_rxtx_dev,
                                  rfdevice_trx_phy_path_enum_type phy_path);
  // Destructor
  ~rfdevice_trx_tdscdma_tx_adapter();

  /*----------------------------------------------------------------------------*/
  boolean tx_init
  ( 
    rfm_device_enum_type device,
    rfcom_mode_enum_type mode,
    rfcom_tdscdma_band_type rf_band,
    rf_device_execution_type execution_type, 
    rf_buffer_intf* script_buffer
  );

  /*----------------------------------------------------------------------------*/
  boolean tx_enter_mode
  ( 
    rfm_device_enum_type device, 
    rfcom_mode_enum_type mode, 
    rfcom_tdscdma_band_type rf_band,
    uint16 chan,
    rf_device_execution_type execution_type, 
    rfdevice_meas_script_data_type *script_buffer
  );

  /*----------------------------------------------------------------------------*/
  boolean tx_enable
  ( 
    rfm_device_enum_type device,
    rfcom_mode_enum_type mode,
    rfcom_tdscdma_band_type rf_band,
    uint16 chan,
    rf_device_execution_type execution_type, 
    rfdevice_meas_script_data_type *script_data_ptr
  );

  /*----------------------------------------------------------------------------*/
  boolean tx_tune_chan
  ( 
    rfm_device_enum_type device,
    rfcom_mode_enum_type mode,
    rfcom_tdscdma_band_type rf_band,
    uint16 chan,
    rf_device_execution_type execution_type, 
    rfdevice_meas_script_data_type *script_data_ptr,
    rfdevice_wtr_config_bitmask_type *wtr_config_bitmask = NULL
  );

  /*----------------------------------------------------------------------------*/
  boolean tx_disable
  ( 
    rfm_device_enum_type device,
    rfcom_mode_enum_type mode,
    rfcom_tdscdma_band_type rf_band,
    rf_device_execution_type execution_type, 
    rf_buffer_intf* script_buffer
  );

  /*----------------------------------------------------------------------------*/
  boolean tx_cmd_dispatch
  ( 
    rfm_device_enum_type device, 
    rfcom_mode_enum_type mode,
    rfcom_tdscdma_band_type rf_band,
    rfdevice_cmd_enum_type cmd, 
    void *data 
  );

  /*----------------------------------------------------------------------------*/
  boolean tx_exit
  ( 
    rfm_device_enum_type device,
    rfcom_mode_enum_type mode,
    rfcom_tdscdma_band_type rf_band,
    rf_device_execution_type execution_type, 
    rf_buffer_intf* script_buffer
  );

  /*----------------------------------------------------------------------------*/
  boolean tx_hdet_read
  ( 
    rfm_device_enum_type rfm_dev,
    rfcom_tdscdma_band_type rf_band,
    boolean mdsp_triggered_read,
    uint16 *hdet_value, 
    rf_device_execution_type execution_type,
    rf_buffer_intf* script_buffer
  );

  /*----------------------------------------------------------------------------*/
  boolean tx_hdet_setup
  (
    rfm_device_enum_type rfm_dev,
    rfcom_tdscdma_band_type rf_band,
    rf_device_execution_type execution_type,
    rf_buffer_intf* script_buffer,
    boolean* data
  );

  /*----------------------------------------------------------------------------*/
  boolean calc_hdet_measurement
  (
    rfm_device_enum_type rfm_dev,
    rfcom_tdscdma_band_type rf_band,
    rfdevice_tdscdma_calc_hdet_measurement_type*  calc_hdet_meas
  );

  /*----------------------------------------------------------------------------*/
  rfdevice_tx_saw_port_type tx_get_port_type 
  (
    rfcom_tdscdma_band_type rf_band, 
    rfm_device_enum_type rfm_device 
  );

  /*----------------------------------------------------------------------------*/
  boolean calc_fbrx_rsb_nv_params 
  (
	rfm_device_enum_type rfm_dev,
	rfcom_tdscdma_band_type rf_band,
	uint8 tx_bw,
	rfdevice_rsb_cal_data_type *rsb_data
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
  rfdevice_rxtx_common_class* get_common_device_pointer(void); 

private:

  rftdscdma_device_tx_type *tx_device;

  rfdevice_rxtx_common_class* common_rxtx_dev;

};

#endif /* FEATURE_TDSCDMA*/
#endif /* RFDEVICE_TRX_TDSCDMA_TX_ADAPTER_H */

