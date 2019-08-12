#ifndef RFNV_WCDMA_TYPES_H
#define RFNV_WCDMA_TYPES_H

/*!
  @file
  rfnv_wcdma_types.h

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
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/nv/inc/rfnv_wcdma_types.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/14   kai     Added support for WCDMA Rx V3 static NVs
10/31/14   ac      tuner NV for DBDC
10/17/14   weiz	   Support PA satic compressed NV
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
07/12/13   kcj      Support checking for critical NV items.
06/05/13   kcj      Initial Revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "nv.h"
#include "rfnv_wcdma.h"
#include "rfnv_items.h"

/* Defines the supported NV tables. */
/* This is used since some bands overlap on the same */
/* physical band and thus share NVs. */
typedef enum
{
  RFNV_WCDMA_TABLE_IMT_2100,
  RFNV_WCDMA_TABLE_PCS_1900,
  RFNV_WCDMA_TABLE_1800,
  RFNV_WCDMA_TABLE_BC4,
  RFNV_WCDMA_TABLE_800,
  RFNV_WCDMA_TABLE_BC8_900,
  RFNV_WCDMA_TABLE_BC11_1500,
  RFNV_WCDMA_NUM_TABLES,
  RFNV_WCDMA_TABLE_INVALID,
} rfnv_wcdma_table_type;

