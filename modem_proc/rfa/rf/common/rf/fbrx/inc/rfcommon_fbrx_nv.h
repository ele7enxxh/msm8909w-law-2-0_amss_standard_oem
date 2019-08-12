#ifndef RFCOMMON_FBRX_NV_H
#define RFCOMMON_FBRX_NV_H
/*!
  @file
  rfcommon_fbrx_nv.h

  @brief
  This module contains interface of common FBRx NV data.
  
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/fbrx/inc/rfcommon_fbrx_nv.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/17/14    vb     LTE band and bandwidth specific Droop cal support
06/10/14    ka     Support for RB-based freq comp  
05/15/14    aa     Update tx freq for temp comp 
05/05/14    ka     Added fbrx gain freq comp 
04/02/14    aa     Moved rfcommon_fbrx_nv_get_control_val api
03/21/14    aa     Fix reload nv items for fbrx
03/14/14   jmf     Function to read/update DC values in global struct
03/13/14    ka     Update nv control for mtpl 
03/07/14    aa     Latest FBRx changes for droop cal
02/27/14    aa     Use rfcom_band_type_u 
02/21/13    aa     Added support for fbrx control via nv 
02/10/14    aa     Added support to read DC & temp comp cal data 
02/05/14    aa     Updated params for droop cal data
01/30/14    aa     Updated gain cal params & added fbrx mode support
01/21/14   jmf     Droop Filter Coeff structure update
01/14/14    aa     Added support to read FBRx NV's during bootup
01/10/14    aa     Backing out change due to RFCal crash
01/08/14    aa     Added support to read FBRx NV's during bootup 
12/16/13    aa     Added fbrx related struc & api's from rfcommon_nv.h file 
11/25/13    aa     Added api to update FBRX Droop Cal data
11/21/13    aa     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfcom.h"
#include "rfnv_items.h"
#include "nv.h"
#include "rfcommon_fbrx_api.h"
#include "rfcommon_nv.h"
#include "rflm_api_fbrx.h"
#include "ftm.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define RFCOMMON_FBRX_NUM_GAIN_STATES 4
#define RFCOMMON_FBRX_NUM_FILTER_TAPS 32
#define RFCOMMON_FBRX_GAIN_VS_FREQ_NUM 2    /* Num of FBRx gain states for 9x25 support */
#define RFCOMMON_FBRX_ATT_SWITCH_PTS_NUM 10
#define RFCOMMON_FBRX_NUM_TEMP_BINS 8
#define RFCOMMON_FBRX_NUM_TEMP_FREQ 16

#define RFCOMMON_FBRX_CONTROL_BITMASK 0x0000000F /* Bit mask for FBRx control settings */

#define RFCOMMON_FBRX_MAX_LTE_BW_COMBOS 8 /* Max number of LTE Bandwidths that are allowed. Can be expanded based on future needs */

typedef struct
{
  boolean wcdma_init;
  boolean cdma_init;
  boolean lte_init;
  boolean tds_init;
}rfcommon_fbrx_nv_init_type;

typedef struct
{
  boolean  data_valid;
  int16  gain;
}rfcommon_fbrx_gain_table_type;

typedef PACK (struct)
{
  /* Number of valid entries in channel list per band */
  uint8  fbrx_gain_chan_size;		  
  /* Channel list , Max list size is 16 */
  uint16 fbrx_gain_chan[RFCOMMON_FBRX_NUM_CAL_CHANNELS];
  /* Variant Marker */
  rfnv_data_variant_marker_type rfnv_data_variant_marker;
  /* list of FBRX gain for each channel and gain stage */
  rfcommon_fbrx_gain_table_type fbrx_gain[RFCOMMON_FBRX_NUM_CAL_CHANNELS][RFCOMMON_FBRX_GAIN_VS_FREQ_NUM];
}rfcommon_fbrx_nv_gain_vs_freq_type;

typedef PACK (struct)
{
  /* Number of valid switch point entries */
  uint8  num_switch_pts;		   
  /* Power in dBm for the fbrx switch points */
  int16 switch_pwr_dbm[RFCOMMON_FBRX_ATT_SWITCH_PTS_NUM];
} rfcommon_fbrx_nv_attn_switch_pt_type;

