/*!
  @file
  rfwcdma_core.c

  @brief
  RF WCDMA Core (Generic) Functions.

  @details

*/

/*===========================================================================

Copyright (c) 2008, 2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/core/src/rfwcdma_core.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/18/14   rmb     Fix Compiler Warning.
08/04/14   rmb     Move the core conversion APIs from rfwcdma_core to rfwcdma_core_util.
06/30/14   vbh     [DR-DS] [1] Clean-up device 0 hardcode 
                   [2]Use MC state variable to reference primary Rx device
05/12/14   kab     Remove RFCOM_RECEIVER_DUAL Support
09/20/13   dw      Fix rfwcdma_core_util_device_to_antenna
07/16/13   aa      Multiple LNA transaction support
04/29/13   jj      Add support to convert band type and band to DL chan
04/24/13   ka      Remove duplicate function 
04/23/13   kg      Add new core function to convert Tx chan to Rx chan 
04/23/13   ka      Initial support for DBDC
11/08/12   dw      Add util function to get RF bus resource script from RF device buffer
10/08/12   vbh     Fix warnings 
09/24/12   vbh     Upadte the band mask to enable the correct bands 
08/19/12   tks     Bypass the band mask check support for bring up 
04/10/12   id      Add support for converting Tx Internal Cal band enum to 
                   RFCOM type 
03/22/12   id      Add support for converting Tx freq to UARFCN
02/13/12   tks     Add support for WCDMA Band 6.  
02/06/12   swb     Remove F3 messages to alleviate ISR overrun
12/19/11   dw      Fix warning
12/15/11   dw      Add missing Band 19 case in rfwcdma_core_convert_ul_chan_to_band()
11/23/11   swb     Map deprecated RFCOM_RECEIVER_DIV to device 1 (instead of DIV placeholder)
10/26/11   dw      Fix missing case for B19.
10/17/11   dw      Fixing bugs for B19 and B11 in utility functions.
08/21/11   ac      BC19 related change
07/19/11   dw      Added support to get frequency from ARFCN only without band
04/11/11   sar     Changed declaration of rfnv_wcdma_get_nv_tbl_ptr to extern.
04/11/11   sar     Removed rfwcdma_core_convert_band_rfcom_to_rfnv.
03/04/11   ad      Fix writing to incorrect RFNV band for first sweep APT data    
03/02/11   kguo    fix edge channel frequency mapping problem for DC CM  
03/01/11   dw      Fix warning 
02/24/11   dw/ka   Added API to get sys_band_mask for any WCDMA band  
02/24/11   dw/ac   Merge B3 support  
02/09/11   aro     Compile Error Fix : Using right CB event enum
02/09/11   dw      Add support for WCDMA CB register
08/04/10   ka      Fix for DC B4 additional channels 
05/12/10   ac      B11 bringup
03/24/10   ka      Add function to calculate freq from chan without limit check
01/08/10   ckl     Removed high level lint error.
11/25/09   ntn     Added rfwcdma_core_convert_band_rf_card_to_rfcom() 
10/01/09   ckl     Added rfwcdma_core_convert_band_rfcom_to_rf_card( )
05/03/09   dyc     Added err.h
03/09/09   ckl     Code cleanup
11/05/08   kc      Added comments in dOxygen format.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfa_variation.h"
#include "rfwcdma_core.h"
#include "bsp.h"
#include "rfcom.h"
#include "err.h"
#include "rfcommon_core_cb.h"
#include "rfm_async_types.h"
#include "rfwcdma_mdsp.h"
#include "rfcommon_nv_mm.h"
#include "rf_hal_buffer.h"
#include "rfwcdma_mc.h"
/*===========================================================================
                           DATA DEFINITIONS
===========================================================================*/
#define RFWCDMA_CORE_DCHSPA_HEADROOM 30

/*===========================================================================
                        FUNCTION DEFINITIONS
===========================================================================*/

extern rfnv_wcdma_nv_tbl_type *rfnv_wcdma_get_nv_tbl_ptr( rfnv_wcdma_supported_mode_type band );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function handles channel numbering schemes for different bands.

  @param band WCDMA operational band
  @param chan UARFCN number
  @param ul_dl Uplink or Downlink

  @return uint32 Tx channel frequency in Khz.