/* This union is used to define the type for a NV item */
/* that is pulled using the generic rfnv_item type. */
typedef union
{
  /* LNA range rise value for LNA to switch from g0 to g1 state */
  int16 lna_hs_range_rise;

  /* LNA range rise value for LNA to switch from g1 to g2 gain state */
  int16 lna_hs_range_rise_2;

  /* LNA range rise value for LNA to switch from g2 to g3 gain state */
  int16 lna_hs_range_rise_3;

  /* LNA range rise value for LNA to switch from g3 to g4 gain state */
  int16 lna_hs_range_rise_4;

  /* LNA range rise value for LNA to switch from g3 to g4 gain state */
  int16 lna_hs_range_rise_5;

  /* LNA range fall value for LNA to switch from g1 to g0 gain state */
  int16 lna_hs_range_fall;

  /* LNA range fall value for LNA to switch from g2 to g1 gain state*/
  int16 lna_hs_range_fall_2;

  /* LNA range fall value for Mixer to switch from g3 to g2 gain state */
  int16 lna_hs_range_fall_3;

  /* LNA range fall value for Mixer to switch from g4 to g3 gain state */
  int16 lna_hs_range_fall_4;

  /* LNA range fall value for Mixer to switch from g4 to g3 gain state */
  int16 lna_hs_range_fall_5;


  /* IM threshold level for LNA to switch from g0 to g1 gain state */
  int16 im_hs_level;

  /* IM threshold level for LNA to switch from g1 to g2 gain state */
  int16 im_hs_level_2;

  /* IM threshold level for Mixer to switch from g2 to g3 gain state */
  int16 im_hs_level_3;


  /* IM threshold level for LNA to switch from g3 to g4 gain state */
  int16 im_hs_level_4;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   VGA GAIN Offset NV items
   The description of these NV items can be found in the WCDMA ZIF RF NV items
   document.
  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  int16 vga_gain_offset;

  int8 vga_gain_offset_vs_freq[NV_FREQ_TABLE_SIZ];
  int8 c1_vga_gain_offset_vs_freq[NV_FREQ_TABLE_SIZ];

  int8 vga_gain_offset_vs_temp[NV_TEMP_TABLE_SIZ];

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   IM2 NV items
   The description of these NV items can be found in the WCDMA ZIF RF NV items
   document.
  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  uint8 im2_i_value;

  uint8 im2_q_value;

  uint8 im2_transconductor_value;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     LNA related NV items

     LNA rise/fall (dBm to MSM register & NV storage)

        [ ( (dBm Rx + 106) / 85.333 ) * 1024 ] - 512

     LNA rise/fall (MSM register & NV storage to dBm)

        [ ( (value + 512) / 1024 ) * 85.333 ] - 106
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* LNA range rise value for LNA to switch from high to mid gain state */
  int16 lna_range_rise;

  /* LNA range rise value for LNA to switch from mid to low gain state */
  int16 lna_range_rise_2;

  /* LNA range rise value for Mixer to switch from high to low gain state */
  int16 lna_range_rise_3;

  /* LNA range rise value for g3 to g4 gain steps */
  int16 lna_range_rise_4;

  /* LNA range rise value for g4 to g5 gain steps */
  int16 lna_range_rise_5;

  /* LNA range fall value for LNA to switch from mid to high gain state */
  int16 lna_range_fall;

  /* LNA range fall value for LNA to switch from low to mid gain state*/
  int16 lna_range_fall_2;

  /* LNA range fall value for Mixer to switch from low to high gain state */
  int16 lna_range_fall_3;

  /* LNA range fall value for g4 to g3 gain steps */
  int16 lna_range_fall_4;

  /* LNA range fall value for g5 to g4 gain steps */
  int16 lna_range_fall_5;

  /* IM threshold level for LNA to switch from high to mid gain state */
  int16 im_level;

  /* IM threshold level for LNA to switch from mid to low gain state */
  int16 im_level_2;

  /* IM threshold level for Mixer to switch from high to low gain state */
  int16 im_level_3;

  /* IM threshold level for g3 to g4 gain steps */
  int16 im_level_4;

  /* LNA non-bypass hold timer from low to high gain - multiples of 66us */
  uint8 nonbypass_timer;

  /* LNA bypass hold timer from high to low gain - multiples of 6.6ms */
  uint16 bypass_timer;

  /* LNA range offset for LNA from high to mid gain */
  int16 lna_range_offset;

  /* LNA range offset for LNA from mid to low gain */
  int16 lna_range_offset_2;

  /* LNA range offset for Mixer from high to low gain */
  int16 lna_range_offset_3;

  /* LNA range offset for g3 to g4 gain steps */
  int16 lna_range_offset_4;

  /* LNA range offset for g4 to g5 gain steps */
  int16 lna_range_offset_5;

  /* LNA bypass range offset vs. freq. compensation */
  int8 lna_offset_vs_freq[NV_FREQ_TABLE_SIZ];
  int8 c1_lna_offset_vs_freq[NV_FREQ_TABLE_SIZ];

  /* LNA bypass range offset vs. freq. compensation */
  int8 lna_offset_vs_freq_2[NV_FREQ_TABLE_SIZ];
  int8 c1_lna_offset_vs_freq_2[NV_FREQ_TABLE_SIZ];

  /* LNA bypass range offset vs. freq. compensation */
  int8 lna_offset_vs_freq_3[NV_FREQ_TABLE_SIZ];
  int8 c1_lna_offset_vs_freq_3[NV_FREQ_TABLE_SIZ];

  /* LNA bypass range offset vs. freq. compensation */
  int8 lna_offset_vs_freq_4[NV_FREQ_TABLE_SIZ];
  int8 c1_lna_offset_vs_freq_4[NV_FREQ_TABLE_SIZ];

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    Values to limit the RX AGC output
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Minimum output from the RX AGC_VALUE accumulator for LNA mid gain */
  int16 rx_agc_min;

  /* Minimum output from the RX AGC_VALUE accumulator for LNA low gain */
  int16 rx_agc_min_2;

  /* Minimum output from the RX AGC_VALUE accumulator for Mixer low gain */
  int16 rx_agc_min_3;

  /* Maximum output from the RX AGC_VALUE accumulator */
  int16 rx_agc_max;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    LNA phase adjustment
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Phase rotation setting for LNA decision */
  uint8 agc_phase_offset;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     PA related NV items
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* PA_R1 rise value for PA switch threshold from Linearizer gain state 0 to 1 */
  uint16 r1_rise;

  /* PA_R1 fall value for PA switch threshold from Linearizer gain state 1 to 0 */
  uint16 r1_fall;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     Tx power limit NV items
     Tx power limit data vs temperature, Tx power limit vs frequency,
     TX_AGC_ADJ vs HDET, and HDET offset and span
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Tx power limit versus temperature */
  /* This table contain absolute TX power limit, NOT DELTA */
  uint8 tx_lim_vs_temp[NV_TEMP_TABLE_SIZ];

  /* Tx UTRAN Tx Power Limit Compensation */
  int16 utran_tx_lim_vs_temp_offset[NV_TEMP_TABLE_SIZ];

  /* Tx power limit versus frequency */
  int16 tx_lim_vs_freq[NV_FREQ_TABLE_SIZ];

  /* CDMA Tx power limit loop gain, scaled by RF_WCDMA_TX_LIM_ADJ_SCALE. */
  /* stored in NV as NV_WCDMA_ADJ_FACTOR */
  uint8 tx_lim_adj_gain;

  /* Expected HDET versus AGC */
  /* 16+1 to allow slope calculation for last index */
  uint8 exp_hdet_vs_agc[NV_WCDMA_EXP_HDET_VS_AGC_SIZ + 1];

  /* ADC HDET reading offset */
  uint8 hdet_off;

  /* ADC HDET reading span */
  uint8 hdet_spn;

  /* UE maximum TX power */
  int8  max_tx_power;

  int16 out_of_service_thresh;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     BTF Delay for RF path
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* CHIPX8 delay for SYNC80M, biased +25.
     This is the RF (RX+TX) path delay from the antenna to the IQ digital MSM pins */
  uint32 enc_btf_dly;

  /* RX path delay */
  int16  rx_delay;

  /* tx delays and update agc time */
  uint16 agc_pa_on_rise_delay;
  uint16 agc_pa_on_fall_delay;
  uint16 agc_tx_on_rise_delay;
  uint16 agc_tx_on_fall_delay;
  uint16 agc_update_tx_agc_time;

  /* pa range update time */
  uint16 pa_gain_up_time;
  uint16 pa_gain_down_time;

  /* tx rotator angles */
  uint16 tx_rot_ang_pa_00;
  uint16 tx_rot_ang_pa_01;
  uint16 tx_rot_ang_pa_10;
  uint16 tx_rot_ang_pa_11;

  uint8 rtr_vco_coarse_tuning[16];

  /* PA Compensation UP for PA switch threshold from Linearizer gain state 0 to 1 */
  int16 pa_compensate_up;

  /* PA Compensation DOWN for PA switch threshold from Linearizer gain state 1 to 0 */
  int16 pa_compensate_down;

  /* PRACH Switch point from Linearizer gain state 0 to 1 */
  int16 prach_r1_rise_offset;

  /* PRACH Switch point from Linearizer gain state 1 to 0 */
  int16 prach_r1_fall_offset;

  /* Cal channel list, reserve RF chain index for future use. 1 chain used. */
  int16 tx_cal_chan[NV_FREQ_TABLE_SIZ];
  int16 rx_cal_chan[NV_FREQ_TABLE_SIZ];
  uint16 c1_rx_cal_chan[NV_FREQ_TABLE_SIZ];

  uint32 tx_cal_chan_freq[NV_FREQ_TABLE_SIZ];
  uint32 rx_cal_chan_freq[NV_FREQ_TABLE_SIZ];
  uint32 c1_rx_cal_chan_freq[NV_FREQ_TABLE_SIZ];

  boolean interpolation_enable;

  /* No. of Tx linearizer state is increased to 4 */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     PA related NV items
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* PA_R2 rise value for PA switch threshold from Linearizer gain state 1 to 2 */
  uint16 r2_rise;

  /* PA_R2 fall value for PA switch threshold from Linearizer gain state 2 to 1 */
  uint16 r2_fall;

  /* PA_R3 rise value for PA switch threshold from Linearizer gain state 2 to 3 */
  uint16 r3_rise;

  /* PA_R3 fall value for PA switch threshold from Linearizer gain state 3 to 2 */
  uint16 r3_fall;


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     PA Compensate NV items
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* PA Compensation UP for PA switch threshold from Linearizer gain state 1 to 2 */
  int16 pa_compensate_up_r2;

  /* PA Compensation DOWN for PA switch threshold from Linearizer gain state 2 to 1 */
  int16 pa_compensate_down_r2;

  /* PA Compensation UP for PA switch threshold from Linearizer gain state 2 to 3 */
  int16 pa_compensate_up_r3;

  /* PA Compensation DOWN for PA switch threshold from Linearizer gain state 3 to 2 */
  int16 pa_compensate_down_r3;


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     PRACH Switch point NV items
  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* PRACH Switch point from Linearizer gain state 1 to 2 */
  int16 prach_r2_rise_offset;

  /* PRACH Switch point from Linearizer gain state 2 to 1 */
  int16 prach_r2_fall_offset;

  /* PRACH Switch point from Linearizer gain state 2 to 3 */
  int16 prach_r3_rise_offset;

  /* PRACH Switch point from Linearizer gain state 3 to 2 */
  int16 prach_r3_fall_offset;

  /* PA Range Map */
  int8 pa_range_map[RFLIN_NUM_TX_LIN_LEGACY_4PA];

  /* DCH AGC Update Tx AGC time - MDSP_modUpdateTxAGCInChipx8 */
  int16 dch_agc_update_tx_agc_time;

  /* AGC Tx Adj PDM delay */
  int16 agc_tx_adj_pdm_delay;

  /* HS Power switch from 2 --> 1 state */
  int16 hs_r1_rise;

  /* HS Power switch from 1 --> 2 state */
  int16 hs_r1_fall;

  /* HS Power switch from 2 --> 1 state */
  int16 hs_r2_rise;

  /* HS Power switch from 1 --> 2 state */
  int16 hs_r2_fall;

  /* HS Power switch from 3 --> 2 state */
  int16 hs_r3_rise;

  /* HS Power switch from 2 --> 3 state */
  int16 hs_r3_fall;

  /* Max Power Backoff Voltages */
  uint8 max_pwr_backoff_voltages[3];

  /* Max Power Backoff in dB12 units @ Volt1 */
  int16 max_pwr_backoff_volt1[4];

  /* Max Power Backoff in dB12 units @ Volt2 */
  int16 max_pwr_backoff_volt2[4];

  /* Max Power Backoff in dB12 units @ Volt3 */
  int16 max_pwr_backoff_volt3[4];

  /* In-Phase 16-bit signed A coefficient for IMT pri RXFE mismatch comp */
  int16     wcdma_rxf_mis_comp_a_coeff;
  /* In-Phase 16-bit signed B coefficient for IMT pri RXFE mismatch comp */
  uint16     wcdma_rxf_mis_comp_b_coeff;
  /* Primary RXFE ICI filter I channel coefficients 0_1 */
  uint32    wcdma_rxf_ici_i_coef0_1;
  /*   primary RXFE ICI filter I channel coefficients 2_3 */
  uint32    wcdma_rxf_ici_i_coef2_3;
  /*   primary RXFE ICI filter I channel coefficients 4_5 */
  uint32    wcdma_rxf_ici_i_coef4_5;
  /*   primary RXFE ICI filter I channel coefficients 6_7 */
  uint32    wcdma_rxf_ici_i_coef6_7;
  /*   primary RXFE ICI filter I channel coefficients 8_9 */
  uint32    wcdma_rxf_ici_i_coef8_9;
  /*   primary RXFE ICI filter I channel coefficients 10_11 */
  uint32    wcdma_rxf_ici_i_coef10_11;
  /*   primary RXFE ICI filter I channel coefficients 12_13 */
  uint32    wcdma_rxf_ici_i_coef12_13;
  /*   primary RXFE ICI filter I channel coefficients 14_15 */
  uint32    wcdma_rxf_ici_i_coef14_15;
  /*   primary RXFE ICI filter Q channel coefficients 0_1 */
  uint32    wcdma_rxf_ici_q_coef0_1;
  /*   primary RXFE ICI filter Q channel coefficients 2_3 */
  uint32    wcdma_rxf_ici_q_coef2_3;
  /*   primary RXFE ICI filter Q channel coefficients 4_5 */
  uint32    wcdma_rxf_ici_q_coef4_5;
  /*   primary RXFE ICI filter Q channel coefficients 6_7 */
  uint32    wcdma_rxf_ici_q_coef6_7;
  /*   primary RXFE ICI filter Q channel coefficients 8_9 */
  uint32    wcdma_rxf_ici_q_coef8_9;
  /*   primary RXFE ICI filter Q channel coefficients 10_11 */
  uint32    wcdma_rxf_ici_q_coef10_11;
  /*   primary RXFE ICI filter Q channel coefficients 12_13 */
  uint32    wcdma_rxf_ici_q_coef12_13;
  /*   primary RXFE ICI filter Q channel coefficients 14_15 */
  uint32    wcdma_rxf_ici_q_coef14_15;
  /*   primary RXFE ICI filter IQ trunc bits */
  uint8     wcdma_rxf_ici_iq_trunc;

  /* diversity path  rx agc offset for each vga gain */
  int16     c1_vga_gain_offset;
  /* diversity path  rx agc offset for each vga gain based on temperature */
  int8      c1_vga_gain_offset_vs_temp[NV_TEMP_TABLE_SIZ];


  /* diversity path  lna threshold from g0 to g1 gain states */
  int16     c1_lna_range_rise;
  /* diversity path  lna threshold from g1 to g2 gain states */
  int16     c1_lna_range_rise_2;
  /* diversity path  lna threshold from g2 to g3 gain states */
  int16     c1_lna_range_rise_3;
  /* diversity path  lna threshold from g3 to g4 gain states */
  int16     c1_lna_range_rise_4;
  /* diversity path  lna threshold from g4 to g5 gain states */
  int16     c1_lna_range_rise_5;
  /* diversity path  lna threshold from g1 to g0 gain states */
  int16     c1_lna_range_fall;
  /* diversity path  lna threshold from g2 to g1 gain states */
  int16     c1_lna_range_fall_2;
  /* diversity path  lna threshold from g3 to g2 gain states */
  int16     c1_lna_range_fall_3;
  /* diversity path  lna threshold from g4 to g3 gain states */
  int16     c1_lna_range_fall_4;
  /* diversity path  lna threshold from g5 to g4 gain states */
  int16     c1_lna_range_fall_5;


  /* diversity path  lna threshold from g1 to g0 gain states */
  int16     c1_lna_range_offset;
  /* diversity path lna gain step from g1 to g2 */
  int16     c1_lna_range_offset_2;
  /* diversity path lna gain step from g2 to g3 */
  int16     c1_lna_range_offset_3;
  /* diversity path  lna gain step from g3 to g4 */
  int16     c1_lna_range_offset_4;
  /* diversity path  lna gain step from g4 to g5 */
  int16     c1_lna_range_offset_5;

  /* diversity path  im threshold from g0 to g1 */
  int16     c1_im_level;
  /* diversity path  im threshold from g1 to g2 gain states */
  int16     c1_im_level_2;
  /* diversity path  im threshold from g2 to g3 gain states */
  int16     c1_im_level_3;
  /* diversity path  im threshold from g3 to g4 gain states */
  int16     c1_im_level_4;


  /* diversity path  i cancellation value for lower in-band jammer */
  uint8     c1_im2_i_value;
  /* diversity path  q cancellation value for lower in-band jammer */
  uint8     c1_im2_q_value;
  /* diversity path  transconductor value for lower in-band jammer */
  uint8     c1_im2_transconductor_value;


  /* diversity path lna non-bypass hold timer from low to high gain - multiples of 66us */
  uint8     c1_nonbypass_timer;
  /* diversity path lna bypass hold timer from high to low gain - multiples of 6.6ms */
  uint16    c1_bypass_timer;

  /* second coarse tune code for umts 1900 band  */
  uint8     rfr_vco_coarse_tune_2_1900[12];

  /* diversity mode select switches */
  uint8    rf_drx_mode_sel[3];

  /* I accumulator for DC in WCDMA band */
  uint16    wcdma_iaccum[6];

  /* Q accumulator for DC in WCDMA band */
  uint16    wcdma_qaccum[6];

  /*timer hysterisis*/
  int16   timer_hysterisis;

  /* _v2 are 12-bit HDET values */
  /* Expected HDET versus AGC */
  /* 16+1 to allow slope calculation for last index */
  uint16 exp_hdet_vs_agc_v2[NV_WCDMA_EXP_HDET_VS_AGC_SIZ + 1];

  /* Cx16 delay between primary and diversity paths */
  int8      wcdma_c0_c1_delay;

  /* primary path LNA phase control in WCDMA band */
  uint16    wcdma_lna_phase_ctl[RF_MAX_LNA_GAIN_STATES];

  /* diversity path LNA phase control in WCDMA band */
  uint16    c1_wcdma_lna_phase_ctl[RF_MAX_LNA_GAIN_STATES];

  uint16    wcdma_rel6_tx_beta_scaling_comp[7];

  uint8     wcdma_rel6_tx_agc_offset[7];

  uint8     wcdma_rel6_tx_mpr_backoff[7];

  /* In-Phase 16-bit signed A coefficient for   div RXFE mismatch comp */
  int16     c1_wcdma_rxf_mis_comp_a_coeff;
  /* In-Phase 16-bit signed B coefficient for   div RXFE mismatch comp */
  uint16     c1_wcdma_rxf_mis_comp_b_coeff;
  /*   diversity RXFE ICI filter I channel coefficients 0_1 */
  uint32    c1_wcdma_rxf_ici_i_coef0_1;
  /*   diversity RXFE ICI filter I channel coefficients 2_3 */
  uint32    c1_wcdma_rxf_ici_i_coef2_3;
  /*   diversity RXFE ICI filter I channel coefficients 4_5 */
  uint32    c1_wcdma_rxf_ici_i_coef4_5;
  /*   diversity RXFE ICI filter I channel coefficients 6_7 */
  uint32    c1_wcdma_rxf_ici_i_coef6_7;
  /*   diversity RXFE ICI filter I channel coefficients 8_9 */
  uint32    c1_wcdma_rxf_ici_i_coef8_9;
  /*   diversity RXFE ICI filter I channel coefficients 10_11 */
  uint32    c1_wcdma_rxf_ici_i_coef10_11;
  /*   diversity RXFE ICI filter I channel coefficients 12_13 */
  uint32    c1_wcdma_rxf_ici_i_coef12_13;
  /*   diversity RXFE ICI filter I channel coefficients 14_15 */
  uint32    c1_wcdma_rxf_ici_i_coef14_15;
  /*   diversity RXFE ICI filter Q channel coefficients 0_1 */
  uint32    c1_wcdma_rxf_ici_q_coef0_1;
  /*   diversity RXFE ICI filter Q channel coefficients 2_3 */
  uint32    c1_wcdma_rxf_ici_q_coef2_3;
  /*   diversity RXFE ICI filter Q channel coefficients 4_5 */
  uint32    c1_wcdma_rxf_ici_q_coef4_5;
  /*   diversity RXFE ICI filter Q channel coefficients 6_7 */
  uint32    c1_wcdma_rxf_ici_q_coef6_7;
  /*   diversity RXFE ICI filter Q channel coefficients 8_9 */
  uint32    c1_wcdma_rxf_ici_q_coef8_9;
  /*   diversity RXFE ICI filter Q channel coefficients 10_11 */
  uint32    c1_wcdma_rxf_ici_q_coef10_11;
  /*   diversity RXFE ICI filter Q channel coefficients 12_13 */
  uint32    c1_wcdma_rxf_ici_q_coef12_13;
  /*   diversity RXFE ICI filter Q channel coefficients 14_15 */
  uint32    c1_wcdma_rxf_ici_q_coef14_15;
  /*   diversity RXFE ICI filter IQ trunc bits */
  uint8     c1_wcdma_rxf_ici_iq_trunc;

  /* LNA range rise value for LNA to switch from high to mid gain state */
  int16 lna_range_rise_car1;

  /* LNA range rise value for LNA to switch from mid to low gain state */
  int16 lna_range_rise_2_car1;

  /* LNA range rise value for Mixer to switch from high to low gain state */
  int16 lna_range_rise_3_car1;

  /* LNA range rise value for g3 to g4 gain steps */
  int16 lna_range_rise_4_car1;

  /* LNA range rise value for g4 to g5 gain steps */
  int16 lna_range_rise_5_car1;

  /* LNA range fall value for LNA to switch from mid to high gain state */
  int16 lna_range_fall_car1;

  /* LNA range fall value for LNA to switch from low to mid gain state*/
  int16 lna_range_fall_2_car1;

  /* LNA range fall value for Mixer to switch from low to high gain state */
  int16 lna_range_fall_3_car1;

  /* LNA range fall value for g4 to g3 gain steps */
  int16 lna_range_fall_4_car1;

  /* LNA range fall value for g5 to g4 gain steps */
  int16 lna_range_fall_5_car1;

  /* LNA range offset for LNA from high to mid gain */
  int16 lna_range_offset_car1;

  /* LNA range offset for LNA from mid to low gain */
  int16 lna_range_offset_2_car1;

  /* LNA range offset for Mixer from high to low gain */
  int16 lna_range_offset_3_car1;

  /* LNA range offset for g3 to g4 gain steps */
  int16 lna_range_offset_4_car1;

   /* LNA range offset for g4 to g5 gain steps */
  int16 lna_range_offset_5_car1;

   /* vga_gain_offset = -141 (0x1B9800) (122.88 MHz sampling frequency)*/
  int16 vga_gain_offset_car1;

  /* LNA range rise value for LNA to switch from high to mid gain state */
  int16 c1_lna_range_rise_car1;

  /* LNA range rise value for LNA to switch from mid to low gain state */
  int16 c1_lna_range_rise_2_car1;

  /* LNA range rise value for Mixer to switch from high to low gain state */
  int16 c1_lna_range_rise_3_car1;

  /* LNA range rise value for g3 to g4 gain steps */
  int16 c1_lna_range_rise_4_car1;

  /* LNA range rise value for g4 to g5 gain steps */
  int16 c1_lna_range_rise_5_car1;

  /* LNA range fall value for LNA to switch from mid to high gain state */
  int16 c1_lna_range_fall_car1;

  /* LNA range fall value for LNA to switch from low to mid gain state*/
  int16 c1_lna_range_fall_2_car1;

  /* LNA range fall value for Mixer to switch from low to high gain state */
  int16 c1_lna_range_fall_3_car1;

  /* LNA range fall value for g4 to g3 gain steps */
  int16 c1_lna_range_fall_4_car1;

  /* LNA range fall value for g5 to g4 gain steps */
  int16 c1_lna_range_fall_5_car1;

  /* LNA range offset for LNA from high to mid gain */
  int16 c1_lna_range_offset_car1;

  /* LNA range offset for LNA from mid to low gain */
  int16 c1_lna_range_offset_2_car1;

  /* LNA range offset for Mixer from high to low gain */
  int16 c1_lna_range_offset_3_car1;

  /* LNA range offset for g3 to g4 gain steps */
  int16 c1_lna_range_offset_4_car1;

   /* LNA range offset for g4 to g5 gain steps */
  int16 c1_lna_range_offset_5_car1;

   /* vga_gain_offset = -141 (0x1B9800) (122.88 MHz sampling frequency)*/
  int16 c1_vga_gain_offset_car1;

  /* ICI calibration data for carrier 0 in dual carrier mode */
  uint32 wcdma_rxf_ici_cal_data_car0[17];

  /* ICI calibration data for carrier 1 in dual carrier mode */
  uint32 wcdma_rxf_ici_cal_data_car1[17];

  /* ICI calibration data for carrier 0 in dual carrier mode */
  uint32 c1_wcdma_rxf_ici_cal_data_car0[17];

  /* ICI calibration data for carrier 1 in dual carrier mode */
  uint32 c1_wcdma_rxf_ici_cal_data_car1[17];

  int16 wcdma_db10_sar_back_off_limit[RFWCDMA_SAR_BACK_OFF_NV_SIZE];

  /* Ant Tuner NV */
  rfcommon_nv_ant_tuner_type ant_tuner;

  /* PA Compensate Up/Down vs. Freq Items */
  int16 pa_comp_up_r1_vs_freq[NV_FREQ_TABLE_SIZ]; /* From R0 to R1 */
  int16 pa_comp_up_r2_vs_freq[NV_FREQ_TABLE_SIZ]; /* From R0/R1 to R2 */
  int16 pa_comp_up_r3_vs_freq[NV_FREQ_TABLE_SIZ]; /* From R0/R1/R2 to R3 */
  int16 pa_comp_dn_r1_vs_freq[NV_FREQ_TABLE_SIZ]; /* From R3/R2/R1 to R0 */
  int16 pa_comp_dn_r2_vs_freq[NV_FREQ_TABLE_SIZ]; /* From R3/R2 to R1 */
  int16 pa_comp_dn_r3_vs_freq[NV_FREQ_TABLE_SIZ]; /* From R3 to R2 */

  /* Lin vs Temp vs Freq Pin Compensation Values */
  int8  tx_lin_vs_temp_0[NV_TEMP_TABLE_SIZ];
  int8  tx_lin_vs_temp_1[NV_TEMP_TABLE_SIZ];
  int8  tx_lin_vs_temp_2[NV_TEMP_TABLE_SIZ];
  int8  tx_lin_vs_temp_3[NV_TEMP_TABLE_SIZ];

  rfcom_txlin_temp_comp_nv_data_type lin_temp_comp_data;

  /* TX_GTR_THRESH NV */
  rfwcdma_nv_tx_gtr_thresh_type tx_gtr_thresh;

  /* EPT DPD static params for expansion and processing */
  uint32 tx_ept_dpd_config_params[MAX_EPT_NUM_CONFIG_PARAMS];

 /* ET path delay value*/
  uint32 et_delay_value;

  /* Max Transmit Power Limit Vs Frequency */
  int8 mtpl_vs_freq[NV_FREQ_TABLE_SIZ];

  /* HDET offset Vs Temperature */
  int16 hdet_vs_temp[NV_TEMP_TABLE_SIZ];
  
  /* Noise floor level for PRx/Carrier 0 in 0.25dB */
  int16 noise_floor;

  /* Noise floor level for PRx/Carrier 1 in 0.25dB */
  int16 noise_floor_car1;

  /* Noise floor level for DRx/Carrier 0 in 0.25dB */
  int16 c1_noise_floor_car0;

  /* Noise floor level for DRx/Carrier 1 in 0.25dB */
  int16 c1_noise_floor_car1;

  rfcommon_nv_cmn_pa_static_type pa_static;
} wcdma_nv_item_type;

