/*!
  @file
  fw_dpd_intf.h

  @brief
  DPD SWI interface definitions
*/

/*===========================================================================

  Copyright (c) 2012-13 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_common/api/fw_dpd_intf.h#1 $
$DateTime: 2016/12/13 08:00:08 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/--/14   DG      Add SW specified Tx upsampling.
12/13/13   DG      Use longer equalizers (3,4 tap) to improve EVM measurements.
12/13/13   DG      Use equalizer which provides better measurement (based on equalizer gain.)
12/13/13   DG      SW specifies thresholds for equalizer selection.
12/13/13   DG      Export equalizer selection metrics.
12/13/13   DG      Discard initial Tx, Rx samples.
10/25/13   DG      Apply frequency offset to Tx samples.
10/25/13   DG      Apply channelization filter to Rx samples.
09/18/13   DG      Add ET envelope path delay metric -- next peak.
07/22/13   DG      Add EVM capture -- compute EVM from RefMode=1 (Tx I,Q).
06/24/13   DG      Compute correlation gain, ET path delay metrics.
04/29/13   DG      SWI changes for ET delay, FBRx cal -- SWI only, no code.
04/29/13   DG      SWI+API changes to separate sample capture and processing.
01/09/13   DG      Add DPD table, distortion scaling to FW output SWI.
01/09/13   DG      Add flag to control DPD construction to FW input SWI.
11/07/12   NB      Port from NikeL. Initial Dime changes. Add Dime SWI.
===========================================================================*/

#ifndef _FW_DPD_INTF_H_
#define _FW_DPD_INTF_H_

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                             CONSTANTS

===========================================================================*/

//! @brief
// Calibration AM, PM distortion table sizes -- same size as sample capture
// Tx magnitude.
//
#define FW_DPD_DISTORTION_SIZE 256

//!
//! @brief
// Calibration AM, PM pre-distortion table size.
//
#define FW_DPD_CAL_DPD_SIZE 16

//! @brief
// Maximum capture size -- 12K samples.
//
#define FW_DPD_MAX_CAPTURE_SAMPLES (12 * 1024)

//! @brief
// Cache line size.
// This symbol may be defined in multiple places.  There is no single place
// for this definition to be inherited by FW and SW. The compiler generate a
// warning the redundantdefinitions are not identical.
//
#define CACHE_LINE_SZ 32

//! @brief
// Maximum uint32 size of Tx sample captures -- 16 bits per sample.
//
// Allocate an additional cache line to accommodate start of sample capture
// not aligned to cache line.
//
#define FW_DPD_MAX_TX_CAPTURE_SIZE32 \
  ((FW_DPD_MAX_CAPTURE_SAMPLES * 16 + 31 + (CACHE_LINE_SZ * 8)) / 32)

//! @brief
// Maximum uint32 size of Rx sample captures -- enough for 12K samples.
//
// Size for 32 bits per sample. Size for 20 bit, bit packed samples,
// FW_DPD_MAX_RX_CAPTURE_SIZE32 = ((FW_DPD_MAX_CAPTURE_SAMPLES * 20 + 31) / 32)
//
#define FW_DPD_MAX_RX_CAPTURE_SIZE32 (FW_DPD_MAX_CAPTURE_SAMPLES)

//! @brief
// Maximum size of Rx bandwidth/channelization filter.
//
#define FW_DPD_MAX_RX_BW_FIRLEN 20

//! @brief
// Minimum size of Tx upsample filter. To accommodate code designed to optimize
// FIR cycles, ((firlen - 1) / 2) >= 1.
//
#define FW_DPD_MIN_TX_UP_FIRLEN 3

//! @brief
// Maximum size of Tx upsample filter.
//
#define FW_DPD_MAX_TX_UP_FIRLEN 30

//! @brief
// Define special value for fw_dpd_cal_config_t.tx_freq used to indicate that
// the Tx samples should be rotated to match the rotation applied by current
// TxC rotator configuration. This value corresponds to 1/(2^32) Hz/sample,
// smaller than any rotation supported by TxC. The corresponding phase shift,
// 0.0008 degrees by end of 9K capture is small enough that excluding this
// frequency does not impact performance.
//
#define FW_DPD_TX_FREQ_TXC_ROT 1

