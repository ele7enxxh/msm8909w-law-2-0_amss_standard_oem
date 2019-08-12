#ifndef RFNV_WCDMA_H
#define RFNV_WCDMA_H

/*!
  @file
  rfnv_wcdma.h

  @brief

  @details
  This header file contains definitions  to interface with RF NV for WCDMA.

*/

/*===========================================================================

Copyright (c) 2008 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/rfnv_wcdma.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/14   ac      tuner support for DBDC
10/17/14   weiz	   Support of PA static compressed NV
09/26/14   kai     Added support for WCDMA Rx V2 NVs
09/20/14   zhh     fix KW warnings
05/01/14   aro     NV item to hold char data for DC ET Delay vs freq vs temp
02/26/14   zhh    Update XPT Delay structure to support DC-UPA
04/14/14   zhh     PinPout NV for DCUPA
03/20/14   zhh     update new MPR backoff NV
04/20/14   bn      WCDMA 8 PA state support and related cleanup
02/21/14   rmb     Add global NV pointers for each of band specific static NVs.
01/15/14   rmb     Add NV support for Noise Floor Correction.
11/25/13   rmb     Add Pin Cal versus freq related NVs for all PA states and bands.
01/03/13   dw      Added rfnv_wcdma_get_nv_tbl_ptr prototype
09/20/13   dw      Replace rfcom_multi_carrier_hspa_id_type with rfcom_multi_carrier_id_type 
                   for carrier type indexin
08/27/13   vs      Set Rx static and Cal NVs to default values
08/23/13   vs      Added Rx static and Cal NVs
08/12/13   kai     Added DPD scaling support
07/24/13   kai     Added XPT path delay offset vs temp vs freq
07/16/13   vs      Added CL tuner NVs
07/03/13   kcj     Dynamically allocated NV struct memory. 
06/23/13   kai     Added WCDMA 16channels xpt delay NVs
05/23/13   yb      Fixing the int16 array accesses and buffering of for 
                   pa_comp_up/down_vs_freq NVs
04/30/13   cd      Add NV loading support for Lin vs Temp items into common
                   TxLin data types
03/26/13   cd      Fixed legacy lin vs temp NV loading into new data structs
03/20/13   cd      Add support for loading new lin vs temp vs freq Pin and 
                   Pout NV items
03/19/13   dw/kai  [1]Added support for mtpl_vs_freq  
                   [2]Added support to limit allowable HDET mod range   
03/14/13   kai     Added ET delay NV support
09/17/12   ka      Update default BTF
06/12/12   id      Add support to get HDET Vs AGC NV Item Id per tech and band
06/12/12   id      Added define for WCDMA Max Power
06/04/12   cri     Add EPT support for WCDMA
05/14/12   dj      Add chan to freq conversion handler 
04/17/12   gh      Add TX_GTR_THRESH support
04/10/12   id      Added support for fetching Tx Multi Lin global structure 
                   and freq to chan conversion handler
03/06/12   gh      16-channel Tx lin_vs_temp feature
11/18/11   ac      Enable PA Comp Up/Down to vary with frequency
09/07/11   gh      Add Ant Tuner support
05/02/11   ad      Added WCDMA second sweep SMPS PA bias items
04/21/11   ad      Fix KW errors 
04/11/11   sar     Added prototype for rfnv_wcdma_convert_band_rfcom_to_rfnv.
03/03/11   ka      Added SAR support 
02/24/11   ad      Connected SMPS NV to RFC structure for WCDMA
02/14/11   ac      APT nv support
01/31/11   ac      GST freq/temp comp bringup
10/29/10   ka      Updates for initial WCDMA APT functionality  
10/14/10   mkv/whc Integrating Reload RFNV from SCMM into MDM9x00 (CL 1315259)
10/11/10   ad      Added preliminary support for WCDMA APT Phase 1  
09/30/10   ac      EFS support
05/19/10   ka      Correct NV item names for ICI    
05/12/10   ac      B11 bringup
04/06/10   ad      Support for reading DC items from NV	
01/25/10   ad      Added Drx LNA offset for G5
01/21/10   ka      Merge DC changes from 9K.
01/12/10   kguo    Merged from mdm9k dev branch to main\latest   
11/18/09   ac      added lNA range offset 5
11/11/09   ka      Updates for dual carrier.
10/01/09   ckl     Added rfwcdma_nv_retrive_items( )
08/12/09    xw     Added RTR VCO coarse tune NV support.
03/18/09   ckl     Delete common enums and change RFNV_SUPPORTED_MODE_NUM to
                   RFNV_WCDMA_SUPPORTED_MODE_NUM in rfnv_wcdma_supported_mode_type
03/17/09   ckl     Modified compile options for SCMM baseline
03/15/09   ckl     Delete rfnv.h and rficap.h
03/09/09   ckl     Code cleanup
08/22/08   ka      Added ICI and IQM NV.
12/05/07   bn      EUL Phase 2 NV support (release 6)
10/10/07   ad      Add lna_phase_ctl variables to rfnv_wcdma_nv_tbl_type
08/09/07   ad      Added support for reading HS LNA switch point NVs during HS calls.
08/07/07   ad      From 6260: Fix for retaining Temp Comp when UTRAN max pwr < NV max pwr.
08/07/07   jfc/ka  Added support for tx secondary freq comp
06/11/07   ad      Added Cx16 wcdma_c0_c1_delay to wcdma nv table type.
01/25/07   lcl     Merge 5th LNA gain state.
05/16/06    dp     Remove obsolete version of diversity antenna select NV structure
04/18/06   lcl     Added DC accumulator value to NV structure.
04/07/06   rsr     Added support to read diversity mode select values from NV.
01/18/06   bhas    Add Rx diversity path NV items to NV structure.
01/12/06    ka     Added tx pdm lists to nv structure.
05/23/05   jfc     Added support for RF_UTRAN_TX_LIM_VS_TEMP_COMP.
03/25/05    bn     Max Power Backoff feature NV support
05/09/05    rv     pa_compensate_* variables changed to int16 to match NV.
03/17/05    bn     HS switch point NV support
03/04/05    bn     ModUpdate and tx_adj_pdm_delay NV support
02/23/05    rv     Merged Raven changes back to mainline.
11/16/04    bn     4-Page Linearizer Support in RF driver
09/23/04   dhh     Linear interpolation optimization.
09/01/04   dhh     Added support for Freq Comp linear interpolation and PRACH
                   PA range.
07/01/04    rv     Moved RFR 6250 NV items to common NV.
05/27/04   xw/rv   GWZRF6250P3 support.
03/10/04   dhh     Added NV items for paCompensateUp/paCompensateDown.
03/01/04   dhh     Provided interface for WL1 to get tx rotator angle from RFNV.
02/23/04   dhh     Added NV to support pa range update time.
01/19/04   dhh     Added tx delays/timing NV items.
08/21/03   ap      Final modifications to support WCDMA 2100/1900
06/17/03   eh      Initial Revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "nv.h"
#include "rfcommon_nv.h"
#include "rfwcdma_core_txlin.h"
#include "rfc_card.h"
#include "rfcom.h"
#include "rfcommon_core_txlin.h"
#include "rfcommon_core_xpt.h"
#include "rfumts.h"
#include "rfnv_item_types.h"


/*===========================================================================

                           CONSTANT DEFINITIONS

===========================================================================*/
/* Constant to make rfnv_wcdma_tx_lim_vs_temp[] back to 10-bits number. */
/* To save NV space, rfnv_wcdma_tx_lim_vs_temp[] is scaled to 8-bits */
/* since TX power limiting is only done on the top 25% of TX power */
#define RFNV_WCDMA_TX_LIM_OFFSET  768

