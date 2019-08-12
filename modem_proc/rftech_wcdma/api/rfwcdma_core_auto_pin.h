#ifndef RFWCDMA_CORE_AUTO_PIN_H
#define RFWCDMA_CORE_AUTO_PIN_H
/*!
  @file
  rfwcdma_core_auto_pin.h

  @brief
  Provides WCDMA auto pin Compensation functionality

  @details

*/

/*===========================================================================

Copyright (c) 2008 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/rfwcdma_core_auto_pin.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/02/15   ag      Add API to update pin through Autopin
11/30/15   ag      Initial version.


============================================================================*/
#include "comdef.h"
#include "rfcom.h"
#include "rfcommon_time_profile.h"
#include "rfcommon_locks.h"
#include "rf_dispatch.h"
#include "rfdevice_cmn_type_defs.h"


/*----------------------------------------------------------------------------*/

void rfwcdma_auto_pin_supported
(
rfm_device_enum_type device,
rfcom_wcdma_band_type band
);

/*----------------------------------------------------------------------------*/

void rfwcdma_core_autopin_init
(
uint32 tx_handle
);

/*----------------------------------------------------------------------------*/
void rfwcdma_core_autopin_update_pin
(
uint32 handle_id, 
uint8 pa_state,
int16 pin_error
);


/*----------------------------------------------------------------------------*/

void rfwcdma_auto_pin_trigger_read_handler
(
  void *cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data
);

/*----------------------------------------------------------------------------*/

void rfwcdma_auto_pin_trigger_read
(
);

#endif /* RFWCDMA_CORE_AUTO_PIN_H */

