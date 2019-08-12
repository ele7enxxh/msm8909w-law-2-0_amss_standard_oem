#ifndef RFM_HDR_FTM_H
#define RFM_HDR_FTM_H

/*!
   @file
   rfm_hdr_ftm.h

   @brief
   RF Driver's HDR interface file for FTM.
*/

/*==============================================================================

  Copyright (c) 2013-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/rf/inc/rfm_hdr_ftm.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/02/14   spa     Added API rfm_hdr_ftm_query_iq_sample_line_offset 
03/20/14   spa     Initial support to test Rx on/off on RFLM side
06/18/13   JJ      Add func to register/de-register fw messages  
02/21/13   spa     Add API to send pilot meas stop stream to FW (FTM IRAT)
02/07/13   spa     Added rfm_hdr_ftm_pilot_meas_cfg prototype 
12/04/12   aro     Added device in FWS config interface
09/22/12   APU     Added FTM support for setting the individual pilot carrier 
                   power.
05/31/12   spa     Added rfm_hdr_ftm_tear_down_radio prototype 
04/18/12   aro     Moved FWS transition logic in upper layer
01/20/12   hdz     Changed interface of rfm_hdr_ftm_set_tx_pwr()
11/10/11   aro     Upadated RF-MDSP interfaces to pass semaphore to 
                   wait on for blocking calls
09/27/11   aro     Added test function to set TXC DP IQ Gain
09/12/11   cd      Update RFM FTM interface to set Tx power
08/12/11   aro     Added support for HDR LNA override clear
07/19/11   Saul    Combined FW enable/disable APIs into one FW config state API.
06/13/11   sty     Added carrier info in rfm_hdr_ftm_get_tx_agc_data
04/12/11   aro     Added Enable/Disable Firmware interface
03/24/11   aro     Unified the RxAGC Read Interface
03/14/11   aro     Renamed RxAGC structure
02/13/11   aro     Added Set Tx Power Interface
02/13/11   aro     [1] Updated LNA query interface
                   [2] Added support for Get TxAGC
02/12/11   aro     Changed the interface to return LNA Gain state and Gain word
01/24/11   aro     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfm_cdma_ftm_types.h"
#include "rfm_cdma_agc_types.h"
#include "rfm_cdma_tx_power_types.h"
#include "rfm_cdma_fw_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
boolean
rfm_hdr_ftm_set_tx_pwr
(
  rfm_device_enum_type device,
  const rfm_cdma_carrier_type carrier,
  rfm_cdma_tx_pwr_mode_type tx_pwr_mode,
  int16 tx_pwr
);

/*----------------------------------------------------------------------------*/
boolean
rfm_hdr_ftm_set_tx_iq_gain
(
  rfm_device_enum_type device,
  uint32 tx_dp_iq_gain
);

/*----------------------------------------------------------------------------*/
void
rfm_hdr_ftm_get_rx_agc_data
(
  const rfm_device_enum_type device,
  rfm_cdma_ftm_rx_agc_data_type *rx_agc_data,
  const rf_cdma_agc_format_type format
);

/*----------------------------------------------------------------------------*/
void
rfm_hdr_ftm_get_tx_agc_data
(
  const rfm_device_enum_type device,
  const rfm_cdma_carrier_type carrier,
  rfm_cdma_ftm_tx_agc_data_type *tx_agc_data,
  const rf_cdma_agc_format_type format
);

/*----------------------------------------------------------------------------*/
boolean
rfm_hdr_ftm_set_rx_gain_state
(
  rfm_device_enum_type device,
  boolean override_mode,
  rfm_lna_gain_state_type state
);

/*----------------------------------------------------------------------------*/
boolean
rfm_hdr_ftm_config_fw_state
(
  rfm_device_enum_type device,
  rfm_cdma_fw_state_type fw_state
);

/*----------------------------------------------------------------------------*/
boolean
rfm_hdr_ftm_tear_down_radio
( 
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean 
rfm_hdr_ftm_pilot_meas_cfg
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean 
rfm_hdr_ftm_fll_xo_cfg_msg
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean 
rfm_hdr_ftm_pilot_meas_stop_stream
(
   rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rfm_hdr_ftm_clear_tx_pwr_override
(
  rfm_device_enum_type device ,
  const rfm_cdma_carrier_type carrier ,
  rfm_cdma_tx_pwr_mode_type tx_pwr_mode 
) ;

/*----------------------------------------------------------------------------*/
boolean
rfm_hdr_ftm_register_fw_msgs
( 
  void
);

/*----------------------------------------------------------------------------*/
boolean
rfm_hdr_ftm_deregister_fw_msgs
( 
  void
);

/*----------------------------------------------------------------------------*/
boolean
rfm_hdr_ftm_test_rx_on_off
( 
  uint32 force_microsleep_enable_disable,
  uint32 rf_off_ustmr_delta,
  uint32 rf_on_ustmr_delta,
  uint32 duration_half_slots,
  uint32 sleep_mode
);

/*----------------------------------------------------------------------------*/
boolean
rfm_hdr_ftm_query_iq_sample_line_offset
(
  const rfm_cdma_carrier_type carrier,
  const rfm_device_enum_type device,
  uint32 *mempool_addr_line_offset
);

#ifdef __cplusplus
}
#endif

#endif /* RFM_HDR_FTM_H */

