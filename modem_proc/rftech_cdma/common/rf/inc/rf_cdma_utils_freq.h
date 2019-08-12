#ifndef RF_CDMA_UTILS_FREQ_H
#define RF_CDMA_UTILS_FREQ_H

/*! 
  @file
  rf_cdma_utils_freq.h
 
  @brief
  CDMA RF driver utility functions specifically for dealing with freqency
  computations.
*/

/*==============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_utils_freq.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/08/14   adk     Added function prototype rf_cdma_get_min_max_tx_freqs()
05/30/13   JJ      Add function rf_cdma_rf_band_type_to_sys_band_type
05/20/13   sty     Code cleanup
04/15/13   APU     Added band chan validity for spreading rate 1 as per the 
                   standard.
02/05/13    kg     Added API's to return rfcom type band mask and max/min
                   freq based on band
03/22/12    id     Added function to convert freq to chan
03/08/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan 
12/01/11   sty     [1] Moved rx_freq_khz_to_chan_preference_type to this file
                   [2] Added rf_cdma_get_rx_inter_chan_separation() and 
                       rf_cdma_convert_rx_freq_khz_to_chan()
07/18/11   aro     Fixed the channel return type
06/22/11   Saul    Added test for Rx and Tx channel computations
06/22/11   Saul    Expanded tune chan computation to handle non-contiguous chans.
06/13/11   Saul    Now computing center channel for tuning.
03/17/11   bmg     Moved rf_cdma_get_freq_from_channel to this file
                   Added rf_cdma_sys_band_type_to_rf_band_type() to translate
                   between SYS band types and RF CDMA band types.
03/15/11   bmg     Initial Revision.  Based on the old rf_freq_utils module.

==============================================================================*/

#include "rfm_cdma_band_types.h"
#include "sys.h" /* For sys_channel_type in rf_cdma_get_freq_from_channel */
#include "rfcom.h" /* For rfcommon_channel_ul_dl_type in rf_cdma_get_freq_from_channel */

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! CDMA Band mask definitions */

#define RF_CDMA_BC0_MASK_IDX  0
#define RF_CDMA_BC1_MASK_IDX  1
#define RF_CDMA_BC2_MASK_IDX  2
#define RF_CDMA_BC3_MASK_IDX  3
#define RF_CDMA_BC4_MASK_IDX  4
#define RF_CDMA_BC5_MASK_IDX  5
#define RF_CDMA_BC6_MASK_IDX  6
#define RF_CDMA_BC7_MASK_IDX  7
#define RF_CDMA_BC8_MASK_IDX  8
#define RF_CDMA_BC9_MASK_IDX  9
#define RF_CDMA_BC10_MASK_IDX 10
#define RF_CDMA_BC11_MASK_IDX 11
#define RF_CDMA_BC12_MASK_IDX 12
#define RF_CDMA_BC13_MASK_IDX 13
#define RF_CDMA_BC14_MASK_IDX 14
#define RF_CDMA_BC15_MASK_IDX 15
#define RF_CDMA_BC16_MASK_IDX 16
#define RF_CDMA_BC17_MASK_IDX 17
#define RF_CDMA_BC18_MASK_IDX 18
#define RF_CDMA_BC19_MASK_IDX 19
#define RF_CDMA_BC20_MASK_IDX 20


