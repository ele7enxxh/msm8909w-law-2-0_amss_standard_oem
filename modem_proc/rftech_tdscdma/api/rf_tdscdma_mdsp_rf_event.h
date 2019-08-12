#ifndef RFTDSCDMA_MDSP_RF_EVENT_H
#define RFTDSCDMA_MDSP_RF_EVENT_H
/*! 
  @file
  rf_tdscdma_mdsp_rf_event.h
 
  @brief
  This file defines the RF events.
*/

/*==============================================================================

  Copyright (c) 2012 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Technologies Proprietary

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/api/rf_tdscdma_mdsp_rf_event.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
10/06/15   whz     Add support to AutoPin script
01/16/15   nsh     Move IP2 cal to enter mode
12/17/14   ms      Add IP2 script support
12/16/14   jyu     Fixed T2X/X2T/T2T tuner issue
12/02/14   jyu     Fixed timing error for RX-TX transition when Rx and 
                   Tx reference time are too close
10/09/14   jyu     Separated Rx wakeup and Tx wakeup scripts
09/16/14   jyu     Support to build home restore script (for AsDiv and Tuner)
09/01/14   jyu     Need to rebuild Rx LNA script when logic device changes
07/17/14   ms      Sawless RX support
07/11/14   jyu     Added support to build TxRxOff script
07/10/14   jyu     Asd bringup with rfc changes and device script
04/28/14   jps     Support for TDS port switching
04/24/14   ych     Added dynamic tuner script for ifreq
01/16/14   jyu     Added support to build WAKEUP scripts
12/10/13   jyu     Added support to build Rx LNA scripts
11/07/13   nsh     FED RxAGC Feature
10/15/13   jyu     Added rxlm and txlm handle into data structure
10/10/13   jyu     Integrated with DM interface
08/19/13   al      Moved macro TFW_RX_TX_MAX_NUM_WRITES_TDS to head file
08/05/13   al      Added support to set ASM switch state
07/26/13   ms      AsDiv support
06/03/13   nsh     Support Get Multi Synth Status in Dime
05/08/13   jyu     Fixed double buffering 
04/30/13   jyu     Added more debug message
04/17/13   jyu     Added support to search tasknum for more task types 
                     -  LNA (SSBI)
                     -  ASM (RFFE)
                     -  TUNER (RFFE)
04/02/13   jyu     Added support to build special TDS BSP for IRAT 
12/12/12   jyu     Added support for more query type for TaskNum Update 
11/30/12   jyu     Added support to pass primary Rx/secondary Rx/Tx devices 
08/07/12   jyu     Updated code due to the CCS optimization change 
06/28/12   jyu     Initial Version
 
==============================================================================*/

#include "comdef.h"
#include "rf_tdscdma_mdsp_types.h"

#include "rfdevice_tdscdma_type_defs.h"
#include "rfdevice_tdscdma_intf.h"
#include "rfdevice_meas_type_defs.h"

#include "rfcommon_mdsp_event_intf.h"
#include "rf_hal_bus_types.h"

#include "rf_buffer_intf.h"
#include "rf_hal_buffer.h"
#include "rfc_card_tdscdma.h"
#include "rfc_class.h"
#include "rfcom.h"

#define RFTDSCDMA_STANDALONE_MAX_SSBI_SCRIPT_SIZE 20
#define RFTDSCDMA_STANDALONE_MAX_RFFE_SCRIPT_SIZE 400//200
#define RFTDSCDMA_STANDALONE_MAX_GRFC_SCRIPT_SIZE 20
#define RFLM_TDS_RX_TX_MAX_NUM_WRITES_TDS 15

#define RF_TDSCDMA_MDSP_MICROSECOND_TO_USTRM_UINT_CONV_X10 192 /* = 19.2*10 */

