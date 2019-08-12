#ifndef RFDEVICE_QTUNER_TYPEDEF_H
#define RFDEVICE_QTUNER_TYPEDEF_H

/*=============================================================================

Copyright (c) 2013,2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. 
  Government. Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qtuner/api/rfdevice_qtuner_typedef.h#1 $
  $DateTime: 2016/12/13 07:58:06 $
  $Author: mplcsds1 $
 
when         who   what, where, why
--------   ---   --------------------------------------------------------------- 
07/22/14   yb     Fix clock settings for V100 and V200 QFE2520
07/08/14   kg    CL Optimization 
06/17/14   yb    Removed PID 4 from RL measurements
05/29/14   yb    1. Fixed tune code override in online mode
                 2. Program ctuner compensation settings in program tune code
05/02/14   yb    Added support for tune code override
05/02/14   yb    Added definition for RL read script size
03/25/14   yb    Added support for linearizer
11/30/13   ndb   Initial version
 
=============================================================================*/
  
/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h"
#include "rfcom.h"
#include "rf_rffe_common.h"
#include "rf_hal_bus_types.h"
#include "rf_buffer_intf.h"
#include "rfdevice_comm_intf.h"
#include "rfdevice_qtuner_api.h"


/*==========================================================================*/
/* DEFINES and ENUMS  */
/*==========================================================================*/

#define RFDEVICE_QTUNER_MAX_SCRIPT_SIZE 50
#define RFC_DEFINED_RFFE_TRANSACTION_TIME 0
#define RFDEVICE_QTUNER_MAX_REG_ADDR 0xFF
#define RFDEVICE_QTUNER_MAX_COMP_TABLES 5

/* Max size of RL return script */
#define RFDEVICE_QTUNER_MAX_RL_RETURN_SCRIPT_SIZE 10

/* Maximum tune code size supported */
#define RFDEVICE_QTUNER_MAX_TUNE_CODE_SIZE 8

/*==========================================================================*/
/* DATA STRUCTURES  */
/*==========================================================================*/

typedef struct
{
  rfdevice_qtuner_distortion_config_type gsm[RFCOM_NUM_GSM_BANDS];
  rfdevice_qtuner_distortion_config_type tdscdma[RFCOM_NUM_TDSCDMA_BANDS];
  rfdevice_qtuner_distortion_config_type cdma[RFM_CDMA_MAX_BAND];
  rfdevice_qtuner_distortion_config_type wcdma[RFCOM_NUM_WCDMA_BANDS];
  rfdevice_qtuner_distortion_config_type lte[RFCOM_NUM_LTE_BANDS];
} rfdevice_qtuner_distortion_config_map_type;

/* Data structure to hold the tuner config .Currently support Version 0 format */ 
typedef struct
{
  uint8 version;
  PACK(union)
  {
    PACK(struct)
    {
      uint8 c1_tune_code;
      uint8 c2_tune_code;
      uint8 switch_config;
    }version_0;
  }tunecode_config;
}rfdevice_qtuner_tune_code_override_data_type;

typedef struct
{

  /* Tuner Preconfig*/ 
  uint8 tuner_preconfig_script_size;
  rf_rffe_data_type *tuner_preconfig;

  /* Tuner Init*/ 
  uint8 tuner_init_script_size;
  rf_rffe_data_type *tuner_init;
  
  /* Tuner disable*/
  uint8 tuner_disable_script_size;
  rf_rffe_data_type *tuner_disable;

  /*Tuner  code settings */
  uint8 tuner_code_script_size;
  rf_rffe_data_type *tuner_code_settings;

  /* RL disable */
  uint8 tuner_rl_disable_script_size;
  rf_rffe_data_type *tuner_rl_disable;
}rfdevice_qtuner_common_settings_type;

/*Compensation Table*/
typedef struct
{
  uint8 min_value;
  uint8 max_value;

  /*max number of columns*/
  uint8 num_bins;
  /*max number of rows*/
  uint8 num_values;

  uint8 reg_bitmask;

  /*Array of bins */
  int8 *bin_array;
  /*Array of  values*/
  int8 *value_array;

  /*2D array:Comp_table[value_index][bin_index]*/
  int8 *comp_table;

}rfdevice_qtuner_compensation_table_type;


typedef struct
{
  /* Pointer to common settings */
  rfdevice_qtuner_common_settings_type common_settings_ag;

  /*Pointer to the C comp Tables*/
  rfdevice_qtuner_compensation_table_type *c_table[RFDEVICE_QTUNER_MAX_COMP_TABLES];

}rfdevice_qtuner_settings_type;