/* The current number of NV items supported by WCDMA. */
#define RFNV_WCDMA_ITEMS_MAX ((sizeof(rfnv_wcdma_nv_items_type))/(sizeof(rfcommon_rfnv_items_list_type)))

/* Explicit definition for the NV item lists. */
/* The order here MUST match the order of */
/* the rfnv_wcdma_nv_copy_funcs_type struct. */
typedef struct
{
  rfcommon_rfnv_items_list_type lna_hs_range_rise;
  rfcommon_rfnv_items_list_type lna_hs_range_rise_2;
  rfcommon_rfnv_items_list_type lna_hs_range_rise_3;
  rfcommon_rfnv_items_list_type lna_hs_range_rise_4;
  rfcommon_rfnv_items_list_type lna_hs_range_rise_5;
  rfcommon_rfnv_items_list_type lna_hs_range_fall;
  rfcommon_rfnv_items_list_type lna_hs_range_fall_2;
  rfcommon_rfnv_items_list_type lna_hs_range_fall_3;
  rfcommon_rfnv_items_list_type lna_hs_range_fall_4;
  rfcommon_rfnv_items_list_type lna_hs_range_fall_5;
  rfcommon_rfnv_items_list_type im_hs_level;
  rfcommon_rfnv_items_list_type im_hs_level_2;
  rfcommon_rfnv_items_list_type im_hs_level_3;
  rfcommon_rfnv_items_list_type im_hs_level_4;
  rfcommon_rfnv_items_list_type vga_gain_offset;
  rfcommon_rfnv_items_list_type vga_gain_offset_vs_freq;
  rfcommon_rfnv_items_list_type c1_vga_gain_offset_vs_freq;
  rfcommon_rfnv_items_list_type vga_gain_offset_vs_temp;
  rfcommon_rfnv_items_list_type im2_i_value;
  rfcommon_rfnv_items_list_type im2_q_value;
  rfcommon_rfnv_items_list_type im2_transconductor_value;
  rfcommon_rfnv_items_list_type lna_range_rise;
  rfcommon_rfnv_items_list_type lna_range_rise_2;
  rfcommon_rfnv_items_list_type lna_range_rise_3;
  rfcommon_rfnv_items_list_type lna_range_rise_4;
  rfcommon_rfnv_items_list_type lna_range_rise_5;
  rfcommon_rfnv_items_list_type lna_range_fall;
  rfcommon_rfnv_items_list_type lna_range_fall_2;
  rfcommon_rfnv_items_list_type lna_range_fall_3;
  rfcommon_rfnv_items_list_type lna_range_fall_4;
  rfcommon_rfnv_items_list_type lna_range_fall_5;
  rfcommon_rfnv_items_list_type im_level;
  rfcommon_rfnv_items_list_type im_level_2;
  rfcommon_rfnv_items_list_type im_level_3;
  rfcommon_rfnv_items_list_type im_level_4;
  rfcommon_rfnv_items_list_type nonbypass_timer;
  rfcommon_rfnv_items_list_type bypass_timer;
  rfcommon_rfnv_items_list_type lna_range_offset;
  rfcommon_rfnv_items_list_type lna_range_offset_2;
  rfcommon_rfnv_items_list_type lna_range_offset_3;
  rfcommon_rfnv_items_list_type lna_range_offset_4;
  rfcommon_rfnv_items_list_type lna_range_offset_5;
  rfcommon_rfnv_items_list_type lna_offset_vs_freq;
  rfcommon_rfnv_items_list_type c1_lna_offset_vs_freq;
  rfcommon_rfnv_items_list_type lna_offset_vs_freq_2;
  rfcommon_rfnv_items_list_type c1_lna_offset_vs_freq_2;
  rfcommon_rfnv_items_list_type lna_offset_vs_freq_3;
  rfcommon_rfnv_items_list_type c1_lna_offset_vs_freq_3;
  rfcommon_rfnv_items_list_type lna_offset_vs_freq_4;
  rfcommon_rfnv_items_list_type c1_lna_offset_vs_freq_4;
  rfcommon_rfnv_items_list_type rx_agc_min;
  rfcommon_rfnv_items_list_type rx_agc_min_2;
  rfcommon_rfnv_items_list_type rx_agc_min_3;
  rfcommon_rfnv_items_list_type rx_agc_max;
  rfcommon_rfnv_items_list_type agc_phase_offset;
  rfcommon_rfnv_items_list_type r1_rise;
  rfcommon_rfnv_items_list_type r1_fall;
  rfcommon_rfnv_items_list_type tx_lim_vs_temp;
  rfcommon_rfnv_items_list_type utran_tx_lim_vs_temp_offset;
  rfcommon_rfnv_items_list_type tx_lim_vs_freq;
  rfcommon_rfnv_items_list_type tx_lim_adj_gain;
  rfcommon_rfnv_items_list_type exp_hdet_vs_agc;
  rfcommon_rfnv_items_list_type hdet_off;
  rfcommon_rfnv_items_list_type hdet_spn;
  rfcommon_rfnv_items_list_type max_tx_power;
  rfcommon_rfnv_items_list_type out_of_service_thresh;
  rfcommon_rfnv_items_list_type enc_btf_dly;
  rfcommon_rfnv_items_list_type rx_delay;
  rfcommon_rfnv_items_list_type agc_pa_on_rise_delay;
  rfcommon_rfnv_items_list_type agc_pa_on_fall_delay;
  rfcommon_rfnv_items_list_type agc_tx_on_rise_delay;
  rfcommon_rfnv_items_list_type agc_tx_on_fall_delay;
  rfcommon_rfnv_items_list_type agc_update_tx_agc_time;
  rfcommon_rfnv_items_list_type pa_gain_up_time;
  rfcommon_rfnv_items_list_type pa_gain_down_time;
  rfcommon_rfnv_items_list_type tx_rot_ang_pa_00;
  rfcommon_rfnv_items_list_type tx_rot_ang_pa_01;
  rfcommon_rfnv_items_list_type tx_rot_ang_pa_10;
  rfcommon_rfnv_items_list_type tx_rot_ang_pa_11;
  rfcommon_rfnv_items_list_type rtr_vco_coarse_tuning;
  rfcommon_rfnv_items_list_type pa_compensate_up;
  rfcommon_rfnv_items_list_type pa_compensate_down;
  rfcommon_rfnv_items_list_type prach_r1_rise_offset;
  rfcommon_rfnv_items_list_type prach_r1_fall_offset;
  rfcommon_rfnv_items_list_type tx_cal_chan;
  rfcommon_rfnv_items_list_type rx_cal_chan;
  rfcommon_rfnv_items_list_type c1_rx_cal_chan;
  rfcommon_rfnv_items_list_type r2_rise;
  rfcommon_rfnv_items_list_type r2_fall;
  rfcommon_rfnv_items_list_type r3_rise;
  rfcommon_rfnv_items_list_type r3_fall;
  rfcommon_rfnv_items_list_type pa_compensate_up_r2;
  rfcommon_rfnv_items_list_type pa_compensate_down_r2;
  rfcommon_rfnv_items_list_type pa_compensate_up_r3;
  rfcommon_rfnv_items_list_type pa_compensate_down_r3;
  rfcommon_rfnv_items_list_type prach_r2_rise_offset;
  rfcommon_rfnv_items_list_type prach_r2_fall_offset;
  rfcommon_rfnv_items_list_type prach_r3_rise_offset;
  rfcommon_rfnv_items_list_type prach_r3_fall_offset;
  rfcommon_rfnv_items_list_type pa_range_map;
  rfcommon_rfnv_items_list_type dch_agc_update_tx_agc_time;
  rfcommon_rfnv_items_list_type agc_tx_adj_pdm_delay;
  rfcommon_rfnv_items_list_type hs_r1_rise;
  rfcommon_rfnv_items_list_type hs_r1_fall;
  rfcommon_rfnv_items_list_type hs_r2_rise;
  rfcommon_rfnv_items_list_type hs_r2_fall;
  rfcommon_rfnv_items_list_type hs_r3_rise;
  rfcommon_rfnv_items_list_type hs_r3_fall;
  rfcommon_rfnv_items_list_type max_pwr_backoff_voltages;
  rfcommon_rfnv_items_list_type max_pwr_backoff_volt1;
  rfcommon_rfnv_items_list_type max_pwr_backoff_volt2;
  rfcommon_rfnv_items_list_type max_pwr_backoff_volt3;
  rfcommon_rfnv_items_list_type wcdma_rxf_mis_comp_a_coeff;
  rfcommon_rfnv_items_list_type wcdma_rxf_mis_comp_b_coeff;
  rfcommon_rfnv_items_list_type wcdma_rxf_ici_i_coef0_1;
  rfcommon_rfnv_items_list_type wcdma_rxf_ici_i_coef2_3;
  rfcommon_rfnv_items_list_type wcdma_rxf_ici_i_coef4_5;
  rfcommon_rfnv_items_list_type wcdma_rxf_ici_i_coef6_7;
  rfcommon_rfnv_items_list_type wcdma_rxf_ici_i_coef8_9;
  rfcommon_rfnv_items_list_type wcdma_rxf_ici_i_coef10_11;
  rfcommon_rfnv_items_list_type wcdma_rxf_ici_i_coef12_13;
  rfcommon_rfnv_items_list_type wcdma_rxf_ici_i_coef14_15;
  rfcommon_rfnv_items_list_type wcdma_rxf_ici_q_coef0_1;
  rfcommon_rfnv_items_list_type wcdma_rxf_ici_q_coef2_3;
  rfcommon_rfnv_items_list_type wcdma_rxf_ici_q_coef4_5;
  rfcommon_rfnv_items_list_type wcdma_rxf_ici_q_coef6_7;
  rfcommon_rfnv_items_list_type wcdma_rxf_ici_q_coef8_9;
  rfcommon_rfnv_items_list_type wcdma_rxf_ici_q_coef10_11;
  rfcommon_rfnv_items_list_type wcdma_rxf_ici_q_coef12_13;
  rfcommon_rfnv_items_list_type wcdma_rxf_ici_q_coef14_15;
  rfcommon_rfnv_items_list_type wcdma_rxf_ici_iq_trunc;
  rfcommon_rfnv_items_list_type c1_vga_gain_offset;
  rfcommon_rfnv_items_list_type c1_vga_gain_offset_vs_temp;
  rfcommon_rfnv_items_list_type c1_lna_range_rise;
  rfcommon_rfnv_items_list_type c1_lna_range_rise_2;
  rfcommon_rfnv_items_list_type c1_lna_range_rise_3;
  rfcommon_rfnv_items_list_type c1_lna_range_rise_4;
  rfcommon_rfnv_items_list_type c1_lna_range_rise_5;
  rfcommon_rfnv_items_list_type c1_lna_range_fall;
  rfcommon_rfnv_items_list_type c1_lna_range_fall_2;
  rfcommon_rfnv_items_list_type c1_lna_range_fall_3;
  rfcommon_rfnv_items_list_type c1_lna_range_fall_4;
  rfcommon_rfnv_items_list_type c1_lna_range_fall_5;
  rfcommon_rfnv_items_list_type c1_lna_range_offset;
  rfcommon_rfnv_items_list_type c1_lna_range_offset_2;
  rfcommon_rfnv_items_list_type c1_lna_range_offset_3;
  rfcommon_rfnv_items_list_type c1_lna_range_offset_4;
  rfcommon_rfnv_items_list_type c1_lna_range_offset_5;
  rfcommon_rfnv_items_list_type c1_im_level;
  rfcommon_rfnv_items_list_type c1_im_level_2;
  rfcommon_rfnv_items_list_type c1_im_level_3;
  rfcommon_rfnv_items_list_type c1_im_level_4;
  rfcommon_rfnv_items_list_type c1_im2_i_value;
  rfcommon_rfnv_items_list_type c1_im2_q_value;
  rfcommon_rfnv_items_list_type c1_im2_transconductor_value;
  rfcommon_rfnv_items_list_type c1_nonbypass_timer;
  rfcommon_rfnv_items_list_type c1_bypass_timer;
  rfcommon_rfnv_items_list_type rfr_vco_coarse_tune_2_1900;
  rfcommon_rfnv_items_list_type rf_drx_mode_sel;
  rfcommon_rfnv_items_list_type wcdma_iaccum;
  rfcommon_rfnv_items_list_type wcdma_qaccum;
  rfcommon_rfnv_items_list_type timer_hysterisis;
  rfcommon_rfnv_items_list_type exp_hdet_vs_agc_v2;
  rfcommon_rfnv_items_list_type wcdma_c0_c1_delay;
  rfcommon_rfnv_items_list_type wcdma_lna_phase_ctl;
  rfcommon_rfnv_items_list_type c1_wcdma_lna_phase_ctl;
  rfcommon_rfnv_items_list_type wcdma_rel6_tx_beta_scaling_comp;
  rfcommon_rfnv_items_list_type wcdma_rel6_tx_agc_offset;
  rfcommon_rfnv_items_list_type wcdma_rel6_tx_mpr_backoff;
  rfcommon_rfnv_items_list_type c1_wcdma_rxf_mis_comp_a_coeff;
  rfcommon_rfnv_items_list_type c1_wcdma_rxf_mis_comp_b_coeff;
  rfcommon_rfnv_items_list_type c1_wcdma_rxf_ici_i_coef0_1;
  rfcommon_rfnv_items_list_type c1_wcdma_rxf_ici_i_coef2_3;
  rfcommon_rfnv_items_list_type c1_wcdma_rxf_ici_i_coef4_5;
  rfcommon_rfnv_items_list_type c1_wcdma_rxf_ici_i_coef6_7;
  rfcommon_rfnv_items_list_type c1_wcdma_rxf_ici_i_coef8_9;
  rfcommon_rfnv_items_list_type c1_wcdma_rxf_ici_i_coef10_11;
  rfcommon_rfnv_items_list_type c1_wcdma_rxf_ici_i_coef12_13;
  rfcommon_rfnv_items_list_type c1_wcdma_rxf_ici_i_coef14_15;
  rfcommon_rfnv_items_list_type c1_wcdma_rxf_ici_q_coef0_1;
  rfcommon_rfnv_items_list_type c1_wcdma_rxf_ici_q_coef2_3;
  rfcommon_rfnv_items_list_type c1_wcdma_rxf_ici_q_coef4_5;
  rfcommon_rfnv_items_list_type c1_wcdma_rxf_ici_q_coef6_7;
  rfcommon_rfnv_items_list_type c1_wcdma_rxf_ici_q_coef8_9;
  rfcommon_rfnv_items_list_type c1_wcdma_rxf_ici_q_coef10_11;
  rfcommon_rfnv_items_list_type c1_wcdma_rxf_ici_q_coef12_13;
  rfcommon_rfnv_items_list_type c1_wcdma_rxf_ici_q_coef14_15;
  rfcommon_rfnv_items_list_type c1_wcdma_rxf_ici_iq_trunc;
  rfcommon_rfnv_items_list_type lna_range_rise_car1;
  rfcommon_rfnv_items_list_type lna_range_rise_2_car1;
  rfcommon_rfnv_items_list_type lna_range_rise_3_car1;
  rfcommon_rfnv_items_list_type lna_range_rise_4_car1;
  rfcommon_rfnv_items_list_type lna_range_rise_5_car1;
  rfcommon_rfnv_items_list_type lna_range_fall_car1;
  rfcommon_rfnv_items_list_type lna_range_fall_2_car1;
  rfcommon_rfnv_items_list_type lna_range_fall_3_car1;
  rfcommon_rfnv_items_list_type lna_range_fall_4_car1;
  rfcommon_rfnv_items_list_type lna_range_fall_5_car1;
  rfcommon_rfnv_items_list_type lna_range_offset_car1;
  rfcommon_rfnv_items_list_type lna_range_offset_2_car1;
  rfcommon_rfnv_items_list_type lna_range_offset_3_car1;
  rfcommon_rfnv_items_list_type lna_range_offset_4_car1;
  rfcommon_rfnv_items_list_type lna_range_offset_5_car1;
  rfcommon_rfnv_items_list_type vga_gain_offset_car1;
  rfcommon_rfnv_items_list_type c1_lna_range_rise_car1;
  rfcommon_rfnv_items_list_type c1_lna_range_rise_2_car1;
  rfcommon_rfnv_items_list_type c1_lna_range_rise_3_car1;
  rfcommon_rfnv_items_list_type c1_lna_range_rise_4_car1;
  rfcommon_rfnv_items_list_type c1_lna_range_rise_5_car1;
  rfcommon_rfnv_items_list_type c1_lna_range_fall_car1;
  rfcommon_rfnv_items_list_type c1_lna_range_fall_2_car1;
  rfcommon_rfnv_items_list_type c1_lna_range_fall_3_car1;
  rfcommon_rfnv_items_list_type c1_lna_range_fall_4_car1;
  rfcommon_rfnv_items_list_type c1_lna_range_fall_5_car1;
  rfcommon_rfnv_items_list_type c1_lna_range_offset_car1;
  rfcommon_rfnv_items_list_type c1_lna_range_offset_2_car1;
  rfcommon_rfnv_items_list_type c1_lna_range_offset_3_car1;
  rfcommon_rfnv_items_list_type c1_lna_range_offset_4_car1;
  rfcommon_rfnv_items_list_type c1_lna_range_offset_5_car1;
  rfcommon_rfnv_items_list_type c1_vga_gain_offset_car1;
  rfcommon_rfnv_items_list_type wcdma_rxf_ici_cal_data_car0;
  rfcommon_rfnv_items_list_type wcdma_rxf_ici_cal_data_car1;
  rfcommon_rfnv_items_list_type c1_wcdma_rxf_ici_cal_data_car0;
  rfcommon_rfnv_items_list_type c1_wcdma_rxf_ici_cal_data_car1;
  rfcommon_rfnv_items_list_type wcdma_db10_sar_back_off_limit;
  rfcommon_rfnv_items_list_type ant_tuner;
  rfcommon_rfnv_items_list_type pa_comp_up_r1_vs_freq;
  rfcommon_rfnv_items_list_type pa_comp_up_r2_vs_freq;
  rfcommon_rfnv_items_list_type pa_comp_up_r3_vs_freq;
  rfcommon_rfnv_items_list_type pa_comp_dn_r1_vs_freq;
  rfcommon_rfnv_items_list_type pa_comp_dn_r2_vs_freq;
  rfcommon_rfnv_items_list_type pa_comp_dn_r3_vs_freq;
  rfcommon_rfnv_items_list_type tx_gtr_thresh;
  rfcommon_rfnv_items_list_type tx_ept_dpd_config_params;
  rfcommon_rfnv_items_list_type et_delay_value;
  rfcommon_rfnv_items_list_type mtpl_vs_freq;
  rfcommon_rfnv_items_list_type hdet_vs_temp;
  rfcommon_rfnv_items_list_type hdet_mod_range;
  rfcommon_rfnv_items_list_type tx_lin_vs_temp_0;
  rfcommon_rfnv_items_list_type tx_lin_vs_temp_1;
  rfcommon_rfnv_items_list_type tx_lin_vs_temp_2;
  rfcommon_rfnv_items_list_type tx_lin_vs_temp_3;
  rfcommon_rfnv_items_list_type tx_lin_vs_temp_0_addl;
  rfcommon_rfnv_items_list_type tx_lin_vs_temp_1_addl;
  rfcommon_rfnv_items_list_type tx_lin_vs_temp_2_addl;
  rfcommon_rfnv_items_list_type tx_lin_vs_temp_3_addl;
  rfcommon_rfnv_items_list_type tx_lin_vs_temp_vs_freq_p_in;
  rfcommon_rfnv_items_list_type tx_lin_vs_temp_vs_freq_p_out;
  rfcommon_rfnv_items_list_type tx_xpt_delay;
  rfcommon_rfnv_items_list_type tx_delay_offsets;
  rfcommon_rfnv_items_list_type ant_tuner_cl_ctrl_info;
  rfcommon_rfnv_items_list_type dpd_scaling_data;
  rfcommon_rfnv_items_list_type rx_static_data;
  rfcommon_rfnv_items_list_type rx_dyn_data;
  rfcommon_rfnv_items_list_type noise_floor;
  rfcommon_rfnv_items_list_type noise_floor_car1;
  rfcommon_rfnv_items_list_type c1_noise_floor_car0;
  rfcommon_rfnv_items_list_type c1_noise_floor_car1;
  rfcommon_rfnv_items_list_type pin_cal_vs_freq;
  rfcommon_rfnv_items_list_type tx_xpt_dc_delay;
  rfcommon_rfnv_items_list_type mpr_backoff;
  rfcommon_rfnv_items_list_type dc_tx_lin_vs_temp_vs_freq_p_in;
  rfcommon_rfnv_items_list_type dc_tx_lin_vs_temp_vs_freq_p_out;
  rfcommon_rfnv_items_list_type tx_dc_delay_offsets;
  rfcommon_rfnv_items_list_type pa_static;
  rfcommon_rfnv_items_list_type pa_static_z; /* for pa static compressed nv use the same copy function as the uncompressed */
  rfcommon_rfnv_items_list_type dc_pa_static; 
  rfcommon_rfnv_items_list_type dc_pa_static_z; /* for dc pa static compressed nv use the same copy function as the uncompressed */
  rfcommon_rfnv_items_list_type rx_static_data_v2;
  rfcommon_rfnv_items_list_type rx_dyn_data_v2;
  rfcommon_rfnv_items_list_type ant_tuner_car1;
  rfcommon_rfnv_items_list_type c0_rx_static_data_v3;
  rfcommon_rfnv_items_list_type c1_rx_static_data_v3;
  rfcommon_rfnv_items_list_type c2_rx_static_data_v3;
  rfcommon_rfnv_items_list_type c3_rx_static_data_v3;
} rfnv_wcdma_nv_items_type;