typedef PACK (struct)
{
  uint8 nv_version;       /* Reserved : Version info */
  uint8 group_delay;      /* Rx Filter Group Delay*/
  uint8 num_filter_taps;  /* Number of FIR filter coefficients to be used */
  int32 filter_coeff[RFCOMMON_FBRX_NUM_FILTER_TAPS];   /* FIR filter coefficients */
}rfcommon_fbrx_nv_droop_type;


/* Datastructures to store LTE droop NVs */
typedef PACK (struct)
{
  /* LTE bandwidth */
  rfcommon_fbrx_tx_cfg_type bw_cfg;      
  
  /* Params for droop cal for various LTE bandwidths */
  rfcommon_fbrx_nv_droop_type droop_params;  
}rfcommon_fbrx_nv_lte_bw_droop_params_type;

typedef PACK (struct)
{
  /* Number of BW payloads that are populated in this NV */
  uint8 num_bw_payloads_populated;      
  
  /* Droop params for various LTE bandwidths.
     !@Note: At max droop params can be stored for
     RFCOMMON_FBRX_MAX_LTE_BW_COMBOS number of bandwidths */
  rfcommon_fbrx_nv_lte_bw_droop_params_type bw_params[RFCOMMON_FBRX_MAX_LTE_BW_COMBOS];
}rfcommon_fbrx_nv_lte_band_droop_params_type;

typedef struct
{
  int32 dc_i;
  int32 dc_q;
} rfcommon_fbrx_dc_params_type;

typedef struct
{
  rfcommon_fbrx_dc_params_type gain_state[RFCOMMON_FBRX_NUM_GAIN_STATES];
} rfcommon_fbrx_dc_num_gain_type;

typedef struct
{
  int16 temp[RFCOMMON_FBRX_NUM_TEMP_BINS][RFCOMMON_FBRX_NUM_TEMP_FREQ];
} rfcommon_fbrx_temp_params_type;

typedef PACK (struct)
{
  uint8 nv_version;
  rfcommon_fbrx_dc_num_gain_type fbrx_mode[RFCOMMON_FBRX_NUM_MODES];
}rfcommon_fbrx_nv_dc_type;

typedef struct
{
  rfcommon_fbrx_temp_params_type gain_state[RFCOMMON_FBRX_NUM_GAIN_STATES];
} rfcommon_fbrx_temp_num_gain_type;

typedef PACK (struct)
{
  uint8 nv_version;
  rfcommon_fbrx_temp_num_gain_type fbrx_mode[RFCOMMON_FBRX_NUM_MODES];
}rfcommon_fbrx_nv_temp_type;

typedef struct
{
  rfcommon_fbrx_nv_gain_vs_freq_type_v2 *gain_data[RFM_CDMA_MAX_BAND];
  rfcommon_fbrx_nv_attn_switch_pt_type *sp_data[RFM_CDMA_MAX_BAND];
  rfcommon_fbrx_nv_dc_type *dc_data[RFM_CDMA_MAX_BAND];
  rfcommon_fbrx_nv_temp_type *temp_comp_data[RFM_CDMA_MAX_BAND];
}rfcommon_fbrx_nv_1x_data_type;

typedef struct
{
  rfcommon_fbrx_nv_gain_vs_freq_type_v2 *gain_data[RFCOM_NUM_WCDMA_BANDS];
  rfcommon_fbrx_nv_attn_switch_pt_type *sp_data[RFCOM_NUM_WCDMA_BANDS];
  rfcommon_fbrx_nv_dc_type *dc_data[RFCOM_NUM_WCDMA_BANDS];
  rfcommon_fbrx_nv_temp_type *temp_comp_data[RFCOM_NUM_WCDMA_BANDS];
}rfcommon_fbrx_nv_wcdma_data_type;