/*----------------------------------------------------------------------------*/
/*                         FUNCTIONAL FLAGS                                   */
/*----------------------------------------------------------------------------*/
extern boolean rf_tdscdma_mdsp_disable_apt_update;
extern boolean rf_tdscdma_mdsp_disable_pa_update;
extern boolean rf_tdscdma_mdsp_disable_lna_update;
extern boolean rf_tdscdma_mdsp_disable_tuner_update;


/*----------------------------------------------------------------------------*/
/*                             DEBUG FLAGS                                    */
/*----------------------------------------------------------------------------*/
extern boolean rf_tdscdma_mdsp_rf_event_script_display_time_profile;         
extern boolean rf_tdscdma_mdsp_display_successful_script;
extern boolean rf_tdscdma_mdsp_tasknum_update_debug_flag;
extern boolean rf_tdscdma_mdsp_rfc_timing_debug_flag;
extern boolean rf_tdscdma_mdsp_build_dynamic_update_debug_flag;
extern boolean rf_tdscdma_mdsp_dynamic_tuner_script_flag;

typedef enum
{
  GRFC_CONTROL_PA_STATE_UPDATE_USE_GRFC_BITMASK,
  RFFE_CONTROL_PA_STATE_UPDATE_USE_RFFE_NON_EXT_WRITE,
  RFFE_CONTROL_QPOET_APT_UPDATE_USE_RFFE_NON_EXT_WRITE,
  SSBI_CONTROL_LNA_UPDATE_USE_SSBI_WRITE,
  RFFE_CONTROL_PRX_ASM_UPDATE_USE_RFFE_NON_EXT_WRITE,
  RFFE_CONTROL_DRX_ASM_UPDATE_USE_RFFE_NON_EXT_WRITE,
  RFFE_CONTROL_TX_ASM_UPDATE_USE_RFFE_NON_EXT_WRITE,
  RFFE_CONTROL_ASM_UPDATE_USE_RFFE_NON_EXT_WRITE,
  RFFE_CONTROL_TUNER_UPDATE_USE_RFFE_NON_EXT_WRITE,

  TASKNUN_UPDATE_TYPE_MAX,
  TASKNUN_UPDATE_TYPE_INVALID
} rftdscdma_mdsp_event_script_tasknum_update_type;


typedef enum
{ 
  RF_TDSCDMA_MDSP_RX_LNA_UPDATE_FOR_SCELL,
  RF_TDSCDMA_MDSP_RX_LNA_UPDATE_FOR_NCELL,
  RF_TDSCDMA_MDSP_RX_LNA_UPDATE_FOR_IRAT  
} rf_tdscdma_mdsp_rx_lna_update_type;

typedef struct
{
  rfm_device_enum_type pri_device;
  rfm_device_enum_type sec_device;
  rfm_device_enum_type tx_device;
  rfcom_tdscdma_band_type band;  
  uint16 channel;
  uint8 buf_id;   
  boolean drx_enable;
  rftdscdma_mdsp_event_script_tasknum_update_type update_type;
  uint8 switch_state;
  uint32 pri_rxlm_buffer_index;
  uint32 sec_rxlm_buffer_index;
  uint32 txlm_buffer_index;
  rf_path_enum_type  path;
  void * data_ptr;
  boolean is_irat;
  boolean is_bho_update;
  boolean to_build_rx_lna_script_on_bands[RFCOM_NUM_TDSCDMA_BANDS];
  rf_tdscdma_mdsp_rx_lna_update_type rx_lna_update_type;
  uint32 ant_sw_pos;
  rfcom_tdscdma_band_type tx_band; 
} rftdscdma_mdsp_common_event_data_type;

/* The assumption is that GRFC get exact start_time from RFC and no need to shift in this file */
typedef struct
{
  int32 ssbi_script_start_time;
  int32 rffe_script_start_time;
  uint8 priority;  
} rf_tds_event_script_info_t;