#define RF_CDMA_BC0_MASK   (uint64)(1<<(uint64)(RF_CDMA_BC0_MASK_IDX))
#define RF_CDMA_BC1_MASK   (uint64)(1<<(uint64)(RF_CDMA_BC1_MASK_IDX))
#define RF_CDMA_BC2_MASK   (uint64)(1<<(uint64)(RF_CDMA_BC2_MASK_IDX))
#define RF_CDMA_BC3_MASK   (uint64)(1<<(uint64)(RF_CDMA_BC3_MASK_IDX))
#define RF_CDMA_BC4_MASK   (uint64)(1<<(uint64)(RF_CDMA_BC4_MASK_IDX))
#define RF_CDMA_BC5_MASK   (uint64)(1<<(uint64)(RF_CDMA_BC5_MASK_IDX))
#define RF_CDMA_BC6_MASK   (uint64)(1<<(uint64)(RF_CDMA_BC6_MASK_IDX))
#define RF_CDMA_BC7_MASK   (uint64)(1<<(uint64)(RF_CDMA_BC7_MASK_IDX))
#define RF_CDMA_BC8_MASK   (uint64)(1<<(uint64)(RF_CDMA_BC8_MASK_IDX))
#define RF_CDMA_BC9_MASK   (uint64)(1<<(uint64)(RF_CDMA_BC9_MASK_IDX))
#define RF_CDMA_BC10_MASK  (uint64)(1<<(uint64)(RF_CDMA_BC10_MASK_IDX))
#define RF_CDMA_BC11_MASK  (uint64)(1<<(uint64)(RF_CDMA_BC11_MASK_IDX))
#define RF_CDMA_BC12_MASK  (uint64)(1<<(uint64)(RF_CDMA_BC12_MASK_IDX))
#define RF_CDMA_BC13_MASK  (uint64)(1<<(uint64)(RF_CDMA_BC13_MASK_IDX))
#define RF_CDMA_BC14_MASK  (uint64)(1<<(uint64)(RF_CDMA_BC14_MASK_IDX))
#define RF_CDMA_BC15_MASK  (uint64)(1<<(uint64)(RF_CDMA_BC15_MASK_IDX))
#define RF_CDMA_BC16_MASK  (uint64)(1<<(uint64)(RF_CDMA_BC16_MASK_IDX))
#define RF_CDMA_BC17_MASK  (uint64)(1<<(uint64)(RF_CDMA_BC17_MASK_IDX))
#define RF_CDMA_BC18_MASK  (uint64)(1<<(uint64)(RF_CDMA_BC18_MASK_IDX))
#define RF_CDMA_BC19_MASK  (uint64)(1<<(uint64)(RF_CDMA_BC19_MASK_IDX))
#define RF_CDMA_BC20_MASK  (uint64)(1<<(uint64)(RF_CDMA_BC20_MASK_IDX))

/*----------------------------------------------------------------------------*/
/*! CDMA Tx Boundry freq in Khz */
#define RF_CDMA_BC0_TX_MIN_FREQ   815040
#define RF_CDMA_BC0_TX_MAX_FREQ   848970
#define RF_CDMA_BC1_TX_MIN_FREQ   1850000
#define RF_CDMA_BC1_TX_MAX_FREQ   1909950
#define RF_CDMA_BC3_TX_MIN_FREQ   887025
#define RF_CDMA_BC3_TX_MAX_FREQ   924975
#define RF_CDMA_BC4_TX_MIN_FREQ   1750000
#define RF_CDMA_BC4_TX_MAX_FREQ   1779950
#define RF_CDMA_BC5_TX_MIN_FREQ   410000
#define RF_CDMA_BC5_TX_MAX_FREQ   483480
#define RF_CDMA_BC6_TX_MIN_FREQ   1920000
#define RF_CDMA_BC6_TX_MAX_FREQ   1979950
#define RF_CDMA_BC10_TX_MIN_FREQ  806000
#define RF_CDMA_BC10_TX_MAX_FREQ  900975
#define RF_CDMA_BC11_TX_MIN_FREQ  411675
#define RF_CDMA_BC11_TX_MAX_FREQ  483480
#define RF_CDMA_BC14_TX_MIN_FREQ  1850000
#define RF_CDMA_BC14_TX_MAX_FREQ  1914950
#define RF_CDMA_BC15_TX_MIN_FREQ  1710000
#define RF_CDMA_BC15_TX_MAX_FREQ  1754950