typedef struct
{
  rfcommon_fbrx_nv_gain_vs_freq_type_v2 *gain_data[RFCOM_NUM_LTE_BANDS];
  rfcommon_fbrx_nv_attn_switch_pt_type *sp_data[RFCOM_NUM_LTE_BANDS];
  rfcommon_fbrx_nv_dc_type *dc_data[RFCOM_NUM_LTE_BANDS];
  rfcommon_fbrx_nv_temp_type *temp_comp_data[RFCOM_NUM_LTE_BANDS];
  rfcommon_fbrx_nv_lte_band_droop_params_type *droop_data[RFCOM_NUM_LTE_BANDS];
}rfcommon_fbrx_nv_lte_data_type;

typedef struct
{
  rfcommon_fbrx_nv_gain_vs_freq_type_v2 *gain_data[RFCOM_NUM_TDSCDMA_BANDS];
  rfcommon_fbrx_nv_attn_switch_pt_type *sp_data[RFCOM_NUM_TDSCDMA_BANDS];
  rfcommon_fbrx_nv_dc_type *dc_data[RFCOM_NUM_TDSCDMA_BANDS];
  rfcommon_fbrx_nv_temp_type *temp_comp_data[RFCOM_NUM_TDSCDMA_BANDS];
}rfcommon_fbrx_nv_tdscdma_data_type;

typedef struct
{
  rfcommon_fbrx_nv_1x_data_type cdma_cal_data;
  rfcommon_fbrx_nv_wcdma_data_type wcdma_cal_data;
  rfcommon_fbrx_nv_lte_data_type lte_cal_data;
  rfcommon_fbrx_nv_tdscdma_data_type tdscdma_cal_data;
}rfcommon_fbrx_nv_cal_data_type;

typedef struct
{
  uint16 gain;
  int16  tx_pwr;
  uint16 predet_index;
}rfcommon_fbrx_nv_gain_freq_comp_type;

/*----------------------------------------------------------------------------*/

rfcommon_fbrx_error_type rfcommon_fbrx_get_wcdma_cal_nv_id
( 
  rfcom_wcdma_band_type band, 
  rfnv_item_id_enum_type *nv_item_gain, 
  rfnv_item_id_enum_type *nv_item_sp,
  rfnv_item_id_enum_type *nv_item_dc,
  rfnv_item_id_enum_type *nv_item_temp   
);

rfcommon_fbrx_error_type rfcommon_fbrx_get_1x_cal_nv_id
( 
  rfm_cdma_band_class_type band, 
  rfnv_item_id_enum_type *nv_item_gain, 
  rfnv_item_id_enum_type *nv_item_sp,
  rfnv_item_id_enum_type *nv_item_dc,
  rfnv_item_id_enum_type *nv_item_temp
);

rfcommon_fbrx_error_type rfcommon_fbrx_get_tdscdma_cal_nv_id
( 
  rfcom_tdscdma_band_type band, 
  rfnv_item_id_enum_type *nv_item_gain, 
  rfnv_item_id_enum_type *nv_item_sp,
  rfnv_item_id_enum_type *nv_item_dc,
  rfnv_item_id_enum_type *nv_item_temp
);

rfcommon_fbrx_error_type rfcommon_fbrx_get_lte_cal_nv_id
( 
  rfcom_lte_band_type band, 
  rfnv_item_id_enum_type *nv_item_gain, 
  rfnv_item_id_enum_type *nv_item_sp,
  rfnv_item_id_enum_type *nv_item_dc,
  rfnv_item_id_enum_type *nv_item_temp
);

rfcommon_fbrx_error_type rfcommon_fbrx_get_droop_cal_nv_id
( 
  rfcommon_fbrx_tx_cfg_type tx_cfg, 
  rfnv_item_id_enum_type *nv_item_droop 
);

rfcommon_fbrx_error_type rfcommon_fbrx_get_cal_nv_items
( 
  rfm_mode_enum_type tech,
  rfcom_band_type_u band,
  rfnv_item_id_enum_type *nv_item_gain,  
  rfnv_item_id_enum_type *nv_item_sp,
  rfnv_item_id_enum_type *nv_item_dc,
  rfnv_item_id_enum_type *nv_item_temp
);

