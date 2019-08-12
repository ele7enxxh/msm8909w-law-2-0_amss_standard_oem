#ifndef FTM_CALIBRATION_DATA_MODULE_H
#define FTM_CALIBRATION_DATA_MODULE_H

/*!
  @file
  ftm_calibration_data_module.h

  @brief
  This module contains defintions and prototypes. This file is used to
  declare all Factory Test Mode functionality related to XPT.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_calibration_data_module.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/25/15   aa      AMPM scaling factor computation
09/03/14   cd      Remove legacy Tx override fields from data module
07/10/14   cd/cri  Merge Pin Cal
03/14/14   zhh    use debug_mode_en to control some ET debug info
03/06/14   zhw     Normalized Pin Cal support
01/08/14   cac     Add RGI delta table for EPT ET in different PA states
12/27/13   rsr     Add compression point in dB10 to DPD map
11/14/13   cd      Support for RFLM-based Tx AGC override
09/10/13   shk     Support to pack M Line results in the response packet
07/30/13   kg      [xPT] Change ftm_cal_data_rgid_row_type uint32 power
                   to int32 power
07/17/13   jmf     [xPT] New Delay Table separate from DPD data module
07/12/13   jmf     [xPT] Define DPD Map Max Num 20
06/21/13   jmf     [xPT] ET delay implementation for Cal V4
04/30/13   cd      Added API for Pin alignment
04/12/13   cri     Add APIs to get and set unity HWIDs
04/11/13   cri     Use XPT mode in power lookup for uniqueness
04/09/13   cri     Add init and deinit for cal data module
04/08/13   cri     Support for 2nd sweep of XPT calv4
03/29/13   cri     Moving the data module to rfcommon_data
03/05/13   cri     Initial check in for data module

==============================================================================*/

#include "rfcom.h"
#include "ftm.h"
#include "rfcommon_mdsp_types.h"
#include "rfcommon_core_txlin_types.h"
#include "rfcommon_core_xpt.h"
#include "fw_dpd_intf.h"
#include "ftm_common_xpt.h"

