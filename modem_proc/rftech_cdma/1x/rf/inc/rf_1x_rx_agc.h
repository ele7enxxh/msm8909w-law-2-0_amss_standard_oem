#ifndef RF_1X_RX_AGC_H
#define RF_1X_RX_AGC_H

/*!
  @file
  rf_1x_rx_agc.h

  @details
  This file includes prototypes and definitions pertaining to 1x RxAGC 
  configuration.
*/

/*==============================================================================

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/inc/rf_1x_rx_agc.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
03/29/12   shb     Added rf_state arg to rf_1x_configure_rx_agc as it is needed 
                   by device lna phase offset query api 
02/27/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan  
02/06/12   hdz     Added rf_1x_update_rx_agc for reloading rx agc in calls 
12/29/11   aro     Updated configure RxAGC interface to have RxLM buffer
07/18/11   aro     Updated RxAGC Config interface to pass AGC Mode Param
03/16/11   aro     Removed unnecessary include
03/14/11   aro     Deleted Activate RxAGC interface
02/01/11   aro     Promoted Power Mode types to RFM
01/23/11   aro     Updated RxAGC config Interface
01/19/11   aro     Renamed RF-MDSP filename
01/04/11   aro     [1] Added RxAGC Config Load and Activate functions
                   [2] Changed Activate function name
12/29/10   aro     Updated rf_1x_rx_agc_configure() to have band_chan as arg
12/19/10   aro     Updated rf_1x_rx_agc_configure() to have band and PM
12/23/10   aro     [1] Added syub to configrue RxAGC
                   [2] Removed Path to replace with device
12/13/10   aro     Initial Release.

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfm_cdma_power_mode_types.h"
#include "rf_1x_mdsp.h"
#include "rf_1x_agc_types.h"
#include "sys.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
boolean
rf_1x_configure_rx_agc
(
  rfm_device_enum_type device,
  lm_handle_type rxlm_handle,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan,
  rfm_cdma_power_mode_type power_mode,
  rf_cdma_state_type rf_state,
  const rf_1x_agc_param_type* const acq_mode_data
);

/*----------------------------------------------------------------------------*/
void rf_1x_rx_agc_freeze
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void rf_1x_rx_agc_unfreeze
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_update_rx_agc
(
  rfm_device_enum_type device
);


#ifdef __cplusplus
}
#endif

#endif /* RF_1X_RX_AGC_H */
