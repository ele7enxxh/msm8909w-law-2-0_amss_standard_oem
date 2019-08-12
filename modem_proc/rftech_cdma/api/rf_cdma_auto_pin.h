#ifndef RF_CDMA_AUTO_PIN_H
#define RF_CDMA_AUTO_PIN_H
/*!
  @file
  rf_cdma_auto_pin.h

  @brief
  Provides CDMA auto pin Compensation functionality

  @details

*/

/*===========================================================================

Copyright (c) 2008 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:55 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/api/rf_cdma_auto_pin.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/20/15   vr      Autopin check-in
08/31/15   wwl     Add autopin c2k state machine support functions
06/26/15   wwl     Initial version.


============================================================================*/
#include "comdef.h"
#include "rfcom.h"
#include "rfcommon_time_profile.h"
#include "rfcommon_locks.h"
#include "rf_dispatch.h"

typedef enum
{
  AUTOPIN_SCRIPT_G1 = 1,
  AUTOPIN_SCRIPT_G2,
}rf_cdma_autopin_scrpt_type_enum;


/*----------------------------------------------------------------------------*/
void
rf_cdma_auto_pin_tx_wakeup
(
  rfm_device_enum_type tx_dev,
  rfm_mode_enum_type rfm_tech
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_auto_pin_tx_sleep
(
  rfm_device_enum_type tx_dev,
  rfm_mode_enum_type rfm_tech
);

/*----------------------------------------------------------------------------*/

void
rf_cdma_auto_pin_read_handler
(
  void *cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_auto_pin_data_valid
(
  rfm_device_enum_type tx_dev
);
/*----------------------------------------------------------------------------*/
boolean
rf_cdma_auto_pin_tx_state
(
  rfm_device_enum_type tx_dev
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_auto_pin_cmd_proc_init
(
  void
);

/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_configure_autopin_trigger
(

  rfm_device_enum_type device,
  rfm_mode_enum_type mode,
  const rfc_cdma_logical_device_params_type* logical_dev,
  rfm_cdma_band_class_type band
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_autopin_stop
(
  rfm_device_enum_type device
);

boolean
rf_cdma_autopin_update_dm
(
  rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type* logical_dev,
  rfm_mode_enum_type rfm_tech,
  const lm_handle_type txlm_handle,
  rfm_cdma_band_class_type band,
  uint16 chan
);

boolean
rf_cdma_autopin_par_support
(
  rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type* logical_dev,
  rfm_mode_enum_type rfm_tech,
  rfm_cdma_band_class_type band  
);

#endif /* RF_CDMA_AUTO_PIN_H */