#define RFNV_PATHS_TABLE_SIZ      2

#define RF_MAX_LNA_GAIN_STATES 6

#define RFWCDMA_NV_MDSP_BTFDELAY_VAL          -294

#define RFWCDMA_NV_RX_DELAY_VAL               2
#define RFWCDMA_NV_1900_RX_DELAY_VAL          2
#define RFWCDMA_NV_BC3_RX_DELAY_VAL           2
#define RFWCDMA_NV_BC4_RX_DELAY_VAL           2
#define RFWCDMA_NV_800_RX_DELAY_VAL           2
#define RFWCDMA_NV_BC8_RX_DELAY_VAL           2
#define RFWCDMA_NV_BC9_RX_DELAY_VAL           2
#define RFWCDMA_NV_BC11_RX_DELAY_VAL          2
#define RFWCDMA_SAR_BACK_OFF_NV_SIZE 8

#define RFWCDMA_MAX_PWR 323

/* The temperature compensation uses interpolated tables of size
   NV_TEMP_TABLE_SIZ.  The temperature value from the therm services
   is scaled 0..THERM_SCALE.  The bin size between the entries in the
   table is RFWCDMA_NV_SINGLE_TEMP_BIN_SIZE.  Note that this is a scalar, and 
   the result of the division can make 1 bin smaller than 
   RFWCDMA_NV_SINGLE_TEMP_BIN_SIZE */
