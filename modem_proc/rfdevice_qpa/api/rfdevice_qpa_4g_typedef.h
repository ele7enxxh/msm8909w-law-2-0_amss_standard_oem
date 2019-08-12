#ifndef RFDEVICE_QPA_4G_TYPEDEF_H
#define RFDEVICE_QPA_4G_TYPEDEF_H

/*=============================================================================

Copyright (c) 2014 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_qpa/api/rfdevice_qpa_4g_typedef.h#1 $
  $DateTime: 2016/12/13 07:59:29 $
  $Author: mplcsds1 $

when       who   what, where, why
--------   ---   ---------------------------------------------------------------
05/09/16   dbc   Added efs_filename field to rfdevice_qpa_4g_settings_type
12/07/15   dbc   Added support for QPA/QASM common tab voting
12/01/15  dbc   Add icq_default field to rfdevice_qpa_4g_settings_type (16 Bit ICQ support)
02/02/15   px    Added EFS debugging support
11/07/14   sn    Initial version

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

#define RFDEVICE_QPA_4G_MAX_SCRIPT_SIZE 50
#define RFC_DEFINED_RFFE_TRANSACTION_TIME 0
#define RFDEVICE_QPA_4G_MAX_REG_ADDR 0xFF
#define RFDEVICE_QPA_4G_MAX_PORTS 0x20 /* Max 8 bit per RFC limitations- need to change to dynamic allocation*/


#define RFDEVICE_QPA_4G_EFS_DEBUG_PATH_STR "/qfe/qpa/dev%d_rev%d"
#define RFDEVICE_QPA_4G_EFS_DEBUG_MAX_PATH_LENGTH 40

#define RFDEVICE_QPA_4G_CTUNE_INDICES_MAX 5

/* Enums for supported gain states */
typedef enum
{
  RFDEVICE_QPA_4G_RANGE_0,
  RFDEVICE_QPA_4G_RANGE_1,
  RFDEVICE_QPA_4G_RANGE_2,
  RFDEVICE_QPA_4G_RANGE_3,
  RFDEVICE_QPA_4G_RANGE_4,
  RFDEVICE_QPA_4G_RANGE_5,
  RFDEVICE_QPA_4G_RANGE_6,
  RFDEVICE_QPA_4G_RANGE_7,
  /* Bolt is spec'd for a max of 8 gain states*/
  RFDEVICE_QPA_4G_RANGE_MAX,
  RFDEVICE_QPA_4G_RANGE_GMSK_MAX = RFDEVICE_QPA_4G_RANGE_MAX,
  RFDEVICE_QPA_4G_RANGE_8PSK_MAX = RFDEVICE_QPA_4G_RANGE_MAX,
  RFDEVICE_QPA_4G_INVALID_GAIN
} rfdevice_qpa_4g_gain_states_type;

/* Enums for supported gain states */
typedef enum
{
  RFDEVICE_QPA_4G_RANGE_0_BIAS,
  RFDEVICE_QPA_4G_RANGE_1_BIAS,
  RFDEVICE_QPA_4G_RANGE_2_BIAS,
  RFDEVICE_QPA_4G_RANGE_3_BIAS,
  RFDEVICE_QPA_4G_RANGE_4_BIAS,
  RFDEVICE_QPA_4G_RANGE_5_BIAS,
  RFDEVICE_QPA_4G_RANGE_6_BIAS,
  RFDEVICE_QPA_4G_RANGE_7_BIAS,
  /* Bolt is spec'd for a max of 8 gain states*/
  RFDEVICE_QPA_4G_BIAS_RANGE_MAX,
  RFDEVICE_QPA_4G_BIAS_RANGE_GMSK_MAX = RFDEVICE_QPA_4G_BIAS_RANGE_MAX,
  RFDEVICE_QPA_4G_BIAS_RANGE_8PSK_MAX = RFDEVICE_QPA_4G_BIAS_RANGE_MAX,
  RFDEVICE_QPA_4G_INVALID_BIAS_GAIN
} rfdevice_qpa_4g_bias_gain_states_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the RFDEVICE_QPA_4G lte_tx_bw enum.
*/
typedef enum 
{
  RFDEVICE_QPA_4G_TX_BW_1P4M, 
  RFDEVICE_QPA_4G_TX_BW_3M, 
  RFDEVICE_QPA_4G_TX_BW_5M, 
  RFDEVICE_QPA_4G_TX_BW_10M, 
  RFDEVICE_QPA_4G_TX_BW_15M, 
  RFDEVICE_QPA_4G_TX_BW_20M, 
  RFDEVICE_QPA_4G_TX_BW_40M, 
  RFDEVICE_QPA_4G_TX_BW_NUM, 
  RFDEVICE_QPA_4G_TX_BW_INVALID, 
} rfdevice_qpa_4g_tx_bw_data_type;