/*! CDMA Tx Boundry freq in Khz */
#define RF_CDMA_BC0_RX_MIN_FREQ	860040
#define RF_CDMA_BC0_RX_MAX_FREQ	893970
#define RF_CDMA_BC1_RX_MIN_FREQ	1930000
#define RF_CDMA_BC1_RX_MAX_FREQ	1989950
#define RF_CDMA_BC3_RX_MIN_FREQ	832025
#define RF_CDMA_BC3_RX_MAX_FREQ	869975
#define RF_CDMA_BC4_RX_MIN_FREQ	1840000
#define RF_CDMA_BC4_RX_MAX_FREQ	1869950
#define RF_CDMA_BC5_RX_MIN_FREQ	420000
#define RF_CDMA_BC5_RX_MAX_FREQ	493480
#define RF_CDMA_BC6_RX_MIN_FREQ	2110000
#define RF_CDMA_BC6_RX_MAX_FREQ	2169950
#define RF_CDMA_BC10_RX_MIN_FREQ	851000
#define RF_CDMA_BC10_RX_MAX_FREQ	939975
#define RF_CDMA_BC11_RX_MIN_FREQ	421675
#define RF_CDMA_BC11_RX_MAX_FREQ	493480
#define RF_CDMA_BC14_RX_MIN_FREQ	1930000
#define RF_CDMA_BC14_RX_MAX_FREQ	1994950
#define RF_CDMA_BC15_RX_MIN_FREQ	2110000
#define RF_CDMA_BC15_RX_MAX_FREQ	2154950


/*----------------------------------------------------------------------------*/
/*! @brief
    Enum used to indicate channel preference to
    rf_convert_rx_freq_khz_to_chan() API
*/
typedef enum
{
  RX_FREQ_KHZ_TO_CHAN_PREFERENCE__LOWER_FREQ, /*!< Low Freq */
  RX_FREQ_KHZ_TO_CHAN_PREFERENCE__HIGHER_FREQ, /*!< High Freq */
  RX_FREQ_KHZ_TO_CHAN_PREFERENCE__ANY, /*!< No Preference */
  RX_FREQ_KHZ_TO_CHAN_PREFERENCE__INVALID /*!< Invalid */
} rx_freq_khz_to_chan_preference_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Table structure to define the channel to frequency mapping for a set of
  channels within a band.

  @details
  The structure of the channel to frequency mapping math is
    base_freq + (ch_sp * (chan + ch_offst)).
  This structure defines a range of channels for a band and the frequency
  computation parameters for that range of channels.
*/
typedef struct 
{
  /*!
    @brief
    The band sub class this record is for.
  */
  int16 sub_class ;

  /*!
    @brief
    The smallest channel for which this record is valid.
  */
  int16 lo_chan ;

  /*!
    @brief
    The largest channel for which this record is valid.
  */
  int16 hi_chan ;

  /*!
    @brief The rf_cdma_band_descriptor table holds all the valid
    entries for a given Band Class. This flag indicates if the
    given group of channels in a given band sub class is
    conditionally valid if at all valid. This flag has 
    meaning only for bands with sub classes. A value of 0 means
    the channel group is valid without any conditions. A value
    of 1 will mean that the given channel group is conditionally
    valid and for determining the validity the next adjacent sub
    class must be marked as valid */
  boolean is_channel_group_conditionally_valid ;
} rf_cdma_band_descriptor ; 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Table structure to define the channel to frequency mapping for a set of
  channels within a band.

  @details
  The structure of the channel to frequency mapping math is
    base_freq + (ch_sp * (chan + ch_offst)).
  This structure defines a range of channels for a band and the frequency
  computation parameters for that range of channels.
