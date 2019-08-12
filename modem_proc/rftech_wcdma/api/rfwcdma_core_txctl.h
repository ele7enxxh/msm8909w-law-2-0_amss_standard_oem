#ifndef RFWCDMA_CORE_TXCTL_H
#define RFWCDMA_CORE_TXCTL_H
/*!
   @file
   rfwcdma_core_txctl.h

   @brief
   Provides TX control. Example PA ctl is handled here.

   @details


*/

/*===========================================================================

Copyright (c) 2013 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/rfwcdma_core_txctl.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/05/15   ag      Add Autopin events for tx dev config type
03/12/15   gh      Added support to build therm_enable script and event
11/4/14    ac      ftm flag to denote the txagc state 
09/15/14   dw      WXE based Tx tune support for Single Tx
09/15/14   vbh     Added device as a parameter to support Tx sharing
09/02/14   dw      Disable TxAGC during Tx bw change
08/06/14   dw      Added rfwcdma_core_tx_disable_txagc()
08/01/14   dw      Initial support for single Tx DSDA
05/15/14   aro     Added buffer to hold Power meter trigger script
05/15/14   rmb     Added API for config device scripts in Tx.
05/01/14   dw      Added update ET delay and set freq adjust in MDSP async set data command
04/09/14   dw      DCHSUPA feature cleanup
03/25/13   dw      Add multi_carrier idx and TxLM handle in config mc
03/11/13   dw      Add Bolt DCHSUPA support
02/21/14   rmb     Add global NV pointers for each of band specific static NVs.
11/25/13   kai     Added generic API to query WCDMA Tx device pointers
09/09/13   dw      Core changes for new Bolt MDSP module - new API to config Tx band/chan
06/26/13   ac      tx freezeaccum api change for both car
12/11/12   dw      Add support for DTx and DRx/LPM. 
05/08/12   tks     Add support for CPC DTx 
04/17/12   gh      Add support for TX_GTR_THRESH
03/01/01   dw      [1] Add support for HSPA APT
                   [2] Rework some call flows for Tx AGC/linearizer control
09/19/11   dw      clean up rfwcdma_core_txctl_set_tx_modulator() 
03/02/11   dw      Nikel updates
01/28/11   dw      Initial TxLM support for WCDMA
10/11/10   ad      Added preliminary support for WCDMA APT Phase 1  
10/01/09   ckl     Added rfwcdma_core_txctl_init_pa_ctl( )
09/17/09   ckl     Moved some functions from mc file.
11/06/08   av      Initial version.

============================================================================*/
#include "comdef.h"
#include "rfcom.h"
#include "rfnv_wcdma.h"
#include "rflm_dm_api.h"
#include "rf_buffer_intf.h"
#include "rfdevice_cmn_type_defs.h"

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Enum definition of RF WCDMA Tx device config type

*/
typedef enum
{
  /*! Tx ON config */
  TX_ON_CFG,
  /*! Tx OFF config */
  TX_OFF_CFG,
  /*! PA ON config */
  PA_ON_CFG,
  /*! PA OFF config */
  PA_OFF_CFG,
  /*! DTx Enable Event (Toggle Tx PLL OFF).*/
  DTX_ENABLE_CFG,
  /*! DTx Disable Event (Toggle Tx PLL ON).*/
  DTX_DISABLE_CFG,
  /*! Event to trigger the Power Meter measurement */
  POWER_METER_TRIGGER,
  /*! TX TUNE event */
  TX_TUNE,
  /*! THERM enable */
  THERM_ENABLE,
  /*! Enable Autopin event for gain state1 */
  ENABLE_AUTOPIN_FBRX_G1,
  /*! Enable Autopin event for gain state 2 */
  ENABLE_AUTOPIN_FBRX_G2,
  /*! Number of Tx device config type */
  TX_DEV_CFG_NUM
}rfwcdma_core_tx_dev_cfg_type;

/*--------------------------------------------------------------------------*/
void rfwcdma_core_txctl_init_bsp_agc_ctl(rfm_device_enum_type device,rfcom_wcdma_band_type band);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_txctl_config_band
( 
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band, 
  uint32 txlm_handle,
  boolean tx_bw_change
);

/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_txctl_config_chan
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band, 
  uint16 tx_chan,
  uint32 txlm_handle 
);

/*--------------------------------------------------------------------------*/
boolean rfwcdma_core_txctl_txagc_config
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band, 
  lm_handle_type txlm_handle
);

/*--------------------------------------------------------------------------*/
void rfwcdma_core_txctl_config_mc
(
  rfcom_wcdma_band_type band,   
  uint32 multi_carrier_idx,
  uint16 tx_chan[RFCOM_MAX_CARRIERS],
  uint32 txlm_handle
);
/*----------------------------------------------------------------------------*/
void rfwcdma_core_txctl_init_pa_ctl(void);

/*----------------------------------------------------------------------------*/
int16 rfwcdma_core_txctl_enable_pwr_ctrl(void);

/*----------------------------------------------------------------------------*/
int16 rfwcdma_core_txctl_set_tx_freezeaccum(uint8 val,rfcom_multi_carrier_id_type car_idx);

/*----------------------------------------------------------------------------*/
boolean rfwcdma_core_txctl_smps_pa_nv_validity(rfnv_wcdma_nv_tbl_type *rfnv_wcdma_tbl);

/*----------------------------------------------------------------------------*/
void rfwcdma_core_txctl_set_tx_gtr_thresh_params(rfm_device_enum_type device,rfcom_wcdma_band_type band);

/*----------------------------------------------------------------------------*/
boolean 
rfwcdma_core_txctl_get_device_info
(
  rfm_device_enum_type device,
  rfcom_wcdma_band_type band,
  rfcommon_core_txlin_agc_device_info *device_info
);

/*----------------------------------------------------------------------------*/
boolean rfwcdma_core_tx_set_freq_adjust
(
  rflm_dm_handle_id_t rflm_handle,
  rfcom_wcdma_band_type band,
  uint16 rx_chan,
  int32 tx_freq_error
);
/*----------------------------------------------------------------------------*/
boolean rfwcdma_core_txctl_config_tune_device( rfm_device_enum_type device,
                                               rfcom_wcdma_band_type band,
                                               uint16 channels[RFCOM_MAX_CARRIERS],
                                               uint32 txlm_handle,
                                               rf_buffer_intf *tx_device_buffer,
                                               rf_device_execution_type execution_type,
                                               uint32* wtr_tune_execution_time );
/*----------------------------------------------------------------------------*/
boolean rfwcdma_core_tx_disable_txagc
(
  rflm_dm_handle_id_t rflm_handle,
  boolean disable
);
/*----------------------------------------------------------------------------*/
boolean rfwcdma_core_tx_ftm_disable_ldo_off
(
  rflm_dm_handle_id_t rflm_handle,
  boolean disable
);

/*----------------------------------------------------------------------------*/
void rfwcdma_core_create_npa_client(void);

#endif /* RFWCDMA_CORE_TXCTL_H */