/* This union defines the NV item list "container". */
/* The union type allows access to explicit items */
/* through the nv_items_type struct -or- allows */
/* iterating using the enum_type array. */
/* Typical use is to write to this container explictly */
/* and to read using the generic array. */
typedef union
{
  rfnv_wcdma_nv_items_type nv_items_type;
  rfcommon_rfnv_items_list_type nv_items_array[RFNV_WCDMA_ITEMS_MAX];
} rfnv_wcdma_nv_items_list;

/* Defines the format that each NV item copy function */
/* shall adhere to. */
/*!
  @brief
  Load the NV item into a container.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
typedef void (*rfnv_wcdma_copy_func_ptr)
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);

/* Explicit definition for the NV copy function pointer lists. */
/* The order here MUST match the order of */
/* the rfnv_wcdma_nv_items_type struct. */
typedef struct
{
  rfnv_wcdma_copy_func_ptr copy_lna_hs_range_rise;
  rfnv_wcdma_copy_func_ptr copy_lna_hs_range_rise_2;
  rfnv_wcdma_copy_func_ptr copy_lna_hs_range_rise_3;
  rfnv_wcdma_copy_func_ptr copy_lna_hs_range_rise_4;
  rfnv_wcdma_copy_func_ptr copy_lna_hs_range_rise_5;
  rfnv_wcdma_copy_func_ptr copy_lna_hs_range_fall;
  rfnv_wcdma_copy_func_ptr copy_lna_hs_range_fall_2;
  rfnv_wcdma_copy_func_ptr copy_lna_hs_range_fall_3;
  rfnv_wcdma_copy_func_ptr copy_lna_hs_range_fall_4;
  rfnv_wcdma_copy_func_ptr copy_lna_hs_range_fall_5;
  rfnv_wcdma_copy_func_ptr copy_im_hs_level;
  rfnv_wcdma_copy_func_ptr copy_im_hs_level_2;
  rfnv_wcdma_copy_func_ptr copy_im_hs_level_3;
  rfnv_wcdma_copy_func_ptr copy_im_hs_level_4;
  rfnv_wcdma_copy_func_ptr copy_vga_gain_offset;
  rfnv_wcdma_copy_func_ptr copy_vga_gain_offset_vs_freq;
  rfnv_wcdma_copy_func_ptr copy_c1_vga_gain_offset_vs_freq;
  rfnv_wcdma_copy_func_ptr copy_vga_gain_offset_vs_temp;
  rfnv_wcdma_copy_func_ptr copy_im2_i_value;
  rfnv_wcdma_copy_func_ptr copy_im2_q_value;
  rfnv_wcdma_copy_func_ptr copy_im2_transconductor_value;
  rfnv_wcdma_copy_func_ptr copy_lna_range_rise;
  rfnv_wcdma_copy_func_ptr copy_lna_range_rise_2;
  rfnv_wcdma_copy_func_ptr copy_lna_range_rise_3;
  rfnv_wcdma_copy_func_ptr copy_lna_range_rise_4;
  rfnv_wcdma_copy_func_ptr copy_lna_range_rise_5;
  rfnv_wcdma_copy_func_ptr copy_lna_range_fall;
  rfnv_wcdma_copy_func_ptr copy_lna_range_fall_2;
  rfnv_wcdma_copy_func_ptr copy_lna_range_fall_3;
  rfnv_wcdma_copy_func_ptr copy_lna_range_fall_4;
  rfnv_wcdma_copy_func_ptr copy_lna_range_fall_5;
  rfnv_wcdma_copy_func_ptr copy_im_level;
  rfnv_wcdma_copy_func_ptr copy_im_level_2;
  rfnv_wcdma_copy_func_ptr copy_im_level_3;
  rfnv_wcdma_copy_func_ptr copy_im_level_4;
  rfnv_wcdma_copy_func_ptr copy_nonbypass_timer;
  rfnv_wcdma_copy_func_ptr copy_bypass_timer;
  rfnv_wcdma_copy_func_ptr copy_lna_range_offset;
  rfnv_wcdma_copy_func_ptr copy_lna_range_offset_2;
  rfnv_wcdma_copy_func_ptr copy_lna_range_offset_3;
  rfnv_wcdma_copy_func_ptr copy_lna_range_offset_4;
  rfnv_wcdma_copy_func_ptr copy_lna_range_offset_5;
  rfnv_wcdma_copy_func_ptr copy_lna_offset_vs_freq;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_offset_vs_freq;
  rfnv_wcdma_copy_func_ptr copy_lna_offset_vs_freq_2;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_offset_vs_freq_2;
  rfnv_wcdma_copy_func_ptr copy_lna_offset_vs_freq_3;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_offset_vs_freq_3;
  rfnv_wcdma_copy_func_ptr copy_lna_offset_vs_freq_4;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_offset_vs_freq_4;
  rfnv_wcdma_copy_func_ptr copy_rx_agc_min;
  rfnv_wcdma_copy_func_ptr copy_rx_agc_min_2;
  rfnv_wcdma_copy_func_ptr copy_rx_agc_min_3;
  rfnv_wcdma_copy_func_ptr copy_rx_agc_max;
  rfnv_wcdma_copy_func_ptr copy_agc_phase_offset;
  rfnv_wcdma_copy_func_ptr copy_r1_rise;
  rfnv_wcdma_copy_func_ptr copy_r1_fall;
  rfnv_wcdma_copy_func_ptr copy_tx_lim_vs_temp;
  rfnv_wcdma_copy_func_ptr copy_utran_tx_lim_vs_temp_offset;
  rfnv_wcdma_copy_func_ptr copy_tx_lim_vs_freq;
  rfnv_wcdma_copy_func_ptr copy_tx_lim_adj_gain;
  rfnv_wcdma_copy_func_ptr copy_exp_hdet_vs_agc;
  rfnv_wcdma_copy_func_ptr copy_hdet_off;
  rfnv_wcdma_copy_func_ptr copy_hdet_spn;
  rfnv_wcdma_copy_func_ptr copy_max_tx_power;
  rfnv_wcdma_copy_func_ptr copy_out_of_service_thresh;
  rfnv_wcdma_copy_func_ptr copy_enc_btf_dly;
  rfnv_wcdma_copy_func_ptr copy_rx_delay;
  rfnv_wcdma_copy_func_ptr copy_agc_pa_on_rise_delay;
  rfnv_wcdma_copy_func_ptr copy_agc_pa_on_fall_delay;
  rfnv_wcdma_copy_func_ptr copy_agc_tx_on_rise_delay;
  rfnv_wcdma_copy_func_ptr copy_agc_tx_on_fall_delay;
  rfnv_wcdma_copy_func_ptr copy_agc_update_tx_agc_time;
  rfnv_wcdma_copy_func_ptr copy_pa_gain_up_time;
  rfnv_wcdma_copy_func_ptr copy_pa_gain_down_time;
  rfnv_wcdma_copy_func_ptr copy_tx_rot_ang_pa_00;
  rfnv_wcdma_copy_func_ptr copy_tx_rot_ang_pa_01;
  rfnv_wcdma_copy_func_ptr copy_tx_rot_ang_pa_10;
  rfnv_wcdma_copy_func_ptr copy_tx_rot_ang_pa_11;
  rfnv_wcdma_copy_func_ptr copy_rtr_vco_coarse_tuning;
  rfnv_wcdma_copy_func_ptr copy_pa_compensate_up;
  rfnv_wcdma_copy_func_ptr copy_pa_compensate_down;
  rfnv_wcdma_copy_func_ptr copy_prach_r1_rise_offset;
  rfnv_wcdma_copy_func_ptr copy_prach_r1_fall_offset;
  rfnv_wcdma_copy_func_ptr copy_tx_cal_chan;
  rfnv_wcdma_copy_func_ptr copy_rx_cal_chan;
  rfnv_wcdma_copy_func_ptr copy_c1_rx_cal_chan;
  rfnv_wcdma_copy_func_ptr copy_r2_rise;
  rfnv_wcdma_copy_func_ptr copy_r2_fall;
  rfnv_wcdma_copy_func_ptr copy_r3_rise;
  rfnv_wcdma_copy_func_ptr copy_r3_fall;
  rfnv_wcdma_copy_func_ptr copy_pa_compensate_up_r2;
  rfnv_wcdma_copy_func_ptr copy_pa_compensate_down_r2;
  rfnv_wcdma_copy_func_ptr copy_pa_compensate_up_r3;
  rfnv_wcdma_copy_func_ptr copy_pa_compensate_down_r3;
  rfnv_wcdma_copy_func_ptr copy_prach_r2_rise_offset;
  rfnv_wcdma_copy_func_ptr copy_prach_r2_fall_offset;
  rfnv_wcdma_copy_func_ptr copy_prach_r3_rise_offset;
  rfnv_wcdma_copy_func_ptr copy_prach_r3_fall_offset;
  rfnv_wcdma_copy_func_ptr copy_pa_range_map;
  rfnv_wcdma_copy_func_ptr copy_dch_agc_update_tx_agc_time;
  rfnv_wcdma_copy_func_ptr copy_agc_tx_adj_pdm_delay;
  rfnv_wcdma_copy_func_ptr copy_hs_r1_rise;
  rfnv_wcdma_copy_func_ptr copy_hs_r1_fall;
  rfnv_wcdma_copy_func_ptr copy_hs_r2_rise;
  rfnv_wcdma_copy_func_ptr copy_hs_r2_fall;
  rfnv_wcdma_copy_func_ptr copy_hs_r3_rise;
  rfnv_wcdma_copy_func_ptr copy_hs_r3_fall;
  rfnv_wcdma_copy_func_ptr copy_max_pwr_backoff_voltages;
  rfnv_wcdma_copy_func_ptr copy_max_pwr_backoff_volt1;
  rfnv_wcdma_copy_func_ptr copy_max_pwr_backoff_volt2;
  rfnv_wcdma_copy_func_ptr copy_max_pwr_backoff_volt3;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rxf_mis_comp_a_coeff;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rxf_mis_comp_b_coeff;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rxf_ici_i_coef0_1;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rxf_ici_i_coef2_3;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rxf_ici_i_coef4_5;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rxf_ici_i_coef6_7;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rxf_ici_i_coef8_9;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rxf_ici_i_coef10_11;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rxf_ici_i_coef12_13;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rxf_ici_i_coef14_15;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rxf_ici_q_coef0_1;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rxf_ici_q_coef2_3;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rxf_ici_q_coef4_5;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rxf_ici_q_coef6_7;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rxf_ici_q_coef8_9;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rxf_ici_q_coef10_11;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rxf_ici_q_coef12_13;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rxf_ici_q_coef14_15;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rxf_ici_iq_trunc;
  rfnv_wcdma_copy_func_ptr copy_c1_vga_gain_offset;
  rfnv_wcdma_copy_func_ptr copy_c1_vga_gain_offset_vs_temp;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_rise;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_rise_2;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_rise_3;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_rise_4;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_rise_5;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_fall;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_fall_2;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_fall_3;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_fall_4;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_fall_5;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_offset;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_offset_2;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_offset_3;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_offset_4;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_offset_5;
  rfnv_wcdma_copy_func_ptr copy_c1_im_level;
  rfnv_wcdma_copy_func_ptr copy_c1_im_level_2;
  rfnv_wcdma_copy_func_ptr copy_c1_im_level_3;
  rfnv_wcdma_copy_func_ptr copy_c1_im_level_4;
  rfnv_wcdma_copy_func_ptr copy_c1_im2_i_value;
  rfnv_wcdma_copy_func_ptr copy_c1_im2_q_value;
  rfnv_wcdma_copy_func_ptr copy_c1_im2_transconductor_value;
  rfnv_wcdma_copy_func_ptr copy_c1_nonbypass_timer;
  rfnv_wcdma_copy_func_ptr copy_c1_bypass_timer;
  rfnv_wcdma_copy_func_ptr copy_rfr_vco_coarse_tune_2_1900;
  rfnv_wcdma_copy_func_ptr copy_rf_drx_mode_sel;
  rfnv_wcdma_copy_func_ptr copy_wcdma_iaccum;
  rfnv_wcdma_copy_func_ptr copy_wcdma_qaccum;
  rfnv_wcdma_copy_func_ptr copy_timer_hysterisis;
  rfnv_wcdma_copy_func_ptr copy_exp_hdet_vs_agc_v2;
  rfnv_wcdma_copy_func_ptr copy_wcdma_c0_c1_delay;
  rfnv_wcdma_copy_func_ptr copy_wcdma_lna_phase_ctl;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_lna_phase_ctl;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rel6_tx_beta_scaling_comp;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rel6_tx_agc_offset;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rel6_tx_mpr_backoff;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_rxf_mis_comp_a_coeff;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_rxf_mis_comp_b_coeff;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_rxf_ici_i_coef0_1;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_rxf_ici_i_coef2_3;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_rxf_ici_i_coef4_5;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_rxf_ici_i_coef6_7;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_rxf_ici_i_coef8_9;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_rxf_ici_i_coef10_11;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_rxf_ici_i_coef12_13;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_rxf_ici_i_coef14_15;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_rxf_ici_q_coef0_1;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_rxf_ici_q_coef2_3;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_rxf_ici_q_coef4_5;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_rxf_ici_q_coef6_7;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_rxf_ici_q_coef8_9;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_rxf_ici_q_coef10_11;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_rxf_ici_q_coef12_13;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_rxf_ici_q_coef14_15;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_rxf_ici_iq_trunc;
  rfnv_wcdma_copy_func_ptr copy_lna_range_rise_car1;
  rfnv_wcdma_copy_func_ptr copy_lna_range_rise_2_car1;
  rfnv_wcdma_copy_func_ptr copy_lna_range_rise_3_car1;
  rfnv_wcdma_copy_func_ptr copy_lna_range_rise_4_car1;
  rfnv_wcdma_copy_func_ptr copy_lna_range_rise_5_car1;
  rfnv_wcdma_copy_func_ptr copy_lna_range_fall_car1;
  rfnv_wcdma_copy_func_ptr copy_lna_range_fall_2_car1;
  rfnv_wcdma_copy_func_ptr copy_lna_range_fall_3_car1;
  rfnv_wcdma_copy_func_ptr copy_lna_range_fall_4_car1;
  rfnv_wcdma_copy_func_ptr copy_lna_range_fall_5_car1;
  rfnv_wcdma_copy_func_ptr copy_lna_range_offset_car1;
  rfnv_wcdma_copy_func_ptr copy_lna_range_offset_2_car1;
  rfnv_wcdma_copy_func_ptr copy_lna_range_offset_3_car1;
  rfnv_wcdma_copy_func_ptr copy_lna_range_offset_4_car1;
  rfnv_wcdma_copy_func_ptr copy_lna_range_offset_5_car1;
  rfnv_wcdma_copy_func_ptr copy_vga_gain_offset_car1;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_rise_car1;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_rise_2_car1;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_rise_3_car1;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_rise_4_car1;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_rise_5_car1;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_fall_car1;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_fall_2_car1;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_fall_3_car1;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_fall_4_car1;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_fall_5_car1;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_offset_car1;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_offset_2_car1;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_offset_3_car1;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_offset_4_car1;
  rfnv_wcdma_copy_func_ptr copy_c1_lna_range_offset_5_car1;
  rfnv_wcdma_copy_func_ptr copy_c1_vga_gain_offset_car1;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rxf_ici_cal_data_car0;
  rfnv_wcdma_copy_func_ptr copy_wcdma_rxf_ici_cal_data_car1;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_rxf_ici_cal_data_car0;
  rfnv_wcdma_copy_func_ptr copy_c1_wcdma_rxf_ici_cal_data_car1;
  rfnv_wcdma_copy_func_ptr copy_wcdma_db10_sar_back_off_limit;
  rfnv_wcdma_copy_func_ptr copy_ant_tuner;
  rfnv_wcdma_copy_func_ptr copy_pa_comp_up_r1_vs_freq;
  rfnv_wcdma_copy_func_ptr copy_pa_comp_up_r2_vs_freq;
  rfnv_wcdma_copy_func_ptr copy_pa_comp_up_r3_vs_freq;
  rfnv_wcdma_copy_func_ptr copy_pa_comp_dn_r1_vs_freq;
  rfnv_wcdma_copy_func_ptr copy_pa_comp_dn_r2_vs_freq;
  rfnv_wcdma_copy_func_ptr copy_pa_comp_dn_r3_vs_freq;
  rfnv_wcdma_copy_func_ptr copy_tx_gtr_thresh;
  rfnv_wcdma_copy_func_ptr copy_tx_ept_dpd_config_params;
  rfnv_wcdma_copy_func_ptr copy_et_delay_value;
  rfnv_wcdma_copy_func_ptr copy_mtpl_vs_freq;
  rfnv_wcdma_copy_func_ptr copy_hdet_vs_temp;
  rfnv_wcdma_copy_func_ptr copy_hdet_mod_range;
  rfnv_wcdma_copy_func_ptr copy_tx_lin_vs_temp_0;
  rfnv_wcdma_copy_func_ptr copy_tx_lin_vs_temp_1;
  rfnv_wcdma_copy_func_ptr copy_tx_lin_vs_temp_2;
  rfnv_wcdma_copy_func_ptr copy_tx_lin_vs_temp_3;
  rfnv_wcdma_copy_func_ptr copy_tx_lin_vs_temp_0_addl;
  rfnv_wcdma_copy_func_ptr copy_tx_lin_vs_temp_1_addl;
  rfnv_wcdma_copy_func_ptr copy_tx_lin_vs_temp_2_addl;
  rfnv_wcdma_copy_func_ptr copy_tx_lin_vs_temp_3_addl;
  rfnv_wcdma_copy_func_ptr copy_tx_lin_vs_temp_vs_freq_p_in;
  rfnv_wcdma_copy_func_ptr copy_tx_lin_vs_temp_vs_freq_p_out;
  rfnv_wcdma_copy_func_ptr copy_tx_xpt_delay;
  rfnv_wcdma_copy_func_ptr copy_tx_delay_offsets;
  rfnv_wcdma_copy_func_ptr copy_ant_tuner_cl_ctrl_info;
  rfnv_wcdma_copy_func_ptr copy_dpd_scaling_data;
  rfnv_wcdma_copy_func_ptr copy_rx_static_data;
  rfnv_wcdma_copy_func_ptr copy_rx_dyn_data;
  rfnv_wcdma_copy_func_ptr copy_noise_floor;
  rfnv_wcdma_copy_func_ptr copy_noise_floor_car1;
  rfnv_wcdma_copy_func_ptr copy_c1_noise_floor_car0;
  rfnv_wcdma_copy_func_ptr copy_c1_noise_floor_car1;
  rfnv_wcdma_copy_func_ptr copy_pin_cal_vs_freq;
  rfnv_wcdma_copy_func_ptr copy_tx_xpt_dc_delay;
  rfnv_wcdma_copy_func_ptr copy_mpr_backoff;
  rfnv_wcdma_copy_func_ptr copy_dc_tx_lin_vs_temp_vs_freq_p_in;
  rfnv_wcdma_copy_func_ptr copy_dc_tx_lin_vs_temp_vs_freq_p_out;
  rfnv_wcdma_copy_func_ptr copy_tx_dc_delay_offsets;
  rfnv_wcdma_copy_func_ptr copy_pa_static;
  rfnv_wcdma_copy_func_ptr copy_pa_static_z;
  rfnv_wcdma_copy_func_ptr copy_dc_pa_static;
  rfnv_wcdma_copy_func_ptr copy_dc_pa_static_z;
  rfnv_wcdma_copy_func_ptr copy_rx_static_data_v2;
  rfnv_wcdma_copy_func_ptr copy_rx_dyn_data_v2;
  rfnv_wcdma_copy_func_ptr copy_ant_tuner_car1;
  rfnv_wcdma_copy_func_ptr copy_c0_rx_static_data_v3;
  rfnv_wcdma_copy_func_ptr copy_c1_rx_static_data_v3;
  rfnv_wcdma_copy_func_ptr copy_c2_rx_static_data_v3;
  rfnv_wcdma_copy_func_ptr copy_c3_rx_static_data_v3;
} rfnv_wcdma_nv_copy_funcs_type;

/* This union defines the NV item copy function "container". */
/* The union type allows access to explicit items */
/* through the nv_copy_funcs_type struct -or- allows */
/* iterating using the func_ptr array. */
/* Typical use is to write to this container explictly */
/* and to read using the generic array. */
typedef union
{
  rfnv_wcdma_nv_copy_funcs_type copy_funcs_type;
  rfnv_wcdma_copy_func_ptr copy_funcs_array[RFNV_WCDMA_ITEMS_MAX];
} rfnv_wcdma_nv_copy_funcs_list;

#endif
