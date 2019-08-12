#ifndef FTM_ENH_INTERNAL_DEVICE_CAL_RSB_H
#define FTM_ENH_INTERNAL_DEVICE_CAL_RSB_H
/*!
  @file
  ftm_enh_internal_device_cal_rsb.h

  @brief
  This module contains prototypes and definitions used by enhanced internal calibration  code.
*/

/*==============================================================================

  Copyright (c) 2013 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_enh_internal_device_cal_rsb.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
02/10/15    br     Send tech,band,bandwidth and fbrx gain state as inputs to stg enable and disable apis
07/09/14   jmf     Redefine APIs for performing RSB coeff calc with DC removed
05/29/14   brath   Updates to provide TX LO frequency for device cal APIs
4/22/14    xsh     adding dynamic fbrx lm rsb update 
4/15/14    xsh     hooking to fbrx rsb cal to rx rsb cal
10/02/13   xsh     Initial Revision

==============================================================================*/

//#include "rfa_variation.h"
#include "comdef.h"
//#include "rfm_device_types.h"
#include "ftmdiag.h"
#include "ftm.h"
#include "rflm_api_cmn.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef T_WINNT
#error code not present
#endif

/*! 
  @brief
  Items to be stored in EFS after RSB cal per tech, band, path on WTR1625
*/
typedef struct
{
  /*! Indicates if cal was performed successfully and hence nv/efs
    for the band/path is active */
  boolean nv_active;

  /*! Sine theta estimate at room temp */
  int16 sin_theta;

  /*! Cosine theta estimate at room temp */
  int16 cos_theta;

  /*! Gain inv estimatee at room temp */
  int16 gain_inv;

}rsb_cal_data_type;

typedef enum
{
  FTM_RX_RSB_IQ_8bit_to_16bit = 0,
  FTM_RX_RSB_IQ_16bit_to_16bit,  
  FTM_RX_RSB_IQ_FORMAT_MAX 
}ftm_rxrsb_cal_iq_format_conversion;


boolean ftm_save_rsb_nv_global_structure
( rfm_device_enum_type device, 
	uint8 tech,
	int16 band,
	uint8 rx_path,
	int16 sinThetaFinal, 
	int16 cosThetaFinal, 
	int16 gainInvFinal
	);

boolean ftm_enh_device_cal_rsb_get_thermistor
(
	rfm_device_enum_type device, 
	uint16 band, 
	uint16 * enh_therm_val
);

boolean get_rxlm_handler
(
	uint8 tech, 
	rflm_handle_rx_t *rxlm_buffer_idx,
	rflm_tech_id_t *rflm_tech_id
);

boolean get_fbrx_lm_handler
(
  ftm_rf_technology_type ftm_rf_tech,
  rflm_handle_rx_t *txlm_buffer_idx, 
  rflm_tech_id_t *rflm_tech_id
);

boolean ftm_convert_rsb_iq_format
( 
  uint32* iq_ptr,  
  boolean reverseOrder,
  uint8 convert_type,
  uint16 sample_size
);


boolean ftm_enh_internal_device_cal_rsb
(
  void const *req_instr,
  uint16 *res_len,
  void *rsp_payload 
);


boolean ftm_enh_internal_device_cal_save_iq
( 
  ftm_rf_technology_type ftm_rf_tech,
  uint16 band,
  uint8 rxPath, 
  uint32 *iq_data, 
  int16 sampleSize,  
  uint8 is_rsb_updated
);

boolean ftm_enh_internal_device_cal_rx_iq_capture
(
  ftm_rf_technology_type ftm_rf_tech,
  uint8 carrier,
  uint8 rx_path,
  uint16 sample_size, 
  uint32* iq_dst_ptr
);

uint16 ftm_calv4_convert_band
(
  ftm_rf_technology_type ftm_rf_tech,
  uint16 rf_band
);

ftm_enh_internal_device_cal_error_code_type ftm_enh_internal_device_cal_rsb_stg_on
(
  rfdevice_rxtx_common_class * device_ptr,
  int32 stg_offset_khz,
  uint8 debug_mode_en,
  uint64* stg_freq_hz,
  rfm_mode_enum_type mode,
  rfcom_band_type_u band,
  rfcom_bw_type_u bw,
  uint8 fbrx_gain_state
);

ftm_enh_internal_device_cal_error_code_type ftm_enh_internal_device_cal_get_stg_freq
(
  rfdevice_rxtx_common_class * device_ptr,
  int32 stg_offset_khz,
  uint8 debug_mode_en,
  uint64 * stg_freq
);

ftm_enh_internal_device_cal_error_code_type ftm_enh_internal_device_cal_fbrx_rsb_iq_capture
(
  rfdevice_rxtx_common_class * device_ptr,
  uint8 rsb_cal_alg,
  int32 stg_offset_khz,
  uint16 num_iq_samples,
  uint8 debug_mode_en,
  uint32 * IQSamples,
  uint32 * dcCompVals,
  rfcom_band_type_u band_union,
  rfcom_bw_type_u rfcom_bw,
  uint8 fbrx_gain_state
);

ftm_enh_internal_device_cal_error_code_type ftm_enh_internal_device_cal_rsb_fetch_iq
(
  uint8  samp_buffers,
  uint16 num_samps,
  uint8 debug_mode_en,
  uint32 * IQSamples,
  uint32 * dcCompVals
);

#ifdef T_WINNT
#error code not present
#endif

#ifdef __cplusplus
}
#endif

#endif /* FTM_ENH_INTERNAL_DEVICE_CAL_RSB_H */




