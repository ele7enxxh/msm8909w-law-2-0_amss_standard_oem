#ifndef RF_CDMA_TX_AGC_MSM_H
#define RF_CDMA_TX_AGC_MSM_H

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_tx_agc_msm.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
03/08/12   cd      Obtain GRFC information from RFC
01/24/12   sty     Added nv_path and band to rf_hdr_get_tx_agc_params()
01/18/12   aro     Updated interface to query Modem specific TxGAC param to use
                   TxLM chain rather than device
06/13/11   sty     Initial Release
==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rf_cdma_mdsp_types.h"
#include "txlm_intf.h"
#include "rf_cdma_nv.h"

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_get_tx_agc_params
(
  rfm_device_enum_type device,
  txlm_chain_type txlm_chain,
  rf_cdma_nv_path_type path,
  rfm_cdma_band_class_type band,
  rf_cdma_mdsp_pa_stm_config_type *pa_stm_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_get_tx_agc_params
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type rf_band,
  txlm_chain_type txlm_chain,
  rf_cdma_mdsp_pa_stm_config_type *pa_stm_data
);

#endif /* RF_CDMA_TX_AGC_MSM_H */