#ifdef __cplusplus
   extern "C"{
#endif

#define FTM_COMMON_XPT_NUM_DPD_MAP_ROWS 32
#define FTM_COMMON_XPT_NUM_DELAY_MAP_ROWS 48
#define FTM_COMMON_XPT_NUM_PIN_MAP_ROWS 16
#define XPT_AM_PM_LUT_SIZE 256
#define XPT_DET_LUT_SIZE 128
#define FTM_COMMON_XPT_INV_CHAN 0xFFFFFFFF
#define FTM_COMMON_XPT_INV_CHAN_IDX 0xFF
#define FTM_COMMON_XPT_INV_OVR_ROW 0xFF

/*! @brief Enum for XPT data status. */
typedef enum
{
  FTM_COMMON_XPT_INVALID = -1,
  FTM_COMMON_XPT_CUR_ORIDE_LOADED,
  FTM_COMMON_XPT_IQ_CAPTURE_COMPLETE,
  FTM_COMMON_XPT_IQ_PROCESSING_COMPLETE,
  FTM_COMMON_XPT_TABLES_LOADED,
  FTM_COMMON_XPT_PIN_CALCULATION_COMPLETE,
  FTM_COMMON_XPT_PIN_CALCULATION_FAILED
}ftm_cal_data_map_status_type; 

typedef rf_common_xpt_delay_capture_type ftm_cal_data_delay_cap_type;

/*---------- DPD Map ----------*/
typedef struct
{
  uint8 row_idx;
  ftm_cal_data_map_status_type status;
  int16 power;
  rfcommon_core_xpt_tx_override_type curr_tx_override;
  boolean measured_entry;
  uint8 cap_buf_idx;
  uint8 res_buf_idx;
  void* dpd;
  int16 compdb10;
}ftm_cal_data_dpd_map_row_type;



/*---------- Delay Table Row ----------*/
typedef struct
{
  uint32 chan; /* channel for which delay is stored */
  uint32 delay_capt_samp_rate; /* needed to converted FW format delay */
  ftm_cal_data_map_status_type status;
  ftm_cal_data_delay_cap_type delay_cap_type; /* keep track of LTE BW */
  int32 et_delay; /* contain delay value in 0p1ns */
  uint8 cap_buf_idx;
  uint8 res_buf_idx;
}ftm_cal_data_delay_map_row_type;

typedef struct
{
  /*! @brief table containing rows of DPD map data */
  ftm_cal_data_dpd_map_row_type table[FTM_COMMON_XPT_NUM_DPD_MAP_ROWS];
  uint8 num_map_entries;
} ftm_cal_data_dpd_map_type;

/*---------- Delay Table Map ----------*/
typedef struct
{
  /*! @brief table containing rows of DPD map data */
  ftm_cal_data_delay_map_row_type table[FTM_COMMON_XPT_NUM_DELAY_MAP_ROWS];
  uint8 num_map_entries;
} ftm_cal_data_delay_map_type;

/*---------- Multiple Mline -----------*/
typedef struct
{
  /*! @brief Multiple Mline information */
  boolean multiple_mline;
  /*! @brief PA state 1 */
  uint8 pa_state_one;
  /*! @brief PA state 2 */
  uint8 pa_state_two;
  /*! @brief Number of PA states*/
  uint8 num_pa_states;
}ftm_cal_multiple_mline;


/*---------- Pin Map Table Row ----------*/
typedef struct
{
  uint32 chan; /* channel for which delay is stored */
  ftm_cal_data_map_status_type status;
  int32 pin_compensation; /* contain delay value in 0p1ns */
  uint8 cap_buf_idx;
  uint8 res_buf_idx;
  float vout[XPT_CAL_MAX_AMAM_SIZE];
  float phase_out[XPT_CAL_MAX_AMAM_SIZE];
}ftm_cal_data_pin_map_row_type;


/*---------- Pin Table Map ----------*/
typedef struct
{
  /*! @brief table containing rows of Pin map data */
  ftm_cal_data_pin_map_row_type table[FTM_COMMON_XPT_NUM_PIN_MAP_ROWS];
  uint8 num_map_entries;
} ftm_cal_data_pin_map_type;

/*---------- RGI delta Table -----------*/
typedef struct
{
  /*! @brief RGI/LUT index */
  uint16 rgi;
  /*! @brief Power */
  int32 power;
}ftm_cal_data_rgid_row_type;

typedef struct
{
  /*! @brief table containing rows of rgi vs power data */
  ftm_cal_data_rgid_row_type table[RFCOM_TXLIN_NUM_SEGMENTS];
} ftm_cal_data_rgid_type;

/* ------- Result buffer tables -------- */
typedef struct
{
  fw_dpd_cal_result_t table[FTM_COMMON_XPT_NUM_DPD_MAP_ROWS];
}ftm_cal_data_dpd_table_type;

/* ------ Unity DPD info --------*/
typedef struct
{
  uint16 table[128];
}ftm_cal_data_det_type;

typedef struct
{
  /*!< Address of AMAM table to load. */
  rfcommon_mdsp_dpd_table_info_t amam_addr;
  /*!< Address of AMPM table to load. */
  rfcommon_mdsp_dpd_table_info_t ampm_addr;
  /*!< Address of detrough table to load. */
  rfcommon_mdsp_dpd_table_info_t detrough_addr;
}ftm_cal_data_unity_hwid_type;

/* ------ All Module Data --------*/

typedef struct
{
  /*!< Associated Channel */
  uint32 chan;
  /*!< Room temp term ADC for Pin/Pout comp reference */
  uint16 therm_adc;
  /*!< DPD map contains current info about the state of calibration */
  ftm_cal_data_dpd_map_type *dpd_map;
  /*!< RGI delta table contains first sweep RGI vs Power entries */
  ftm_cal_data_rgid_type *rgi_delta_table;
  /*!< RGI delta table contains first sweep RGI vs Power entries for Second Mline*/
  ftm_cal_data_rgid_type *rgi_delta_table_two;
  /*!< Multiple Mline information*/
  ftm_cal_multiple_mline mline;
  /*!< DPD table contains computed DPD results */
  ftm_cal_data_dpd_table_type *dpd_table;
}ftm_cal_data_module_table_type;

typedef struct
{
  uint32 dpd_map_cur_chan;
  uint8 dpd_map_cur_chan_idx;
  ftm_cal_data_module_table_type data_module[RFCOM_TXLIN_NUM_CHANNELS];
  ftm_cal_data_det_type data_module_nom_det;
  ftm_cal_data_unity_hwid_type unity_hwid;
  ftm_calibration_mline_result_module_type mline_results;

  /* Current overrides */
  uint8 current_override_row;
  rfcommon_core_xpt_tx_override_type curr_tx_override;
}ftm_cal_data_module_type;

/* -------------------------------------------------------------------------- */
void ftm_cal_data_init_data_module(void);
/* -------------------------------------------------------------------------- */
void ftm_cal_data_init_delay_map(void);
/* -------------------------------------------------------------------------- */
void ftm_cal_data_teardown_data_module(void);
/* -------------------------------------------------------------------------- */
void ftm_cal_data_teardown_delay_map(void);
/* -------------------------------------------------------------------------- */
void ftm_cal_data_set_current_override_row(uint8 override_row);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_init_cur_channel_map(uint32 chan);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_is_chan_initialized(uint32 chan);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_push_cur_override_to_dpd_map(
                                      uint32 chan,
                                      uint8 cap_buf_idx,
                                      uint8 res_buf_idx);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_push_delay_data_to_delay_map(
                           uint32 chan,
                           ftm_cal_data_delay_cap_type delay_cap_type,
                           uint8 cap_buf_idx,
                           uint8 res_buf_idx);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_set_capture_complete(uint32 chan, uint8 cap_buf_id);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_set_delay_capture_complete ( 
   uint32 chan, 
   uint8 cap_buf_id,
   ftm_cal_data_delay_cap_type delay_cap_type,
   uint32 samp_rate
   );
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_set_proc_complete(uint32 chan, uint8 res_buf_id);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_set_delay_proc_complete(
   uint32 chan, 
   uint8 res_buf_id,
   ftm_cal_data_delay_cap_type delay_cap_type
   );
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_store_nom_epdt_table(ftm_cal_data_det_type *nom_det_ptr);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_get_nom_epdt_table(ftm_cal_data_det_type* nom_det);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_get_unity_hwids(
                                    ftm_cal_data_unity_hwid_type* unity_hwids);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_set_unity_hwids(
                                    ftm_cal_data_unity_hwid_type* unity_hwids);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_push_rgid_data(uint32 chan,
                                    ftm_cal_data_rgid_type *rgid_table);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_push_multiple_rgid_data(uint32 chan,
                                    uint8 pastate,        
                                    uint8 xptMode,
                                    ftm_cal_data_rgid_type *rgid_table);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_get_rgid_table(uint32 chan,
                                    int8 pa_state,
                                    ftm_cal_data_rgid_type *rgid_table);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_set_epdt_hwid(uint32 chan,
                                   uint16 power, 
                                   uint8 xpt_mode,
                                   rfcommon_mdsp_dpd_table_info_t epdt_hwid);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_get_dpd_result(uint32 chan,
                                    uint16 power,
                                    uint8 xpt_mode,
                                    void* result);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_set_hwid_by_power(uint32 chan,
                                       uint16 power, 
                                       uint8 xpt_mode,
                                       rfcommon_mdsp_dpd_table_info_t amam_hwid,
                                       rfcommon_mdsp_dpd_table_info_t ampm_hwid,
                                       rfcommon_mdsp_dpd_table_info_t epdt_hwid);
/* -------------------------------------------------------------------------- */
int32 ftm_cal_data_get_delay_from_table
(
  uint32 chan,
  ftm_cal_data_delay_cap_type delay_cap_type,
  boolean debug_mode_en
);

/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_get_row_data(uint32 chan,
                                  ftm_cal_data_dpd_map_row_type* row,
                                  int8 pa, 
                                  int16 rgi, 
                                  int16 bias);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_set_row_data(uint32 chan,
                                  ftm_cal_data_dpd_map_row_type *row);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_get_row_data_by_power(uint32 chan,
                                           ftm_cal_data_dpd_map_row_type* row,
                                           uint16 power,
                                           uint8 xpt_mode);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_clear_current_dpd_map(uint32 chan);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_set_therm_adc(uint32 chan, uint16 *therm_adc);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_get_therm_adc(uint32 chan, uint16 *therm_adc);
/* -------------------------------------------------------------------------- */
boolean
ftm_cal_data_apply_pin_alignment(uint32 ref_chan,
                                 int8 pa_state,
                                 ftm_cal_data_dpd_map_row_type *in_override_row,
                                 ftm_cal_data_dpd_map_row_type *out_override_row,
                                 int16 pcomp_val);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_store_mline_res_ptr(uint32 chan,
                                       rf_common_xpt_type xpt_type,
                                       ftm_calibration_mline_result_xpt_type *mline_results );
/* -------------------------------------------------------------------------- */
int32 ftm_cal_data_get_pin_from_table(uint32 chan, 
                                      uint8 compression_point,
                                      uint32 ref_chan,
                                      uint8 *peak_compression,
                                      boolean *pin_cal_status,
                                      boolean calculate_ampm_scale_factor,
                                      int32 *ampm_scale_factor,
                                      boolean *ampm_scale_status);
/* -------------------------------------------------------------------------- */
void ftm_cal_data_init_pin_map(void);
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_push_pin_data_to_pin_map(uint32 chan,
                                              uint8 cap_buf_idx,
                                              uint8 res_buf_idx);

/* -------------------------------------------------------------------------- */
boolean
ftm_cal_data_store_pin_cal_offset
(  
  const rfcom_txlin_temp_comp_nv_data_type* lin_temp_comp_data,
  rfcommon_core_txlin_pin_cal_vs_freq_type* pin_cal_nv, 
  uint8 pa_state,
  uint32 channel,
  int16 pin_val
);

/* -------------------------------------------------------------------------- */
int16
ftm_cal_data_get_pin_cal_offset
(  
  uint16 ref_chan,
  const rfcom_txlin_temp_comp_nv_data_type* lin_temp_comp_data,
  uint8 pa_state,
  uint32 channel
);

/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_set_pin_capture_complete ( uint32 chan, uint8 cap_buf_id );
/* -------------------------------------------------------------------------- */
boolean ftm_cal_data_set_pin_proc_complete( uint32 chan, uint8 res_buf_id );
/* -------------------------------------------------------------------------- */
void ftm_cal_data_teardown_pin_map(void);
/* -------------------------------------------------------------------------- */
void ftm_cal_data_print_pin_table(void);

/* Debug */
void ftm_cal_data_print_dpd_map(void);
void ftm_cal_data_print_delay_table(void);

#ifdef __cplusplus
}
#endif

#endif /* FTM_CALIBRATION_DATA_MODULE_H */