*/
typedef struct 
{
  /*!
    @brief
    The band class this record is for.
  */
  rfm_cdma_band_class_type band;

  /*!
    @brief
    The smallest channel for which this record is valid.
  */
  int16 lo_chan;

  /*!
    @brief
    The largest channel for which this record is valid.
  */
  int16 hi_chan;

  /*!
    @brief
    Spacing in 1/10th KHz between consecutive channels.

    @details
    Examples: 50 KHz channel spacing would be encoded as 500.
    30 KHz channel spacing would be encoded as 300.
    12.5 KHz channel spacing would be encoded as 125.
  */
  int16 ch_sp;

  /*!
    @brief
    A constant that is added to the channel number before it is
    multiplied by the channel spacing.

    @details
    The channel offset may be used to change the "base" of the
    channel numbering scheme relative to the bottom of the band.
    For example, if a band consists of 1MHz channels starting
    at 800MHz, and the first channel number is 34, it may be more
    convenient to note the frequency as 1MHz * (chan - 34) + 800MHz.

    This offset is often seen in the 3GPP2 C.S0057 CDMA band class
    definition.  While not strictly necessary, this factor makes
    it easier to translate the document to the software.
  */
  int16 ch_offst;

  /*!
    @brief
    The minimum step size of the channel in this range.

    @details
    Most bands define channels as consequitive non-negative integers.
    However BC3 only allows even-numbered channels.  This parameter
    is used to denote that the valid channels in this entry are
    [lo_chan, lo_chan + 1*ch_step, lo_chan + 2*ch_step, ..., hi_chan]
  */
  int16 ch_step;

  /*!
    @brief
    The base frequency offset that is applied to the channel raster
    for the channels in this range.
  */
  int32 base_freq;
} rf_carrier_freq_type;

/*----------------------------------------------------------------------------*/
const 
rf_cdma_band_descriptor*
rf_get_band_descriptor 
(  
  rfm_cdma_band_class_type band , 
  size_t* size 
) ;

/*----------------------------------------------------------------------------*/
const 
rf_carrier_freq_type *
rf_cdma_get_rx_freq_tbl
( 
  uint16 * size
);

/*----------------------------------------------------------------------------*/
const 
rf_carrier_freq_type *
rf_cdma_get_tx_freq_tbl
( 
  uint16 * size
);

/*----------------------------------------------------------------------------*/
rfm_cdma_band_class_type
rf_cdma_sys_band_type_to_rf_band_type
(
  sys_band_class_e_type band
);

/*----------------------------------------------------------------------------*/
sys_band_class_e_type 
rf_cdma_rf_band_type_to_sys_band_type
(
  rfm_cdma_band_class_type band
);

/*----------------------------------------------------------------------------*/
rfm_cdma_chan_type 
rf_cdma_compute_center_tune_chan
(
  const uint32 num_band_chan,
  const rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type * const chan_array
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_convert_rx_freq_khz_to_chan
(
  uint32 const                       chan_freq_khz,
  rfm_cdma_band_class_type const     band_class,
  rx_freq_khz_to_chan_preference_type const mid_point_preference,
  word                               *const chan_ptr,
  uint32                             *const converted_chan_freq_khz_ptr
);

/*----------------------------------------------------------------------------*/
uint32
rf_cdma_get_tx_carrier_freq
(
  rfm_cdma_band_class_type band,
  int chan
);

/*----------------------------------------------------------------------------*/
uint32
rf_cdma_get_rx_carrier_freq
(
  rfm_cdma_band_class_type band,
  int chan
);

/*----------------------------------------------------------------------------*/
int32
rf_cdma_get_rx_inter_chan_separation
(
 rfm_cdma_band_class_type const band
);

/*----------------------------------------------------------------------------*/
uint16
rf_cdma_get_tx_chan_from_freq
(
  rfm_cdma_band_class_type band,
  uint32 freq
);

/*----------------------------------------------------------------------------*/
uint64 
rf_cdma_get_band_mask
(
  rfm_cdma_band_class_type band
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_get_min_max_rx_freqs
(
  rfm_cdma_band_class_type band, uint32* min, uint32* max
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_get_min_max_tx_freqs
(
  rfm_cdma_band_class_type band, uint32* min, uint32* max
);

/*----------------------------------------------------------------------------*/
uint64 
rf_cdma_get_sys_band_mask
(
  rfm_cdma_band_class_type band
);

#ifdef __cplusplus
}
#endif

#endif /* RF_CDMA_UTILS_FREQ_H */

