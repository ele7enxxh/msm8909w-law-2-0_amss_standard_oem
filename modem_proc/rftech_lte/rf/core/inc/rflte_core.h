#ifndef RFLTE_CORE_H
#define RFLTE_CORE_H

/*! 
  @file
  rflte_core.h
 
 @brief
 This file is used to define an interface to all core layer APIs needed by
 the MC layer.
 */

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/rf/core/inc/rflte_core.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/30/15   bir     Added support for tx max pwr based EARFCN NV
08/18/14   kab     TxPL changes to support SGLTE Tx power backoff with Coex.
07/31/14   ndb     Added support for extended EARFCN for LTE
06/13/14   svi     Notch Application based on Tx AFC
09/12/13   svi     Added MC interfaces
09/05/13   cri     Initial check in

==============================================================================*/

#include "comdef.h"
#include "rfcom.h"
#include "rflte_mc.h"
#include "rflte_nv.h"
#include "rfc_card_lte.h"
#include "rfcommon_core_xpt.h"
#include "rflte_state.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
  RFLTE_CORE_SGLTE_MTPL_UPDATE,
  RFLTE_CORE_WLAN_COEX_MTPL_UPDATE
}rflte_core_mtpl_type;

#define RF_MTPL_LOG_PACKET  0

extern rflte_core_txpl_sm_type rflte_core_txpl_sm[RFA_RF_LTE_MAX_CELL_SUPPORTED];

/*----------------------------------------------------------------------------*/
void rflte_core_enter_mode(void);
/*----------------------------------------------------------------------------*/
void rflte_core_exit_mode(uint32 carrier_index);
/*----------------------------------------------------------------------------*/
void rflte_core_sleep(uint32 carrier_index);
/*----------------------------------------------------------------------------*/
void rflte_core_tx_config(rflte_mc_carrier_info_type carrier_info,
                          rfcom_lte_band_type band,
                          rfcom_lte_bw_type bw,
                          rfc_lte_band_lpm_type lpm_status_info,
                          xpt_cfg_type xpt_cfg,
                          rfa_rf_lte_tdd_ul_dl_cfg_type tdd_ul_dl_cfg,
                          rfcom_lte_earfcn_type tx_channel);
/*----------------------------------------------------------------------------*/
void rflte_core_tx_disable(rflte_mc_carrier_info_type carrier_info,
                           rfcom_lte_band_type band,
                           xpt_cfg_type xpt_cfg);
/*----------------------------------------------------------------------------*/
void rflte_core_update_rxagc_freq_comp(rfm_device_enum_type device,
                                       rfcom_lte_band_type band,
                                       rfcom_lte_earfcn_type rx_chan,
                                       rflte_nv_tbl_type* nv_tbl_ptr,
                                       boolean meas_nbr_flag,
                                       boolean intra_band_ca_flag,
                                       uint32 carrier_idx,
                                       boolean apply_ota_flag);
/*----------------------------------------------------------------------------*/
void rflte_core_update_sar(void);
/*----------------------------------------------------------------------------*/
void rflte_core_set_mtpl(rflte_core_mtpl_type mtpl_type,
                         uint32 carrier_idx,
                         uint32 plimit_type_mask,
                         int16 sar_limit_dB10,
                         int16 coex_limit_dB10,
                         int16 wcn_limit_dB10,
                         int16 subframe_coex_limit_dB10);
/*----------------------------------------------------------------------------*/
void rflte_core_enter_critical_section(uint32 carrier_idx);
/*----------------------------------------------------------------------------*/
void rflte_core_leave_critical_section(uint32 carrier_idx);
/*----------------------------------------------------------------------------*/
void rflte_core_program_afc_notch_update ( uint32 cell_index );
/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* RFLTE_CORE_H */
