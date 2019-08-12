#ifndef RF_HDR_RX_AGC_H
#define RF_HDR_RX_AGC_H

/*!
  @file
  rf_hdr_rx_agc.h

  @details
  This file includes prototypes and definitions pertaining to HDR RxAGC 
  configuration.
*/

/*==============================================================================

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/rf/inc/rf_hdr_rx_agc.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/11/14   spa     Move configure JD thresh API to cdma rxagc 
06/07/13   spa     Added band as arg to jammer detect thresh API 
04/13/13   spa     Added is_irat_mode flag to configure_rx_agc
03/29/12   shb     Added rf_state arg to rf_hdr_configure_rx_agc as it is needed 
                   by device lna phase offset query api  
03/27/13   ry      Added interface for rf_hdr_configure_jd_thresh()
03/08/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan  
03/02/12   Saul    MCDO. Rx AGC freq comp and device droop compensation framework.
02/06/12   hdz     Added rf_hdr_mdsp_resume_rx_agc() 
12/29/11   aro     Updated configure RxAGC interface to have RxLM buffer
11/30/11   sty     Added arg tune_lo_to_band_chan to rf_hdr_configure_rx_agc()
07/18/11   aro     Updated Configure RxAGC interface to pass only required
                   param to simplify intelliceiver interface
06/07/11   Saul    Updates for multi-carrier driver.
04/21/11   aro     [1] Moved ADC Clock Manager to HAL
                   [2] Moved RxAGC implementation to RFM
04/11/11   aro     Removed unused RxAGC deactivate interface
03/24/11   aro     Updated HDR RxAGC Configure interface
03/03/11   aro     Added HDR RxAGC deactivate function
02/01/11   aro     Promoted Power Mode types to RFM
01/23/11   aro     Added Support for HDR RxAGC Query
01/19/11   aro     Added LNA Config and RxAGC activate interfaces
01/18/11   aro     Added Configure HDR RxAGC function
12/23/10   aro     Removed Path to replace with device
12/013/10  aro     Initial Release.

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfm_hdr_agc_types.h"
#include "rfm_cdma_carrier_types.h"
#include "rfm_cdma_power_mode_types.h"
#include "sys.h"
#include "rf_cdma_utils.h"
#include "rf_cdma_state_types.h"
#include "rf_cdma_data.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_configure_rx_agc
(
  rfm_device_enum_type device,
  lm_handle_type rxlm_handle,
  uint32 num_carriers,
  rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type * const chan_array,
  const rfm_cdma_chan_type tune_lo_to_chan,
  const rfm_hdr_agc_acq_type* const acq_mode_data,
  rfm_cdma_power_mode_type power_mode,
  rf_cdma_state_type rf_state,
  boolean is_irat_mode,
  const uint16 * const carrier_index
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_rx_agc_freeze
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_rx_agc_unfreeze
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_update_rx_agc
(
  rfm_device_enum_type device
);


#ifdef __cplusplus
}
#endif

#endif /* RF_HDR_RX_AGC_H */