#define RFWCDMA_NV_SCALED_THERM_MIN 0
#define RFWCDMA_NV_SCALED_THERM_MAX 255
#define RFWCDMA_NV_SINGLE_TEMP_BIN_SIZE \
                       (RFWCDMA_NV_SCALED_THERM_MAX/(NV_TEMP_TABLE_SIZ-1))
#define RFWCDMA_XPT_PATH_DELAY_ARRAY_SIZE     16
#define RFWCDMA_RX_DEFAULT_REF_CHAN_INDEX 7

#define RFWCDMA_NUM_LNA_STATES 5

/* Default Pin versus freq value for RFNV - 
This indicates Pin is not calibrated for that particular PA state*/
#define RFNV_WCDMA_DEFAULT_PIN_CAL_VS_FREQ 32767

/*===========================================================================

                       PUBLIC DATA DECLARATIONS

===========================================================================*/

/*----------------------------------------------------------------------------*/
/*! 
  @brief  
  TX_GTR_THRESH NV structure
*/
typedef PACK (struct)
{
  uint8 active;                   /* Indicator of whether it is active or not  */
  int16 rise_value;               /* Units of dB10 */
  int16 fall_value;               /* Units of dB10 */
  int16 index;                    /* Index of which of the 3 tuplets to be used*/
}rfwcdma_nv_tx_gtr_thresh_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief  
  Tx Lin vs Temp vs Freq Pin/Pout NV structure
*/
typedef PACK (struct)
{
  /*! Lin vs Temp vs Freq NV information */
  uint8 reserved;
  int16 lin_vs_temp_vs_freq[RFLIN_NUM_TX_LIN_LEGACY_4PA]
                           [NV_TEMP_TABLE_SIZ]
                           [NV_FREQ_TABLE_SIZ];
}rfwcdma_nv_tx_lin_vs_temp_vs_freq_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief  
  XPT path delay NV information
*/
typedef PACK(struct)
{
  uint8 num_of_active_entries;
  uint16 upper_bound_tx_chan[RFWCDMA_XPT_PATH_DELAY_ARRAY_SIZE];
  int32 delay_value[RFWCDMA_XPT_PATH_DELAY_ARRAY_SIZE];
}rfwcdma_nv_xpt_path_delay_type;

typedef PACK(struct)
{
  uint8 num_of_active_entries;
  uint16 upper_bound_tx_chan[RFWCDMA_XPT_PATH_DELAY_ARRAY_SIZE];
  int32 dc_delay[RFWCDMA_XPT_PATH_DELAY_ARRAY_SIZE];
}rfwcdma_nv_xpt_dc_delay_type;


typedef PACK (struct)
{
  uint16 tx_beta_scaling_comp[8];
  uint16 tx_agc_offset[8];
  uint16 tx_mpr_backoff[8];
  uint16 pa_mpr_backoff[7];
}rfnv_wcdma_mpr_backoff_st_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief  
  MPR backoff NV information
*/
typedef PACK(struct)
{
  uint8 mpr_table_entries;
  rfnv_wcdma_mpr_backoff_st_type sc_backoff;
  rfnv_wcdma_mpr_backoff_st_type dc_backoff;
}rfwcdma_nv_mpr_backoff_type;