/*==========================================================================*/
/* DATA STRUCTURES  */
/*==========================================================================*/

/*----------------------------------------------------------------------------*/
/*!
   It defines the QPA_4G reg struct.
*/
typedef struct 
{
  uint16 address; 
  uint16 product_rev; 
  uint8 instruction; 
} rfdevice_qpa_4g_reg_data_type;

typedef struct
{
  const rfdevice_qpa_4g_reg_data_type* ag_reg_tbl;
  const uint8* ag_data_tbl;
} rfdevice_qpa_4g_ag_list_data_type;

/*----------------------------------------------------------------------------*/
/*!
   It defines the QPA_4G gain_reg struct.
*/
typedef struct 
{
  uint16 product_rev; 
  uint16 address[RFDEVICE_QPA_4G_BIAS_RANGE_MAX]; 
  uint8 instruction; 
} rfdevice_qpa_4g_gain_reg_data_type;

typedef struct
{
  const rfdevice_qpa_4g_gain_reg_data_type* ag_reg_tbl;
  const uint8* ag_data_tbl;
} rfdevice_qpa_4g_ag_gain_list_data_type;

/*----------------------------------------------------------------------------*/
/*!
   These define the QPA_4G EFS reg structs.
*/
typedef struct
{
  rfdevice_qpa_4g_reg_data_type* reg_data_type;
  uint8* data_tbl;
} rfdevice_qpa_4g_efs_settings_type;

typedef struct
{
  rfdevice_qpa_4g_gain_reg_data_type* reg_data_type;
  uint8* data_tbl;
} rfdevice_qpa_4g_efs_gain_settings_type;

typedef struct
{
  uint8 gsm_gmsk[RFCOM_NUM_GSM_BANDS];
  uint8 gsm_8psk[RFCOM_NUM_GSM_BANDS];
  uint8 tdscdma[RFCOM_NUM_TDSCDMA_BANDS];
  uint8 cdma_revA[RFM_CDMA_MAX_BAND];
  uint8 cdma_revB[RFM_CDMA_MAX_BAND];
  uint8 wcdma[RFCOM_NUM_WCDMA_BANDS];
  uint8 lte[RFCOM_NUM_LTE_BANDS];
} rfdevice_qpa_4g_port_info_type;

typedef struct
{
  boolean extended_cmd;  /*!< RFFE Indicator to use extended cmd or not */
  rf_hal_bus_resource_script_settings_type settings;
} rfdevice_qpa_4g_ag_cfg_type;

