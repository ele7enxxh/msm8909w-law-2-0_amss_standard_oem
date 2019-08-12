#ifndef RFLTE_CORE_HDET_H
#define RFLTE_CORE_HDET_H

/*! 
  @file
  rflte_core_hdet.h
 
 @brief
 This file is used to define an interface to all core layer APIs needed by
 TPL to run HDET.
 */

/*==============================================================================

  Copyright (c) 2013-15 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/rf/core/inc/rflte_core_hdet.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/28/15   as      Fix for TxAGC Value convertion
09/16/14   sbm/qzh Fix HDET error threshold to 12.3 dbm
06/18/14   sbm     Added support for WTR HDET
02/04/14    pv     Added support for FBRx Gain error processing.
09/05/13   cri     Initial check in

==============================================================================*/

#include "comdef.h"
#include "rfcom.h"
/* HDET read return packet def is part of do_mtpl, without moving that
   the def of the struct needs to stay in API folder. Do not want to add
   another L1 dep for now */
#include "rflte_core_txpl.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* FBRx Gain Error correction */
#define RFLTE_CORE_TXPL_ABS_FBRX_GAIN_ERR_MAX 123

#define RFLTE_CORE_TXPL_HDET_MAX 50000
#define RFLTE_CORE_TXPL_HPM_HDET_THRESHOLD 140
#define RFLTE_CORE_TXPL_LPM_HDET_THRESHOLD 10
#define RFLTE_CORE_TXPL_HDET_HPM_MIN_THRESHOLD 100
/* index for top of HDET curve high zone, 15 */
#define RFLTE_CORE_TXPL_EXP_HDET_VS_AGC_MAX_IDX  (LTE_HDET_VS_AGC_SIZ-1)
/* offset for the 256 element Tx AGC vs HDET table index */
#define RFLTE_CORE_TXPL_TX_AGC_VS_HDET_TBL_OFF   768
/* offset for Tx AGC vs HDET table index for low PDET mode*/
#define RFLTE_CORE_TXPL_TX_AGC_VS_HDET_TBL_OFF_LOW_PDET   670
/* Step size assumed in EXP_HDET_VS_AGC table in units of dB10 */
#define RFLTE_CORE_TXPL_EXP_HDET_VS_AGC_STEP_SIZE 16
/* max Tx AGC value (with offset) */
#define RFLTE_CORE_TXPL_TX_AGC_MAX               1023

/* Function Definitions */
/*----------------------------------------------------------------------------*/
uint16* rflte_core_txpl_get_hdet_vs_agc(uint32 carrier_idx, 
                                        int16 txagc_val);
/*----------------------------------------------------------------------------*/
int16 rflte_core_txpl_convert_hdet_adc_to_txagc(uint32 carrier_idx, 
                                               uint16 hdet_adc, 
                                               int16 txagc_val, 
                                               uint16 *exp_hdet_vs_agc_ptr);
/*----------------------------------------------------------------------------*/
/* TODO: ACK/TRK mode update based on power delta. Power delta can be
   measured using FBRx or HDET/TDET */
void rflte_core_txpl_update_mode(uint32 carrier_idx, 
                                 int16 power_delta);
/*----------------------------------------------------------------------------*/
float rflte_core_txpl_filter_hdet_error(uint32 carrier_idx, 
                                        int16 hdet_error);
/*----------------------------------------------------------------------------*/
/* TODO: move back once MC layer reorg is done. 
void rflte_core_txpl_set_hdet_low_power_mode(uint32 carrier_idx, 
                                             boolean enable);*/
/*----------------------------------------------------------------------------*/
//TODO: Make sure these get moved to state layer
//uint16 rflte_core_txpl_get_hdet_adc_for_logging(uint32 carrier_idx);
/*----------------------------------------------------------------------------*/
//int16 rflte_core_txpl_get_hdet_for_logging(uint32 carrier_idx);
/*----------------------------------------------------------------------------*/
//uint8 rflte_core_txpl_get_hdet_mode_for_logging(uint32 carrier_idx);
/*----------------------------------------------------------------------------*/
//uint16 rflte_core_txpl_get_pdet_mode_swpt(uint32 carrier_idx);
/*----------------------------------------------------------------------------*/
//boolean rflte_core_txpl_get_low_power_mode_enabled(uint32 carrier_idx);
/*----------------------------------------------------------------------------*/
//uint16 rflte_core_txpl_get_hdet_threshold(uint32 carrier_idx);
/*----------------------------------------------------------------------------*/
//uint8 rflte_core_txpl_get_lim_vs_freq_rb_block(uint32 carrier_idx);
/*----------------------------------------------------------------------------*/
uint16 rflte_core_txpl_get_hdet_adc(rfcom_lte_band_type band, 
                                    rflte_core_txpl_hdet_rd_result_type hdet_rd_event_result);
/*--------------------------------------------------------------------------*/
void rflte_core_txpl_do_mtpl_fw_reads_hdet_phy_rb(
#ifdef RFLTE_BOLT_UL_CA_SUPPORTED
                                                 uint32 carrier_idx,
#endif
                                                 int16 txpl_txagc_val, 
                                                 uint16 hdet_adc,
                                                 uint8 phy_rb_block,
                                                 uint8 phy_start_rb,
                                                 rflte_core_txpl_hdet_rd_result_type hdet_rd_event_result);  

void rflte_core_txpl_calc_hdet_err(uint32 carrier_idx,
                                   int16 txpl_txagc_val,
                                   uint16 hdet_adc,
                                   uint8 phy_rb_block,
                                   uint8 phy_start_rb,
                                   int16 *hdet_agc_error);

/* This API will support the FBRx MTPL update */
void rflte_core_txpl_do_fbrx_based_mtpl_update(uint8 carrier_idx, 
                                               int16 txpl_txagc_val, 
                                               int16 fbrx_gain_err,
                                               uint8 phy_rb_block,
                                               uint8 phy_start_rb,
                                               uint8 sys_bw );  
#ifdef __cplusplus
}
#endif

#endif /* RFLTE_CORE_HDET_H */