/* RC tuner cal data */
typedef struct
{
  /* R tuner cal data */
  boolean r_tuner_data_valid;  
  uint8 *r_tuner_val;
  uint8 num_r;

  /* C tuner cal data */
  boolean c_tuner_data_valid;
  uint8 *c_idxs;
  uint8 num_c;
}rfdevice_qtuner_cal_data_type;


typedef struct
{
  /*Supported Algorithm*/
  uint8 algo_mapping_mask;

  /*Size of the tune code in bytes*/
  uint8 tune_code_size;

  /*Internal Cal data */
  rfdevice_qtuner_cal_data_type calibrated_data;
}rfdevice_qtuner_params_type;



/*==========================================================================*/
/* Function Pointers  */
/*==========================================================================*/


/*List of RL function ptr's  for CL algorithm*/
typedef boolean (*qtuner_postprocess_return_loss_fn_ptr)(rfdevice_comm_intf *rfdevice_comm,
	                                                             rf_buffer_intf* script,
	                                                             uint16* reflected,
	                                                             uint16* incident,
	                                                             uint16* return_loss_dB10,
                                                               float* return_loss_raw,
                                                               uint16* mismatched_loss_dB10,
	                                                             rf_hal_bus_rffe_type* rl_script, 
	                                                             uint8 read_size);


typedef boolean (*qtuner_read_return_loss_fn_ptr)(rfdevice_comm_intf *rfdevice_comm,
	                                                     rf_buffer_intf* script,
	                                                     uint16 delay);



typedef boolean (*qtuner_setup_return_loss_fn_ptr)( rfdevice_comm_intf *rfdevice_comm,
	                                                       rf_buffer_intf* script,
	                                                       uint32  tx_freq,
	                                                       rfcom_mode_enum_type mode,
	                                                       uint8* pid5_gain, 
	                                                       uint8* pid6_gain, 
	                                                       uint8* tia_range, 
	                                                       uint16* delay, 
	                                                       uint8* num_avg,
	                                                       boolean ftm_override);

typedef boolean (*qtuner_perform_r_tuner_cal)( rfdevice_comm_intf *rfdevice_comm,
	                                                  rf_buffer_intf* script,
	                                                  uint8* r_tuner_val);

typedef boolean (*qtuner_perform_c_tuner_cal)( rfdevice_comm_intf *rfdevice_comm,
	                                                  rf_buffer_intf* script,
	                                                  int* c_bins);

typedef boolean (*qtuner_load_r_tuner_cal)( rfdevice_comm_intf *rfdevice_comm,
	                                              rf_buffer_intf* script,
	                                              uint8* r_tuner_val);

typedef boolean (*qtuner_load_c_tuner_cal)( rfdevice_comm_intf *rfdevice_comm,
	                                              rf_buffer_intf* script,
	                                              uint8* c_idxs);

typedef boolean (*qtuner_program_linearizer)( rfdevice_comm_intf *rfdevice_comm,
                                              rf_buffer_intf* script,
                                              uint32  tx_freq, 
                                              rfcom_mode_enum_type mode,
                                              rfdevice_qtuner_distortion_config_type im_distortion);

typedef boolean (*qtuner_get_tune_code_override_val)( rfdevice_comm_intf *rfdevice_comm,
                                                      rf_buffer_intf* script,
                                                      uint8  c1,
                                                      uint8  c2,
                                                      uint8  switch_config,
                                                      uint8* tune_code_ptr);

typedef boolean (*qtuner_program_clk_closed_loop_fn_ptr)( rfdevice_comm_intf *rfdevice_comm,
	                                                  rf_buffer_intf* script);

typedef struct 
{ 

  /*List of RL function ptr's  for CL algorithm*/
  qtuner_setup_return_loss_fn_ptr          setup_return_loss;
  qtuner_read_return_loss_fn_ptr           read_return_loss;
  qtuner_postprocess_return_loss_fn_ptr    postprocess_return_loss;

  /*Perform the Internal Cal routines*/
  qtuner_perform_r_tuner_cal perform_r_tuner_cal;
  qtuner_perform_c_tuner_cal perform_c_tuner_cal;

  /* Load the Cal data */
  qtuner_load_r_tuner_cal load_r_tuner_cal;
  qtuner_load_c_tuner_cal load_c_tuner_cal;
  
  /* Program Linearizer */
  qtuner_program_linearizer program_linearizer;
  /* Get tune code override val */
  qtuner_get_tune_code_override_val get_tune_code_override_val;
  /* Program clock closed loop control */
  qtuner_program_clk_closed_loop_fn_ptr program_clk_closed_loop;
}rfdevice_qtuner_func_tbl_type;

#endif /*RFDEVICE_QTUNER_TYPEDEF */