typedef struct
{
  uint8 num_device_per_chain;
  uint8 num_write[RFC_MAX_DEVICE_INSTANCES_PER_BAND];
  uint8 update_info_array[RFC_MAX_DEVICE_INSTANCES_PER_BAND][RFC_MAX_DEVICE_INSTANCES];
} rftdscdma_mdsp_build_dynamic_info_result_single_chain_data_type;

typedef struct
{
  uint8 buf_index;
  /* Rx0 device */
  rftdscdma_mdsp_build_dynamic_info_result_single_chain_data_type rx0_info;
  /* Rx1 device */
  rftdscdma_mdsp_build_dynamic_info_result_single_chain_data_type rx1_info;
  /* Tx device */
  rftdscdma_mdsp_build_dynamic_info_result_single_chain_data_type tx_info;

  uint16 control_version;
} rftdscdma_mdsp_build_dynamic_info_result_data_type;

typedef enum
{
  RF_TDS_EVT_OFF_TO_RX,
  RF_TDS_EVT_OFF_TO_TX,
  RF_TDS_EVT_RX_TO_OFF,
  RF_TDS_EVT_TX_TO_OFF,
  RF_TDS_EVT_RX_TO_TX,
  RF_TDS_EVT_TX_TO_RX,
  RF_TDS_EVT_TX_TO_TX,

  RF_TDS_EVT_BHO_STARTUP,
  RF_TDS_EVT_IFREQ_STARTUP,  
  RF_TDS_EVT_IFREQ_CLEANUP,  

  RF_TDS_EVT_DO_HDET,
  RF_TDS_EVT_DO_THERM,

  RF_TDS_EVT_DO_PLL,

  RF_TDS_EVT_TDS_SPECIAL_BSP_FOR_IRAT,
  RF_TDS_EVT_BUILD_RX_LNA,
  RF_TDS_EVT_BUILD_WAKEUP,
  RF_TDS_EVT_BUILD_TX_WAKEUP,
  RF_TDS_EVT_DYNAMIC_UPDATE,
  RF_TDS_EVT_DYNAMIC_SCRIPT,
  RF_TDS_EVT_DO_JDET,
  RF_TDS_EVT_BUILD_HOME_RESTORE,
  RF_TDS_EVT_DO_IP2_CAL,

  RF_TDS_EVT_BUILD_AUTOPIN,

}rftdscdma_event_eum_type;


typedef struct
{
  uint32          event_id; 
  rf_buffer_intf* script_buf_ptr; 
  void*           shared_mem_buf_ptr; 
  uint32          shared_mem_max_size_bytes; 
  boolean         is_issue_seq; 
  int16           return_start_time;
  uint8           priority;  
  uint8           max_tasks_allocated;
} rftdscdma_mdsp_rf_event_param_type;

