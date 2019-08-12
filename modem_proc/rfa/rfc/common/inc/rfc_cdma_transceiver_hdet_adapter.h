#ifndef RFC_CDMA_TRANSCEIVER_HDET_ADAPTER_H
#define RFC_CDMA_TRANSCEIVER_HDET_ADAPTER_H

/*!
   @file
   rfc_cdma_transceiver_hdet_adapter.h

   @brief
   Declaration of the CDMA Transceiver device to HDET interface adapter.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rfc/common/inc/rfc_cdma_transceiver_hdet_adapter.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/06/12   bmg     Added get_fw_trigger_script() interface.
12/03/12   bmg     Initial version.

==============================================================================*/

#include "rfdevice_hdet_cdma.h"
#include "rfdevice_cdma_type_defs.h"

class rfc_cdma_transceiver_hdet_adapter : public rfdevice_hdet_cdma
{
public:
  rfc_cdma_transceiver_hdet_adapter(rfdevice_cdma_tx_type* cdma_tx_ptr);
  ~rfc_cdma_transceiver_hdet_adapter();

  virtual void init();

  virtual uint16 multi_read_hdet(uint16 num_of_reads);

  virtual boolean setup_hdet(rf_buffer_intf* buff_obj_ptr,
                             rf_device_execution_type dev_action,
                             rfdevice_cdma_hdet_power_mode_type power_mode);

  virtual boolean read_hdet(boolean mdsp_triggered_read,
                            rfdevice_cdma_hdet_read_type* hdet_output);

  virtual boolean hdet_init(rf_buffer_intf* buff_obj_ptr,
                            rf_device_execution_type dev_action,
                            uint32 tx_freq);

  virtual boolean get_hdet_scripts(rfdevice_cdma_hdet_scripts_type* hdet_scripts);

  virtual boolean get_fw_trigger_script(rf_buffer_intf* buff_obj_ptr);

  virtual boolean get_hdet_param(rfdevice_cdma_hdet_param_type* hdet_param);

  virtual boolean enable_hdet(rf_buffer_intf* buff_obj_ptr,
                              rf_device_execution_type dev_action);

  virtual boolean disable_hdet(rf_buffer_intf* buff_obj_ptr,
                               rf_device_execution_type dev_action);

private:
  rfdevice_cdma_tx_type* cdma_tx_ptr;
}; /* class rfc_cdma_transceiver_hdet_adapter */

#endif /* RFC_CDMA_TRANSCEIVER_HDET_ADAPTER_H */
