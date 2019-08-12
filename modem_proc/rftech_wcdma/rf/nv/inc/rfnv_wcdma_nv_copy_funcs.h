#ifndef RFNV_WCDMA_NV_COPY_FUNCS_H
#define RFNV_WCDMA_NV_COPY_FUNCS_H

/*!
  @file
  rfnv_wcdma_nv_copy_funcs.h

  @brief

  @details
  This header file contains definitions to interface with RF NV for WCDMA.

*/

/*===========================================================================

Copyright (c) 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/nv/inc/rfnv_wcdma_nv_copy_funcs.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/03/14   kai     Added support for WCDMA Rx V3 static NVs
10/31/14   ac      tuner NV support for DBDC
09/26/14   kai     Added support for WCDMA Rx V2 NVs
05/01/14   aro     NV item to hold char data for DC ET Delay vs freq vs temp
02/26/14   zhh    Update XPT Delay structure to support DC-UPA
04/14/14   zhh     PinPout NV for DCUPA
03/20/14   zhh     update new MPR backoff NV
04/20/14   bn       WCDMA 8 PA state support and related cleanup
01/15/14   rmb      Add NV support for Noise Floor Correction.
11/25/13   rmb      Add Pin Cal versus freq related NVs for all PA states and bands.
08/23/13   vs       Added Rx static and Cal NVs
08/12/13   kai      Added DPD scaling support 
07/24/13   kai      Added XPT path delay offset vs temp vs freq 
07/16/13   vs       Added CL tuner NVs
06/05/13   kcj      Initial Revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfcommon_nv.h"
#include "rfnv_wcdma.h"
#include "rfcom.h"

/*===========================================================================

                         FUNCTION PROTOTYPES

===========================================================================*/

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_hs_range_rise
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_hs_range_rise_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_hs_range_rise_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_hs_range_rise_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_hs_range_rise_5
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_hs_range_fall
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_hs_range_fall_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_hs_range_fall_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_hs_range_fall_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_hs_range_fall_5
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_im_hs_level
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_im_hs_level_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_im_hs_level_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_im_hs_level_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_vga_gain_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_vga_gain_offset_vs_freq
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_vga_gain_offset_vs_freq
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_vga_gain_offset_vs_temp
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_im2_i_value
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_im2_q_value
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_im2_transconductor_value
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
/* R99 LNA Range Functions */

void rfnv_wcdma_copy_lna_range_rise
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_rise_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_rise_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_rise_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_rise_5
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_fall
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_fall_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_fall_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_fall_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_fall_5
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_im_level
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_im_level_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_im_level_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_im_level_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
/* Timer Copy Functions */
void rfnv_wcdma_copy_nonbypass_timer
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_bypass_timer
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
/* LNA Range Offset Copy Functions */
void rfnv_wcdma_copy_lna_range_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_offset_2
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_offset_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_offset_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_offset_5
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_offset_vs_freq
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_offset_vs_freq
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_offset_vs_freq_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_offset_vs_freq_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_offset_vs_freq_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_offset_vs_freq_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_offset_vs_freq_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_offset_vs_freq_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
/* RX AGC Min/Max Copy Functions*/
void rfnv_wcdma_copy_rx_agc_min
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_rx_agc_min_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_rx_agc_min_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_rx_agc_max
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
/* Miscellaneous */
void rfnv_wcdma_copy_agc_phase_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_r1_rise
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_r1_fall
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_lim_vs_temp
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_utran_tx_lim_vs_temp_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_lim_vs_freq
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_lim_adj_gain
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_exp_hdet_vs_agc
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_hdet_off
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_hdet_spn
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_max_tx_power
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_out_of_service_thresh
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_enc_btf_dly
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_rx_delay
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
/* TX AGC Copy Functions */
void rfnv_wcdma_copy_agc_pa_on_rise_delay
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_agc_pa_on_fall_delay
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_agc_tx_on_rise_delay
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_agc_tx_on_fall_delay
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_agc_update_tx_agc_time
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_pa_gain_up_time
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_pa_gain_down_time
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_rot_ang_pa_00
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_rot_ang_pa_01
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_rot_ang_pa_10
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_rot_ang_pa_11
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_rtr_vco_coarse_tuning
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_pa_compensate_up
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_pa_compensate_down
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_prach_r1_rise_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_prach_r1_fall_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_cal_chan
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_rx_cal_chan
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_rx_cal_chan
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_r2_rise
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_r2_fall
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_r3_rise
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_r3_fall
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_pa_compensate_up_r2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_pa_compensate_down_r2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_pa_compensate_up_r3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_pa_compensate_down_r3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_prach_r2_rise_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_prach_r2_fall_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_prach_r3_rise_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_prach_r3_fall_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_pa_range_map
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_dch_agc_update_tx_agc_time
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_agc_tx_adj_pdm_delay
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_hs_r1_rise
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_hs_r1_fall
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_hs_r2_rise
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_hs_r2_fall
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_hs_r3_rise
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_hs_r3_fall
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_max_pwr_backoff_voltages
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_max_pwr_backoff_volt1
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_max_pwr_backoff_volt2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_max_pwr_backoff_volt3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rxf_mis_comp_a_coeff
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rxf_mis_comp_b_coeff
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rxf_ici_i_coef0_1
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rxf_ici_i_coef2_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rxf_ici_i_coef4_5
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rxf_ici_i_coef6_7
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rxf_ici_i_coef8_9
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rxf_ici_i_coef10_11
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rxf_ici_i_coef12_13
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rxf_ici_i_coef14_15
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rxf_ici_q_coef0_1
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rxf_ici_q_coef2_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rxf_ici_q_coef4_5
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rxf_ici_q_coef6_7
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rxf_ici_q_coef8_9
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rxf_ici_q_coef10_11
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rxf_ici_q_coef12_13
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rxf_ici_q_coef14_15
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rxf_ici_iq_trunc
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_vga_gain_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_vga_gain_offset_vs_temp
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_rise
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_rise_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_rise_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_rise_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_rise_5
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_fall
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_fall_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_fall_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_fall_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_fall_5
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_offset_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_offset_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_offset_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_offset_5
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_im_level
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_im_level_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_im_level_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_im_level_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_im2_i_value
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_im2_q_value
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_im2_transconductor_value
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_nonbypass_timer
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_bypass_timer
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_rfr_vco_coarse_tune_2_1900
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_rf_drx_mode_sel
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_iaccum
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_qaccum
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_timer_hysterisis
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_exp_hdet_vs_agc_v2
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_c0_c1_delay
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_lna_phase_ctl
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_lna_phase_ctl
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rel6_tx_beta_scaling_comp
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rel6_tx_agc_offset
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rel6_tx_mpr_backoff
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_rxf_mis_comp_a_coeff
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_rxf_mis_comp_b_coeff
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef0_1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef2_3
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef4_5
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef6_7
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef8_9
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef10_11
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef12_13
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef14_15
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef0_1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef2_3
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef4_5
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef6_7
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef8_9
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef10_11
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef12_13
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef14_15
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_iq_trunc
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
/* R99 Carrier 1 LNA Range Functions */