typedef enum
{
  RFNV_WCDMA_2100,
  RFNV_WCDMA_1900,
  RFNV_WCDMA_1800,
  RFNV_WCDMA_1500,
  RFNV_WCDMA_BC4,
  RFNV_WCDMA_900,
  RFNV_WCDMA_800,
  RFNV_WCDMA_2100_RXDIV,
  RFNV_WCDMA_1900_RXDIV,
  RFNV_WCDMA_1800_RXDIV,
  RFNV_WCDMA_1500_RXDIV,  
  RFNV_WCDMA_BC4_RXDIV,
  RFNV_WCDMA_900_RXDIV,
  RFNV_WCDMA_800_RXDIV,

  /* add additional mode enum field here */

  RFNV_WCDMA_SUPPORTED_MODE_NUM
} rfnv_wcdma_supported_mode_type;

/*----------------------------------------------------------------------------*/
typedef enum
{
  RFWCDMA_NV_RX_STATIC_DATA_V1 = 1,
  RFWCDMA_NV_RX_STATIC_DATA_V2 = 2,
  RFWCDMA_NV_RX_STATIC_DATA_MAX
}rfwcdma_nv_rx_static_version_type;

typedef enum
{
  RFWCDMA_NV_RX_DYNAMIC_DATA_V1 = 1,
  RFWCDMA_NV_RX_DYNAMIC_DATA_V2 = 2,
  RFWCDMA_NV_RX_DYNAMIC_DATA_MAX
}rfwcdma_nv_rx_dyn_version_type;

typedef enum
{
  RX_CAL_CHAN_LIST,
  VGA_OFFSET,
  LNA_OFFSET,
  VGA_OFFSET_VS_FREQ,
  LNA_OFFSET_VS_FREQ_1,
  LNA_OFFSET_VS_FREQ_2,
  LNA_OFFSET_VS_FREQ_3,
  LNA_OFFSET_VS_FREQ_4
}rfwcdma_nv_rx_dynamic_element_type;

typedef enum
{
  LNA_RISE,
  LNA_FALL,
  LNA_HS_RISE,
  LNA_HS_FALL
}rfwcdma_nv_rx_static_element_type;

typedef enum 
{
  /*!> Use legacy NVs for PA static items */
  RFWCDMA_NV_LEGACY_FOUR_STATE_PA = 0,

  /*!> CMM PA static NV Version 1 */
  RFWCDMA_NV_PA_STATIC_VER_1 = 1,                      

  /*!> MAX Num of PA static NV version */
  RFWCDMA_NV_PA_STATIC_VER_MAX,

  /*!> Invalid */  
  RFWCDMA_NV_PA_STATIC_VER_INVALID

}rfwcdma_nv_pa_static_ver_type;

typedef struct
{
  /* VGA gain offset */
  int16 vga_offset;

  /* LNA offsets */
  int16 lna_offsets[RFWCDMA_NUM_LNA_STATES];

}rfwcdma_nv_rx_offset_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   LNA related NV items

   LNA rise/fall (dBm to MSM register & NV storage)

      [ ( (dBm Rx + 106) / 85.333 ) * 1024 ] - 512
  
   LNA rise/fall (MSM register & NV storage to dBm)

      [ ( (value + 512) / 1024 ) * 85.333 ] - 106
 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
typedef struct
{
  /* LNA Rise points */
  int16 lna_rise[RFWCDMA_NUM_LNA_STATES];

  /* LNA Fall points */
  int16 lna_fall[RFWCDMA_NUM_LNA_STATES];

  /* LNA Rise points */
  int16 lna_hs_rise[RFWCDMA_NUM_LNA_STATES];

  /* LNA Fall points */
  int16 lna_hs_fall[RFWCDMA_NUM_LNA_STATES];

}rfwcdma_nv_rx_lna_switchpoints_type;

typedef struct
{
  /* Rx LNA rise/fall switchpoints per chain and per carrier */
  rfwcdma_nv_rx_lna_switchpoints_type rx_lna_swpts[RF_PATHS_TABLE_SIZ][RFCOM_MAX_CARRIERS] ;
  
}rfwcdma_nv_rx_static_payload_v1_type;

