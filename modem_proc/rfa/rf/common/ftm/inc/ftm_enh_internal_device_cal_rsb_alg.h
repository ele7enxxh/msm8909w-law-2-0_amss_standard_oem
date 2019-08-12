#ifndef FTM_ENH_INTERNAL_DEVICE_CAL_RSB_ALG_H
#define FTM_ENH_INTERNAL_DEVICE_CAL_RSB_ALG_H
/*!
  @file
  ftm_enh_internal_device_cal_rsb.h

  @brief
  This module contains prototypes and definitions used by enhanced internal calibration  code.
*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_enh_internal_device_cal_rsb_alg.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
02/2/14   xsh    Initial Revision

==============================================================================*/

#include "comdef.h"
//#include "rfm_device_types.h"
#include "ftmdiag.h"
#include "ftm.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef T_WINNT
#error code not present
#endif

/*----------------------------------------------------------------------------*/
/*!
   It defines the rsb struct.
*/
typedef struct 
{
  boolean rsb_calibration_required; 
  int16 error_gain_cold_x1000; 
  int16 error_gain_room_x1000; 
  int16 error_gain_hot_x1000; 
  int16 error_phase_cold_x1000; 
  int16 error_phase_room_x1000; 
  int16 error_phase_hot_x1000; 
  int16 sin_phase_room_x2pow14; 
  int16 cos_phase_room_x2pow14; 
  int16 error_gain_room_x2pow14; 
  uint16 rf_chain; 
} rsb_three_thermal_range_data_type;

#define RSB_SINCOS_LUT_SIZE 201
#define RSB_GAIN_LUT_SIZE 1001

/*----------------------------------------------------------------------------*/
/*!
   It defines the  rsb_sincos enum.
*/
typedef enum 
{
  RSB_COSINE, 
  RSB_SINE, 
  RSB_SINCOS_NUM, 
  RSB_SINCOS_INVALID, 
} rsb_sincos_data_type;


/*----------------------------------------------------------------------------*/
/*!
   It defines the rsb_gain enum.
*/
typedef enum 
{
  GAIN_POSITIVE, 
  GAIN_NEGATIVE, 
  RSB_GAIN_NUM, 
  RSB_GAIN_INVALID, 
} rsb_gain_data_type;


double 	compute_rsb
(
	rfdevice_rsb_cal_data_type rsb_data
);

void common_compute_rsb_cal_params_gain_phase_lut_index
(
	uint64 i_sq, uint64 q_sq, int64 iq, 
	uint16 therm_read,
	uint8 stg_is_bad_fuse, 
	const rsb_three_thermal_range_data_type* g_ph_vec, 
	int16* sin_val, int16* cos_val, int16* gain_inv_val, 
	uint16* gain_lut_index, uint16* phase_lut_index, 
	int8* gain_sign, int8* phase_sign
);

void common_compute_rsb_cal_nv_params
(
	int16 sin_val, 
	int16 cos_val, 
	int16 gain_inv_val, 
	int16 sin_delta, 
	int16 cos_delta, 
	int16 gain_inv_delta, 
	int16* sin_theta_final, 
	int16* cos_theta_final, 
	int16* gain_inv_final
);

boolean ftm_compute_rsb_nv_params
( 
  rfm_device_enum_type rfm_device,
  uint8  tech,
  uint16 rf_band, 
  rfcom_lte_bw_type rx_bw,
  uint64   i_sq,
  uint64   q_sq,
  int64    i_q,
  int16    therm_data, 
  int16*   sin_theta_final, 
  int16*   cos_theta_final, 
  int16*   gain_inv_final,
  uint8 stg_is_bad_fuse
);

void ftm_common_compute_rsb_coeffs_a_b_params(int16 sin_val, int16 cos_val, int16 gain_inv_val, int16 sin_delta, 
	int16 cos_delta, int16 gain_inv_delta, uint8 rsb_cal_is_on,
	int32* a_coeff, int32* b_coeff);



#ifdef T_WINNT
#error code not present
#endif

#ifdef __cplusplus
}
#endif

#endif /* FTM_ENH_INTERNAL_DEVICE_CAL_RSB_ALG_H */