void rfnv_wcdma_copy_lna_range_rise_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_rise_2_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_rise_3_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_rise_4_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_rise_5_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_fall_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_fall_2_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_fall_3_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_fall_4_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_fall_5_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
/* Carrier 1 LNA Range Offset Copy Functions */
void rfnv_wcdma_copy_lna_range_offset_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_offset_2_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_offset_3_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_offset_4_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_lna_range_offset_5_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_vga_gain_offset_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
/* R99 Path 1 Carrier 1 LNA Range Functions */

void rfnv_wcdma_copy_c1_lna_range_rise_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_rise_2_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_rise_3_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_rise_4_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_rise_5_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_fall_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_fall_2_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_fall_3_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_fall_4_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_fall_5_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
/* Carrier 1 LNA Range Offset Copy Functions */
void rfnv_wcdma_copy_c1_lna_range_offset_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_offset_2_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_offset_3_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_offset_4_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_lna_range_offset_5_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_vga_gain_offset_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rxf_ici_cal_data_car0
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_rxf_ici_cal_data_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_cal_data_car0
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_cal_data_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_wcdma_db10_sar_back_off_limit
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_ant_tuner
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_ant_tuner_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_pa_comp_up_r1_vs_freq
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_pa_comp_up_r2_vs_freq
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_pa_comp_up_r3_vs_freq
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_pa_comp_dn_r1_vs_freq
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_pa_comp_dn_r2_vs_freq
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_pa_comp_dn_r3_vs_freq
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_gtr_thresh
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_ept_dpd_config_params
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_et_delay_value
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_mtpl_vs_freq
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_hdet_vs_temp
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_hdet_mod_range
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_lin_vs_temp_0
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_lin_vs_temp_1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_lin_vs_temp_2
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_lin_vs_temp_3
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_lin_vs_temp_0_addl
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_lin_vs_temp_1_addl
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_lin_vs_temp_2_addl
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_lin_vs_temp_3_addl
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_lin_vs_temp_vs_freq_p_in
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_lin_vs_temp_vs_freq_p_out
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_xpt_delay
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_delay_offsets
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_ant_tuner_cl_ctrl_info
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_dpd_scaling_value
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_rx_static_data
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_rx_cal_data
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_rx_cal_data_v2
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_noise_floor
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_noise_floor_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_noise_floor_car0
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_noise_floor_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);
/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_pa_static
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);
/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_pin_cal_vs_freq
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);
/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_mpr_backoff
(
    rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
    rfnv_item_type *item_ptr,
    rfcom_wcdma_band_type band
);
/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_dc_tx_lin_vs_temp_vs_freq_p_in
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);
/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_xpt_dc_delay
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);
/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_dc_tx_lin_vs_temp_vs_freq_p_out
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);
/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_dc_pa_static
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_tx_dc_delay_offsets
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c0_rx_static_data
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c1_rx_static_data
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c2_rx_static_data
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_copy_c3_rx_static_data
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

#endif