typedef struct
{
  /* Rx LNA rise/fall switchpoints per chain and per carrier */
  rfwcdma_nv_rx_lna_switchpoints_type rx_lna_swpts[RFM_DEVICE_4][RFCOM_MAX_CARRIERS] ;
  
}rfwcdma_nv_rx_static_payload_v2_type;

typedef union
{
  rfwcdma_nv_rx_static_payload_v1_type rx_stat_v1;
  rfwcdma_nv_rx_static_payload_v2_type rx_stat_v2;

}rfwcdma_nv_rx_static_payload_type;

typedef struct
{
   /*Variant Marker*/
  rfnv_data_variant_marker_type rx_stat_variant_marker;

  /* Static Payload type */
  rfwcdma_nv_rx_static_payload_type payload;

}rfwcdma_nv_rx_static_data_type;

typedef struct
{
   /* VGA offset vs freq compensation */
  int8 vga_gain_offset_vs_freq;
  
  /* LNA bypass range offset vs. freq. compensation */
  int8 lna_offset_vs_freq_1;
  
  /* LNA bypass range offset vs. freq. compensation */
  int8 lna_offset_vs_freq_2;
  
  /* LNA bypass range offset vs. freq. compensation */
  int8 lna_offset_vs_freq_3;
  
  /* LNA bypass range offset vs. freq. compensation */
  int8 lna_offset_vs_freq_4;

}rfwcdma_nv_rx_offset_vs_freq_type;

typedef struct
{
  /* Rx Calibration channel list for path 0 and 1 */
  uint16 rx_cal_chan[RFNV_PATHS_TABLE_SIZ][NV_FREQ_TABLE_SIZ];

  /* Offset Vs Freq */
  rfwcdma_nv_rx_offset_vs_freq_type freq_offset[RFNV_PATHS_TABLE_SIZ][NV_FREQ_TABLE_SIZ];

  /* Rx LNA and VGA offsets per chain and per carrier */
  rfwcdma_nv_rx_offset_type rx_offsets[RF_PATHS_TABLE_SIZ][RFCOM_MAX_CARRIERS];
  
}rfwcdma_nv_rx_dynamic_payload_v1_type;

typedef struct
{
  /* Rx Calibration channel list for Device 0,1,2,3 */
  uint16 rx_cal_chan[RFM_DEVICE_4][NV_FREQ_TABLE_SIZ];

  /* Offset Vs Freq */
  rfwcdma_nv_rx_offset_vs_freq_type freq_offset[RFM_DEVICE_4][NV_FREQ_TABLE_SIZ];

  /* Rx LNA and VGA offsets per chain and per carrier */
  rfwcdma_nv_rx_offset_type rx_offsets[RFM_DEVICE_4][RFCOM_MAX_CARRIERS];
  
}rfwcdma_nv_rx_dynamic_payload_v2_type;

typedef union
{
  rfwcdma_nv_rx_dynamic_payload_v1_type rx_dyn_v1;
  rfwcdma_nv_rx_dynamic_payload_v2_type rx_dyn_v2;
}rfwcdma_nv_rx_dynamic_payload_type;

typedef struct
{
  /*Variant Marker*/
  rfnv_data_variant_marker_type rx_dyn_variant_marker;

  /* Dynamic Payload type */
  rfwcdma_nv_rx_dynamic_payload_type payload;
  
}rfwcdma_nv_rx_dynamic_data_type;
 
/*----------------------------------------------------------------------------*/

