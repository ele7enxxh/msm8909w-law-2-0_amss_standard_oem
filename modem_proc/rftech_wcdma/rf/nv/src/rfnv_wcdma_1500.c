/*!
  @file
  rfnv_wcdma_1500.c

  @brief

  @details
  This module retrieves all NV items that will be used by the WCDMA 1500 RF
  unit.

*/

/*===========================================================================

Copyright (c) 2008 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/nv/src/rfnv_wcdma_1500.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/03/14   kai     Added support for WCDMA Rx V3 static NVs
10/31/14   ac      changed the NV to add car1
10/28/14   ac      nv tuner items for DBDC
10/17/14   weiz	   Support of PA static compressed NV
09/20/14   zhh     fix KW warnings
05/01/14   aro     NV item to hold char data for DC ET Delay vs freq vs temp
02/26/14   zhh    Update XPT Delay structure to support DC-UPA
04/14/14   zhh     PinPout NV for DCUPA
03/20/14   zhh     update new MPR backoff NV
04/20/14   bn      WCDMA 8 PA state support and related cleanup
01/15/14   rmb     Add NV support for Noise Floor Correction.
11/25/13   rmb     Add Pin Cal versus freq related NVs for all PA states and bands.
08/23/13   vs      Added Rx static and Cal NVs
08/12/13   kai     Added DPD scaling support
07/24/13   kai     Added XPT path delay offset vs temp vs freq
07/16/13   vs      Added CL tuner NVs
07/12/13   kcj     Support checking for critical NV items.
07/08/13   kcj     Can't use designated initialization with OFT's compiler. 
07/03/13   kcj     Dynamically allocated NV struct memory. 
06/23/13   kai     Added WCDMA 16channels xpt delay NVs 
05/23/13   yb      Fixing the int16 array accesses and buffering of for 
                   pa_comp_up/down_vs_freq NVs
04/30/13   cd      Add NV loading support for Lin vs Temp items into common
                   TxLin data types
04/05/13   cd      Pin polarity will be handled in NV by characterization. 
                   No need to invert in SW.
03/29/13   cd      XPT Temp Comp, Pin NV polarity needs to be handled 
                   correctly
03/26/13   cd      Fixed legacy lin vs temp NV loading into new data structs
03/20/13   cd      Add support for loading new lin vs temp vs freq Pin and 
                   Pout NV items
03/19/13   dw/kai  [1]Added support for mtpl_vs_freq  
                   [2]Added support to limit allowable HDET mod range
03/14/13   kai     Added ET delay NV support
06/10/12   kai     Correct dualcarrier NVs default values 
06/07/12   kai     Correct WCDMA band 1500 dualcarrier NVs to match the NVs used by tools
06/04/12   cri     Add EPT support for WCDMA
05/21/12   dw      Fix some bug in copying NV  
05/09/12   kai     Change AGC Tx Adj PDM delay default to 0 
04/17/12   gh      Add TX_GTR_THRESH support
03/06/12   gh      16-channel Tx lin_vs_temp feature
02/24/12   ac      memcopy to for loop for pa comp vs freq since EFS NV def is int16
01/16/12   ac      fix the pa_comp_up_r1_vs_freq data type 
11/18/11   ac      Enable PA Comp Up/Down to vary with frequency
09/07/11   gh      Add Ant Tuner support
08/23/11   ac      rfc cleanup
06/06/11   dw      Rail default LNA 4/5 switchpoints
05/23/11   dw      Remove rfwcdmalib_msm.h   
05/02/11   ad      Added WCDMA second sweep SMPS PA bias items 
03/28/11   ka      Remove warning 
03/22/11   ka      Add APT items and increase SAR default  
03/03/11   ka      Added SAR support 
02/25/11   ka      Remove warnings 
01/31/11   ac      GST temp comp support
01/18/11   dej     Mainline RF_HAS_VOYAGER2_SUPPORT
02/25/11   ac/dw   Merge EFS drx car1 NV support for dual carrier 
12/20/10   ka      Initial GST support   
11/16/10   ac      decrease dc fall thresholds by 3dB  
10/11/10   ad      Added preliminary support for WCDMA APT Phase 1  
09/30/10   ac      EFS support for all bands
07/30/10   dw      Revert the last change
07/29/10   dw      Change default ICI IQ trunc bits for SC
06/29/10   dw      New ICI coefficients for C1
06/28/10   dw      New ICI coefficients
06/22/10   ac      adding ici coeff
05/19/10   ac      adding the new C1 nv items
05/19/10   ac      Changed default ICI coeffs for C1
05/12/10   ac      added B11 nv support for j8220
01/18/10   ckl     Removed med level lint error.
01/08/10   ckl     Removed high level lint error.
09/04/09   ckl     Remove warnings caused by rfwcdma_core_get_freq_from_uarfcn
08/12/09   ckl     Merge diversity NV items into main NV item list.
08/12/09    xw     Added RTR VCO coarse tune NV support.
03/17/09   ckl     Used new WCDMA definitions and fixed compilation errors
                   in SCMM baseline
03/15/09   ckl     Adapted rfcommon_nv.h
03/09/09   ckl     Code cleanup
09/08/08   clh     Updated ICI default vals to get ICI improvement from -26dB to -30dB.
08/22/08   ka      Added ICI and IQM NV.
12/05/07   bn      EUL Phase 2 NV support (release 6)
10/26/07   dw      Fixed Lint errors.
10/10/07   ad      Added lna_phase_ctl support for PRx/DRx paths in wcdma 1500 nv table
08/09/07   ad      Added support for reading HS LNA switch point NVs during HS calls.
08/07/07   ad      From 6260: Fix for retaining Temp Comp when UTRAN max pwr < NV max pwr.
08/07/07   jfc/ka  Added support for tx secondary freq comp
06/11/07   ad      Added Cx16 delay between primary and diversity paths to wcdma 1500 nv table.
02/28/07   ycl     Fix compiler warnings.
02/05/07   ycl     Merge to supprot RTR6285.
05/16/06    dp     Remove obsolete version of diversity antenna select NV structure.
                   Make new drx antenna NV non-critical.  Move 2nd coarse
                   tune table into non-diversity item list as non-critical.
04/18/06   lcl/ap  Added DC NV item support for interBand CM.
04/07/06   rsr     Added support to read diversity mode select values from NV.
03/07/06   jfc     Changed IM2 and transconductor default to chip defaults.
03/03/06   bhas    Add Rx diversity path NV items.
02/07/06    ka     Removed featurization around TX_PDM_LIN items.
01/12/06    ka     Added support for tx pdm list nv items.
11/09/05    ka     Merged in MSM6275 changes.
06/03/05    bn     Changed pwr backoff defaults to [0 0 12 24]
05/23/05    rv     Changed power back off def values to 0.
05/23/05   jfc     Added support for RF_UTRAN_TX_LIM_VS_TEMP_COMP.
03/23/05    rv    Changed pwr back off NV items to non-critical.
03/25/05    bn     Max Power Backoff feature NV support
03/29/05    xw     Deleted RF_HAS_RFR6250 and RF_HAS_WCDMA1500 etc for single RF driver.
03/17/05    bn     HS switch point NV support
03/04/05    bn     ModUpdate and tx_adj_pdm_delay NV support
02/23/05    rv     Merged Raven changes back to mainline.
01/20/05    rv     Updated defaults for GWZRF6275POL RF card.
12/01/04   jfc     Fixed check of first element in Tx, Rx cal chan list
11/16/04    bn     4-Page Linearizer Support in RF driver
09/23/04   dhh     Linear interpolation optimization.
09/01/04   dhh     Added support for Freq Comp linear interpolation and PRACH
                   PA range.
07/01/04    rv     Added support for NV_RFR_VCO_COARSE_TUNING_1500_I.
06/08/04   dhh     Used band specific items.
05/07/04   dhh     Mainlined paCompensateUp/Down. Updated paGainUp/Down default.
04/17/04   jtn     Lint.
03/10/04   dhh     Added NV items for paCompensateUp/paCompensateDown.
03/10/04    eh     Updated the default values for some NV static items.
03/01/04   dhh     Provided interface for WL1 to get tx rotator angle from RFNV.
02/25/04   eh      Changed BTF NV default value to -217
02/23/04   dhh     Added NV to support pa range update time.
02/02/04   eh      Changed BTF NV default value to -222
01/19/04   dhh     Added tx delays/timing NV items.
08/21/03   ap      Final modifications for new NV mould
06/17/03   eh      Initial Revision.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfnv_wcdma_1500.h"
#include "rfnv_items.h"

/*===========================================================================

                           CONSTANT DEFINITIONS

===========================================================================*/

