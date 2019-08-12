#ifndef RFC_TDSCDMA_TRANSCEIVER_HDET_ADAPTER_H
#define RFC_TDSCDMA_TRANSCEIVER_HDET_ADAPTER_H

/*!
   @file
   rfc_tdscdma_transceiver_hdet_adapter.h

   @brief
   Declaration of the TDSCDMA Transceiver device to HDET interface adapter.
*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/common/inc/rfc_tdscdma_transceiver_hdet_adapter.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/16/13   zg      Updates to support C++ class based TDSCDMA RX and TX interface
03/22/13   ms      Featurize TDSCDMA
03/13/13   ms      Initial version.

==============================================================================*/

#ifdef FEATURE_TDSCDMA
#include "rfdevice_hdet_tdscdma.h"
#include "rfdevice_tdscdma_type_defs.h"

class rfc_tdscdma_transceiver_hdet_adapter : public rfdevice_hdet_tdscdma
{
public:
  rfc_tdscdma_transceiver_hdet_adapter(rftdscdma_device_tx_type *tdscdma_tx_device);
  ~rfc_tdscdma_transceiver_hdet_adapter();

  virtual void init();

  virtual boolean read_mdsp_triggered_incident(rf_buffer_intf* buff_obj_ptr,
                                               rf_device_execution_type dev_action,
                                               uint16* hdet_val);

  virtual boolean hdet_init(rf_buffer_intf* buff_obj_ptr,
                            rf_device_execution_type dev_action,
                            uint32 tx_freq);

  virtual boolean get_mdsp_config_data(rfdevice_hdet_mdsp_config_data_type* data);

  virtual boolean tx_hdet_read
  (
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  boolean mdsp_triggered_read,
  uint16 *hdet_value,
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action
  );

  virtual boolean tx_hdet_setup
  (   
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  rf_device_execution_type dev_action,
  rf_buffer_intf* buff_obj_ptr,
  void *data
  );

  virtual boolean tx_hdet_script_read
  (
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  rf_device_execution_type dev_action,
  rf_buffer_intf* buff_obj_ptr,
  void* data
  );

  virtual boolean calc_hdet_measurement
  (
  rfm_device_enum_type rfm_dev,
  rfcom_tdscdma_band_type rf_band,
  rfdevice_tdscdma_calc_hdet_measurement_type*  calc_hdet_meas
  );

private:
  rftdscdma_device_tx_type *tx_device;
}; /* class rfc_tdscdma_transceiver_hdet_adapter */

#endif

#endif /* RFC_TDSCDMA_TRANSCEIVER_HDET_ADAPTER_H */

