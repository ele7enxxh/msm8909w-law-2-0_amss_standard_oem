#ifndef RFDEVICE_QPA_TYPEDEF_H
#define RFDEVICE_QPA_TYPEDEF_H

/*=============================================================================

Copyright (c) 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qpa/api/rfdevice_qpa_typedef.h#1 $
  $DateTime: 2016/12/13 07:59:29 $
  $Author: mplcsds1 $

when         who   what, where, why
--------   ---   ---------------------------------------------------------------
11/19/14   vv     Added rfdevice_reg_dump() API  for debug
09/22/14   dbc    Add gain_state param to qpa_calc_icq
08/27/14   vv     Update temperature read from int8 to int16
08/01/14   vv     Added calibration state support
07/18/14   vv     Added therm support
06/11/14   sn     Added Rtune and Ctune support
08/05/13   aca    Initial version

=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "comdef.h"
#include "rfcom.h"

#include "rf_hal_bus_types.h"
#include "rf_buffer_intf.h"
#include "rfdevice_cmn_type_defs.h"

/*==========================================================================*/
/* DEFINES and ENUMS  */
/*==========================================================================*/

#define RFDEVICE_QPA_MAX_SCRIPT_SIZE 60
#define RFC_DEFINED_RFFE_TRANSACTION_TIME 0
#define RFDEVICE_QPA_MAX_REG_ADDR 0xFF
#define RFDEVICE_QPA_MAX_PORTS 0x20 /* Max 8 bit per RFC limitations- need to change to dynamic allocation*/


#define RFDEVICE_QPA_EFS_DEBUG_PATH_STR "/qfe/qpa/dev%d_rev%d"
#define RFDEVICE_QPA_EFS_DEBUG_MAX_PATH_LENGTH 40

#define RFDEVICE_QPA_CTUNE_INDICES_MAX 5

/* Enums for supported gain states */
typedef enum
{
  RFDEVICE_QPA_RANGE_0,
  RFDEVICE_QPA_RANGE_1,
  RFDEVICE_QPA_RANGE_2,
  RFDEVICE_QPA_RANGE_3,
  RFDEVICE_QPA_RANGE_4,
  RFDEVICE_QPA_RANGE_5,
  RFDEVICE_QPA_RANGE_6,
  RFDEVICE_QPA_RANGE_7,
  /* Bolt is spec'd for a max of 8 gain states*/
  RFDEVICE_QPA_RANGE_MAX,
  RFDEVICE_QPA_INVALID_GAIN
} rfdevice_qpa_gain_states_type;

/* Enums for supported gain states */
typedef enum
{
  RFDEVICE_QPA_RANGE_0_BIAS,
  RFDEVICE_QPA_RANGE_1_BIAS,
  RFDEVICE_QPA_RANGE_2_BIAS,
  RFDEVICE_QPA_RANGE_3_BIAS,
  RFDEVICE_QPA_RANGE_4_BIAS,
  RFDEVICE_QPA_RANGE_5_BIAS,
  RFDEVICE_QPA_RANGE_6_BIAS,
  RFDEVICE_QPA_RANGE_7_BIAS,
  /* Bolt is spec'd for a max of 8 gain states*/
  RFDEVICE_QPA_BIAS_RANGE_MAX,
  RFDEVICE_QPA_INVALID_BIAS_GAIN
} rfdevice_qpa_bias_gain_states_type;

/*==========================================================================*/
/* DATA STRUCTURES  */
/*==========================================================================*/

typedef struct
{
  const uint8** data_tbl;
  const uint8** reg_tbl;
} rfdevice_qpa_ag_settings_type;

typedef struct
{
  const uint8* data_tbl;
  const uint8* reg_tbl;
} rfdevice_qpa_efs_settings_type;

typedef struct
{
  uint8 gsm_gmsk[RFCOM_NUM_GSM_BANDS];
  uint8 gsm_8psk[RFCOM_NUM_GSM_BANDS];
  uint8 tdscdma[RFCOM_NUM_TDSCDMA_BANDS];
  uint8 cdma_revA[RFM_CDMA_MAX_BAND];
  uint8 cdma_revB[RFM_CDMA_MAX_BAND];
  uint8 wcdma[RFCOM_NUM_WCDMA_BANDS];
  uint8 lte[RFCOM_NUM_LTE_BANDS];
} rfdevice_qpa_port_info_type;

typedef struct
{
  boolean extended_cmd;  /*!< RFFE Indicator to use extended cmd or not */
  rf_hal_bus_resource_script_settings_type settings;
} rfdevice_qpa_ag_cfg_type;