/* WCDMA NV calibration table */
typedef struct
{
  /* IM threshold level for LNA to switch from g0 to g1 gain state */
  int16 im_hs_level;

  /* IM threshold level for LNA to switch from g1 to g2 gain state */
  int16 im_hs_level_2;

  /* IM threshold level for Mixer to switch from g2 to g3 gain state */
  int16 im_hs_level_3;


  /* IM threshold level for LNA to switch from g3 to g4 gain state */
  int16 im_hs_level_4;

  int16 vga_gain_offset_vs_temp[NV_TEMP_TABLE_SIZ];

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   IM2 NV items
   The description of these NV items can be found in the WCDMA ZIF RF NV items
   document.
  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  uint8 im2_i_value;

  uint8 im2_q_value;

  uint8 im2_transconductor_value;

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
     Tx power limit NV items
     Tx power limit data vs temperature, Tx power limit vs frequency,
     TX_AGC_ADJ vs HDET, and HDET offset and span
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Tx power limit versus temperature */
  /* This table contain absolute TX power limit, NOT DELTA */
  int16 tx_lim_vs_temp[NV_TEMP_TABLE_SIZ];

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

  uint8 rtr_vco_coarse_tuning[16];

  /* PRACH Switch point from Linearizer gain state 0 to 1 */
  int16 prach_r1_rise_offset;

  /* PRACH Switch point from Linearizer gain state 1 to 0 */
  int16 prach_r1_fall_offset;

  /* Cal channel list, reserve RF chain index for future use. 1 chain used. */
  uint16 tx_cal_chan[RFNV_PATHS_TABLE_SIZ][NV_FREQ_TABLE_SIZ];

  uint32 tx_cal_chan_freq[RFNV_PATHS_TABLE_SIZ][NV_FREQ_TABLE_SIZ];

  boolean interpolation_enable;

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

  /* diversity path  rx agc offset for each vga gain based on temperature */
  int16      c1_vga_gain_offset_vs_temp[NV_TEMP_TABLE_SIZ];

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

  uint16    wcdma_rel6_tx_agc_offset[7];

  uint16    wcdma_rel6_tx_mpr_backoff[7];

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

  rfcom_txlin_temp_comp_nv_data_type lin_temp_comp_data;

  /* TX_GTR_THRESH NV */
  rfwcdma_nv_tx_gtr_thresh_type tx_gtr_thresh;

  /* EPT DPD static params for expansion and processing */
  uint32 tx_ept_dpd_config_params[MAX_EPT_NUM_CONFIG_PARAMS];

 /* ET path delay value*/
  int32 et_delay_value;

  /* Max Transmit Power Limit Vs Frequency */
  int8 mtpl_vs_freq[NV_FREQ_TABLE_SIZ];

  /* HDET offset Vs Temperature */
  int16 hdet_vs_temp[NV_TEMP_TABLE_SIZ];

  /* Allowable HDET correction range from static temp comp*/
  int16 hdet_mod_range[2][NV_TEMP_TABLE_SIZ];

  /* XPT path 16channels delay info */
  rfwcdma_nv_xpt_path_delay_type xpt_path_delay;

  /* XPT path 16channels DC delay info */
  rfwcdma_nv_xpt_dc_delay_type xpt_dc_delay;

  /* XPT path delay offset vs temp vs freq */
  rfnv_tx_delay_offsets_type tx_delay_offsets;

  /* XPT path delay offset vs temp vs freq */
  rfnv_tx_delay_offsets_type tx_dc_delay_offsets;
  
  /* CL antenna tuner control info*/
  rfcommon_nv_ant_tuner_cl_ctrl_type ant_tuner_cl_ctrl_info;
  
  /* DPD scaling data */
  xpt_dpd_scaling_data_type dpd_scaling_data;

  /* Static Rx data - R99/HS LNA rise/fall points */
  rfwcdma_nv_rx_static_data_type rx_static_data;

  /* Dynamic Rx data - LNA/DVGA offsets, etc */
  rfwcdma_nv_rx_dynamic_data_type rx_dyn_data;

  /* Noise floor level for PRx/Carrier 0 in 0.25dB */
  int16 noise_floor;

  /* Noise floor level for PRx/Carrier 1 in 0.25dB */
  int16 noise_floor_car1;

  /* Noise floor level for DRx/Carrier 0 in 0.25dB */
  int16 c1_noise_floor_car0;

  /* Noise floor level for DRx/Carrier 1 in 0.25dB */
  int16 c1_noise_floor_car1;

   /* MPR backoff valuee */
  rfwcdma_nv_mpr_backoff_type mpr_backoff;

  rfcom_txlin_temp_comp_nv_data_type dc_lin_temp_comp_data;

  rfcommon_nv_cmn_pa_static_type pa_static;

  rfcommon_nv_cmn_pa_static_type dc_pa_static;

  rfcommon_nv_ant_tuner_type ant_tuner_car1;

} rfnv_wcdma_nv_tbl_type;

