#ifndef RF_1X_TX_AGC_H
#define RF_1X_TX_AGC_H

/*!
  @file
  rf_1x_tx_agc.h

  @details
  This file includes prototypes and definitions pertaining to 1x TxAGC 
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/inc/rf_1x_tx_agc.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
07/10/13   JJ      Remove function rf_1x_tx_agc_set_qpoet_bias
03/07/13   sr      changed the "pwr_tracker" name to "papm".
12/11/12   Saul    CDMA. TX AGC XPT Dev. Get max PA state based on lin NV.
11/21/12   spa     Added API to set QPOET bias
08/24/12   cd      CDMA EPT Temp and HDET feature support
02/29/12   cd      - Added reverse link data rate enum type
                   - Added rf_1x_tx_agc_config_pa_params()
02/08/12   hdz     Cleaned up function
02/06/12   hdz     Added rf_1x_update_tx_agc for reloading tx agc in calls
01/19/12   cd      Use linearizer limit configuration API from PLIM module
12/01/11   cd      Provide TxLM handle information to configure Tx AGC  
11/03/11   cd      Add plim data to tx AGC configuration API
07/20/11   sty     Added NV pointers in arg list for rf_1x_configure_tx_agc
03/24/11   sty     Deleted rf_1x_load_tx_linearizers()
03/16/11   sty     Added band_chan arg to rf_1x_configure_tx_agc
03/13/11   sty     Added prototype for rf_1x_load_tx_linearizers
03/03/11   sty     Deleted redundant header files
03/03/11   sty     Changed arg in rf_1x_configure_tx_agc() to const
01/24/11   sty     Deleted rf_1x_tx_agc_param_type and removed it as an arg 
                   from rf_1x_configure_tx_agc()
01/19/11   aro     Renamed RF-MDSP filename
01/12/11   sty     Initial Release.

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfm_cdma_band_types.h"
#include "rf_cdma_nv.h"
#include "rf_cdma_power_limiting.h"

/* QPOET */
#include "rfdevice_papm_intf.h" 

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Enumeration for reverse link SCH rates supported for Tx AGC config */
typedef enum
{

  RF_1X_TX_AGC_SCH_FULL_RATE,
  /*!< RC3: 9.6 Kbps, RC4: 14.4 Kbps */

  RF_1X_TX_AGC_SCH_2X_RATE,
  /*!< RC3: 19.2 Kbps, RC4: 28.8 Kbps */

  RF_1X_TX_AGC_SCH_4X_RATE,
  /*!< RC3: 38.4 Kbps, RC4: 57.6 Kbps */

  RF_1X_TX_AGC_SCH_8X_RATE,
  /*!< RC3: 76.8 Kbps, RC4: 115.2 Kbps */

  RF_1X_TX_AGC_SCH_16X_RATE,
  /*!< RC3: 153.6 Kbps, RC4: 230.4 Kbps */

  RF_1X_TX_AGC_SCH_32X_RATE,
  /*!< RC3: 307.2Kbps, RC4: Not supported */

  RF_1X_TX_AGC_SCH_RATE_NUM
  /*!< Total number of SCH Rates Available */

} rf_1x_tx_agc_sch_rate_type;

/*----------------------------------------------------------------------------*/
boolean
rf_1x_configure_tx_agc
(
  rfm_device_enum_type const device,
  rfm_cdma_band_class_type const band_chan,
  uint16 const channel,
  lm_handle_type txlm_handle,
  const rf_1x_config_type *rf_1x_config_ptr
);

/*----------------------------------------------------------------------------*/
boolean
rf_1x_tx_agc_update_pa_params
(
  rfm_device_enum_type const device,
  rfm_cdma_band_class_type const rf_band,
  uint16 rf_channel,
  const rf_1x_config_type *rf_1x_config_ptr,
  rf_cdma_mdsp_pa_stm_config_type *pa_stm_data
);

#ifdef __cplusplus
}
#endif

#endif /* RF_1X_TX_AGC_H */