typedef struct
{
  /* PA config */
  uint8 pa_config_script_size;
  rfdevice_qpa_ag_settings_type pa_config;
  /* PA control */
  uint8 pa_on_script_size;
  rfdevice_qpa_ag_settings_type pa_on;
  uint8 pa_off_script_size;
  rfdevice_qpa_ag_settings_type pa_off;
  uint8 pa_range_script_size;
  rfdevice_qpa_ag_settings_type pa_cal_state;
  uint8 pa_cal_state_script_size;

  /* WCDMA, C2K, LTE, TDSCDMA PA ranges */
  rfdevice_qpa_ag_settings_type pa_range[RFDEVICE_QPA_RANGE_MAX];
  /* GSM PA Ranges */
  rfdevice_qpa_ag_settings_type pa_range_gmsk[RFDEVICE_QPA_RANGE_MAX];
  rfdevice_qpa_ag_settings_type pa_range_8psk[RFDEVICE_QPA_RANGE_MAX];
  /* PA bias */
  uint8 pa_bias_range_script_size;
  rfdevice_qpa_ag_settings_type pa_bias_range[RFDEVICE_QPA_BIAS_RANGE_MAX];

  uint8 ctune_script_size;
  rfdevice_qpa_ag_settings_type ctune[RFDEVICE_QPA_CTUNE_INDICES_MAX];
} rfdevice_qpa_port_settings_type;

typedef struct
{
  /* PA trigger*/ /*TODO do you need trigger per action above */
  uint8 pa_trigger_script_size;
  rfdevice_qpa_ag_settings_type pa_trigger;

  /* PA trigger*/ /*TODO do you need trigger per action above */
  uint8 pa_sleep_script_size;
  rfdevice_qpa_ag_settings_type pa_sleep;

  /* PA trigger*/ /*TODO do you need trigger per action above */
  uint8 pa_wake_up_script_size;
  rfdevice_qpa_ag_settings_type pa_wake_up;
} rfdevice_qpa_common_settings_type;


typedef struct
{
  uint8 ctune_indices;

  /* Number of active ports */
  uint8 num_ports;
  /* Number of gain ranges */
  uint8 num_gain_ranges;

  /* Number of bias gain ranges */
  uint8 num_bias_gain_ranges;

  /* Pointer to port data */
  rfdevice_qpa_port_settings_type port_settings_ag;

  /* Pointer to port independent data */
  rfdevice_qpa_common_settings_type common_settings_ag;

} rfdevice_qpa_settings_type;

/* The following structure is used for debug/ OFF target */
typedef struct
{
  /* Flag to track if we are in debug mode */
  boolean debug_mode;
  /* Internal buffer showing chip data */
  uint8 *reg_data;
  /* Internal address buffer to store sequence of wrties */
  rf_buffer_intf *rf_buff;

} rfdevice_qpa_settings_debug_type;

/*==========================================================================*/
/* Function Pointers  */
/*==========================================================================*/

typedef void (*qpa_therm_setup_fn_ptr)(uint8 bus_id, uint8 slave_id);

typedef void (*qpa_therm_cleanup_fn_ptr)(uint8 bus_id, uint8 slave_id);

typedef void (*qpa_therm_read_fn_ptr)(uint8 bus_id, uint8 slave_id,int16* therm_degrees_C);

typedef void (*qpa_calc_icq_fn_ptr)(uint8 gain_state, uint8 icq_override, uint8 icq_default, uint8 *ret_val);

typedef void (*qpa_calc_rtune_fn_ptr)(uint8 bus_id, uint8 slave_id);

typedef void (*qpa_vth_tune_fn_ptr)(uint8 bus_id, uint8 slave_id, uint8 *vth_code);

typedef void (*qpa_vth_write_fn_ptr)(uint8 bus_id, uint8 slave_id, uint8 vth_code);

typedef void (*qpa_calc_ctune_fn_ptr)(uint8 *Ctune_idx, uint8 bus_id, uint8 slave_id);

//todo
typedef void (*qpa_convert_port_mapping_ptr)(rfcom_band_type_u band, rfdevice_pa_bw_enum_type bw, rfm_mode_enum_type mode, uint8 *port_index);


typedef struct
{
  qpa_calc_icq_fn_ptr calc_icq;

  qpa_calc_rtune_fn_ptr calc_rtune;

  qpa_calc_ctune_fn_ptr calc_ctune;

  qpa_vth_tune_fn_ptr vth_tune;  
  
  qpa_vth_write_fn_ptr vth_write;  

  qpa_therm_setup_fn_ptr therm_setup;

  qpa_therm_cleanup_fn_ptr therm_cleanup;

  qpa_therm_read_fn_ptr read_therm;

//todo
  qpa_convert_port_mapping_ptr convert_port_map;
} rfdevice_qpa_func_tbl_type;

#endif /*RFDEVICE_QPA_TYPEDEF */

