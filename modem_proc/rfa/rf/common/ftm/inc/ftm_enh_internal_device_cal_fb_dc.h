#ifndef FTM_ENH_INTERNAL_DEVICE_CAL_DC_H
#define FTM_ENH_INTERNAL_DEVICE_CAL_DC_H
/*!
  @file
  ftm_enh_internal_device_cal_dc.h

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_enh_internal_device_cal_fb_dc.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
02/10/15    br     Send tech,band,bandwidth and fbrx gain state as inputs to stg enable and disable apis 
04/08/14   jmf     Updates to accommodate error code returns
02/07/2014 br     Updated FBRx DC cal API prototype
01/21/14   br    Add support to update global NV structure for DC FB Rx cal
12/30/13   br   Initial Revision

==============================================================================*/

//#include "rfa_variation.h"
#include "comdef.h"
//#include "rfm_device_types.h"
#include "ftmdiag.h"
#include "ftm.h"
#include "ftm_common_enh_internal_device_cal.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef T_WINNT
#error code not present
#endif

/*---------------------------FB Rx DC cal-------------------------------------------------*/

#define MAX_NUM_GAIN_STATES 4
#define NUM_DC_COMPONENT 2


ftm_enh_internal_device_cal_error_code_type ftm_enh_internal_device_cal_fb_dc_capture_sequence
(
  rfm_mode_enum_type rf_mode,
  rfcom_bw_type_u rfcom_bw,                                   
  uint8 fbrx_gain_state,
  uint8 num_captures,
  uint16 num_iq_samples,
  uint8 debug_mode_en
);


//boolean ftm_enh_internal_device_cal_fb_droop_mod_sequence(void);

boolean ftm_enh_internal_device_cal_fb_dc_iq_capture(void);
boolean ftm_enh_internal_device_cal_fb_dc
(
  void const *req_instr,
  uint16 * res_len,
  void *fb_dc_result 
);

boolean ftm_enh_internal_device_cal_fb_dc_process
(
   uint8 cap_buf_id, 
   uint16 num_iq_samples,
   uint16 discard_samps,
   int32 * dc_val_i, 
   int32 * dc_val_q
);

boolean ftm_enh_internal_device_cal_fb_dc_global_nv
(
 uint8 debug_mode_en,
 rfm_mode_enum_type fbrx_rfm_mode,
 ftm_rf_technology_type fbrx_tech,
 uint16 fbrx_band,
 uint16 nv_band,
 uint8 fbrx_mode_index,
 uint8 fbrx_gain_state,
 int32 * dc_i_nv,
 int32 * dc_q_nv,
 int32 dc_val_i,
 int32 dc_val_q,
 ftm_enh_internal_device_cal_data_type *internal_device_cal_data
 );

/*
  @brief
  Structure to hold FBRx Result Data
*/

typedef PACK(struct)
{
// tech-specific fbrx mode that has been internally written to NV
   uint32 fbrx_mode_nv;
   // the gain state for which dc value is calibrated
   uint8 fbrx_gain_state[MAX_NUM_GAIN_STATES];
   //the dc value calibrated
   int32  dcval[MAX_NUM_GAIN_STATES][NUM_DC_COMPONENT];
}ftm_fbrx_dc_result_data_type; 

/*
  @brief
  Structure to hold FBRx DC Calibration Data
*/
typedef PACK(struct)
{
  ftm_fbrx_dc_result_data_type fbrx_dc_res;
 
} ftm_enh_internal_device_cal_fb_dc_result_type;

/*
@brief
  Structure template for Internal_Device_Cal_FB_DC Instruction.

  @details
  Contains the FB DC Internal Cal  instruction (Payload)
*/
typedef PACK(struct)
{
  

   uint8 fbrx_gain_state[MAX_NUM_GAIN_STATES];
  // dc values to be obtained for  all the possible gain states

  uint8 num_fbrx_gain_states;
  // number of gain states; 


  uint16 num_iq_samples; 
  // number of IQ samples for each of num_captures captures
} ftm_enh_internal_device_cal_instr_payload_fb_dc_type;

boolean ftm_enh_internal_device_cal_fb_dc_config_fbrx(void);


boolean ftm_enh_internal_device_cal_fb_dc_update_global_nv_struct(void);

boolean ftm_enh_internal_device_cal_fb_dc_get_global_nv_struct(void);

#ifdef T_WINNT
#error code not present
#endif

#ifdef __cplusplus
}
#endif

#endif /* FTM_ENH_INTERNAL_DEVICE_CAL_DC_H */




