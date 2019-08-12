#ifndef RF_HDR_TX_AGC_H
#define RF_HDR_TX_AGC_H

/*!
  @file
  rf_hdr_tx_agc.h

  @details
  This file includes prototypes and definitions pertaining to HDR TxAGC 
  configuration.
*/

/*==============================================================================

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/rf/inc/rf_hdr_tx_agc.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/11/12   Saul    CDMA. TX AGC XPT Dev. Get max PA state based on lin NV.
08/24/12   cd      CDMA EPT Temp and HDET feature support
02/29/12   cd      - Added reverse link data RRI enum type
                   - Added rf_hdr_tx_agc_config_pa_params()
02/09/12   Saul    MCDO. Tx AGC uses MCDO switch-points.
02/08/12   hdz     Cleaned up function 
02/06/12   hdz     Added rf_hdr_update_tx_agc for reloading tx agc in calls 
01/19/12   cd      Use linearizer limit configuration API from PLIM module
12/01/11   cd      Provide TxLM handle information to configure Tx AGC  
11/03/11   cd      Add plim data to tx AGC configuration API 
07/20/11   sty     Added NV pointers in arg list for rf_hdr_configure_tx_agc
03/25/11   sty     Added rf_hdr_set_tx_power_adjust
03/21/11   sty     Added band_chan as arg in rf_hdr_configure_tx_agc
02/02/11   sty     Initial Release.
==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfm_cdma_band_types.h"
#include "rf_cdma_nv.h"
#include "rf_cdma_power_limiting.h"

/*----------------------------------------------------------------------------*/
/*! Enumeration for RRI rates supported for Tx AGC config */
typedef enum
{

  RF_HDR_TX_AGC_PILOT_ONLY_RRI,
  /*!< RRI0: 0 kbps */

  RF_HDR_TX_AGC_9K6_BPS_RRI,
  /*!< RRI1: 9.6 kbps */

  RF_HDR_TX_AGC_19K2_BPS_RRI,
  /*!< RRI2: 19.2 kbps */

  RF_HDR_TX_AGC_38K4_BPS_RRI,
  /*!< RRI3: 38.4 kbps */

  RF_HDR_TX_AGC_76K8_BPS_RRI,
  /*!< RRI4: 76.8 kbps */

  RF_HDR_TX_AGC_153K6_BPS_RRI,
  /*!< RRI5: 153.6 kbps */

  RF_HDR_TX_AGC_REVA_RRI,
  /*!< RRI6: DO Rev A */

  RF_HDR_TX_AGC_RRI_NUM
  /*!< Total number of RRI Available */

} rf_hdr_tx_agc_rri_type;

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_configure_tx_agc
(
  rfm_device_enum_type const device,
  uint32 num_carriers,
  rfm_cdma_band_class_type const band_chan,
  uint16 const rf_channel,
  lm_handle_type txlm_handle,
  const rf_hdr_config_type *rf_hdr_config_ptr
);

/*----------------------------------------------------------------------------*/
void
rf_hdr_set_tx_power_adjust
(
  rfm_device_enum_type const device,
  const rfm_cdma_band_class_type band,
  const int16 power_dbx2,
  int16 const curr_nominal_pwr
);

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_tx_agc_update_pa_params
(
  rfm_device_enum_type const device,
  uint32 num_carriers,
  rfm_cdma_band_class_type const rf_band,
  uint16 rf_channel,
  const rf_hdr_config_type *rf_hdr_config_ptr,
  rf_cdma_mdsp_pa_stm_config_type *pa_stm_data
);


#endif /* RF_HDR_TX_AGC_H */