rfcommon_fbrx_error_type
rfcommon_fbrx_nv_load_droop
(
  void *fbrx_droop_cal_nv_data,
  rfnv_item_id_enum_type fbrx_droop_cal_item_id,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
);

rfcommon_fbrx_error_type
rfcommon_fbrx_nv_load_dc
(
  void *fbrx_dc_cal_nv_data,
  rfnv_item_id_enum_type fbrx_dc_cal_item_id,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
);

rfcommon_fbrx_error_type
rfcommon_fbrx_nv_load_temp
(
  void *fbrx_temp_comp_cal_nv_data,
  rfnv_item_id_enum_type fbrx_temp_comp_cal_item_id,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
);

rfcommon_fbrx_error_type
rfcommon_fbrx_nv_extract_gain
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  rfcommon_fbrx_nv_gain_vs_freq_type *fbrx_gain_nv_data
);

rfcommon_fbrx_error_type
rfcommon_fbrx_nv_extract_gain_v2
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  rfcommon_fbrx_nv_gain_vs_freq_type_v2 *fbrx_gain_nv_data
);

rfcommon_fbrx_error_type
rfcommon_fbrx_nv_load_gain
(
  void *fbrx_gain_nv_data,
  rfnv_item_id_enum_type fbrx_gain_data_item,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
);

rfcommon_fbrx_error_type
rfcommon_fbrx_nv_load_attn_switch_pts
(
  rfcommon_fbrx_nv_attn_switch_pt_type *fbrx_attn_switch_pts_nv_data,
  rfnv_item_id_enum_type fbrx_attn_switch_pts_data_item,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
);

rfcommon_fbrx_error_type
rfcommon_fbrx_nv_update_dm
( 
  rflm_fbrx_dm_template_t* fbrx_dm_ptr, 
  rfm_mode_enum_type tech,
  rfcom_band_type_u band,
  uint16 chan_array[],                                       
  uint8 num_channels
);

rfcommon_fbrx_error_type rfcommon_fbrx_override_droop_globals
( 
  ftm_rf_technology_type ftm_tech,
  rfcom_band_type_u band_in_use,
  rfcommon_fbrx_tx_cfg_type tx_cfg,
  rfcommon_fbrx_nv_droop_type *droop_data
);

rfcommon_fbrx_error_type rfcommon_fbrx_read_current_droop_globals
( 
  ftm_rf_technology_type ftm_tech,
  rfcom_band_type_u band_in_use,
  rfcommon_fbrx_tx_cfg_type tx_cfg,
  rfcommon_fbrx_nv_droop_type *droop_data
);

rfcommon_fbrx_error_type
rfcommon_fbrx_nv_read_dc_globals( rfm_mode_enum_type tech,
                                  uint16 band,
                                  uint8 fbrx_mode_index,
                                  uint8 gain_state_index,
                                  int32 * dc_i,
                                  int32 * dc_q);

rfcommon_fbrx_error_type
rfcommon_fbrx_nv_update_dc_globals( rfm_mode_enum_type tech,
                                    uint16 band,
                                    uint8 fbrx_mode_index,
                                    uint8 gain_state_index,
                                    int32 dc_i,
                                    int32 dc_q);

boolean 
rfcommon_fbrx_nv_allocate_mem ( rfm_mode_enum_type tech,
                                rfcom_band_type_u band  );

boolean 
rfcommon_fbrx_nv_deallocate_mem ( rfm_mode_enum_type tech,
                                  rfcom_band_type_u band  );

rfcommon_fbrx_nv_gain_freq_comp_type rfcommon_fbrx_nv_get_freq_comp_gain
(
  rfm_mode_enum_type tech,
  rfcom_band_type_u band,
  rfcommon_fbrx_nv_gain_vs_freq_type_v2 *gain_ptr,
  int32 tx_freq,
  uint8  mode_idx,
  uint8  gs_idx,
  rflm_fbrx_dm_template_t* fbrx_dm_ptr
);

#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_FBRX_NV_H */
