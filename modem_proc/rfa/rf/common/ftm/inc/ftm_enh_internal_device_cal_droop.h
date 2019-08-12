#ifndef FTM_ENH_INTERNAL_DEVICE_CAL_DROOP_H
#define FTM_ENH_INTERNAL_DEVICE_CAL_DROOP_H
/*!
  @file
  ftm_enh_internal_device_cal_droop.h

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_enh_internal_device_cal_droop.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
02/10/15    br    Send tech,band,bandwidth and fbrx gain state as inputs to stg enable and disable apis 
08/30/14   jmf    Add STG tone power return from FBRx Droop Cal FTM API
08/18/14   jmf    New functions to compute Complex FBRx Droop Filter
04/08/14   jmf    Updates to accommodate error code returns / Droop Filter Processing
02/06/14   jmf    Updated FBRx Droop Cal API prototype
01/21/14   jmf    Add support to update global NV structure for Droop Filter
12/30/13   jmf    Initial Revision

==============================================================================*/

//#include "rfa_variation.h"
#include "comdef.h"
//#include "rfm_device_types.h"
#include "ftmdiag.h"
#include "ftm.h"
#include "rfcommon_fbrx_nv.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef T_WINNT
#error code not present
#endif


boolean ftm_enh_internal_device_cal_fb_droop_config_fbrx(void);

boolean ftm_enh_internal_device_cal_fb_droop_config_stg(void);

boolean ftm_enh_internal_device_cal_fb_droop_calc_coeffs(void);

boolean ftm_enh_internal_device_cal_fb_droop_update_global_nv_struct(void);

boolean ftm_enh_internal_device_cal_fb_droop_get_global_nv_struct(void);

ftm_enh_internal_device_cal_error_code_type ftm_enh_internal_device_cal_fb_droop_stg_sequence(uint8 tx_device,
                                                          uint8 num_captures,
                                                          int32 * stg_offset_khz,
                                                          uint16 num_iq_samples,
                                                          uint8 debug_mode_en,
                                                          uint8 do_processing,
                                                          uint8 fbrx_gain_state
                                                          );

ftm_enh_internal_device_cal_error_code_type ftm_enh_internal_device_cal_fb_droop_stg_cplx(uint8 tx_device,
                                                          uint8 num_captures,
                                                          int32 * stg_offset_khz,
                                                          uint16 num_iq_samples,
                                                          uint8 debug_mode_en,
                                                          uint8 do_processing,
                                                          uint32 * tonePwr,
                                                          uint8 fbrx_gain_state
                                                          );


ftm_enh_internal_device_cal_error_code_type ftm_enh_internal_device_cal_fb_droop_mod_sequence(void);

boolean ftm_enh_internal_device_cal_fb_droop_iq_capture(void);

boolean ftm_enh_internal_device_cal_fb_droop
(
  void const *fb_droop_instr,
  uint16 *res_len,
  void *fb_droop_result 
);
	

ftm_enh_internal_device_cal_error_code_type 
ftm_enh_internal_device_cal_droop_stg_process(
   ftm_enh_internal_device_cal_fbrx_droop_cal_method_type alg_type, 
   uint8 num_captures, 
   int32 * stg_offset_khz, 
   uint8 * samp_buffers,
   uint16 num_samps
   );

ftm_enh_internal_device_cal_error_code_type 
ftm_enh_internal_device_cal_calc_tone_pwr(
   uint8 samp_buf_id, 
   uint16 num_iq_samples, 
   int32 stg_offset_khz, 
   uint32 samp_rate, 
   uint32* tonePwr
);

ftm_enh_internal_device_cal_error_code_type
ftm_enh_internal_device_cal_calc_nom_eq_gain_sq(
  uint16 * eqGainSq, 
  uint8 * numNomEq, 
  uint8 fbrx_bw, 
  int32 * stg_offset_khz,
  uint8 num_captures, 
  uint32 samp_rate,
  uint8 * filter_techbw
);

ftm_enh_internal_device_cal_error_code_type
ftm_enh_internal_device_cal_calc_eq
(
   uint8 num_captures, 
   uint32 * tonePwrs, 
   int32 * stg_offset_khz, 
   uint16 * eqGainSq, 
   uint8 * filter_techbw, 
   uint8 numNomEq, 
   uint32 refTonePower , 
   int32 refFreq
);


#ifdef T_WINNT
#error code not present
#endif

#ifdef __cplusplus
}
#endif

#endif /* FTM_ENH_INTERNAL_DEVICE_CAL_RSB_H */