/*===========================================================================

                           DATA DEFINITIONS

===========================================================================*/


/* RF NV item identifier array */
/* This array defines RF NV items that are supported and will be read */
rfnv_wcdma_nv_items_list rfnv_wcdma_1500_nv_items_list;

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize the NV list for this band.

  @details
  All items are initialized to be Non-critical.
  All item ids are set for the respective enum ID.
 
*/
void rfnv_wcdma_1500_init_nv_item_list()
{
  uint16 i = 0;

  // Initialize all items as NON-CRITICAL. Critical items can be set explicitly.
  for (i=0; i<RFNV_WCDMA_ITEMS_MAX; i++)
  {
    rfnv_wcdma_1500_nv_items_list.nv_items_array[i].rfnv_item_attrib = NV_ITEM_ATTRIB_NON_CRTCL_BMSK;
  }

  // Initialize all of the rfnv_item_id types.
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_hs_range_rise.rfnv_item_id = RFNV_WCDMA_1500_HS_LNA_RANGE_RISE_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_hs_range_rise_2.rfnv_item_id = RFNV_WCDMA_1500_HS_LNA_RANGE_RISE_2_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_hs_range_rise_3.rfnv_item_id = RFNV_WCDMA_1500_HS_LNA_RANGE_RISE_3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_hs_range_rise_4.rfnv_item_id = RFNV_WCDMA_1500_HS_LNA_RANGE_RISE_4_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_hs_range_rise_5.rfnv_item_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_hs_range_fall.rfnv_item_id = RFNV_WCDMA_1500_HS_LNA_RANGE_FALL_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_hs_range_fall_2.rfnv_item_id = RFNV_WCDMA_1500_HS_LNA_RANGE_FALL_2_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_hs_range_fall_3.rfnv_item_id = RFNV_WCDMA_1500_HS_LNA_RANGE_FALL_3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_hs_range_fall_4.rfnv_item_id = RFNV_WCDMA_1500_HS_LNA_RANGE_FALL_4_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_hs_range_fall_5.rfnv_item_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.im_hs_level.rfnv_item_id = RFNV_WCDMA_1500_HS_IM_LEVEL_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.im_hs_level_2.rfnv_item_id = RFNV_WCDMA_1500_HS_IM_LEVEL_2_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.im_hs_level_3.rfnv_item_id = RFNV_WCDMA_1500_HS_IM_LEVEL_3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.im_hs_level_4.rfnv_item_id = RFNV_WCDMA_1500_HS_IM_LEVEL_4_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.vga_gain_offset.rfnv_item_id = RFNV_WCDMA_1500_VGA_GAIN_OFFSET_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.vga_gain_offset_vs_freq.rfnv_item_id = RFNV_WCDMA_1500_VGA_GAIN_OFFSET_VS_FREQ_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_vga_gain_offset_vs_freq.rfnv_item_id = RFNV_C1_WCDMA_1500_VGA_GAIN_OFFSET_VS_FREQ_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.vga_gain_offset_vs_temp.rfnv_item_id = RFNV_WCDMA_1500_VGA_GAIN_OFFSET_VS_TEMP_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.im2_i_value.rfnv_item_id = RFNV_WCDMA_1500_IM2_I_VALUE_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.im2_q_value.rfnv_item_id = RFNV_WCDMA_1500_IM2_Q_VALUE_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.im2_transconductor_value.rfnv_item_id = RFNV_WCDMA_1500_IM2_TRANSCONDUCTOR_VALUE_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_rise.rfnv_item_id = RFNV_WCDMA_1500_LNA_RANGE_RISE_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_rise_2.rfnv_item_id = RFNV_WCDMA_1500_LNA_RANGE_RISE_2_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_rise_3.rfnv_item_id = RFNV_WCDMA_1500_LNA_RANGE_RISE_3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_rise_4.rfnv_item_id = RFNV_WCDMA_1500_LNA_RANGE_RISE_4_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_rise_5.rfnv_item_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_fall.rfnv_item_id = RFNV_WCDMA_1500_LNA_RANGE_FALL_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_fall_2.rfnv_item_id = RFNV_WCDMA_1500_LNA_RANGE_FALL_2_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_fall_3.rfnv_item_id = RFNV_WCDMA_1500_LNA_RANGE_FALL_3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_fall_4.rfnv_item_id = RFNV_WCDMA_1500_LNA_RANGE_FALL_4_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_fall_5.rfnv_item_id = RFNV_ITEMS_ENUM_MAX; 
  rfnv_wcdma_1500_nv_items_list.nv_items_type.im_level.rfnv_item_id = RFNV_WCDMA_1500_IM_LEVEL_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.im_level_2.rfnv_item_id = RFNV_WCDMA_1500_IM_LEVEL_2_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.im_level_3.rfnv_item_id = RFNV_WCDMA_1500_IM_LEVEL_3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.im_level_4.rfnv_item_id = RFNV_WCDMA_1500_IM_LEVEL_4_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.nonbypass_timer.rfnv_item_id = RFNV_WCDMA_1500_NONBYPASS_TIMER_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.bypass_timer.rfnv_item_id = RFNV_WCDMA_1500_BYPASS_TIMER_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_offset.rfnv_item_id = RFNV_WCDMA_1500_LNA_RANGE_OFFSET_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_offset_2.rfnv_item_id = RFNV_WCDMA_1500_LNA_RANGE_OFFSET_2_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_offset_3.rfnv_item_id = RFNV_WCDMA_1500_LNA_RANGE_OFFSET_3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_offset_4.rfnv_item_id = RFNV_WCDMA_1500_LNA_RANGE_OFFSET_4_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_offset_5.rfnv_item_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_offset_vs_freq.rfnv_item_id = RFNV_WCDMA_1500_LNA_OFFSET_VS_FREQ_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_offset_vs_freq.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_OFFSET_VS_FREQ_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_offset_vs_freq_2.rfnv_item_id = RFNV_WCDMA_1500_LNA_OFFSET_VS_FREQ_2_I; 
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_offset_vs_freq_2.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_OFFSET_VS_FREQ_2_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_offset_vs_freq_3.rfnv_item_id = RFNV_WCDMA_1500_LNA_OFFSET_VS_FREQ_3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_offset_vs_freq_3.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_OFFSET_VS_FREQ_3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_offset_vs_freq_4.rfnv_item_id = RFNV_WCDMA_1500_LNA_OFFSET_VS_FREQ_4_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_offset_vs_freq_4.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_OFFSET_VS_FREQ_4_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.rx_agc_min.rfnv_item_id = RFNV_WCDMA_1500_RX_AGC_MIN_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.rx_agc_min_2.rfnv_item_id = RFNV_WCDMA_1500_RX_AGC_MIN_2_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.rx_agc_min_3.rfnv_item_id = RFNV_WCDMA_1500_RX_AGC_MIN_3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.rx_agc_max.rfnv_item_id = RFNV_WCDMA_1500_RX_AGC_MAX_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.agc_phase_offset.rfnv_item_id = RFNV_WCDMA_1500_AGC_PHASE_OFFSET_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.r1_rise.rfnv_item_id = RFNV_WCDMA_1500_R1_RISE_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.r1_fall.rfnv_item_id = RFNV_WCDMA_1500_R1_FALL_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_lim_vs_temp.rfnv_item_id = RFNV_WCDMA_1500_TX_LIM_VS_TEMP_I; 
  rfnv_wcdma_1500_nv_items_list.nv_items_type.utran_tx_lim_vs_temp_offset.rfnv_item_id = RFNV_WCDMA_1500_UTRAN_TX_LIM_VS_TEMP_OFFSET_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_lim_vs_freq.rfnv_item_id = RFNV_WCDMA_1500_TX_LIM_VS_FREQ_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_lim_adj_gain.rfnv_item_id = RFNV_WCDMA_1500_ADJ_FACTOR_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.exp_hdet_vs_agc.rfnv_item_id = RFNV_WCDMA_1500_EXP_HDET_VS_AGC_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.hdet_off.rfnv_item_id = RFNV_WCDMA_1500_HDET_OFF_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.hdet_spn.rfnv_item_id = RFNV_WCDMA_1500_HDET_SPN_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.max_tx_power.rfnv_item_id = RFNV_WCDMA_1500_MAX_TX_POWER_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.out_of_service_thresh.rfnv_item_id = RFNV_WCDMA_1500_OUT_OF_SERVICE_THRESH_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.enc_btf_dly.rfnv_item_id = RFNV_WCDMA_1500_ENC_BTF_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.rx_delay.rfnv_item_id = RFNV_WCDMA_1500_RX_DELAY_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.agc_pa_on_rise_delay.rfnv_item_id = RFNV_WCDMA_1500_AGC_PA_ON_RISE_DELAY_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.agc_pa_on_fall_delay.rfnv_item_id = RFNV_WCDMA_1500_AGC_PA_ON_FALL_DELAY_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.agc_tx_on_rise_delay.rfnv_item_id = RFNV_WCDMA_1500_AGC_TX_ON_RISE_DELAY_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.agc_tx_on_fall_delay.rfnv_item_id = RFNV_WCDMA_1500_AGC_TX_ON_FALL_DELAY_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.agc_update_tx_agc_time.rfnv_item_id = RFNV_WCDMA_1500_AGC_UPDATE_TX_AGC_TIME_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.pa_gain_up_time.rfnv_item_id = RFNV_WCDMA_1500_PA_GAIN_UP_TIME_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.pa_gain_down_time.rfnv_item_id = RFNV_WCDMA_1500_PA_GAIN_DOWN_TIME_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_rot_ang_pa_00.rfnv_item_id = RFNV_WCDMA_1500_TX_ROT_ANGLE_PA_STATE_00_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_rot_ang_pa_01.rfnv_item_id = RFNV_WCDMA_1500_TX_ROT_ANGLE_PA_STATE_01_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_rot_ang_pa_10.rfnv_item_id = RFNV_WCDMA_1500_TX_ROT_ANGLE_PA_STATE_10_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_rot_ang_pa_11.rfnv_item_id = RFNV_WCDMA_1500_TX_ROT_ANGLE_PA_STATE_11_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.rtr_vco_coarse_tuning.rfnv_item_id = RFNV_RFR_VCO_COARSE_TUNING_1900_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.pa_compensate_up.rfnv_item_id = RFNV_WCDMA_1500_PA_COMPENSATE_UP_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.pa_compensate_down.rfnv_item_id = RFNV_WCDMA_1500_PA_COMPENSATE_DOWN_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.prach_r1_rise_offset.rfnv_item_id = RFNV_WCDMA_1500_PRACH_R1_RISE_OFFSET_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.prach_r1_fall_offset.rfnv_item_id = RFNV_WCDMA_1500_PRACH_R1_FALL_OFFSET_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_cal_chan.rfnv_item_id = RFNV_WCDMA_1500_TX_CAL_CHAN_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.rx_cal_chan.rfnv_item_id = RFNV_WCDMA_1500_RX_CAL_CHAN_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_rx_cal_chan.rfnv_item_id = RFNV_C1_WCDMA_1500_RX_CAL_CHAN_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.r2_rise.rfnv_item_id = RFNV_WCDMA_1500_R2_RISE_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.r2_fall.rfnv_item_id = RFNV_WCDMA_1500_R2_FALL_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.r3_rise.rfnv_item_id = RFNV_WCDMA_1500_R3_RISE_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.r3_fall.rfnv_item_id = RFNV_WCDMA_1500_R3_FALL_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.pa_compensate_up_r2.rfnv_item_id = RFNV_WCDMA_1500_PA_COMPENSATE_UP_R2_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.pa_compensate_down_r2.rfnv_item_id = RFNV_WCDMA_1500_PA_COMPENSATE_DOWN_R2_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.pa_compensate_up_r3.rfnv_item_id = RFNV_WCDMA_1500_PA_COMPENSATE_UP_R3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.pa_compensate_down_r3.rfnv_item_id = RFNV_WCDMA_1500_PA_COMPENSATE_DOWN_R3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.prach_r2_rise_offset.rfnv_item_id = RFNV_WCDMA_1500_PRACH_R2_RISE_OFFSET_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.prach_r2_fall_offset.rfnv_item_id = RFNV_WCDMA_1500_PRACH_R2_FALL_OFFSET_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.prach_r3_rise_offset.rfnv_item_id = RFNV_WCDMA_1500_PRACH_R3_RISE_OFFSET_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.prach_r3_fall_offset.rfnv_item_id = RFNV_WCDMA_1500_PRACH_R3_FALL_OFFSET_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.pa_range_map.rfnv_item_id = RFNV_WCDMA_1500_PA_RANGE_MAP_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.dch_agc_update_tx_agc_time.rfnv_item_id = RFNV_WCDMA_1500_DCH_AGC_UPDATE_TX_AGC_TIME_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.agc_tx_adj_pdm_delay.rfnv_item_id = RFNV_WCDMA_1500_AGC_TX_ADJ_PDM_DELAY_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.hs_r1_rise.rfnv_item_id = RFNV_WCDMA_1500_HS_R1_RISE_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.hs_r1_fall.rfnv_item_id = RFNV_WCDMA_1500_HS_R1_FALL_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.hs_r2_rise.rfnv_item_id = RFNV_WCDMA_1500_HS_R2_RISE_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.hs_r2_fall.rfnv_item_id = RFNV_WCDMA_1500_HS_R2_FALL_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.hs_r3_rise.rfnv_item_id = RFNV_WCDMA_1500_HS_R3_RISE_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.hs_r3_fall.rfnv_item_id = RFNV_WCDMA_1500_HS_R3_FALL_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.max_pwr_backoff_voltages.rfnv_item_id = RFNV_WCDMA_1500_MAX_PWR_BACKOFF_VOLTAGES_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.max_pwr_backoff_volt1.rfnv_item_id = RFNV_WCDMA_1500_MAX_PWR_BACKOFF_VOLT1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.max_pwr_backoff_volt2.rfnv_item_id = RFNV_WCDMA_1500_MAX_PWR_BACKOFF_VOLT2_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.max_pwr_backoff_volt3.rfnv_item_id = RFNV_WCDMA_1500_MAX_PWR_BACKOFF_VOLT3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rxf_mis_comp_a_coeff.rfnv_item_id = RFNV_WCDMA_1500_RXF_MIS_COMP_A_COEFF_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rxf_mis_comp_b_coeff.rfnv_item_id = RFNV_WCDMA_1500_RXF_MIS_COMP_B_COEFF_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rxf_ici_i_coef0_1.rfnv_item_id = RFNV_WCDMA_1500_RXF_ICI_I_COEF0_1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rxf_ici_i_coef2_3.rfnv_item_id = RFNV_WCDMA_1500_RXF_ICI_I_COEF2_3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rxf_ici_i_coef4_5.rfnv_item_id = RFNV_WCDMA_1500_RXF_ICI_I_COEF4_5_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rxf_ici_i_coef6_7.rfnv_item_id = RFNV_WCDMA_1500_RXF_ICI_I_COEF6_7_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rxf_ici_i_coef8_9.rfnv_item_id = RFNV_WCDMA_1500_RXF_ICI_I_COEF8_9_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rxf_ici_i_coef10_11.rfnv_item_id = RFNV_WCDMA_1500_RXF_ICI_I_COEF10_11_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rxf_ici_i_coef12_13.rfnv_item_id = RFNV_WCDMA_1500_RXF_ICI_I_COEF12_13_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rxf_ici_i_coef14_15.rfnv_item_id = RFNV_WCDMA_1500_RXF_ICI_I_COEF14_15_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rxf_ici_q_coef0_1.rfnv_item_id = RFNV_WCDMA_1500_RXF_ICI_Q_COEF0_1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rxf_ici_q_coef2_3.rfnv_item_id = RFNV_WCDMA_1500_RXF_ICI_Q_COEF2_3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rxf_ici_q_coef4_5.rfnv_item_id = RFNV_WCDMA_1500_RXF_ICI_Q_COEF4_5_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rxf_ici_q_coef6_7.rfnv_item_id = RFNV_WCDMA_1500_RXF_ICI_Q_COEF6_7_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rxf_ici_q_coef8_9.rfnv_item_id = RFNV_WCDMA_1500_RXF_ICI_Q_COEF8_9_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rxf_ici_q_coef10_11.rfnv_item_id = RFNV_WCDMA_1500_RXF_ICI_Q_COEF10_11_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rxf_ici_q_coef12_13.rfnv_item_id = RFNV_WCDMA_1500_RXF_ICI_Q_COEF12_13_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rxf_ici_q_coef14_15.rfnv_item_id = RFNV_WCDMA_1500_RXF_ICI_Q_COEF14_15_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rxf_ici_iq_trunc.rfnv_item_id = RFNV_WCDMA_1500_RXF_ICI_IQ_TRUNC_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_vga_gain_offset.rfnv_item_id = RFNV_C1_WCDMA_1500_VGA_GAIN_OFFSET_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_vga_gain_offset_vs_temp.rfnv_item_id = RFNV_C1_WCDMA_1500_VGA_GAIN_OFFSET_VS_TEMP_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_rise.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_RISE_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_rise_2.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_RISE_2_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_rise_3.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_RISE_3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_rise_4.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_RISE_4_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_rise_5.rfnv_item_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_fall.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_FALL_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_fall_2.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_FALL_2_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_fall_3.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_FALL_3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_fall_4.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_FALL_4_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_fall_5.rfnv_item_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_offset.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_OFFSET_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_offset_2.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_OFFSET_2_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_offset_3.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_OFFSET_3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_offset_4.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_OFFSET_4_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_offset_5.rfnv_item_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_im_level.rfnv_item_id = RFNV_C1_WCDMA_1500_IM_LEVEL_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_im_level_2.rfnv_item_id = RFNV_C1_WCDMA_1500_IM_LEVEL_2_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_im_level_3.rfnv_item_id = RFNV_C1_WCDMA_1500_IM_LEVEL_3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_im_level_4.rfnv_item_id = RFNV_C1_WCDMA_1500_IM_LEVEL_4_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_im2_i_value.rfnv_item_id = RFNV_C1_WCDMA_1500_IM2_I_VALUE_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_im2_q_value.rfnv_item_id = RFNV_C1_WCDMA_1500_IM2_Q_VALUE_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_im2_transconductor_value.rfnv_item_id = RFNV_C1_WCDMA_1500_IM2_TRANSCONDUCTOR_VALUE_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_nonbypass_timer.rfnv_item_id = RFNV_C1_WCDMA_1500_NONBYPASS_TIMER_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_bypass_timer.rfnv_item_id = RFNV_C1_WCDMA_1500_BYPASS_TIMER_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.rfr_vco_coarse_tune_2_1900.rfnv_item_id = RFNV_RFR_VCO_COARSE_TUNE_2_1900_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.rf_drx_mode_sel.rfnv_item_id = RFNV_WCDMA_1500_DRX_MODE_SEL_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_iaccum.rfnv_item_id = RFNV_WCDMA_1500_IACCUM_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_qaccum.rfnv_item_id = RFNV_WCDMA_1500_QACCUM_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.timer_hysterisis.rfnv_item_id = RFNV_WCDMA_1500_TIMER_HYSTERISIS_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.exp_hdet_vs_agc_v2.rfnv_item_id = RFNV_WCDMA_1500_EXP_HDET_VS_AGC_V2_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_c0_c1_delay.rfnv_item_id = RFNV_RFR_WCDMA_C0_C1_DELAY_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_lna_phase_ctl.rfnv_item_id = RFNV_WCDMA_1500_LNA_PHASE_CTL_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_lna_phase_ctl.rfnv_item_id = RFNV_C1_WCDMA_LNA_PHASE_CTL_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rel6_tx_beta_scaling_comp.rfnv_item_id = RFNV_WCDMA_1500_REL6_TX_BETA_SCALING_COMP_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rel6_tx_agc_offset.rfnv_item_id = RFNV_WCDMA_1500_REL6_TX_AGC_OFFSET_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rel6_tx_mpr_backoff.rfnv_item_id = RFNV_WCDMA_1500_REL6_TX_MPR_BACKOFF_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_rxf_mis_comp_a_coeff.rfnv_item_id = RFNV_C1_WCDMA_1500_RXF_MIS_COMP_A_COEFF_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_rxf_mis_comp_b_coeff.rfnv_item_id = RFNV_C1_WCDMA_1500_RXF_MIS_COMP_B_COEFF_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_rxf_ici_i_coef0_1.rfnv_item_id = RFNV_C1_WCDMA_1500_RXF_ICI_I_COEF0_1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_rxf_ici_i_coef2_3.rfnv_item_id = RFNV_C1_WCDMA_1500_RXF_ICI_I_COEF2_3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_rxf_ici_i_coef4_5.rfnv_item_id = RFNV_C1_WCDMA_1500_RXF_ICI_I_COEF4_5_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_rxf_ici_i_coef6_7.rfnv_item_id = RFNV_C1_WCDMA_1500_RXF_ICI_I_COEF6_7_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_rxf_ici_i_coef8_9.rfnv_item_id = RFNV_C1_WCDMA_1500_RXF_ICI_I_COEF8_9_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_rxf_ici_i_coef10_11.rfnv_item_id = RFNV_C1_WCDMA_1500_RXF_ICI_I_COEF10_11_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_rxf_ici_i_coef12_13.rfnv_item_id = RFNV_C1_WCDMA_1500_RXF_ICI_I_COEF12_13_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_rxf_ici_i_coef14_15.rfnv_item_id = RFNV_C1_WCDMA_1500_RXF_ICI_I_COEF14_15_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_rxf_ici_q_coef0_1.rfnv_item_id = RFNV_C1_WCDMA_1500_RXF_ICI_Q_COEF0_1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_rxf_ici_q_coef2_3.rfnv_item_id = RFNV_C1_WCDMA_1500_RXF_ICI_Q_COEF2_3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_rxf_ici_q_coef4_5.rfnv_item_id = RFNV_C1_WCDMA_1500_RXF_ICI_Q_COEF4_5_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_rxf_ici_q_coef6_7.rfnv_item_id = RFNV_C1_WCDMA_1500_RXF_ICI_Q_COEF6_7_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_rxf_ici_q_coef8_9.rfnv_item_id = RFNV_C1_WCDMA_1500_RXF_ICI_Q_COEF8_9_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_rxf_ici_q_coef10_11.rfnv_item_id = RFNV_C1_WCDMA_1500_RXF_ICI_Q_COEF10_11_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_rxf_ici_q_coef12_13.rfnv_item_id = RFNV_C1_WCDMA_1500_RXF_ICI_Q_COEF12_13_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_rxf_ici_q_coef14_15.rfnv_item_id = RFNV_C1_WCDMA_1500_RXF_ICI_Q_COEF14_15_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_rxf_ici_iq_trunc.rfnv_item_id = RFNV_C1_WCDMA_1500_RXF_ICI_IQ_TRUNC_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_rise_car1.rfnv_item_id = RFNV_WCDMA_B11_LNA_RANGE_RISE_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_rise_2_car1.rfnv_item_id = RFNV_WCDMA_B11_LNA_RANGE_RISE_2_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_rise_3_car1.rfnv_item_id = RFNV_WCDMA_B11_LNA_RANGE_RISE_3_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_rise_4_car1.rfnv_item_id = RFNV_WCDMA_B11_LNA_RANGE_RISE_4_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_rise_5_car1.rfnv_item_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_fall_car1.rfnv_item_id = RFNV_WCDMA_B11_LNA_RANGE_FALL_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_fall_2_car1.rfnv_item_id = RFNV_WCDMA_B11_LNA_RANGE_FALL_2_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_fall_3_car1.rfnv_item_id = RFNV_WCDMA_B11_LNA_RANGE_FALL_3_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_fall_4_car1.rfnv_item_id = RFNV_WCDMA_B11_LNA_RANGE_FALL_4_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_fall_5_car1.rfnv_item_id = RFNV_ITEMS_ENUM_MAX;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_offset_car1.rfnv_item_id = RFNV_WCDMA_B11_LNA_RANGE_OFFSET_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_offset_2_car1.rfnv_item_id = RFNV_WCDMA_B11_LNA_RANGE_OFFSET_2_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_offset_3_car1.rfnv_item_id = RFNV_WCDMA_B11_LNA_RANGE_OFFSET_3_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_offset_4_car1.rfnv_item_id = RFNV_WCDMA_B11_LNA_RANGE_OFFSET_4_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.lna_range_offset_5_car1.rfnv_item_id = RFNV_WCDMA_B11_LNA_RANGE_OFFSET_5_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.vga_gain_offset_car1.rfnv_item_id = RFNV_WCDMA_B11_VGA_GAIN_OFFSET_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_rise_car1.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_RISE_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_rise_2_car1.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_RISE_2_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_rise_3_car1.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_RISE_3_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_rise_4_car1.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_RISE_4_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_rise_5_car1.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_RISE_5_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_fall_car1.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_FALL_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_fall_2_car1.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_FALL_2_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_fall_3_car1.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_FALL_3_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_fall_4_car1.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_FALL_4_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_fall_5_car1.rfnv_item_id = RFNV_C1_WCDMA_1500_LNA_RANGE_FALL_5_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_offset_car1.rfnv_item_id = RFNV_C1_WCDMA_B11_LNA_RANGE_OFFSET_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_offset_2_car1.rfnv_item_id = RFNV_C1_WCDMA_B11_LNA_RANGE_OFFSET_2_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_offset_3_car1.rfnv_item_id = RFNV_C1_WCDMA_B11_LNA_RANGE_OFFSET_3_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_offset_4_car1.rfnv_item_id = RFNV_C1_WCDMA_B11_LNA_RANGE_OFFSET_4_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_lna_range_offset_5_car1.rfnv_item_id = RFNV_C1_WCDMA_B11_LNA_RANGE_OFFSET_5_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_vga_gain_offset_car1.rfnv_item_id = RFNV_C1_WCDMA_B11_VGA_GAIN_OFFSET_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rxf_ici_cal_data_car0.rfnv_item_id = RFNV_WCDMA_B11_RXF_ICI_CAL_DATA_CAR0_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_rxf_ici_cal_data_car1.rfnv_item_id = RFNV_WCDMA_B11_RXF_ICI_CAL_DATA_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_rxf_ici_cal_data_car0.rfnv_item_id = RFNV_C1_WCDMA_B11_RXF_ICI_CAL_DATA_CAR0_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_wcdma_rxf_ici_cal_data_car1.rfnv_item_id = RFNV_C1_WCDMA_B11_RXF_ICI_CAL_DATA_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.wcdma_db10_sar_back_off_limit.rfnv_item_id = RFNV_WCDMA_B11_DB10_SAR_BACK_OFF_LIMIT;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.ant_tuner.rfnv_item_id = RFNV_WCDMA_B2_ANT_TUNER_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.pa_comp_up_r1_vs_freq.rfnv_item_id = RFNV_WCDMA_1500_PA_COMPENSATE_UP_R1_VS_FREQ_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.pa_comp_up_r2_vs_freq.rfnv_item_id = RFNV_WCDMA_1500_PA_COMPENSATE_UP_R2_VS_FREQ_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.pa_comp_up_r3_vs_freq.rfnv_item_id = RFNV_WCDMA_1500_PA_COMPENSATE_UP_R3_VS_FREQ_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.pa_comp_dn_r1_vs_freq.rfnv_item_id = RFNV_WCDMA_1500_PA_COMPENSATE_DOWN_R1_VS_FREQ_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.pa_comp_dn_r2_vs_freq.rfnv_item_id = RFNV_WCDMA_1500_PA_COMPENSATE_DOWN_R2_VS_FREQ_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.pa_comp_dn_r3_vs_freq.rfnv_item_id = RFNV_WCDMA_1500_PA_COMPENSATE_DOWN_R3_VS_FREQ_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_gtr_thresh.rfnv_item_id = RFNV_WCDMA_1500_TX_GTR_THRESH_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_ept_dpd_config_params.rfnv_item_id = RFNV_WCDMA_1500_TX_EPT_DPD_CONFIG_PARAMS_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.et_delay_value.rfnv_item_id = RFNV_WCDMA_1500_TX_ET_DELAY_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.mtpl_vs_freq.rfnv_item_id = RFNV_WCDMA_1500_MTPL_VS_FREQ_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.hdet_vs_temp.rfnv_item_id = RFNV_WCDMA_1500_HDET_VS_TEMP_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.hdet_mod_range.rfnv_item_id = RFNV_WCDMA_1500_HDET_MOD_RANGE_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_lin_vs_temp_0.rfnv_item_id = RFNV_WCDMA_1500_TX_LIN_VS_TEMP_0_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_lin_vs_temp_1.rfnv_item_id = RFNV_WCDMA_1500_TX_LIN_VS_TEMP_1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_lin_vs_temp_2.rfnv_item_id = RFNV_WCDMA_1500_TX_LIN_VS_TEMP_2_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_lin_vs_temp_3.rfnv_item_id = RFNV_WCDMA_1500_TX_LIN_VS_TEMP_3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_lin_vs_temp_0_addl.rfnv_item_id = RFNV_WCDMA_B11_TX_LIN_VS_TEMP_0_ADDL_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_lin_vs_temp_1_addl.rfnv_item_id = RFNV_WCDMA_B11_TX_LIN_VS_TEMP_1_ADDL_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_lin_vs_temp_2_addl.rfnv_item_id = RFNV_WCDMA_B11_TX_LIN_VS_TEMP_2_ADDL_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_lin_vs_temp_3_addl.rfnv_item_id = RFNV_WCDMA_B11_TX_LIN_VS_TEMP_3_ADDL_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_lin_vs_temp_vs_freq_p_in.rfnv_item_id = RFNV_WCDMA_1500_TX_LIN_VS_TEMP_VS_FREQ_P_IN_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_lin_vs_temp_vs_freq_p_out.rfnv_item_id = RFNV_WCDMA_1500_TX_LIN_VS_TEMP_VS_FREQ_P_OUT_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_xpt_delay.rfnv_item_id = RFNV_WCDMA_1500_TX_XPT_DELAY_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_delay_offsets.rfnv_item_id = RFNV_WCDMA_1500_TX_ET_DELAY_OFFSETS_VS_FREQ_VS_TEMP_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.ant_tuner_cl_ctrl_info.rfnv_item_id = RFNV_WCDMA_C0_B11_ANT_TUNER_CL_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.dpd_scaling_data.rfnv_item_id = RFNV_WCDMA_1500_TX_XPT_DPD_SCALING_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.rx_static_data.rfnv_item_id = RFNV_WCDMA_1500_RX_STATIC_DATA_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.rx_dyn_data.rfnv_item_id = RFNV_WCDMA_1500_RX_CAL_DATA_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.noise_floor.rfnv_item_id = RFNV_WCDMA_1500_NOISE_FLOOR_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.noise_floor_car1.rfnv_item_id = RFNV_WCDMA_1500_NOISE_FLOOR_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_noise_floor_car0.rfnv_item_id = RFNV_C1_WCDMA_1500_NOISE_FLOOR_CAR0_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_noise_floor_car1.rfnv_item_id = RFNV_C1_WCDMA_1500_NOISE_FLOOR_CAR1_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.pin_cal_vs_freq.rfnv_item_id = RFNV_WCDMA_1500_PIN_CAL_VS_FREQ_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_xpt_dc_delay.rfnv_item_id = RFNV_WCDMA_1500_TX_XPT_DC_DELAY_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.mpr_backoff.rfnv_item_id = RFNV_WCDMA_1500_MPR_BACKOFF_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.dc_tx_lin_vs_temp_vs_freq_p_in.rfnv_item_id = RFNV_DC_WCDMA_1500_TX_LIN_VS_TEMP_VS_FREQ_P_IN_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.dc_tx_lin_vs_temp_vs_freq_p_out.rfnv_item_id = RFNV_DC_WCDMA_1500_TX_LIN_VS_TEMP_VS_FREQ_P_OUT_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.tx_dc_delay_offsets.rfnv_item_id = RFNV_WCDMA_1500_TX_ET_DC_DELAY_OFFSETS_VS_FREQ_VS_TEMP_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.pa_static.rfnv_item_id = RFNV_WCDMA_C0_1500_PA_STATIC_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.pa_static_z.rfnv_item_id = RFNV_WCDMA_C0_1500_PA_STATIC_Z_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.dc_pa_static.rfnv_item_id = RFNV_WCDMA_C0_DC_1500_PA_STATIC_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.dc_pa_static_z.rfnv_item_id = RFNV_WCDMA_C0_DC_1500_PA_STATIC_Z_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.rx_static_data_v2.rfnv_item_id = RFNV_WCDMA_1500_RX_STATIC_DATA_V2_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.rx_dyn_data_v2.rfnv_item_id = RFNV_WCDMA_1500_RX_CAL_DATA_V2_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.ant_tuner_car1.rfnv_item_id = RFNV_WCDMA_C1_B11_ANT_TUNER_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c0_rx_static_data_v3.rfnv_item_id = RFNV_WCDMA_C0_1500_RX_STATIC_DATA_V3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c1_rx_static_data_v3.rfnv_item_id = RFNV_WCDMA_C1_1500_RX_STATIC_DATA_V3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c2_rx_static_data_v3.rfnv_item_id = RFNV_WCDMA_C2_1500_RX_STATIC_DATA_V3_I;
  rfnv_wcdma_1500_nv_items_list.nv_items_type.c3_rx_static_data_v3.rfnv_item_id = RFNV_WCDMA_C3_1500_RX_STATIC_DATA_V3_I;
}

