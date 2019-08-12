#ifndef RF_CDMA_UTILS_H
#define RF_CDMA_UTILS_H

/*! 
  @file
  rf_cdma_utils.h
 
  @brief
  This file defines definitions and prototypes used for common 1xHDR 
  utilities.
*/

/*==============================================================================

  Copyright (c) 2010 - 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_utils.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
07/17/14   spa     Added API to convert RFM band to RFC band
04/21/14   JJ      Added api to check is band is supported on given device
05/30/13   JJ      Change func input param from sys_band type to rfm_cdma type
05/23/13   JJ      change rf_cdma_get_turnaround_constant input param type
01/10/12   zhw     Move away from using common PA device state
12/09/12   zhw     API for converting CDMPA PA state to common device PA state
10/07/12   zhw     API for converting Cx1 to USTMR units.
09/21/12   zhw     Added API to convert PA state into PA gain range
07/09/12   cd      Added function to convert IQ gain offset from dB10 to HW 
                   units
02/29/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan 
12/20/11   aro     MC function to perform mapping between PA range and state
12/10/11   sty     Added base_freq_offset_hz as arg to 
                   rf_cdma_compute_carrier_info()
12/01/11   sty     [1] Added rf_cdma_band_to_rfm_card_band()
10/11/11   aro     Added interface to check high band and low band
10/11/11   aro     Added interface to convert PA Switchpoint from NV units to
                   dBm10
07/05/11   Saul    Added bandwidth computation.
06/21/11   Saul    Updates for multi-carrier driver.
06/14/11   sty     Invoke APIs to convert AGC to requested format
06/14/11   Saul    Now configuring CSR freq offsets.
06/13/11   Saul    Fixed doxygen documentation.
06/10/11   Saul    Updated CDMA BW defines.
06/10/11   Saul    Now configuring carriers based on mask.
06/10/11   cd      Removed unused rf_cdma_utils_interpolate_chan_offset()
06/09/11   Saul    Added support for 5X bandwidth.
06/09/11   Saul    RxLM updates for multi-carrier driver.
05/10/11   cd      Removed rf_cdma_utils_get_freq_bin()
04/21/11   aro     Added Get Calibration compensation function
03/17/11   bmg     Moved rf_cdma_get_freq_from_channel to rf_cdma_utils_freq.h
03/14/11   sty     Added C++ protection
03/09/11   sty     Reverted previous change
03/09/11   sty     Changed arg type in rf_cdma_get_tx_min_max_pwr
03/04/11   sty     Added rf_cdma_get_turnaround_constant()
03/03/11   sty     Fixed warnings
03/03/11   sty     Deleted RF_SATURATE - rf_limit_s32(0 provides the same 
                   functionality
03/03/11   sty     Added rf_cdma_get_tx_min_max_pwr() and RF_SATURATE
02/01/11   aro     Added Channel Offset interpolation function
01/31/11   aro     Added CDMA saturate Macro
01/19/11   aro     Added MC BW enumeration
12/29/10   aro     Removed unused Macro
12/29/10   aro     Added function to return Frequency Bin for given band/chan
12/29/10   aro     Added SC BW enumeration
12/13/10   aro     Added CDMA utility function
12/10/10   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "rfcom.h"
#include "sys.h"
#include "rfm_cdma_carrier_types.h"
#include "rfm_cdma_agc_types.h"
#include "rfm_cdma_band_types.h"
#include "rfm_device_types.h"
#include "rfm_gain_state_types.h"
#include "rfdevice_pa_intf.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! These are the largest bandwidhts in KHz 
    that the RF driver will consider to be Nx. */

/*! Define for CDMA carrier bandwidth invalid */
#define RF_CDMA_BW_INVALID 0 /* INVALID */
/*! Define for CDMA carrier bandwidth 1X */
#define RF_CDMA_BW_1X 1250 /* 1X */
/*! Define for CDMA carrier bandwidth 3X */
#define RF_CDMA_BW_3X 3950 /* 3X */
/*! Define for CDMA carrier bandwidth 5X */
#define RF_CDMA_BW_5X 6650 /* 5X */
/*! Define for CDMA carrier bandwidth 7X */
#define RF_CDMA_BW_7X 9350 /* 7X */

/*----------------------------------------------------------------------------*/
/*! Macro to Saturate the value within Min and Max */
#define RF_CDMA_SATURATE( val, min, max )  MAX( MIN( (val), (max) ), (min) )

/*----------------------------------------------------------------------------*/
rfm_cdma_band_class_type
rf_cdma_band_to_rfm_card_band
(
  rf_card_band_type rf_card_band
);

/*----------------------------------------------------------------------------*/
rf_card_band_type
rf_cdma_band_to_card_band
(
  rfcom_1x_band_type rf1x_band
);

/*----------------------------------------------------------------------------*/
uint32 
rf_cdma_compute_bandwidth
( 
  const uint32 num_band_chan,
  const rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type * const chan_array
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_compute_carrier_info
( 
  rfm_cdma_band_class_type band,
  const uint16 * const chans,
  uint32 num_carriers, 
  const uint16 * const carrier_index,
  rfm_cdma_carrier_en_mask_type* carrier_mask,
  int32* csr_freq_offsets_hz,
  int32 base_freq_offset_hz
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_is_high_band
(
  rfm_cdma_band_class_type band
);

/*----------------------------------------------------------------------------*/
void 
rf_cdma_get_tx_min_max_pwr 
(
  rfm_cdma_band_class_type const band,
  int32  *p_tx_min_pwr, 
  int32 *p_tx_max_pwr
);

/*----------------------------------------------------------------------------*/
int
rf_cdma_get_turnaround_constant
(
  rfm_cdma_band_class_type const band
);

/*----------------------------------------------------------------------------*/
int16
rf_cdma_get_calibration_compensation
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan
);

/*----------------------------------------------------------------------------*/
int16
rf_cdma_convert_rxagc
(
  const rf_cdma_agc_format_type format,
  const int16 raw_rxagc
);

/*----------------------------------------------------------------------------*/
int16
rf_cdma_convert_txagc
(
  const rf_cdma_agc_format_type format,
  const int16 turnaround_const,
  const int16 raw_txagc
);

/*----------------------------------------------------------------------------*/
int16
rf_cdma_covert_pa_switchpoint_to_dbm
(
  int16 pa_sp_nv,
  rfm_cdma_band_class_type band
);

/*----------------------------------------------------------------------------*/
rfm_pa_gain_state_type
rf_cdma_map_pa_range_to_pa_state
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfm_pa_range_type pa_range
);

/*----------------------------------------------------------------------------*/
rfm_pa_range_type
rf_cdma_map_pa_state_to_gain_range
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfm_pa_gain_state_type pa_state
);

/*----------------------------------------------------------------------------*/
uint64
rf_cdma_utils_compute_tx_iq_gain_adj_val
(
  int8 dBx10_offset
);

/*----------------------------------------------------------------------------*/
int32
rf_cdma_convert_cx1_to_ustmr_units
(
  int32 cx1_units
);

/*----------------------------------------------------------------------------*/
sys_sband_e_type
rf_cdma_convert_sys_band_to_sys_sband
( 
  sys_band_class_e_type band
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_is_band_supported
(
  const rfm_device_enum_type device, 
  const rfm_cdma_band_class_type band
);

/*----------------------------------------------------------------------------*/
rf_card_band_type
rf_cdma_convert_rfm_band_to_card_band
(
  rfm_cdma_band_class_type rfm_band
); 

#ifdef __cplusplus
}
#endif

#endif /* RF_CDMA_UTILS_H */