/*===========================================================================

                                TYPES

===========================================================================*/

//! @brief
// Capture processing parameters common to all captures.
//
typedef struct
{
  //! @brief
  // Initial Tx samples to discard.
  //
  uint16 tx_discard;

  //! @brief
  // Initial Rx samples to discard.
  //
  uint16 rx_discard;

  //! @brief
  // Guard space for split sample captures.
  //
  uint16 split_margin;

  //! @brief
  // Expected Rx offset of correlation between Rx and Tx samples
  //
  int16 search_center;

  //! @brief
  // Rx-Tx correlation uncertainty.
  //
  uint8 search_width;

  //! @brief
  // First used summation bin. Lower bins are assumed to be too noisy and are
  // ignored.
  //
  uint8 first_bin;

  //! @brief
  // First upper bin used extrapolating upper bins to compute end bin.
  //
  uint8 extrap_bin;

  //! @brief
  // Minimum number of samples required for merged bin.
  //
  uint8 min_bin_count;

  //! @brief
  // Minimum number of samples required for first merged bin -- larger then
  // min_bin_count.
  //
  uint8 min_first_bin_count;

  //! @brief
  // Minimum number of samples for using upper end bin without merging --
  // larger then min_bin_count.
  //
  uint8 min_last_bin_count;

  //! @brief
  // Flag to enable Rx spectral inversion: 0=no, 1=yes.
  //
  uint8 rx_spec_inv;

  //! @brief
  // Flag to enable DPD construction: 0=no (pre-distortion only), 1=yes.
  //
  uint8 construct_dpd;

  //! @brief
  // Tx upsample filter length.
  //
  uint8 tx_up_fir_len;

  //! @brief
  // Tx upsample filter shift (-(Q factor).)
  //
  int8 tx_up_fir_shift;

  //! @brief
  // Tx upsample filter coefficients.
  // Must be INT32 aligned to allow reading coefficients two at a time.
  //
  int16 tx_up_fir [FW_DPD_MAX_TX_UP_FIRLEN] ALIGN(4);

  //! @brief Tx frequency offset in Hz/sample in Q32.
  //
  int32 tx_freq;

  //! @brief
  // Rx bandwidth/channelization filter length.
  //
  uint8 rx_bw_fir_len;

  //! @brief
  // Rx bandwidth/channelization filter shift (-(Q factor).)
  //
  int8 rx_bw_fir_shift;

  //! @brief
  // Rx bandwidth/channelization filter coefficients.
  //
  int16 rx_bw_fir [FW_DPD_MAX_RX_BW_FIRLEN];

  //! @brief
  // Bit mask of equalizers for EVM captures.
  //
  uint8 eq_mask;

  //! @brief
  // Maximum equalizer gain for EVM captures. If gain is higher than
  // threshold, reduce equalizer length. Continue until either gain is
  // acceptable or the minimum equalizer length is reached. Units: 0.01 dB.
  //
  int16 max_eq_gain;

} fw_dpd_cal_config_t;

//! @brief
// Raw 64-bit sums from FBRx sample processing.
//
typedef struct
{
  int64 txi; //!< @brief Sum of TxI.
  int64 txq; //!< @brief Sum of TxQ.
  int64 rxi; //!< @brief Sum of RxI.
  int64 rxq; //!< @brief Sum of RxQ.
  int64 txi_txi; //!< @brief Sum of TxI^2.
  int64 txi_txq; //!< @brief Sum of TxI * TxQ.
  int64 txi_rxi; //!< @brief Sum of TxI * RxI.
  int64 txi_rxq; //!< @brief Sum of TxI * RxQ.
  int64 txq_txq; //!< @brief Sum of TxQ^q.
  int64 txq_rxi; //!< @brief Sum of TxQ * RxI.
  int64 txq_rxq; //!< @brief Sum of RxQ * RxQ.
  int64 rxi_rxi; //!< @brief Sum of RxI^2.
  int64 rxi_rxq; //!< @brief Sum of RxI * RxQ.
  int64 rxq_rxq; //!< @brief Sum of RxQ^2.
} fw_dpd_fbrx_sum_t;