typedef struct
{
  rfnv_item_id_enum_type hdet_vs_agc_nv;
}rfnv_wcdma_hdet_vs_agc_nv_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief  
  Pin Cal versus Freq NV structure
*/
typedef PACK (struct)
{
  /*! Pin Cal versus Freq NV information */
  int16 pin_cal_vs_freq[RFCOM_TXLIN_NUM_PA_STATES]
                       [RFCOM_TXLIN_NUM_CHANNELS];
}rfnv_wcdma_pin_cal_vs_freq_type;

/* pointer to WCDMA NV table */
extern rfnv_wcdma_nv_tbl_type   *rfnv_wcdma_tbl_ptr[RFCOM_NUM_WCDMA_BANDS];

/*===========================================================================

                         FUNCTION PROTOTYPES

===========================================================================*/

/*--------------------------------------------------------------------------*/
void *rfnv_wcdma_cmn_item_table(void);

/*--------------------------------------------------------------------------*/
uint16 rfnv_wcdma_cmn_list_size( void );

/*--------------------------------------------------------------------------*/
void rfnv_wcdma_cmn_copy_nv_item
(
  rfnv_item_id_enum_type  item_code,    
  rfnv_item_type          *rfnv_item_ptr   
);

/* ----------------------------------------------------------------------- */
rfnv_wcdma_supported_mode_type rfnv_wcdma_convert_band_rfcom_to_rfnv 
(
  rfcom_wcdma_band_type band
);

/* ----------------------------------------------------------------------- */
boolean rfnv_wcdma_get_txlin_cal_data 
(
  uint8 band,
  uint8 device, 
  rfnv_item_id_enum_type* item_code,
  tx_band_cal_type **tx_band_cal_data
);

/* ----------------------------------------------------------------------- */
rfcommon_core_convert_chan_handler_type 
rfnv_wcdma_get_tx_freq_to_chan_handler
(
  uint8 band
);

/* ----------------------------------------------------------------------- */
rfcommon_core_convert_chan_handler_type
rfnv_wcdma_get_chan_to_tx_freq_handler
(
  uint8 band
);

/* ----------------------------------------------------------------------- */
boolean 
rfnv_wcdma_get_hdet_vs_agc_nvid
( 
  uint8 band,
  uint8 device,
  rfnv_item_id_enum_type* item_code
);

/* ----------------------------------------------------------------------- */
void 
rfnv_wcdma_load_legacy_lin_vs_temp_offset
(
  rfnv_wcdma_nv_tbl_type *nv_tbl_ptr,
  rfnv_item_type         *rfnv_item_ptr,
  uint8 pa_state
);

/* ----------------------------------------------------------------------- */
void 
rfnv_wcdma_load_legacy_lin_vs_temp_vs_freq_delta
(
  rfnv_wcdma_nv_tbl_type *nv_tbl_ptr,
  rfnv_item_type         *rfnv_item_ptr,
  uint8 pa_state
);

/* ----------------------------------------------------------------------- */
void 
rfnv_wcdma_load_lin_vs_temp_tx_char_chan
(
  rfnv_wcdma_nv_tbl_type *nv_tbl_ptr,
  rfnv_item_type         *rfnv_item_ptr,
  rfcom_wcdma_band_type  band
);

/* ----------------------------------------------------------------------- */
void
rfnv_wcdma_load_equidistant_therm_bins_data
(
  rfnv_wcdma_nv_tbl_type *nv_tbl_ptr
);