typedef enum
{
  /* OFF-to-RX event: RF_TDS_EVT_OFF_TO_RX */
  RF_TDS_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_0,
  RF_TDS_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_1,
  RF_TDS_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_0,
  RF_TDS_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_1,
  RF_TDS_EVT_OFF_TO_RX_EVENT_RX_ENABLE_SCRIPT_BUF_0,
  RF_TDS_EVT_OFF_TO_RX_EVENT_RX_ENABLE_SCRIPT_BUF_1,
  RF_TDS_EVT_OFF_TO_RX_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_PRX_ONLY_BUF_0,
  RF_TDS_EVT_OFF_TO_RX_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_PRX_ONLY_BUF_1,
  RF_TDS_EVT_OFF_TO_RX_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_PRX_DRX_BUF_0,
  RF_TDS_EVT_OFF_TO_RX_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_PRX_DRX_BUF_1,

  /* RX-to-OFF event: RF_TDS_EVT_RX_TO_OFF */
  RF_TDS_EVT_RX_TO_OFF_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_PRX_ONLY_BUF_0,
  RF_TDS_EVT_RX_TO_OFF_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_PRX_ONLY_BUF_1,
  RF_TDS_EVT_RX_TO_OFF_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_PRX_DRX_BUF_0,
  RF_TDS_EVT_RX_TO_OFF_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_PRX_DRX_BUF_1,

  /* TX-to-OFF event: RF_TDS_EVT_TX_TO_OFF */
  RF_TDS_EVT_TX_TO_OFF_EVENT_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_0,
  RF_TDS_EVT_TX_TO_OFF_EVENT_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_1,
  RF_TDS_EVT_TX_TO_OFF_EVENT_TXRX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_0,
  RF_TDS_EVT_TX_TO_OFF_EVENT_TXRX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_1,  

  /* OFF-to-TX event: RF_TDS_EVT_OFF_TO_TX */  
  RF_TDS_EVT_OFF_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_0,
  RF_TDS_EVT_OFF_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_1,
  RF_TDS_EVT_OFF_TO_TX_EVENT_TX_ENABLE_SCRIPT_BUF_0,  
  RF_TDS_EVT_OFF_TO_TX_EVENT_TX_ENABLE_SCRIPT_BUF_1,  
  RF_TDS_EVT_OFF_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_POWER_L_TO_H_BUF_0,
  RF_TDS_EVT_OFF_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_POWER_L_TO_H_BUF_1,

  /* TX-to-RX event: RF_TDS_EVT_TX_TO_RX */  
  RF_TDS_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_0,
  RF_TDS_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_1,
  RF_TDS_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_0,
  RF_TDS_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_1,
  RF_TDS_EVT_TX_TO_RX_EVENT_RX_ENABLE_SCRIPT_BUF_0,
  RF_TDS_EVT_TX_TO_RX_EVENT_RX_ENABLE_SCRIPT_BUF_1,
  RF_TDS_EVT_TX_TO_RX_EVENT_RX_ACTIVATE_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_PRX_ONLY_BUF_0,
  RF_TDS_EVT_TX_TO_RX_EVENT_RX_ACTIVATE_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_PRX_ONLY_BUF_1,
  RF_TDS_EVT_TX_TO_RX_EVENT_RX_ACTIVATE_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_PRX_DRX_BUF_0,
  RF_TDS_EVT_TX_TO_RX_EVENT_RX_ACTIVATE_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_PRX_DRX_BUF_1,

  /* TX-to-TX event: RF_TDS_EVT_TX_TO_TX */  
  RF_TDS_EVT_TX_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_POWER_L_TO_H_BUF_0,
  RF_TDS_EVT_TX_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_POWER_L_TO_H_BUF_1,

  /* RX-to-TX event: RF_TDS_EVT_RX_TO_TX */  
  RF_TDS_EVT_RX_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_0,
  RF_TDS_EVT_RX_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_1,
  RF_TDS_EVT_RX_TO_TX_EVENT_TX_ENABLE_SCRIPT_BUF_0,
  RF_TDS_EVT_RX_TO_TX_EVENT_TX_ENABLE_SCRIPT_BUF_1,
  RF_TDS_EVT_RX_TO_TX_EVENT_RX_DEACTIVATE_TRIGGER_ISSUE_SEQ_SCRIPT_PRX_ONLY_BUF_0,
  RF_TDS_EVT_RX_TO_TX_EVENT_RX_DEACTIVATE_TRIGGER_ISSUE_SEQ_SCRIPT_PRX_ONLY_BUF_1,
  RF_TDS_EVT_RX_TO_TX_EVENT_RX_DEACTIVATE_TRIGGER_ISSUE_SEQ_SCRIPT_PRX_DRX_BUF_0,
  RF_TDS_EVT_RX_TO_TX_EVENT_RX_DEACTIVATE_TRIGGER_ISSUE_SEQ_SCRIPT_PRX_DRX_BUF_1,
  RF_TDS_EVT_RX_TO_TX_EVENT_TX_ACTIVATE_NON_TRIGGER_ISSUE_SEQ_SCRIPT_PRX_ONLY_POWER_L_TO_H_BUF_0,
  RF_TDS_EVT_RX_TO_TX_EVENT_TX_ACTIVATE_NON_TRIGGER_ISSUE_SEQ_SCRIPT_PRX_ONLY_POWER_L_TO_H_BUF_1,
  RF_TDS_EVT_RX_TO_TX_EVENT_TX_ACTIVATE_NON_TRIGGER_ISSUE_SEQ_SCRIPT_PRX_DRX_POWER_L_TO_H_BUF_0,
  RF_TDS_EVT_RX_TO_TX_EVENT_TX_ACTIVATE_NON_TRIGGER_ISSUE_SEQ_SCRIPT_PRX_DRX_POWER_L_TO_H_BUF_1,

  /* IFREQ_STARTUP event: RF_TDS_EVT_IFREQ_STARTUP */ 
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_0,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_1,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_2,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_3,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_4,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_5,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_6,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_7,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_8,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_9,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_10,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_11,

  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_0,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_1,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_2,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_3,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_4,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_5,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_6,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_7,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_8,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_9,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_10,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_11,

  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_BUF_0,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_BUF_1,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_BUF_2,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_BUF_3,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_BUF_4,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_BUF_5,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_BUF_6,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_BUF_7,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_BUF_8,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_BUF_9,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_BUF_10,
  RF_TDS_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_BUF_11,

  /* IFREQ_CLEANUP event: */
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_0,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_1,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_2,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_3,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_4,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_5,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_6,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_7,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_8,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_9,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_10,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_11,

  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_PRX_DRX_BUF_0,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_PRX_DRX_BUF_1,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_PRX_DRX_BUF_2,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_PRX_DRX_BUF_3,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_PRX_DRX_BUF_4,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_PRX_DRX_BUF_5,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_PRX_DRX_BUF_6,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_PRX_DRX_BUF_7,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_PRX_DRX_BUF_8,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_PRX_DRX_BUF_9,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_PRX_DRX_BUF_10,
  RF_TDS_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_PRX_DRX_BUF_11,

  /* IFREQ_TO_TX event: */
  RF_TDS_EVT_IFREQ_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_0,
  RF_TDS_EVT_IFREQ_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_1,

  /* BHO_STARTUP event: RF_TDS_EVT_BHO_STARTUP */ 
  RF_TDS_EVT_BHO_STARTUP_EVENT_TX_ONE_TIME_INIT_SCRIPT,
 
  /* DO_HDET event: RF_TDS_EVT_DO_HDET */ 
  RF_TDS_EVT_DO_HDET_EVENT_HDET_WRITE_SCRIPT,
  RF_TDS_EVT_DO_HDET_EVENT_HDET_READ_SCRIPT,

  /* DO_THERM event: RF_TDS_EVT_DO_THERM */ 
  RF_TDS_EVT_DO_THERM_EVENT_THERM_WRITE_SCRIPT,
  RF_TDS_EVT_DO_THERM_EVENT_THERM_READ_SCRIPT,
  RF_TDS_EVT_DO_THERM_EVENT_TUNER_THERM_READ_SCRIPT,
  
  /* DO PLL READ event: RF_TDS_EVT_DO_PLL_READING*/
  RF_TDS_EVT_DO_RX_PLL_EVENT_READ_SCRIPT,
  RF_TDS_EVT_DO_TX_PLL_EVENT_READ_SCRIPT,

  /* for three bands and 4 gain states each */
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_0,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_1,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_2,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_3,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_4,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_5,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_6,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_7,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_8,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_9,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_10,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_11,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_12,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_13,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_14,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_15,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_16,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_17,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_18,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_19,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_20,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_21,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_22,
  RF_TDS_EVT_BUILD_RX_LNA_SCRIPT_BUF_23,

  RF_TDS_EVT_BUILD_RX_LNA_LATCH_PRX_ONLY_SCRIPT,
  RF_TDS_EVT_BUILD_RX_LNA_LATCH_DRX_ONLY_SCRIPT,
  RF_TDS_EVT_BUILD_RX_LNA_LATCH_PRX_DRX_SCRIPT,

  /* DO WAKEUP event: RF_TDS_EVT_DO_WAKEUP */
  RF_TDS_EVT_DO_WAKEUP_EVENT_RX_WAKEUP_SCRIPT,
  RF_TDS_EVT_DO_WAKEUP_EVENT_TX_WAKEUP_SCRIPT,  

  /* DYNAMIC_SCRIPT event: RF_TDS_EVT_DYNAMIC_SCRIPT */
  RF_TDS_EVT_DYNAMIC_SCRIPT_EVENT_IFREQ_STARTUP_BUF_0,
  RF_TDS_EVT_DYNAMIC_SCRIPT_EVENT_IFREQ_STARTUP_BUF_1,
  RF_TDS_EVT_DYNAMIC_SCRIPT_EVENT_IFREQ_CLEANUP_BUF_0,
  RF_TDS_EVT_DYNAMIC_SCRIPT_EVENT_IFREQ_CLEANUP_BUF_1,

  RF_TDS_EVT_DO_JDET_EVENT_JDET_READ_SCRIPT,

  RF_TDS_EVT_HOME_RESTORE_EVENT_SCRIPT_BUF_0,
  RF_TDS_EVT_HOME_RESTORE_EVENT_SCRIPT_BUF_1,

  // To add more event scripts
  RF_TDS_EVT_DO_IP2_CAL_CODE_WRITE_SCRIPT,

  /* AutoPin */
  RF_TDS_EVT_AUTOPIN_WRITE_SCRIPT_BUF_0,
  RF_TDS_EVT_AUTOPIN_WRITE_SCRIPT_BUF_1,

  RF_TDS_EVT_MAX_NUM_SCRIPT,
  RF_TDS_EVT_INVALID_SCRIPT
}rf_tds_event_id_e;