//! @brief
// Per-capture DPD calibration results.
//
// This structure is used for FW output during calibration.
//
typedef struct
{
  //! @brief
  // Calibration status: (0)=success, (>0)=failure
  //
  uint8 status;

  //! @brief
  // Capture status (bit flags.)
  //
  uint8 capture_stat;

  //! @brief
  // Equalizer length.
  //
  uint8 eq_len;

  //! @brief
  // AM distortion table shift. Multiply table entries by 2^am_dist_shift.
  //
  int8 am_dist_shift;

  //! @brief
  // Equalization FIR shift. Multiply by coefficients by 2^eq_fir_shift.
  //
  int16 eq_fir_shift;

  //! @brief
  // Tx RMS magnitude (x 100).
  //
  uint16 tx_rms_mag;

  //! @brief
  // Rx post-EQ RMS magnitude (x 100).
  //
  uint16 eq_rms_mag;

  //! @brief
  // Rx peak offset relative to search center.
  //
  int16 peak_offset;

  //! @brief
  // Correlation gain (dB x 100).
  //
  int16 corr_gain;

  //! @brief
  // Equalizer gain (dB x 100).
  //
  int16 eq_gain;

  //! @brief
  // Post-EQ Signal-To-Noise ratio (dB x 100).
  //
  int16 snr;

  //! @brief
  // Post-EQ Error Vector Magnitude (percent x 100).
  //
  int16 evm;

  //! @brief
  // Post-EQ Peak-To-Average ratio (x 100).
  //
  int16 par;

  //! @brief
  // Post-EQ Peak-To-Average ratio for max peak (x 100).
  //
  int16 par_max;

  //! @brief
  // Rx DC I.
  //
  int32 rx_dc_i;

  //! @brief
  // Rx DC Q.
  //
  int32 rx_dc_q;

  //! @brief
  // Rx pre-EQ RMS magnitude (after DC cancellation.)
  //
  uint32 rx_rms_mag;

  //! @brief
  // Equalization FIR.
  //
  int32 eq_fir_coeff [2] /* 2 tap */ [2] /* I,Q */;

  // Overlay non-concurrent fields.
  //
  union
  {
    struct
    {
      //! @brief
      // AM distortion table.
      //
      uint16 am_dist [FW_DPD_DISTORTION_SIZE];

      //! @brief
      // PM distortion table.
      //
      int16 pm_dist [FW_DPD_DISTORTION_SIZE];

      //! @brief
      // AM pre-distortion table.
      //
      uint16 am_dpd [FW_DPD_CAL_DPD_SIZE];

      //! @brief
      // AM pre-distortion table.
      //
      int16 pm_dpd [FW_DPD_CAL_DPD_SIZE];

    }; // struct

    //! @brief
    // ET path delay results.
    //
    struct
    {
      //! @brief
      // IQ path delay (relative to search center) in sample/2^10 units.
      //
      int32 iq_delay;

      //! @brief
      // Envelope path delay (relative to search center) in sample/2^10 units.
      //
      int32 env_delay;

      //! @brief
      // Envelope delay Tx RMS magnitude (x 100).
      //
      uint32 env_tx_rms_mag;

      //! @brief
      // Envelope delay Rx RMS magnitude (after DC cancellation.)
      //
      uint32 env_rx_rms_mag;

      //! @brief
      // Envelope delay correlation gain (dB x 100).
      //
      int16 env_corr_gain;

      //! @brief
      // Envelope delay fraction of peak above DC (dB x 100).
      //
      int16 env_above_dc;

      //! @brief
      // Envelope delay ratio of next peak to primary peak (dB x 100).
      //
      int16 env_next_peak;

    }; // struct

    //! @brief
    // FBRx results.
    //
    struct
    {
      //! @brief
      // FBRx sample processing sums.
      //
      fw_dpd_fbrx_sum_t fbrx_sum;

    }; // struct

  }; // union

} fw_dpd_cal_result_t;

#endif // _FW_DPD_INTF_H_
