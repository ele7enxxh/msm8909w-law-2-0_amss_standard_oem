#ifndef RFC_WCDMA_TRANSCEIVER_HDET_ADAPTER_H
#define RFC_WCDMA_TRANSCEIVER_HDET_ADAPTER_H

/*!
   @file
   rfc_wcdma_transceiver_hdet_adapter.h

   @brief
   Declaration of the WCDMA Transceiver device to HDET interface adapter.
*/

/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/common/inc/rfc_wcdma_transceiver_hdet_adapter.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
03/11/13   hm      Updated HDET functions to directly call the C-function ptrs, 
                              instead of calling the functions in rfdevice_wcdma_intf file
                              done so because WTR does not provide HDET functionality and
                              hence these functions do not exist in base TRX class
02/01/13   ac      based on rfc the common api will call either tdet of trx hdet
12/03/12   bmg     Initial version.

==============================================================================*/

#include "rfdevice_hdet_wcdma.h"
#include "rfdevice_wcdma_type_defs.h"

class rfc_wcdma_transceiver_hdet_adapter : public rfdevice_hdet_wcdma
{
public:
  rfc_wcdma_transceiver_hdet_adapter(rfwcdma_device_tx_type *wcdma_tx_device);
  ~rfc_wcdma_transceiver_hdet_adapter();

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
  rfcom_wcdma_band_type rf_band,
  boolean mdsp_triggered_read,
  uint16 *hdet_value,
  rf_buffer_intf* buff_obj_ptr,
  rf_device_execution_type dev_action
  );

  virtual boolean tx_hdet_setup
  (   
  rfm_device_enum_type rfm_dev,
  rfcom_wcdma_band_type rf_band,
  rf_device_execution_type dev_action,
  rf_buffer_intf* buff_obj_ptr
  );

private:
  rfwcdma_device_tx_type *tx_device;
}; /* class rfc_wcdma_transceiver_hdet_adapter */

#endif /* RFC_WCDMA_TRANSCEIVER_HDET_ADAPTER_H */