boolean rf_tdscdma_mdsp_rf_event_initial_script_buffer(void);

boolean rf_tdscdma_mdsp_rf_event_destroy_script_buffer(void);

boolean rf_tdscdma_mdsp_rf_event_cmd_dispatch(rftdscdma_event_eum_type event_type, void* data);

boolean rf_tdscdma_mdsp_build_dynamic_update_info(rftdscdma_mdsp_common_event_data_type * data);
boolean rf_tdscdma_mdsp_build_lna_script(rftdscdma_mdsp_common_event_data_type * data);

#ifdef FEATURE_RF_ASDIV
/*! Structure to hold the data to be used by the callback function used to 
create AsDiv XSW/GRFC script
*/
typedef struct
{
  rfm_device_enum_type device;
  /*!<  RFM device for which GRFC switch script is to be created */
  rfcom_tdscdma_band_type band;
  /*!< TDSCDMA band for which the script is to be created */
  boolean immediate;
  /*!< Flag indicating if a script is to be created or immediate action is to
  be done */
} rf_tdscdma_asdiv_script_data;

boolean 
rf_tdscdma_mdsp_build_asdiv_script
(
  uint32 ant_sw_pos, 
  rf_buffer_intf *rf_buffer,
  void *cb_data 
);

/*----------------------------------------------------------------------------*/
/*! Structure to hold the data to be used by the callback function used to 
create ASM script
*/
typedef struct
{
  rfm_device_enum_type device;
  /*!<  RFM device for which GRFC switch script is to be created */
  rfcom_tdscdma_band_type band;
  /*!< TDSCDMA band for which the script is to be created */
  boolean immediate;
  /*!< Flag indicating if a script is to be created or immediate action is to
  be done */

} rf_tdscdma_asdiv_asm_script_data;
#endif /* #ifdef FEATURE_RF_ASDIV */

#endif /* RFTDSCDMA_MDSP_RF_EVENT_H */

