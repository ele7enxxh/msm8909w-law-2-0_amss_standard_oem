#ifndef RFWCDMA_CORE_H
#define RFWCDMA_CORE_H

/*!
  @file
  rfwcdma_core.h

  @brief
  RF WCDMA Core (Generic) Functions.

  @details

*/

/*===========================================================================

Copyright (c) 2008, 2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/wcdma/rfwcdma_core.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/04/14   rmb     Move the core conversion APIs from rfwcdma_core to rfwcdma_core_util.
07/16/13   aa      Multiple LNA transaction support 
04/29/13   jj      Add support to convert band to DL chan
04/24/13   ka      Remove duplicate function 
04/23/13   kg      Add new core function to convert Tx chan to Rx chan 
04/23/13   ka      Initial support for DBDC
04/05/13   rmb     Added prototype for rfwcdma_core_convert_ul_chan_to_band() to fix Compiler Warning.
04/10/12   id      Add support for converting Tx Internal Cal band enum to 
                   RFCOM type 
03/22/12   id      Add support for converting Tx freq to UARFCN 
08/02/11   dw      Add rfwcdma_core_is_band_supported
08/02/11   dw      Add support to populate multi-linearizer data strucures
07/19/11   dw      Added support to get frequency from ARFCN only without band
04/11/11   sar     Relocated rfwcdma_core_convert_band_rfcom_to_rfnv_wcdma.c/h 
                   and reomoved rfnv_wcdma.h.
04/08/11   aki     Included "rfnv_wcdma.h" as needed by rfnv_wcdma_supported_mode_type
03/04/11   ad      Fix writing to incorrect RFNV band for first sweep APT data
02/24/11   ka/dw   Added API to get sys_band_mask for any WCDMA band  
02/09/11   dw      Add support for WCDMA AGC cmd control
03/24/10   ka      Add function to calculate freq from chan without limit check
11/25/09   ntn     Added rfwcdma_core_convert_band_rf_card_to_rfcom() 
10/01/09   ckl     Added rfwcdma_core_convert_band_rfcom_to_rf_card( )
03/15/09   ckl     Deleted rfnv.h
03/09/09   ckl     Code cleanup
11/05/08   kc      Added comments in dOxygen format.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfcom.h"

/*===========================================================================
                           DATA DEFINITIONS
===========================================================================*/

typedef enum
{
  RFWCDMA_CORE_UARFCN_UL,
  RFWCDMA_CORE_UARFCN_DL
} rfwcdma_core_uarfcn_ul_dl_type;

typedef enum
{
  RFWCDMA_CORE_CB_AGC_STATUS,
  RFWCDMA_CORE_CB_DELAY_EXPIRED,
  RFWCDMA_CORE_CB_MAX
} rfwcdma_core_cb_type;

typedef enum
{
  RFWCDMA_BAND_IMT = 1,
  RFWCDMA_BAND_1900 = 2,
  RFWCDMA_BAND_BC3 = 3,
  RFWCDMA_BAND_BC4 = 4,
  RFWCDMA_BAND_800 = 5,
  RFWCDMA_BAND_BC8 = 8,
  RFWCDMA_BAND_BC9 = 9,
  RFWCDMA_BAND_BC11 = 11,
  RFWCDMA_BAND_BC19 = 19,
  RFWCDMA_NUM_WCDMA_BANDS = 63,
  RFWCDMA_BAND_INVALID = 64
} rfwcdma_int_cal_bc_type;
/*===========================================================================
                           FUNCTION PROTOTYPES
===========================================================================*/

/* ----------------------------------------------------------------------- */
extern uint32 rfwcdma_core_get_freq_from_uarfcn
(
  rfcom_wcdma_band_type band,
  word chan,                            /* input channel number */
  rfwcdma_core_uarfcn_ul_dl_type ul_dl
);

/* ----------------------------------------------------------------------- */
extern uint32 rfwcdma_core_get_freq_from_uarfcn_no_limit
(
  rfcom_wcdma_band_type band,
  word chan,                            /* input channel number */
  rfwcdma_core_uarfcn_ul_dl_type ul_dl
);

/* ----------------------------------------------------------------------- */
rf_path_enum_type rfwcdma_core_util_device_to_path_map
(
  rfcom_device_enum_type device
);

/* ----------------------------------------------------------------------- */
rfcom_wcdma_band_type rfwcdma_core_convert_chan_to_band( uint16 chan );

/* ----------------------------------------------------------------------- */
rfcom_wcdma_band_type rfwcdma_core_convert_band_rfi_to_rfcom
(
  rfi_band_type band
);

/* ----------------------------------------------------------------------- */
uint16 rfwcdma_core_map_rx_to_tx_chan
(
  uint16 rx_channel,
  rfcom_wcdma_band_type band
);

/* ----------------------------------------------------------------------- */
rfi_band_type rfwcdma_core_convert_band_rfcom_to_rfi
(
  rfcom_wcdma_band_type band
);

/* ----------------------------------------------------------------------- */
rf_card_band_type rfwcdma_core_convert_band_rfcom_to_rf_card
(
  rfcom_wcdma_band_type band
);

/* ----------------------------------------------------------------------- */
rfcom_wcdma_band_type rfwcdma_core_convert_band_rf_card_to_rfcom
(
  rf_card_band_type band
);
/* ----------------------------------------------------------------------- */
void rfwcdma_core_register_cb(
                               rfm_cb_handler_type cb_handler,
                               const void *user_data_ptr,
                               rfwcdma_core_cb_type event
                             );
/* ----------------------------------------------------------------------- */
sys_band_mask_type rfwcdma_core_get_sys_band_mask_from_band_rfi
( 
  rfi_band_type band 
);
/* ----------------------------------------------------------------------- */
uint32 rfwcdma_core_get_freq_from_ul_arfcn 
(
  word chan 
);
/* ----------------------------------------------------------------------- */
boolean rfwcdma_core_is_band_supported(
                                        rfcom_wcdma_band_type band, 
                                        rf_path_enum_type path
                                      );

/* ----------------------------------------------------------------------- */
extern uint16 rfwcdma_core_get_ul_arfcn_from_freq (uint32 freq);

/* ----------------------------------------------------------------------- */
uint16 rfwcdma_core_get_uarfcn_from_freq
(
  rfcom_wcdma_band_type band,
  uint32 freq,                            /* input freq */
  rfwcdma_core_uarfcn_ul_dl_type ul_dl
);

/* ----------------------------------------------------------------------- */
rfcom_wcdma_band_type rfwcdma_core_convert_ul_freq_to_band( uint32 freq );

/* ----------------------------------------------------------------------- */
rfcom_wcdma_band_type 
convert_wcdma_int_cal_band_to_rfcom_band
(
  rfwcdma_int_cal_bc_type wcdma_band
);
/* ----------------------------------------------------------------------- */
rfcom_wcdma_band_type rfwcdma_core_convert_ul_chan_to_band( uint16 chan );
/*---------------------------------------------------------------------------*/
uint16 rfwcdma_core_map_tx_to_rx_chan( uint16 tx_channel, rfcom_wcdma_band_type band );


/* ----------------------------------------------------------------------- */
uint16 rfwcdma_core_get_middle_DL_chan_from_band(rfcom_wcdma_band_type band);

/* ----------------------------------------------------------------------- */
#endif /*RFWCDMA_CORE_H*/