/* ----------------------------------------------------------------------- */
void 
rfnv_wcdma_load_lin_vs_temp_vs_freq
(
  rfnv_wcdma_nv_tbl_type *nv_tbl_ptr,
  boolean                is_pin
);
/* ----------------------------------------------------------------------- */
void rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv
(
  rfcom_wcdma_band_type band,
  rfm_device_enum_type device_id,
  rfcom_multi_carrier_id_type carr_id,
  rfwcdma_nv_rx_dynamic_element_type element,
  uint8 index,
  int16 val,
  rfnv_wcdma_nv_tbl_type *rfnv_item_ptr
);
/* ----------------------------------------------------------------------- */
void rfnv_wcdma_copy_rx_static_data_from_legacy_nv
(
  rfcom_wcdma_band_type band,
  rfm_device_enum_type device_id,
  rfcom_multi_carrier_id_type carr_id,
  rfwcdma_nv_rx_static_element_type element,
  uint8 index,
  int16 val,
  rfnv_wcdma_nv_tbl_type *rfnv_item_ptr
);
/* ----------------------------------------------------------------------- */
int16 rfnv_wcdma_get_info_from_rx_dynamic_data
(
  rfm_device_enum_type device_id,
  rfcom_multi_carrier_hspa_id_type carr_id,
  rfwcdma_nv_rx_dynamic_element_type element,
  uint8 index,
  rfwcdma_nv_rx_dynamic_data_type *rfnv_ptr
);

/* ----------------------------------------------------------------------- */
int16 rfnv_wcdma_get_info_from_rx_static_data
(
  rfm_device_enum_type device_id,
  rfcom_multi_carrier_hspa_id_type carr_id,
  rfwcdma_nv_rx_static_element_type element,
  uint8 index,
  rfwcdma_nv_rx_static_data_type *rfnv_ptr
);
/* ----------------------------------------------------------------------- */
void rfnv_wcdma_load_rx_static_data_nv
(
  rfnv_wcdma_nv_tbl_type *rfnv_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band,
  uint8 input_chain_id
);
/* ----------------------------------------------------------------------- */
void rfnv_wcdma_load_rx_dynamic_data_nv
(
  rfnv_wcdma_nv_tbl_type *rfnv_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
);
/*--------------------------------------------------------------------------*/
void rfnv_wcdma_load_rx_dynamic_data_v2_nv
(
  rfnv_wcdma_nv_tbl_type *rfnv_ptr,
  rfnv_rx_cal_data_type *rx_band_cal_data_ptr,
  rfcom_wcdma_band_type band
);
/* ----------------------------------------------------------------------- */
void rfnv_wcdma_set_rx_static_data_to_default
(
  rfnv_wcdma_nv_tbl_type *rfnv_ptr
);
/* ----------------------------------------------------------------------- */
void rfnv_wcdma_set_rx_cal_data_to_default
(
  rfnv_wcdma_nv_tbl_type *rfnv_ptr
);

/*--------------------------------------------------------------------------*/
rfwcdma_nv_pa_static_ver_type
rfnv_wcdma_get_pa_static_nv_version
(
  rfcom_wcdma_band_type band
);
/*--------------------------------------------------------------------------*/
void
rfnv_wcdma_set_pa_static_nv_version
(
  rfcom_wcdma_band_type band,
  int8 version
);
/* ----------------------------------------------------------------------- */
rfnv_wcdma_nv_tbl_type *rfnv_wcdma_get_nv_tbl_ptr
( 
  rfnv_wcdma_supported_mode_type band 
);
/* ----------------------------------------------------------------------- */
void
rfnv_wcdma_load_dc_equidistant_therm_bins_data
(
  rfnv_wcdma_nv_tbl_type *nv_tbl_ptr
);

/* ----------------------------------------------------------------------- */
void 
rfnv_wcdma_load_dc_lin_vs_temp_vs_freq
(
  rfnv_wcdma_nv_tbl_type *nv_tbl_ptr,
  boolean                is_pin
);
/* ----------------------------------------------------------------------- */
rfwcdma_nv_pa_static_ver_type
rfnv_wcdma_get_dc_pa_static_nv_version
(
  rfcom_wcdma_band_type band
);
/* ----------------------------------------------------------------------- */
void
rfnv_wcdma_set_dc_pa_static_nv_version
(
  rfcom_wcdma_band_type band,
  int8 version
);
/*----------------------------------------------------------------------------*/
boolean
rfnv_wcdma_is_pa_static_nv
(
  rfnv_item_id_enum_type rfnv_item_id
);

#endif /* RFNV_WCDMA_H */