typedef struct
{
  uint8 pa_common_script_size;
  rfdevice_qpa_4g_ag_list_data_type pa_common;
  
  /* PA wakeup*/
  uint8 pa_wake_up_script_size;
  rfdevice_qpa_4g_ag_list_data_type pa_wake_up;
  
  /* PA sleep*/
  uint8 pa_sleep_script_size;
  rfdevice_qpa_4g_ag_list_data_type pa_sleep;
  
  /* PA trigger*/
  uint8 pa_trigger_script_size;
  rfdevice_qpa_4g_ag_list_data_type pa_trigger;

  /* PA on */
  uint8 pa_on_script_size;
  rfdevice_qpa_4g_ag_list_data_type pa_on;
  
  /* PA off */
  uint8 pa_off_script_size;
  rfdevice_qpa_4g_ag_list_data_type pa_off;
  
  /* PA common power up init */
  uint8 pa_common_power_up_init_script_size;
  rfdevice_qpa_4g_ag_list_data_type pa_common_power_up_init;
  
  /* PA common wake up */
  uint8 pa_common_wake_up_script_size;
  rfdevice_qpa_4g_ag_list_data_type pa_common_wake_up;
  
  /* PA common sleep */
  uint8 pa_common_sleep_script_size;
  rfdevice_qpa_4g_ag_list_data_type pa_common_sleep;
  
  //rfdevice_qpa_4g_ag_list_data_type pa_cal_state;
  //uint8 pa_cal_state_script_size;

  /* PA config */
  uint8 pa_config_script_size;
  rfdevice_qpa_4g_ag_list_data_type **pa_config;

  uint8 pa_calibration_state_on_script_size;
  rfdevice_qpa_4g_ag_list_data_type **pa_calibration_state_on;
  
  /* WCDMA, C2K, LTE, TDSCDMA PA ranges */
  uint8 pa_gain_range_script_size;
  rfdevice_qpa_4g_ag_gain_list_data_type **pa_gain_range;

  /* GSM PA Ranges */
  uint8 pa_gain_range_gmsk_script_size;
  uint8 pa_gain_range_8psk_script_size;
  rfdevice_qpa_4g_ag_gain_list_data_type **pa_gain_range_gmsk;
  rfdevice_qpa_4g_ag_gain_list_data_type **pa_gain_range_8psk;
  
  /* PA bias */
  uint8 pa_bias_range_script_size;
  rfdevice_qpa_4g_ag_gain_list_data_type **pa_bias_range;

  /* GSM Bias Ranges */
  uint8 pa_bias_range_gmsk_script_size;
  uint8 pa_bias_range_8psk_script_size;
  rfdevice_qpa_4g_ag_gain_list_data_type **pa_bias_range_gmsk;
  rfdevice_qpa_4g_ag_gain_list_data_type **pa_bias_range_8psk;

  const uint8 *null_data_tbl;
  const uint8 *null_gain_data_tbl;
} rfdevice_qpa_4g_port_settings_type;


typedef struct
{
  /* Number of active ports */
  uint8 num_ports;

  /* Number of gain ranges */
  uint8 num_gain_ranges;

  /* Pointer to port data */
  rfdevice_qpa_4g_port_settings_type port_settings_ag;

  /* Default ICQ value */
  uint16 icq_default;

  /* EFS PA dat file name */
  const char* efs_filename; 

} rfdevice_qpa_4g_settings_type;

/* The following structure is used for debug/ OFF target */
typedef struct
{
  /* Flag to track if we are in debug mode */
  boolean debug_mode;
  /* Internal buffer showing chip data */
  uint8 *reg_data;
  /* Internal address buffer to store sequence of wrties */
  rf_buffer_intf *rf_buff;

} rfdevice_qpa_4g_settings_debug_type;

/*==========================================================================*/
/* Function Pointers  */
/*==========================================================================*/

typedef void (*qpa_therm_setup_fn_ptr)(uint8 bus_id, uint8 slave_id);

typedef void (*qpa_therm_cleanup_fn_ptr)(uint8 bus_id, uint8 slave_id);

typedef void (*qpa_therm_read_fn_ptr)(uint8 bus_id, uint8 slave_id,int16* therm_degrees_C);

typedef void (*qpa_calc_icq_fn_ptr)(uint8 gain_state, uint8 icq_override, uint8 icq_default, uint8 *ret_val);

typedef void (*qpa_calc_rtune_fn_ptr)(uint8 bus_id, uint8 slave_id);

typedef uint8 (*qpa_get_efs_data_column_fn_ptr)(uint8 port_index, rfdevice_qpa_4g_tx_bw_data_type lcl_bw);

//todo
typedef void (*qpa_convert_port_mapping_ptr)(rfcom_band_type_u band, rfdevice_pa_bw_enum_type bw, rfm_mode_enum_type mode, uint8 *port_index);


typedef struct
{
  qpa_calc_icq_fn_ptr calc_icq;

  qpa_calc_rtune_fn_ptr calc_rtune;

  qpa_therm_setup_fn_ptr therm_setup;

  qpa_therm_cleanup_fn_ptr therm_cleanup;

  qpa_therm_read_fn_ptr read_therm;

  qpa_get_efs_data_column_fn_ptr get_efs_data_column;

//todo
  qpa_convert_port_mapping_ptr convert_port_map;
} rfdevice_qpa_4g_func_tbl_type;

#endif /*RFDEVICE_QPA_4G_TYPEDEF */