*/
uint32 rfwcdma_core_get_freq_from_uarfcn
(
  rfcom_wcdma_band_type band,
  word chan,                            /* input channel number */
  rfwcdma_core_uarfcn_ul_dl_type ul_dl
)
{
  uint32 freq = BSP_RF_TX_CHAN_WCDMA_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;

  if ( chan == 0 )
  {
    return (0);
  }

  switch ( band )
  {
    case RFCOM_BAND_IMT:

      freq = BSP_RF_TX_CHAN_WCDMA_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;

      if ( ( chan >= BSP_RF_RX_CHAN_WCDMA_MIN &&
             chan <= BSP_RF_RX_CHAN_WCDMA_MAX ) &&
           ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        freq =  chan * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ( chan >= BSP_RF_TX_CHAN_WCDMA_MIN  &&
                  chan <= BSP_RF_TX_CHAN_WCDMA_MAX ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        freq =  chan * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else
      {
        switch (ul_dl)
        {
          case RFWCDMA_CORE_UARFCN_DL:
            freq = BSP_RF_RX_CHAN_WCDMA_BC1_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC1 RX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
          case RFWCDMA_CORE_UARFCN_UL:
            freq = BSP_RF_TX_CHAN_WCDMA_BC1_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC1 TX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
        }
      }

      break;

    case RFCOM_BAND_1900:

      if ( ( chan >= BSP_RF_RX_CHAN_WCDMA_BC2_MIN &&
             chan <= BSP_RF_RX_CHAN_WCDMA_BC2_MAX ) &&
           ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        freq =  chan * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC2_MIN ) &&
                ( chan <= BSP_RF_TX_CHAN_WCDMA_BC2_MAX ) &&
           ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        freq =  chan * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ( chan >= BSP_RF_RX_CHAN_WCDMA_BC2_ADDITIONAL_MIN &&
                  chan <= BSP_RF_RX_CHAN_WCDMA_BC2_ADDITIONAL_MAX ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_DL)
      {
        freq = ( ( chan + BSP_BC2_REG_TO_ADDITIONAL_CH_OFFSET ) * BSP_RF_FREQ_WCDMA_CH_STEP ) + 100;
      }
      else if ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC2_ADDITIONAL_MIN &&
                  chan <= BSP_RF_TX_CHAN_WCDMA_BC2_ADDITIONAL_MAX ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_UL)
      {
        freq = ( ( chan + BSP_BC2_REG_TO_ADDITIONAL_CH_OFFSET ) * BSP_RF_FREQ_WCDMA_CH_STEP ) + 100;
      }
      else
      {
        switch (ul_dl)
        {
          case RFWCDMA_CORE_UARFCN_DL:
            freq = BSP_RF_RX_CHAN_WCDMA_BC2_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC2 RX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
          case RFWCDMA_CORE_UARFCN_UL:
            freq = BSP_RF_TX_CHAN_WCDMA_BC2_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC2 TX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
        }
      }

      break;

    case RFCOM_BAND_BC3:

      /* rfnv_1800.c needs to call this function for the tx/rx cal channel
       * lists, and does not know in advance whether the phone supports BC3
       * or BC9. So we map them to the same case and use the rfwcdma_core_uses_bc3 flag
       */
        if ( ( chan >= BSP_RF_RX_CHAN_WCDMA_BC3_MIN &&
               chan <= BSP_RF_RX_CHAN_WCDMA_BC3_MAX ) &&
             ul_dl == RFWCDMA_CORE_UARFCN_DL )
        {
          freq = ( chan + ( 5 * BSP_RF_RX_BC3_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
        }
        else if ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC3_MIN &&
                    chan <= BSP_RF_TX_CHAN_WCDMA_BC3_MAX ) &&
                  ul_dl == RFWCDMA_CORE_UARFCN_UL )
        {
          freq = ( chan + ( 5 * BSP_RF_TX_BC3_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
        }
        else
        {
          switch (ul_dl)
          {
            case RFWCDMA_CORE_UARFCN_DL:
              freq = ( BSP_RF_RX_CHAN_WCDMA_BC3_MIN + ( 5 * BSP_RF_RX_BC3_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
              MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC3 RX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
              break;
            case RFWCDMA_CORE_UARFCN_UL:
              freq = ( BSP_RF_TX_CHAN_WCDMA_BC3_MIN + ( 5 * BSP_RF_TX_BC3_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
              MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC3 TX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
              break;
          }
        }
      break;
      case RFCOM_BAND_BC9:
        if ( ( chan >= BSP_RF_RX_CHAN_WCDMA_BC9_MIN &&
               chan <= BSP_RF_RX_CHAN_WCDMA_BC9_MAX ) &&
             ul_dl == RFWCDMA_CORE_UARFCN_DL )
        {
          freq = chan * BSP_RF_FREQ_WCDMA_CH_STEP;
        }
        else if ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC9_MIN &&
                    chan <= BSP_RF_TX_CHAN_WCDMA_BC9_MAX ) &&
                  ul_dl == RFWCDMA_CORE_UARFCN_UL )
        {
          freq = chan * BSP_RF_FREQ_WCDMA_CH_STEP;
        }
        else
        {
          switch (ul_dl)
          {
            case RFWCDMA_CORE_UARFCN_DL:
              freq = BSP_RF_RX_CHAN_WCDMA_BC9_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;
              MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC9 RX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
              break;
            case RFWCDMA_CORE_UARFCN_UL:
              freq = BSP_RF_TX_CHAN_WCDMA_BC9_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;
              MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC9 TX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
              break;
          }
        }

      break;

    case RFCOM_BAND_BC4:

      if ( ( chan >= BSP_RF_RX_CHAN_WCDMA_BC4_MIN &&
             chan <= BSP_RF_RX_CHAN_WCDMA_BC4_MAX ) &&
           ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        freq = ( chan + ( 5 * BSP_RF_RX_BC4_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC4_MIN &&
                  chan <= BSP_RF_TX_CHAN_WCDMA_BC4_MAX ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        freq = ( chan + ( 5 * BSP_RF_TX_BC4_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ( chan >= BSP_RF_RX_CHAN_WCDMA_BC4_ADDITIONAL_MIN &&
                  chan <= BSP_RF_RX_CHAN_WCDMA_BC4_ADDITIONAL_MAX ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        freq = ( ( ( chan + BSP_BC4_REG_TO_ADDITIONAL_CH_OFFSET ) + (5 * BSP_RF_RX_BC4_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP ) + 100; /*lint !e732*/
      }
      else if ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC4_ADDITIONAL_MIN &&
                  chan <= BSP_RF_TX_CHAN_WCDMA_BC4_ADDITIONAL_MAX ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        freq = ( ( ( chan + BSP_BC4_REG_TO_ADDITIONAL_CH_OFFSET ) + (5 * BSP_RF_TX_BC4_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP ) + 100; /*lint !e732*/
      }
      else
      {
        switch (ul_dl)
        {
          case RFWCDMA_CORE_UARFCN_DL:
            freq = ( BSP_RF_RX_CHAN_WCDMA_BC4_MIN + ( 5 * BSP_RF_RX_BC4_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC4 RX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
          case RFWCDMA_CORE_UARFCN_UL:
            freq = ( BSP_RF_TX_CHAN_WCDMA_BC4_MIN + ( 5 * BSP_RF_TX_BC4_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC4 TX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
        }
      }

      break;


    case RFCOM_BAND_800:
      freq = BSP_RF_TX_CHAN_WCDMA_BC5_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;

      if ( ( chan >= BSP_RF_RX_CHAN_WCDMA_BC5_MIN &&
             chan <= BSP_RF_RX_CHAN_WCDMA_BC5_MAX ) &&
           ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        freq =  chan * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC5_MIN &&
                  chan <= BSP_RF_TX_CHAN_WCDMA_BC5_MAX ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        freq =  chan * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ( chan >= BSP_RF_RX_CHAN_WCDMA_BC5_ADDITIONAL_MIN &&
                  chan <= BSP_RF_RX_CHAN_WCDMA_BC5_ADDITIONAL_MAX ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        freq = ( ( chan + BSP_BC5_REG_TO_ADDITIONAL_CH_OFFSET ) * BSP_RF_FREQ_WCDMA_CH_STEP ) + 100; /*lint !e732*/
      }
      else if ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC5_ADDITIONAL_MIN &&
                  chan <= BSP_RF_TX_CHAN_WCDMA_BC5_ADDITIONAL_MAX ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        freq = ( ( chan + BSP_BC5_REG_TO_ADDITIONAL_CH_OFFSET ) * BSP_RF_FREQ_WCDMA_CH_STEP ) + 100; /*lint !e732*/
      }
      else
      {
        switch (ul_dl)
        {
          case RFWCDMA_CORE_UARFCN_DL:
            freq = BSP_RF_RX_CHAN_WCDMA_BC5_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC5 RX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
          case RFWCDMA_CORE_UARFCN_UL:
            freq = BSP_RF_TX_CHAN_WCDMA_BC5_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC5 TX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
        }
      }
      break;

    /* RFCN to Frequency calculation for WCDMA Band XIX */
    case RFCOM_BAND_BC19:
      if ( ( chan >= BSP_RF_RX_CHAN_WCDMA_BC19_MIN && chan <= BSP_RF_RX_CHAN_WCDMA_BC19_MAX ) &&
             ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        freq = ( chan + ( 5 * BSP_RF_RX_BC19_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC19_MIN && chan <= BSP_RF_TX_CHAN_WCDMA_BC19_MAX ) &&
                  ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        freq = ( chan + ( 5 * BSP_RF_TX_BC19_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ( chan >= BSP_RF_RX_CHAN_WCDMA_BC19_ADDITIONAL_MIN && chan <= BSP_RF_RX_CHAN_WCDMA_BC19_ADDITIONAL_MAX ) &&
                  ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        freq = ( ( ( chan - BSP_BC19_REG_TO_ADDITIONAL_CH_OFFSET ) + (5 * BSP_RF_RX_BC19_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP ) + 100; /*lint !e732*/
      }
      else if ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC19_ADDITIONAL_MIN && chan <= BSP_RF_TX_CHAN_WCDMA_BC19_ADDITIONAL_MAX ) &&
                  ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        freq = ( ( ( chan - BSP_BC19_REG_TO_ADDITIONAL_CH_OFFSET ) + (5 * BSP_RF_TX_BC19_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP ) + 100; /*lint !e732*/
      }
      else
      {
        switch (ul_dl)
        {
          case RFWCDMA_CORE_UARFCN_DL:
            freq = ( BSP_RF_RX_CHAN_WCDMA_BC19_MIN + ( 5 * BSP_RF_RX_BC19_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC19 RX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
          case RFWCDMA_CORE_UARFCN_UL:
            freq = ( BSP_RF_TX_CHAN_WCDMA_BC19_MIN + ( 5 * BSP_RF_TX_BC19_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC19 TX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
        }
      }
      break;

    case RFCOM_BAND_BC8:
      freq = BSP_RF_TX_CHAN_WCDMA_BC8_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;

      if ( ( chan >= BSP_RF_RX_CHAN_WCDMA_BC8_MIN &&
             chan <= BSP_RF_RX_CHAN_WCDMA_BC8_MAX ) &&
           ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        freq = ( chan + ( 5 * BSP_RF_RX_BC8_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC8_MIN &&
                  chan <= BSP_RF_TX_CHAN_WCDMA_BC8_MAX ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        freq = ( chan + ( 5 * BSP_RF_TX_BC8_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else
      {
        switch (ul_dl)
        {
          case RFWCDMA_CORE_UARFCN_DL:
            freq = ( BSP_RF_RX_CHAN_WCDMA_BC8_MIN + ( 5 * BSP_RF_RX_BC8_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC8 RX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
          case RFWCDMA_CORE_UARFCN_UL:
            freq = ( BSP_RF_TX_CHAN_WCDMA_BC8_MIN + ( 5 * BSP_RF_TX_BC8_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC8 TX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
        }
      }

      break;

case RFCOM_BAND_BC11:
      freq = BSP_RF_TX_CHAN_WCDMA_BC11_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;

      if ( ( chan >= BSP_RF_RX_CHAN_WCDMA_BC11_MIN &&
             chan <= BSP_RF_RX_CHAN_WCDMA_BC11_MAX ) &&
           ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        freq = ( chan + ( 5 * BSP_RF_RX_BC11_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC11_MIN &&
                  chan <= BSP_RF_TX_CHAN_WCDMA_BC11_MAX ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        freq = ( chan + ( 5 * BSP_RF_TX_BC11_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else
      {
        switch (ul_dl)
        {
          case RFWCDMA_CORE_UARFCN_DL:
            freq = ( BSP_RF_RX_CHAN_WCDMA_BC11_MIN + ( 5 * BSP_RF_RX_BC11_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC11 RX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
          case RFWCDMA_CORE_UARFCN_UL:
            freq = ( BSP_RF_TX_CHAN_WCDMA_BC11_MIN + ( 5 * BSP_RF_TX_BC11_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC11 TX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
        }
      }

      break;

    default :
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid RF Band = %d", band);
  }

  return (freq);

} /* rfwcdma_core_get_freq_from_uarfcn */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function handles channel numbering schemes for different bands. It
  disregards the upper limit on the channel numbers. This is needed when the
  radio is in dual carrier mode but is being used to measure a single carrier
  channel at the band edge, where the channel number for the second carrier
  would have exceeded the upper limit.

  @param band WCDMA operational band
  @param chan UARFCN number
  @param ul_dl Uplink or Downlink

  @return uint32 Tx channel frequency in Khz.
*/
uint32 rfwcdma_core_get_freq_from_uarfcn_no_limit
(
  rfcom_wcdma_band_type band,
  word chan,                            /* input channel number */
  rfwcdma_core_uarfcn_ul_dl_type ul_dl
)
{
  uint32 freq = BSP_RF_TX_CHAN_WCDMA_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;

  if ( chan == 0 )
  {
    return (0);
  }

  switch ( band )
  {
    case RFCOM_BAND_IMT:

      freq = BSP_RF_TX_CHAN_WCDMA_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;

      if ( ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        freq =  chan * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        freq =  chan * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      break;

    case RFCOM_BAND_BC11:

      freq = BSP_RF_TX_CHAN_WCDMA_BC11_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;

      if ( ( chan >= (BSP_RF_RX_CHAN_WCDMA_BC11_MIN - RFWCDMA_CORE_DCHSPA_HEADROOM)) &&
           ( chan <= (BSP_RF_RX_CHAN_WCDMA_BC11_MAX + RFWCDMA_CORE_DCHSPA_HEADROOM)) &&
           ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        freq = ( chan + ( 5 * BSP_RF_RX_BC11_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ( chan >= (BSP_RF_TX_CHAN_WCDMA_BC11_MIN - RFWCDMA_CORE_DCHSPA_HEADROOM)) &&
                ( chan <= (BSP_RF_TX_CHAN_WCDMA_BC11_MAX + RFWCDMA_CORE_DCHSPA_HEADROOM)) &&
                ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        freq = ( chan + ( 5 * BSP_RF_TX_BC11_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      break;

    case RFCOM_BAND_1900:

      if ( ( chan >= (BSP_RF_RX_CHAN_WCDMA_BC2_MIN - RFWCDMA_CORE_DCHSPA_HEADROOM )) &&
           ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        freq =  chan * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ( chan >= (BSP_RF_TX_CHAN_WCDMA_BC2_MIN - RFWCDMA_CORE_DCHSPA_HEADROOM)) &&
           ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        freq =  chan * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ( chan >= (BSP_RF_RX_CHAN_WCDMA_BC2_ADDITIONAL_MIN - RFWCDMA_CORE_DCHSPA_HEADROOM)) &&
                ul_dl == RFWCDMA_CORE_UARFCN_DL)
      {
        freq = ( ( chan + BSP_BC2_REG_TO_ADDITIONAL_CH_OFFSET ) * BSP_RF_FREQ_WCDMA_CH_STEP ) + 100;
      }
      else if ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC2_ADDITIONAL_MIN ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_UL)
      {
        freq = ( ( chan + BSP_BC2_REG_TO_ADDITIONAL_CH_OFFSET ) * BSP_RF_FREQ_WCDMA_CH_STEP ) + 100;
      }
      else
      {
        switch (ul_dl)
        {
          case RFWCDMA_CORE_UARFCN_DL:
            freq = BSP_RF_RX_CHAN_WCDMA_BC2_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC2 RX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
          case RFWCDMA_CORE_UARFCN_UL:
            freq = BSP_RF_TX_CHAN_WCDMA_BC2_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC2 TX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
        }
      }

      break;

    case RFCOM_BAND_BC3:

      if ( ( chan >= (BSP_RF_RX_CHAN_WCDMA_BC3_MIN -RFWCDMA_CORE_DCHSPA_HEADROOM)) &&
             ( chan <= (BSP_RF_RX_CHAN_WCDMA_BC3_MAX +RFWCDMA_CORE_DCHSPA_HEADROOM) ) &&
             ul_dl == RFWCDMA_CORE_UARFCN_DL )
        {
          freq = ( chan + ( 5 * BSP_RF_RX_BC3_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
        }
        else if ( ( chan >= (BSP_RF_TX_CHAN_WCDMA_BC3_MIN -RFWCDMA_CORE_DCHSPA_HEADROOM)) &&
                    (chan <= (BSP_RF_TX_CHAN_WCDMA_BC3_MAX +RFWCDMA_CORE_DCHSPA_HEADROOM)) &&
                  ul_dl == RFWCDMA_CORE_UARFCN_UL )
        {
          freq = ( chan + ( 5 * BSP_RF_TX_BC3_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
        }
        else
        {
          switch (ul_dl)
          {
            case RFWCDMA_CORE_UARFCN_DL:
              freq = ( BSP_RF_RX_CHAN_WCDMA_BC3_MIN + ( 5 * BSP_RF_RX_BC3_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
              MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC3 RX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
              break;
            case RFWCDMA_CORE_UARFCN_UL:
              freq = ( BSP_RF_TX_CHAN_WCDMA_BC3_MIN + ( 5 * BSP_RF_TX_BC3_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
              MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC3 TX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
              break;
          }
        } 

        break; 

    case RFCOM_BAND_BC9:
        
        if ( ( chan >= (BSP_RF_RX_CHAN_WCDMA_BC9_MIN -RFWCDMA_CORE_DCHSPA_HEADROOM)) &&
               ( chan <= (BSP_RF_RX_CHAN_WCDMA_BC9_MAX +RFWCDMA_CORE_DCHSPA_HEADROOM )) &&
             ul_dl == RFWCDMA_CORE_UARFCN_DL )
        {
          freq = chan * BSP_RF_FREQ_WCDMA_CH_STEP;
        }
        else if ( ( chan >= (BSP_RF_TX_CHAN_WCDMA_BC9_MIN -RFWCDMA_CORE_DCHSPA_HEADROOM)) &&
                    (chan <= (BSP_RF_TX_CHAN_WCDMA_BC9_MAX +RFWCDMA_CORE_DCHSPA_HEADROOM)) &&
                  ul_dl == RFWCDMA_CORE_UARFCN_UL )
        {
          freq = chan * BSP_RF_FREQ_WCDMA_CH_STEP;
        }
        else
        {
          switch (ul_dl)
          {
            case RFWCDMA_CORE_UARFCN_DL:
              freq = BSP_RF_RX_CHAN_WCDMA_BC9_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;
              MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC9 RX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
              break;
            case RFWCDMA_CORE_UARFCN_UL:
              freq = BSP_RF_TX_CHAN_WCDMA_BC9_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;
              MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC9 TX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
              break;
          }
        }
       break;

    case RFCOM_BAND_BC4:

      if ( ( chan >= (BSP_RF_RX_CHAN_WCDMA_BC4_MIN -RFWCDMA_CORE_DCHSPA_HEADROOM))&&
           ( chan <= (BSP_RF_RX_CHAN_WCDMA_BC4_MAX + RFWCDMA_CORE_DCHSPA_HEADROOM)) &&
           ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        freq = ( chan + ( 5 * BSP_RF_RX_BC4_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ( chan >= (BSP_RF_TX_CHAN_WCDMA_BC4_MIN -RFWCDMA_CORE_DCHSPA_HEADROOM)) &&
                ( chan <= (BSP_RF_TX_CHAN_WCDMA_BC4_MAX + RFWCDMA_CORE_DCHSPA_HEADROOM)) &&
                ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        freq = ( chan + ( 5 * BSP_RF_TX_BC4_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ( chan >= (BSP_RF_RX_CHAN_WCDMA_BC4_ADDITIONAL_MIN -RFWCDMA_CORE_DCHSPA_HEADROOM)) &&
                ( chan <= (BSP_RF_RX_CHAN_WCDMA_BC4_ADDITIONAL_MAX + RFWCDMA_CORE_DCHSPA_HEADROOM)) &&
                ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        freq = ( ( ( chan + BSP_BC4_REG_TO_ADDITIONAL_CH_OFFSET ) + (5 * BSP_RF_RX_BC4_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP ) + 100; /*lint !e732*/
      }
      else if ( ( chan >= (BSP_RF_TX_CHAN_WCDMA_BC4_ADDITIONAL_MIN - RFWCDMA_CORE_DCHSPA_HEADROOM)) &&
                ( chan <= (BSP_RF_TX_CHAN_WCDMA_BC4_ADDITIONAL_MAX + RFWCDMA_CORE_DCHSPA_HEADROOM)) &&
                ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        freq = ( ( ( chan + BSP_BC4_REG_TO_ADDITIONAL_CH_OFFSET ) + (5 * BSP_RF_TX_BC4_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP ) + 100; /*lint !e732*/
      }
      else
      {
        switch (ul_dl)
        {
          case RFWCDMA_CORE_UARFCN_DL:
            freq = ( BSP_RF_RX_CHAN_WCDMA_BC4_MIN + ( 5 * BSP_RF_RX_BC4_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC4 RX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
          case RFWCDMA_CORE_UARFCN_UL:
            freq = ( BSP_RF_TX_CHAN_WCDMA_BC4_MIN + ( 5 * BSP_RF_TX_BC4_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC4 TX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
        }
      }

      break;

       /* RFCN to Frequency calculation for WCDMA Band XIX */
    case RFCOM_BAND_BC19:
      if ( ( chan >= BSP_RF_RX_CHAN_WCDMA_BC19_MIN && chan <= BSP_RF_RX_CHAN_WCDMA_BC19_MAX ) &&
             ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        freq = ( chan + ( 5 * BSP_RF_RX_BC19_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC19_MIN && chan <= BSP_RF_TX_CHAN_WCDMA_BC19_MAX ) &&
                  ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        freq = ( chan + ( 5 * BSP_RF_TX_BC19_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ( chan >= BSP_RF_RX_CHAN_WCDMA_BC19_ADDITIONAL_MIN && chan <= BSP_RF_RX_CHAN_WCDMA_BC19_ADDITIONAL_MAX ) &&
                  ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        freq = ( ( ( chan - BSP_BC19_REG_TO_ADDITIONAL_CH_OFFSET ) + (5 * BSP_RF_RX_BC19_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP ) + 100; /*lint !e732*/
      }
      else if ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC19_ADDITIONAL_MIN && chan <= BSP_RF_TX_CHAN_WCDMA_BC19_ADDITIONAL_MAX ) &&
                  ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        freq = ( ( ( chan - BSP_BC19_REG_TO_ADDITIONAL_CH_OFFSET ) + (5 * BSP_RF_TX_BC19_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP ) + 100; /*lint !e732*/
      }
      else
      {
        switch (ul_dl)
        {
          case RFWCDMA_CORE_UARFCN_DL:
            freq = ( BSP_RF_RX_CHAN_WCDMA_BC19_MIN + ( 5 * BSP_RF_RX_BC19_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC19 RX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
          case RFWCDMA_CORE_UARFCN_UL:
            freq = ( BSP_RF_TX_CHAN_WCDMA_BC19_MIN + ( 5 * BSP_RF_TX_BC19_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC19 TX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
        }
      }
      break;

    case RFCOM_BAND_800:
      freq = BSP_RF_TX_CHAN_WCDMA_BC5_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;

      if ( ( chan >= (BSP_RF_RX_CHAN_WCDMA_BC5_MIN -RFWCDMA_CORE_DCHSPA_HEADROOM) ) &&
           ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        freq =  chan * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ( chan >= (BSP_RF_TX_CHAN_WCDMA_BC5_MIN -RFWCDMA_CORE_DCHSPA_HEADROOM ) ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        freq =  chan * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ( chan >= (BSP_RF_RX_CHAN_WCDMA_BC5_ADDITIONAL_MIN - RFWCDMA_CORE_DCHSPA_HEADROOM)  ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        freq = ( ( chan + BSP_BC5_REG_TO_ADDITIONAL_CH_OFFSET ) * BSP_RF_FREQ_WCDMA_CH_STEP ) + 100; /*lint !e732*/
      }
      else if ( ( chan >= (BSP_RF_TX_CHAN_WCDMA_BC5_ADDITIONAL_MIN - RFWCDMA_CORE_DCHSPA_HEADROOM) ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        freq = ( ( chan + BSP_BC5_REG_TO_ADDITIONAL_CH_OFFSET ) * BSP_RF_FREQ_WCDMA_CH_STEP ) + 100; /*lint !e732*/
      }
      else
      {
        switch (ul_dl)
        {
          case RFWCDMA_CORE_UARFCN_DL:
            freq = BSP_RF_RX_CHAN_WCDMA_BC5_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC5 RX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
          case RFWCDMA_CORE_UARFCN_UL:
            freq = BSP_RF_TX_CHAN_WCDMA_BC5_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;
            MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR,"BC5 TX invalid channel %d, freq %d.%d MHz", chan, freq/1000, freq%1000);
            break;
        }
      }
      break;

    case RFCOM_BAND_BC8:
      freq = BSP_RF_TX_CHAN_WCDMA_BC8_MIN * BSP_RF_FREQ_WCDMA_CH_STEP;

      if ( ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        freq = ( chan + ( 5 * BSP_RF_RX_BC8_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
      }
      else if ( ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        freq = ( chan + ( 5 * BSP_RF_TX_BC8_FREQUENCY_OFFSET ) ) * BSP_RF_FREQ_WCDMA_CH_STEP;
      }

      break;

    default :
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid RF Band = %d", band);
  }

  return (freq);

} /* rfwcdma_core_get_freq_from_uarfcn */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function handles channel numbering schemes for different bands.

  @param device Devices supported by RF driver

  @return rf_path_enum_type Primary or Secondary path

  @todo kevin chung Check what the existing code is doing for dual_ in RFM layer.

*/
rf_path_enum_type rfwcdma_core_util_device_to_path_map( rfcom_device_enum_type device )
{
  rf_path_enum_type temp_path;

  switch (device)
  {
    case RFCOM_TRANSCEIVER_0 :
      temp_path = RF_PATH_0;
      break;

    case RFM_DEVICE_1 :
      temp_path = RF_PATH_1;
      break;

    default :
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"invalid device: no path found.",0);
      temp_path = RF_PATH_0;
      break;
  }

  return temp_path;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function converts WCDMA channel number to WCDMA band.

  @param chan UARFCN channel number.

  @return rfi_band_type WCDMA band
*/
rfcom_wcdma_band_type rfwcdma_core_convert_chan_to_band( uint16 chan )
{
  rfcom_wcdma_band_type band;

  if ( ( chan >= BSP_RF_RX_CHAN_WCDMA_MIN ) && ( chan <= BSP_RF_RX_CHAN_WCDMA_MAX ) )
  {
    band =  RFCOM_BAND_IMT;
  } else if ( ( ( chan >= BSP_RF_RX_CHAN_WCDMA_PCS_MIN ) && ( chan <= BSP_RF_RX_CHAN_WCDMA_PCS_MAX ) ) ||
              ( ( chan >= BSP_RF_RX_CHAN_WCDMA_PCS_100KHZ_OFFSET_MIN ) &&
                ( chan <= BSP_RF_RX_CHAN_WCDMA_PCS_100KHZ_OFFSET_MAX ) )
            )
  {
    band = RFCOM_BAND_1900;
  } else if ( ( chan >= BSP_RF_RX_CHAN_WCDMA_BC3_MIN ) && ( chan <= BSP_RF_RX_CHAN_WCDMA_BC3_MAX )
            )
  {
    band = RFCOM_BAND_BC3;
  } else if ( ( ( chan >= BSP_RF_RX_CHAN_WCDMA_BC4_MIN ) && ( chan <= BSP_RF_RX_CHAN_WCDMA_BC4_MAX ) ) ||
              ( ( chan >= BSP_RF_RX_CHAN_WCDMA_BC4_ADDITIONAL_MIN ) &&
                ( chan <= BSP_RF_RX_CHAN_WCDMA_BC4_ADDITIONAL_MAX ) )
            )
  {
    band = RFCOM_BAND_BC4;
  } else if ( ( ( chan >= BSP_RF_RX_CHAN_WCDMA_850_MIN ) && ( chan <= BSP_RF_RX_CHAN_WCDMA_850_MAX ) ) ||
              ( ( chan >= BSP_RF_RX_CHAN_WCDMA_850_ADDITIONAL_MIN ) &&
                ( chan <= BSP_RF_RX_CHAN_WCDMA_850_ADDITIONAL_MAX ) )
            )
  {
    band = RFCOM_BAND_800;
  } else if ( ( chan >= BSP_RF_RX_CHAN_WCDMA_BC8_MIN ) && ( chan <= BSP_RF_RX_CHAN_WCDMA_BC8_MAX )
            )
  {
    band = RFCOM_BAND_BC8;
  } else if ( ( chan >= BSP_RF_RX_CHAN_WCDMA_BC9_MIN ) && ( chan <= BSP_RF_RX_CHAN_WCDMA_BC9_MAX )
            )
  {
    band = RFCOM_BAND_BC9;
  } else if ( ( chan >= BSP_RF_RX_CHAN_WCDMA_BC11_MIN ) && ( chan <= BSP_RF_RX_CHAN_WCDMA_BC11_MAX )
            )
  {
    band = RFCOM_BAND_BC11;
   } else if ( ( ( chan >= BSP_RF_RX_CHAN_WCDMA_B19_MIN ) && ( chan <= BSP_RF_RX_CHAN_WCDMA_B19_MAX ) ) ||
              ( ( chan >= BSP_RF_RX_CHAN_WCDMA_B19_ADDITIONAL_MIN ) &&
                ( chan <= BSP_RF_RX_CHAN_WCDMA_B19_ADDITIONAL_MAX ) )
            )
  {
    band = RFCOM_BAND_BC19;
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid Channel=%d. Default to WCDMA 2100 or band I", chan);
    band =  RFCOM_BAND_IMT;
  }

  return (band);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function converts WCDMA uplink channel number to WCDMA band.

  @param chan UARFCN uplink channel number.

  @return rfi_band_type WCDMA band
*/
rfcom_wcdma_band_type rfwcdma_core_convert_ul_chan_to_band( uint16 chan )
{
  rfcom_wcdma_band_type band;

  if ( ( chan >= BSP_RF_TX_CHAN_WCDMA_MIN ) && ( chan <= BSP_RF_TX_CHAN_WCDMA_MAX ) )
  {
    band =  RFCOM_BAND_IMT;
  } else if ( ( ( chan >= BSP_RF_TX_CHAN_WCDMA_PCS_MIN ) && ( chan <= BSP_RF_TX_CHAN_WCDMA_PCS_MAX ) ) ||
              ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC2_ADDITIONAL_MIN ) &&
                ( chan <= BSP_RF_TX_CHAN_WCDMA_BC2_ADDITIONAL_MAX ) )
            )
  {
    band = RFCOM_BAND_1900;
  } else if ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC3_MIN ) && ( chan <= BSP_RF_TX_CHAN_WCDMA_BC3_MAX )
            )
  {
    band = RFCOM_BAND_BC3;
  } else if ( ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC4_MIN ) && ( chan <= BSP_RF_TX_CHAN_WCDMA_BC4_MAX ) ) ||
              ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC4_ADDITIONAL_MIN ) &&
                ( chan <= BSP_RF_TX_CHAN_WCDMA_BC4_ADDITIONAL_MAX ) )
            )
  {
    band = RFCOM_BAND_BC4;
  } else if ( ( ( chan >= BSP_RF_TX_CHAN_WCDMA_850_MIN ) && ( chan <= BSP_RF_TX_CHAN_WCDMA_850_MAX ) ) ||
              ( ( chan >= BSP_RF_TX_CHAN_WCDMA_850_ADDITIONAL_MIN ) &&
                ( chan <= BSP_RF_TX_CHAN_WCDMA_850_ADDITIONAL_MAX ) )
            )
  {
    band = RFCOM_BAND_800;
  } else if ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC8_MIN ) && ( chan <= BSP_RF_TX_CHAN_WCDMA_BC8_MAX )
            )
  {
    band = RFCOM_BAND_BC8;
  } else if ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC9_MIN ) && ( chan <= BSP_RF_TX_CHAN_WCDMA_BC9_MAX )
            )
  {
    band = RFCOM_BAND_BC9;
  } else if ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC11_MIN ) && ( chan <= BSP_RF_TX_CHAN_WCDMA_BC11_MAX )
            )
  {
    band = RFCOM_BAND_BC11;
  } else if ( ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC19_MIN ) && ( chan <= BSP_RF_TX_CHAN_WCDMA_BC19_MAX ) )||
              ( ( chan >= BSP_RF_TX_CHAN_WCDMA_BC19_ADDITIONAL_MIN ) &&
                ( chan <= BSP_RF_TX_CHAN_WCDMA_BC19_ADDITIONAL_MAX ) )
            )
  {
    band = RFCOM_BAND_BC19;
  } else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid Channel=%d. Default to WCDMA 2100 or band I", chan);
    band =  RFCOM_BAND_IMT;
  }

  return (band);
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns Tx frequency given UL ARFCN number

  @param chan WCDMA UL ARFCN number

  @return uint32 Tx channel frequency in Khz.
*/
uint32 rfwcdma_core_get_freq_from_ul_arfcn (word chan )
{
  rfcom_wcdma_band_type band = rfwcdma_core_convert_ul_chan_to_band(chan);
  return (rfwcdma_core_get_freq_from_uarfcn_no_limit(band,chan,RFWCDMA_CORE_UARFCN_UL));
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function converts rfi_band_type to rfcom_wcdma_band_type.

  @param band rfi_band_type WCDMA band

  @return rfcom_wcdma_band_type
*/
rfcom_wcdma_band_type rfwcdma_core_convert_band_rfi_to_rfcom( rfi_band_type band )
{
  rfcom_wcdma_band_type w_band = RFCOM_BAND_IMT;

  switch (band)
  {
    case RFI_IMT_BAND:
      w_band = RFCOM_BAND_IMT;
      break;

    case RFI_WCDMA_1900_BAND:
      w_band = RFCOM_BAND_1900;
      break;

    case RFI_WCDMA_BC3_BAND:
      w_band = RFCOM_BAND_BC3;
      break;

    case RFI_WCDMA_BC4_BAND:
      w_band = RFCOM_BAND_BC4;
      break;

    case RFI_WCDMA_800_BAND:
      w_band = RFCOM_BAND_800;
      break;

    case RFI_WCDMA_BC8_BAND:
      w_band = RFCOM_BAND_BC8;
      break;

    case RFI_WCDMA_BC9_BAND:
      w_band = RFCOM_BAND_BC9;
      break;

    case RFI_WCDMA_BC11_BAND:
      w_band = RFCOM_BAND_BC11;
      break;
            
    case RFI_WCDMA_BC19_BAND:
      w_band = RFCOM_BAND_BC19;
      break;
            
    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid band in rf_convert_band_rfi_to_rfcom, %d", band);
      break;
  }

  return (w_band);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function converts rfi_wcdma_band_type to rfcom_band_type.

  @param band rfi_wcdma_band_type WCDMA band

  @return rfi_band_type
*/
rfi_band_type rfwcdma_core_convert_band_rfcom_to_rfi( rfcom_wcdma_band_type band )
{
  rfi_band_type w_band = RFI_IMT_BAND;

  switch (band)
  {
    case RFCOM_BAND_IMT:
      w_band = RFI_IMT_BAND;
      break;

    case RFCOM_BAND_1900:
      w_band = RFI_WCDMA_1900_BAND;
      break;

    case RFCOM_BAND_BC3:
      w_band = RFI_WCDMA_BC3_BAND ;
      break;

    case RFCOM_BAND_BC4:
      w_band = RFI_WCDMA_BC4_BAND;
      break;

    case RFCOM_BAND_800:
      w_band = RFI_WCDMA_800_BAND;
      break;

    case RFCOM_BAND_BC8:
      w_band = RFI_WCDMA_BC8_BAND;
      break;

    case RFCOM_BAND_BC9:
      w_band = RFI_WCDMA_BC9_BAND;
      break;

    case RFCOM_BAND_BC11:
      w_band = RFI_WCDMA_BC11_BAND;
      break;

    case RFCOM_BAND_BC19:
      w_band = RFI_WCDMA_BC19_BAND;
      break;

    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid band in rf_convert_band_rfcom_to_rfi, %d", band);
      break;
  }

  return (w_band);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function converts rfcom_wcdma_band_type to rf_card_band_type.

  @param band rfcom_wcdma_band_type WCDMA band

  @return rf_card_band_type
*/
rf_card_band_type rfwcdma_core_convert_band_rfcom_to_rf_card( rfcom_wcdma_band_type band )
{
  rf_card_band_type w_band = RF_BCI_BAND;

  switch (band)
  {
    case RFCOM_BAND_IMT:
      w_band = RF_BCI_BAND;
      break;

    case RFCOM_BAND_1900:
      w_band = RF_BCII_BAND;
      break;

    case RFCOM_BAND_BC3:
      w_band = RF_BCIII_BAND;
      break;

    case RFCOM_BAND_BC4:
      w_band = RF_BCIV_BAND;
      break;

    case RFCOM_BAND_800:
      w_band = RF_BCV_BAND;
      break;

    case RFCOM_BAND_BC8:
      w_band = RF_BCVIII_BAND;
      break;

    case RFCOM_BAND_BC9:
      w_band = RF_BCIX_BAND;
      break;

    case RFCOM_BAND_BC11:
      w_band = RF_BCXI_BAND;
      break;

    case RFCOM_BAND_BC19:
      w_band = RF_BCXIX_BAND;
      break;

    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid band in rf_convert_band_rfcom_to_rf_card, %d", band);
      break;
  }

  return (w_band);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function converts rf_card_band_type to rfcom_wcdma_band_type.

  @param band rf_card_band_type

  @return rfcom_wcdma_band_type
*/
rfcom_wcdma_band_type rfwcdma_core_convert_band_rf_card_to_rfcom(rf_card_band_type band)
{
rfcom_wcdma_band_type w_band = RFCOM_BAND_IMT;

  switch (band)
  {
    case RF_BCI_BAND:
      w_band = RFCOM_BAND_IMT;
      break;

    case RF_BCII_BAND:
      w_band = RFCOM_BAND_1900;
      break;

    case RF_BCIII_BAND:
      w_band = RFCOM_BAND_BC3;
      break;

    case RF_BCIV_BAND:
      w_band = RFCOM_BAND_BC4;
      break;

    case RF_BCV_BAND:
    case RF_BCVI_BAND:
      w_band = RFCOM_BAND_800;
      break;

    case RF_BCVIII_BAND:
      w_band = RFCOM_BAND_BC8;
      break;

    case RF_BCIX_BAND:
      w_band = RFCOM_BAND_BC9;
      break;

    case RF_BCXI_BAND:
      w_band = RFCOM_BAND_BC11;
      break;

    case RF_BCXIX_BAND:
      w_band = RFCOM_BAND_BC19;
      break;

    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid band in rfwcdma_core_convert_band_rf_card_to_rfcom, %d", band);
      break;
  }

  return (w_band);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the TX channel number given the RX channel number
  and the band.

  @param rx_channel WCDMA channel number
  @param band WCDMA operational band

  @return uint16 Tx channel number
*/
uint16 rfwcdma_core_map_rx_to_tx_chan( uint16 rx_channel, rfcom_wcdma_band_type band )
{
  switch ( band )
  {
  case RFCOM_BAND_IMT:
    return (rx_channel - BSP_RF_WCDMA_BC1_RX_TX_CHAN_NUMBER_OFFSET);

  case RFCOM_BAND_1900:
    return (rx_channel - BSP_RF_WCDMA_BC2_RX_TX_CHAN_NUMBER_OFFSET);

  case RFCOM_BAND_BC3:
    return (rx_channel - BSP_RF_WCDMA_BC3_RX_TX_CHAN_NUMBER_OFFSET);

  case RFCOM_BAND_BC4:
    return (rx_channel - BSP_RF_WCDMA_BC4_RX_TX_CHAN_NUMBER_OFFSET);

  case RFCOM_BAND_800:
    return (rx_channel - BSP_RF_WCDMA_BC5_RX_TX_CHAN_NUMBER_OFFSET);

  case RFCOM_BAND_BC8:
    return (rx_channel - BSP_RF_WCDMA_BC8_RX_TX_CHAN_NUMBER_OFFSET);

  case RFCOM_BAND_BC9:
    return (rx_channel - BSP_RF_WCDMA_BC9_RX_TX_CHAN_NUMBER_OFFSET);

  case RFCOM_BAND_BC11:
    return (rx_channel - BSP_RF_WCDMA_BC11_RX_TX_CHAN_NUMBER_OFFSET);

  case RFCOM_BAND_BC19:
    return (rx_channel - BSP_RF_WCDMA_BC19_RX_TX_CHAN_NUMBER_OFFSET);

  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfi_get_tx_chan_from_rx_chan: unknown band",0);
    return 0;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @details
  This function returns band specific RF parameters

  @param band WCDMA operational band

  @return rfnv_wcdma_nv_tbl_type pointer for RF parameters
*/
rfnv_wcdma_nv_tbl_type *rfwcdma_core_init_nv_table(rfcom_wcdma_band_type band)
{
  rfnv_wcdma_nv_tbl_type *rfnv_ptr=NULL;

  switch ( band )
  {
    case RFCOM_BAND_IMT:
      rfnv_ptr = rfnv_wcdma_get_nv_tbl_ptr(RFNV_WCDMA_2100);
      break;

    case RFCOM_BAND_1900:
      rfnv_ptr = rfnv_wcdma_get_nv_tbl_ptr(RFNV_WCDMA_1900);
      break;

    case RFCOM_BAND_BC3:
      rfnv_ptr = rfnv_wcdma_get_nv_tbl_ptr(RFNV_WCDMA_1800);
      break;

    case RFCOM_BAND_BC4:
      rfnv_ptr = rfnv_wcdma_get_nv_tbl_ptr(RFNV_WCDMA_BC4);
      break;

    case RFCOM_BAND_800:
    case RFCOM_BAND_BC19:
      rfnv_ptr = rfnv_wcdma_get_nv_tbl_ptr(RFNV_WCDMA_800);
      break;

    case RFCOM_BAND_BC8:
      rfnv_ptr = rfnv_wcdma_get_nv_tbl_ptr(RFNV_WCDMA_900);
      break;

    case RFCOM_BAND_BC9:
      rfnv_ptr = rfnv_wcdma_get_nv_tbl_ptr(RFNV_WCDMA_1800);
      break;

    case RFCOM_BAND_BC11:
      rfnv_ptr = rfnv_wcdma_get_nv_tbl_ptr(RFNV_WCDMA_1500);
      break;

    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid band. WCDMA NV table initialization failed.", band);
  }

  return(rfnv_ptr);

}
/*----------------------------------------------------------------------------*/
/*!
  @details
  This function register the callbacks for WCDMA RF APIs

  @param cb_handler: callback handler
         user_data_ptr: user specified pointer for the callback handler
         event: RF WCDMA callback event

  @return 
*/
void rfwcdma_core_register_cb(
                               rfm_cb_handler_type cb_handler,
                               const void *user_data_ptr,
                               rfwcdma_core_cb_type event
                             )
{
  rfcommon_core_cb_struct_type *cb_ptr;
  /* Register callback function */
  if ( cb_handler != NULL )
  {
    /* Get call back buffer, setup call back event,  and register call
       back event. */
    cb_ptr = rfcommon_core_cb_get_event_buffer();
    cb_ptr->cb_handler = (rfcom_cb_handler_type)cb_handler;
    cb_ptr->user_data_ptr = user_data_ptr;
    cb_ptr->num_events = 1;
    switch (event)
    {
    case RFWCDMA_CORE_CB_DELAY_EXPIRED:
      cb_ptr->cb_events[0].event = RFCOM_TIME_DELAY_EXPIRED;
      break;

    case RFWCDMA_CORE_CB_AGC_STATUS:
      if (rfwcdma_mdsp_get_rxagc_cmd_status() == TRUE)
      {
        cb_ptr->cb_events[0].event = RFM_WCDMA_AGC_CMD_COMPLETE;
      }
      else
      {
        cb_ptr->cb_events[0].event = RFM_WCDMA_AGC_CMD_FAIL;
      }

      break;
    default:
     cb_ptr->cb_events[0].event = RFM_NUM_CB_EVENTS;
     MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Unknown cb event! :%d", event);
     break;    
    }
    cb_ptr->cb_events[0].delay = 500;
    rfcommon_core_cb_register_events( cb_ptr );
  }
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns sys_band_mask for a WCDMA rfi_band

  @param band rfi_band_type WCDMA band

  @return sys_band_mask for the passed and
*/
sys_band_mask_type rfwcdma_core_get_sys_band_mask_from_band_rfi( rfi_band_type band )
{
 sys_band_mask_type sys_band_mask = SYS_BAND_MASK_EMPTY;

 switch(band)	 
 {
  case  RFI_IMT_BAND:
   sys_band_mask = SYS_BAND_MASK_WCDMA_I_IMT_2000;
   break;
   
  case  RFI_WCDMA_1900_BAND:
   sys_band_mask = SYS_BAND_MASK_WCDMA_II_PCS_1900;
   break;
   
  case  RFI_WCDMA_BC3_BAND:
   sys_band_mask = SYS_BAND_MASK_WCDMA_III_1700;
   break;
   
  case  RFI_WCDMA_BC4_BAND: 
   sys_band_mask = SYS_BAND_MASK_WCDMA_IV_1700;
   break;
   
  case  RFI_WCDMA_800_BAND:
   #ifdef FEATURE_WCDMA_850
   sys_band_mask = SYS_BAND_MASK_WCDMA_V_850;
   #else
   sys_band_mask = SYS_BAND_MASK_WCDMA_VI_800;
   #endif
   break;
	
  case RFI_WCDMA_BC8_BAND:  
   sys_band_mask = SYS_BAND_MASK_WCDMA_VIII_900;
   break;
   
  case RFI_WCDMA_BC9_BAND:  
   sys_band_mask = SYS_BAND_MASK_WCDMA_IX_1700;
   break;
   
  case RFI_WCDMA_BC11_BAND:
    sys_band_mask = SYS_BAND_MASK_WCDMA_XI_1500;
    break;

  case RFI_WCDMA_BC19_BAND:
    sys_band_mask = SYS_BAND_MASK_WCDMA_XIX_850;
    break;	
    	
   default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid band in get_sys_band_mask_from_band_rfi, %d", band);
    sys_band_mask = SYS_BAND_MASK_EMPTY;
    break;
  }

  return sys_band_mask;
} 
boolean rfwcdma_core_is_band_supported(rfcom_wcdma_band_type band, rf_path_enum_type path)
{
  sys_band_mask_type sys_band_mask = \
  rfwcdma_core_get_sys_band_mask_from_band_rfi(rfwcdma_core_convert_band_rfcom_to_rfi(band));

  /*Check if the RFI band is supported in Band config NV*/
  if ( (rfcommon_nv_get_rf_bc_config(path) & sys_band_mask) != 0)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns Tx UL ARFCN given UL frequency

  @param uint32 Tx channel frequency in Khz.

  @return chan WCDMA UL ARFCN number
*/
uint16 rfwcdma_core_get_ul_arfcn_from_freq (uint32 freq)
{
  rfcom_wcdma_band_type band;
  band = rfwcdma_core_convert_ul_freq_to_band(freq);

  return (rfwcdma_core_get_uarfcn_from_freq(band,freq,RFWCDMA_CORE_UARFCN_UL));
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns Tx UL Chan for given frequency

  @param uint32 freq - Uplink Frequency in kHz

  @return uint16 chan - Uplink UARFCN
*/
uint16 rfwcdma_core_get_uarfcn_from_freq
(
  rfcom_wcdma_band_type band,
  uint32 freq,                            /* input freq */
  rfwcdma_core_uarfcn_ul_dl_type ul_dl
)
{
  uint16 chan = BSP_RF_TX_CHAN_WCDMA_MIN;

  if ( freq == 0 )
  {
    return (0);
  }

  /* All frequency numbers are in KHz and hence, the appropriate conversion factors are applied here */
  switch ( band )
  {
    case RFCOM_BAND_IMT:
      if( ul_dl == RFWCDMA_CORE_UARFCN_DL && freq >= BSP_RF_RX_FREQ_WCDMA_MIN && freq <= BSP_RF_RX_FREQ_WCDMA_MAX )
      {
        chan =  (uint16)((freq * BSP_RF_FREQ_WCDMA_MULT_FACTOR)/1000);
      }
      else if( ul_dl == RFWCDMA_CORE_UARFCN_UL && freq >= BSP_RF_TX_FREQ_WCDMA_MIN && freq <= BSP_RF_TX_FREQ_WCDMA_MAX )
      {
        chan =  (uint16)((freq * BSP_RF_FREQ_WCDMA_MULT_FACTOR)/1000);
      }
      break;

    case RFCOM_BAND_BC11:

      if( ul_dl == RFWCDMA_CORE_UARFCN_DL && freq >= BSP_RF_RX_FREQ_WCDMA_BC11_MIN && freq <= BSP_RF_RX_FREQ_WCDMA_BC11_MAX )
      {
        chan = (uint16)((BSP_RF_FREQ_WCDMA_MULT_FACTOR * ( freq - (BSP_RF_RX_BC11_FREQUENCY_OFFSET * 1000) ))/1000);
      }
      else if( ul_dl == RFWCDMA_CORE_UARFCN_UL && freq >= BSP_RF_TX_FREQ_WCDMA_BC11_MIN && freq <= BSP_RF_TX_FREQ_WCDMA_BC11_MAX )
      {
        chan = (uint16)((BSP_RF_FREQ_WCDMA_MULT_FACTOR * ( freq - (BSP_RF_TX_BC11_FREQUENCY_OFFSET * 1000) ))/1000);
      }
      break;

    case RFCOM_BAND_1900:

      if( ul_dl == RFWCDMA_CORE_UARFCN_DL && freq >= BSP_RF_RX_FREQ_WCDMA_BC2_MIN && freq <= BSP_RF_RX_FREQ_WCDMA_BC2_MAX )
      {
        chan = (uint16)((freq * BSP_RF_FREQ_WCDMA_MULT_FACTOR)/1000);
      }
      else if( ul_dl == RFWCDMA_CORE_UARFCN_UL && freq >= BSP_RF_TX_FREQ_WCDMA_BC2_MIN && freq <= BSP_RF_TX_FREQ_WCDMA_BC2_MAX )
      {
        chan = (uint16)((freq * BSP_RF_FREQ_WCDMA_MULT_FACTOR)/1000);
      }
      else if( ( freq >= BSP_RF_RX_FREQ_WCDMA_BC2_ADDITIONAL_MIN ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_DL)
      {
        chan = (uint16)(( ( ( freq - 100 ) * BSP_RF_FREQ_WCDMA_MULT_FACTOR )/1000 )) - BSP_BC2_REG_TO_ADDITIONAL_CH_OFFSET;
      }
      else if( ( freq >= BSP_RF_TX_FREQ_WCDMA_BC2_ADDITIONAL_MIN ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_UL)
      {
        chan = ((uint16)( ( ( freq - 100 ) * BSP_RF_FREQ_WCDMA_MULT_FACTOR )/1000 )) - BSP_BC2_REG_TO_ADDITIONAL_CH_OFFSET;
      }
      break;

    case RFCOM_BAND_BC3:

        if( ul_dl == RFWCDMA_CORE_UARFCN_DL && freq >= BSP_RF_RX_FREQ_WCDMA_BC3_MIN && freq <= BSP_RF_RX_FREQ_WCDMA_BC3_MAX )
        {
          chan = (uint16)((BSP_RF_FREQ_WCDMA_MULT_FACTOR * ( freq - (BSP_RF_RX_BC3_FREQUENCY_OFFSET * 1000) ))/1000);
        }
        else if( ul_dl == RFWCDMA_CORE_UARFCN_UL && freq >= BSP_RF_TX_FREQ_WCDMA_BC3_MIN && freq <= BSP_RF_TX_FREQ_WCDMA_BC3_MAX )
        {
          chan = (uint16)((BSP_RF_FREQ_WCDMA_MULT_FACTOR * ( freq - (BSP_RF_TX_BC3_FREQUENCY_OFFSET * 1000) ))/1000);
        }
        break; 

    case RFCOM_BAND_BC9:
        
        if( ul_dl == RFWCDMA_CORE_UARFCN_DL && freq >= BSP_RF_RX_FREQ_WCDMA_BC9_MIN && freq <= BSP_RF_RX_FREQ_WCDMA_BC9_MAX )
        {
          chan = (uint16)((freq * BSP_RF_FREQ_WCDMA_MULT_FACTOR)/1000);
        }
        else if( ul_dl == RFWCDMA_CORE_UARFCN_UL && freq >= BSP_RF_TX_FREQ_WCDMA_BC9_MIN && freq <= BSP_RF_TX_FREQ_WCDMA_BC9_MAX )
        {
          chan = (uint16)((freq * BSP_RF_FREQ_WCDMA_MULT_FACTOR)/1000);
        }
        break;

    case RFCOM_BAND_BC4:

      if ( ul_dl == RFWCDMA_CORE_UARFCN_DL && freq >= BSP_RF_RX_FREQ_WCDMA_BC4_MIN && freq <= BSP_RF_RX_FREQ_WCDMA_BC4_MAX )
      {
        chan = (uint16)((BSP_RF_FREQ_WCDMA_MULT_FACTOR * ( freq - (BSP_RF_RX_BC4_FREQUENCY_OFFSET * 1000) ))/1000);
      }
      else if ( ul_dl == RFWCDMA_CORE_UARFCN_UL && freq >= BSP_RF_TX_FREQ_WCDMA_BC4_MIN && freq <= BSP_RF_TX_FREQ_WCDMA_BC4_MAX )
      {
        chan = (uint16)((BSP_RF_FREQ_WCDMA_MULT_FACTOR * ( freq - (BSP_RF_TX_BC4_FREQUENCY_OFFSET * 1000) ))/1000);
      }
      else if ( ( freq >= BSP_RF_RX_FREQ_WCDMA_BC4_ADDITIONAL_MIN ) &&
                ( freq <= BSP_RF_RX_FREQ_WCDMA_BC4_ADDITIONAL_MAX ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        chan =((uint16) ( ( ( freq - (BSP_RF_RX_BC4_FREQUENCY_OFFSET * 1000) - 100 ) * BSP_RF_FREQ_WCDMA_MULT_FACTOR )/1000 )) - BSP_BC4_REG_TO_ADDITIONAL_CH_OFFSET;
      }
      else if ( ( freq >= BSP_RF_TX_FREQ_WCDMA_BC4_ADDITIONAL_MIN ) &&
                ( freq <= BSP_RF_TX_FREQ_WCDMA_BC4_ADDITIONAL_MAX ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        chan = ((uint16)( ( ( freq - (BSP_RF_TX_BC4_FREQUENCY_OFFSET * 1000) - 100 ) * BSP_RF_FREQ_WCDMA_MULT_FACTOR )/1000 )) - BSP_BC4_REG_TO_ADDITIONAL_CH_OFFSET;
      }
      break;

       /* RFCN to Frequency calculation for WCDMA Band XIX */
    case RFCOM_BAND_BC19:
      if ( ul_dl == RFWCDMA_CORE_UARFCN_DL && freq >= BSP_RF_RX_FREQ_WCDMA_BC19_MIN && freq <= BSP_RF_RX_FREQ_WCDMA_BC19_MAX )
      {
        chan = (uint16)((BSP_RF_FREQ_WCDMA_MULT_FACTOR * ( freq - (BSP_RF_RX_BC19_FREQUENCY_OFFSET * 1000) ))/1000);
      }
      else if ( ul_dl == RFWCDMA_CORE_UARFCN_UL && freq >= BSP_RF_TX_FREQ_WCDMA_BC19_MIN && freq <= BSP_RF_TX_FREQ_WCDMA_BC19_MAX )
      {
        chan = (uint16)((BSP_RF_FREQ_WCDMA_MULT_FACTOR * ( freq - (BSP_RF_TX_BC19_FREQUENCY_OFFSET * 1000) ))/1000);
      }
      else if ( ( freq >= BSP_RF_RX_FREQ_WCDMA_BC19_ADDITIONAL_MIN && freq <= BSP_RF_RX_FREQ_WCDMA_BC19_ADDITIONAL_MAX ) &&
                  ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        chan = ((uint16)( ( ( freq - (BSP_RF_RX_BC19_FREQUENCY_OFFSET * 1000) - 100 ) * BSP_RF_FREQ_WCDMA_MULT_FACTOR )/1000 )) - BSP_BC19_REG_TO_ADDITIONAL_CH_OFFSET;
      }
      else if ( ( freq >= BSP_RF_TX_FREQ_WCDMA_BC19_ADDITIONAL_MIN && freq <= BSP_RF_TX_FREQ_WCDMA_BC19_ADDITIONAL_MAX ) &&
                  ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        chan =((uint16) ( ( ( freq - (BSP_RF_TX_BC19_FREQUENCY_OFFSET * 1000) - 100 ) * BSP_RF_FREQ_WCDMA_MULT_FACTOR )/1000 )) - BSP_BC19_REG_TO_ADDITIONAL_CH_OFFSET;
      }
      break;

    case RFCOM_BAND_800:

      if ( ( freq >= BSP_RF_RX_FREQ_WCDMA_BC5_MIN ) &&
           ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        chan =  (uint16)((freq * BSP_RF_FREQ_WCDMA_MULT_FACTOR)/1000);
      }
      else if ( ( freq >= BSP_RF_TX_FREQ_WCDMA_BC5_MIN ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        chan =  (uint16)((freq * BSP_RF_FREQ_WCDMA_MULT_FACTOR)/1000);
        MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"Converted Chan : %d", chan);
      }
      else if ( ( freq >= BSP_RF_RX_FREQ_WCDMA_BC5_ADDITIONAL_MIN ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_DL )
      {
        chan = ((uint16)( ( ( freq - 100 ) * BSP_RF_FREQ_WCDMA_MULT_FACTOR )/1000 )) - BSP_BC5_REG_TO_ADDITIONAL_CH_OFFSET;
      }
      else if ( ( freq >= BSP_RF_TX_FREQ_WCDMA_BC5_ADDITIONAL_MIN ) &&
                ul_dl == RFWCDMA_CORE_UARFCN_UL )
      {
        chan = ((uint16)( ( ( freq - 100 ) * BSP_RF_FREQ_WCDMA_MULT_FACTOR )/1000 )) - BSP_BC5_REG_TO_ADDITIONAL_CH_OFFSET;
        MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"Converted Chan (additional) : %d", chan);
      }
      break;

    case RFCOM_BAND_BC8:

      if ( ul_dl == RFWCDMA_CORE_UARFCN_DL && freq >= BSP_RF_RX_FREQ_WCDMA_BC8_MIN && freq <= BSP_RF_RX_FREQ_WCDMA_BC8_MAX )
      {
        chan = (uint16)((BSP_RF_FREQ_WCDMA_MULT_FACTOR * ( freq - (BSP_RF_RX_BC8_FREQUENCY_OFFSET * 1000) ))/1000);
      }
      else if ( ul_dl == RFWCDMA_CORE_UARFCN_UL && freq >= BSP_RF_TX_FREQ_WCDMA_BC8_MIN && freq <= BSP_RF_TX_FREQ_WCDMA_BC8_MAX )
      {
        chan = (uint16)((BSP_RF_FREQ_WCDMA_MULT_FACTOR * ( freq - (BSP_RF_TX_BC8_FREQUENCY_OFFSET * 1000) ))/1000);
      }

      break;

    default :
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid RF Band = %d", band);
  }

  return (chan);

} /* rfwcdma_core_get_freq_from_uarfcn */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function converts WCDMA uplink frequency to WCDMA band.

  @param freq Uplink Frequency.

  @return rfcom_wcdma_band_type WCDMA band
*/
rfcom_wcdma_band_type rfwcdma_core_convert_ul_freq_to_band( uint32 freq )
{
  rfcom_wcdma_band_type band;

  if ( ( freq >= BSP_RF_TX_FREQ_WCDMA_MIN ) && ( freq <= BSP_RF_TX_FREQ_WCDMA_MAX ) )
  {
    band =  RFCOM_BAND_IMT;
  } 
  else if ( ( ( freq >= BSP_RF_TX_FREQ_WCDMA_BC2_MIN ) && ( freq <= BSP_RF_TX_FREQ_WCDMA_BC2_MAX ) ) ||
              ( ( freq >= BSP_RF_TX_FREQ_WCDMA_BC2_ADDITIONAL_MIN ) &&
                ( freq <= BSP_RF_TX_FREQ_WCDMA_BC2_ADDITIONAL_MAX ) )
            )
  {
    band = RFCOM_BAND_1900;
  } 
  else if ( ( freq >= BSP_RF_TX_FREQ_WCDMA_BC3_MIN ) && ( freq <= BSP_RF_TX_FREQ_WCDMA_BC3_MAX ) )
  {
    band = RFCOM_BAND_BC3;
  } 
  else if ( ( ( freq >= BSP_RF_TX_FREQ_WCDMA_BC4_MIN ) && ( freq <= BSP_RF_TX_FREQ_WCDMA_BC4_MAX ) ) ||
              ( ( freq >= BSP_RF_TX_FREQ_WCDMA_BC4_ADDITIONAL_MIN ) &&
                ( freq <= BSP_RF_TX_FREQ_WCDMA_BC4_ADDITIONAL_MAX ) )
            )
  {
    band = RFCOM_BAND_BC4;
  } 
  else if ( ( ( freq >= BSP_RF_TX_FREQ_WCDMA_BC5_MIN ) && ( freq <= BSP_RF_TX_FREQ_WCDMA_BC5_MAX ) ) ||
              ( ( freq >= BSP_RF_TX_FREQ_WCDMA_BC5_ADDITIONAL_MIN ) &&
                ( freq <= BSP_RF_TX_FREQ_WCDMA_BC5_ADDITIONAL_MAX ) )
            )
  {
    band = RFCOM_BAND_800;
  } 
  else if ( ( freq >= BSP_RF_TX_FREQ_WCDMA_BC8_MIN ) && ( freq <= BSP_RF_TX_FREQ_WCDMA_BC8_MAX ) )
  {
    band = RFCOM_BAND_BC8;
  } 
  else if ( ( freq >= BSP_RF_TX_FREQ_WCDMA_BC9_MIN ) && ( freq <= BSP_RF_TX_FREQ_WCDMA_BC9_MAX ) )
  {
    band = RFCOM_BAND_BC9;
  } 
  else if ( ( freq >= BSP_RF_TX_FREQ_WCDMA_BC11_MIN ) && ( freq <= BSP_RF_TX_FREQ_WCDMA_BC11_MAX ) )
  {
    band = RFCOM_BAND_BC11;
  } 
  else if ( ( ( freq >= BSP_RF_TX_FREQ_WCDMA_BC19_MIN ) && ( freq <= BSP_RF_TX_FREQ_WCDMA_BC19_MAX ) )||
              ( ( freq >= BSP_RF_TX_FREQ_WCDMA_BC19_ADDITIONAL_MIN ) &&
                ( freq <= BSP_RF_TX_FREQ_WCDMA_BC19_ADDITIONAL_MAX ) )
            )
  {
    band = RFCOM_BAND_BC19;
  } 
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid Frequency=%d. Default to WCDMA 2100 or band I", freq);
    band =  RFCOM_BAND_IMT;
  }

  return (band);
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Convert WCDMA Tx Int Cal band enum type to RFCOM band enum type for WCDMA.
 
  @details
  The WCDMA Tx Int Cal band enum type is defined based on the goal NV for Tx
  Internal device cal (one bit per band). This is mapped to RFCOM_WCDMA_BAND_TYPE.
   
  @param
  rfwcdma_int_cal_bc_type wcdma_band.
  
  @return
  rfcom_wcdma_band_type is the return value based on the mapping.
  
*/
rfcom_wcdma_band_type 
convert_wcdma_int_cal_band_to_rfcom_band
(
  rfwcdma_int_cal_bc_type wcdma_band
)
{
    // Convert WCDMA Tx Int Cal BC TYPE to RFCOM_WCDMA_BAND_TYPE
    switch( wcdma_band )
    {
    case RFWCDMA_BAND_IMT:
        return( RFCOM_BAND_IMT );
    case RFWCDMA_BAND_1900:
        return( RFCOM_BAND_1900 );
    case RFWCDMA_BAND_BC3:
        return( RFCOM_BAND_BC3 );
    case RFWCDMA_BAND_BC4:
        return( RFCOM_BAND_BC4 );
    case RFWCDMA_BAND_800:
        return( RFCOM_BAND_800 );
    case RFWCDMA_BAND_BC8:
        return( RFCOM_BAND_BC8 );
    case RFWCDMA_BAND_BC9:
        return( RFCOM_BAND_BC9 );
    case RFWCDMA_BAND_BC11:
        return( RFCOM_BAND_BC11 );
    case RFWCDMA_BAND_BC19:
        return( RFCOM_BAND_BC19 );
    default:
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid band ID for FTM_WCDMA_BC_TYPE : %d", 
                  wcdma_band);
        return( RFCOM_NUM_WCDMA_BANDS );
    }
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Util function to extract RF resource script from RF buffer
 
  @details
  If no bus resource type found from the buffer, will return RF_HAL_BUS_RESOURCE_MAX
   
  @param
  buffer_ptr RF buffer pointer
  
  @return
  script_ptr RF bus script pointer
  
*/
const rf_hal_bus_resource_script_type* rfwcdma_core_get_resource_script_from_buffer
(
  rf_buffer_intf* buffer_ptr
)
{
  rf_hal_bus_resource_type bus_resource;
  const rf_hal_bus_resource_script_type* script_ptr = NULL;

  for (bus_resource = RF_HAL_BUS_RESOURCE_SSBI; bus_resource < RF_HAL_BUS_RESOURCE_MAX; bus_resource++)
  {
    script_ptr = rf_buffer_get_script_ptr(buffer_ptr, bus_resource);
    if (script_ptr != NULL && script_ptr->num_trans != 0)
    {
      /* valid RF script found*/
      return script_ptr;
    }
  }
  return NULL;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the RX channel number given the TX channel number
  and the band.

  @param tx_channel WCDMA channel number
  @param band WCDMA operational band

  @return uint16 Rx channel number
*/
uint16 rfwcdma_core_map_tx_to_rx_chan( uint16 tx_channel, rfcom_wcdma_band_type band )
{
  switch ( band )
  {
  case RFCOM_BAND_IMT:
    return (tx_channel + BSP_RF_WCDMA_BC1_RX_TX_CHAN_NUMBER_OFFSET);

  case RFCOM_BAND_1900:
    return (tx_channel + BSP_RF_WCDMA_BC2_RX_TX_CHAN_NUMBER_OFFSET);

  case RFCOM_BAND_BC3:
    return (tx_channel + BSP_RF_WCDMA_BC3_RX_TX_CHAN_NUMBER_OFFSET);

  case RFCOM_BAND_BC4:
    return (tx_channel + BSP_RF_WCDMA_BC4_RX_TX_CHAN_NUMBER_OFFSET);

  case RFCOM_BAND_800:
    return (tx_channel + BSP_RF_WCDMA_BC5_RX_TX_CHAN_NUMBER_OFFSET);

  case RFCOM_BAND_BC8:
    return (tx_channel + BSP_RF_WCDMA_BC8_RX_TX_CHAN_NUMBER_OFFSET);

  case RFCOM_BAND_BC9:
    return (tx_channel + BSP_RF_WCDMA_BC9_RX_TX_CHAN_NUMBER_OFFSET);

  case RFCOM_BAND_BC11:
    return (tx_channel + BSP_RF_WCDMA_BC11_RX_TX_CHAN_NUMBER_OFFSET);

  case RFCOM_BAND_BC19:
    return (tx_channel + BSP_RF_WCDMA_BC19_RX_TX_CHAN_NUMBER_OFFSET);

  default:
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfi_get_tx_chan_from_rx_chan: unknown band",0);
    return 0;
  }
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Convert band to middle DL chan
===========================================================================*/
uint16 rfwcdma_core_get_middle_DL_chan_from_band(rfcom_wcdma_band_type band)
{
   switch(band)	 
    {
     case  RFCOM_BAND_IMT:
      return 10700;
      
     case  RFCOM_BAND_1900:
      return 9800;

     case  RFCOM_BAND_BC3:
      return 1338;
      
     case  RFCOM_BAND_BC4:
      return 1638;

     case  RFCOM_BAND_800:
      return 4408;
      
     case  RFCOM_BAND_BC8:
      return 3013;

     case  RFCOM_BAND_BC9:
      return 9312;
      
     case  RFCOM_BAND_BC11:
      return 3750;

     case  RFCOM_BAND_BC19:
      return 738;

     default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid RFCOM WCDMA Band %d", band);
      return 0;
   }
}
