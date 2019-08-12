/*!
  @file
  rflm_tds_dm_intf.h

  @brief
  RFLM TDSCDMA DM INTF header file
  
  @detail
  Define type defintion and function prototype.
  
  
*/

/*==============================================================================

  Copyright (c) 2013 - 2015  Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

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

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/rftech/tdscdma/inc/rflm_tds_dm_intf.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/06/15   whz     Add support to AutoPin script for TA
08/05/15   jr      Increase RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS from 7 to 10
08/04/15   par     Increase RFLM_TDS_RF_EVT_BUILD_WAKEUP_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 
                   from 150 to 185
03/14/15   jak     Optimize JDET & IP2 cal script size allocated in DM buffer
03/02/15   jak     Fix JDET reading registers pair for consecutive DL condition
01/19/15   lwu     Increase RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS/
				   RFLM_TDS_RF_EVT_RX_TO_OFF_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS to 17
12/25/14   nsh     Seperate DM buffer for 2 consecutive JDET reading				   
12/17/14   ms      Add IP2 script support
12/02/14   jyu     Fixed timing error for RX-TX transition when Rx and 
                   Tx reference time are too close
11/19/14   whz     Increase WAKEUP_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS to 150
09/25/14   jyu     Added support to pre-process issue_seq scripts (for timeline saving)
09/16/14   jyu     Support for homeRestoreScpt for Tuner and AsDiv xsw restore
09/08/14   jps/al  Incresed ISEQ task max size
09/01/14   jyu     Removed rxLnawLatch script (for space/time saving)
08/25/14   jps     Incresed ISEQ task max size
07/16/15   kb      Increase max tasks and RFFE transactions for HDET write script.
06/28/14   jyu     Cleaned up unused scripts to release script buffer
06/28/14   jyu     Added Tx non-trx one time init script for BHO and single Tx DSDA
06/26/14   ms      Increase RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS to 10
06/20/14   jyu     Increase IFREQ buffer size to 12 for DR-DSDS
06/20/14   ych     Increase RFLM_TDS_RF_EVT_RX_TO_OFF_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS to 15
06/04/14   jyu     Added support to separate non-trigger and trigger writes
04/24/14   ych     Added dynamic tuner script for ifreq
04/24/14   ms      Increase RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS to 7
04/22/14   ych     Update Rx_To_Off max CCS tasks
04/03/14   jyu     Added support for port A/B feature
03/15/14   jyu     Added support for double LNA latch
02/10/14   jyu     Added support for LNA latch
02/05/14   jyu     Added support to log event start time offset
02/02/14   jyu     Optimized the script size for buffer space saving
01/30/14   jyu     Added support for X2T meas
01/16/14   jyu     Added script for wakeup timeline optimization
12/12/13   jyu     Added more info (e.g., freq) into script buffer and added
                   rfEventInfo
12/10/13   jyu     Updated Rx LNA script data definitions
11/26/13   jyu     Added support for BHO Rx one-time init script and IFREQ
                   LNA pre-load script
10/09/13   jyu     Initial version 
==============================================================================*/

#ifndef RFLM_TDS_DM_INTF_H
#define RFLM_TDS_DM_INTF_H


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#ifdef FEATURE_RFA_D3925
#include "tfw_rf_sm_intf.h"
#include "tfw_sw_intf_msg.h"
#else
#include "rflm_ccs_rf_event_intf.h"
#include "rflm_ccs_rf_intf.h"
#endif


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

#define RFLM_TDS_RX_TX_MAX_NUM_TX_GAIN_STATES       4
#define RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS  2
#define RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS  12
#define RFLM_TDS_RFC_MAX_DEVICE_INSTANCES_PER_BAND  5
#define RFLM_TDS_RX_TX_MAX_NUM_WRITES_UPDATE       15
#define RFLM_TDS_RX_TX_MAX_NUM_BANDS                3 
#define RFLM_TDS_RX_TX_MAX_NUM_RX_GAIN_STATES       4 
#define RFLM_TDS_RX_TX_MAX_NUM_CHAINS               2 
#define RFLM_TDS_MAX_PINGPONG_BUFFERS               2
#define RFLM_TDS_RX_TX_MAX_NUM_RX_LIN_STATES        2
#define RFLM_TDS_MAX_NUM_HOME_RESTORE_SCRIPT_BUFFER 2


#ifdef FEATURE_RFA_D3925
/* ***************************************************************************/
/*                                                                           */
/*                        USE LEGACY RF-FW SHM INTERFACE                     */
/*                                                                           */
/* ***************************************************************************/

#define RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_MAX_TASKS              TFW_RF_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_MAX_TASKS         			         
#define RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_TASKS               TFW_RF_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_TASKS          			          
#define RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_TASKS                             TFW_RF_EVT_OFF_TO_RX_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_TASKS                        			                        
#define RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS           TFW_RF_EVT_OFF_TO_RX_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS             	             
#define RFLM_TDS_RF_EVT_RX_TO_OFF_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS         TFW_RF_EVT_RX_TO_OFF_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS             	
#define RFLM_TDS_RF_EVT_TX_TO_OFF_EVENT_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS         TFW_RF_EVT_TX_TO_OFF_EVENT_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS             	
#define RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_TASKS                       TFW_RF_EVT_OFF_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_TASKS             				
#define RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_TASKS                             TFW_RF_EVT_OFF_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_TASKS             						
#define RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS           TFW_RF_EVT_OFF_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS             	
#define RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_TASKS                     TFW_RF_EVT_IFREQ_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_TASKS            				
#define RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_TASKS                           TFW_RF_EVT_IFREQ_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_TASKS             					
#define RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS         TFW_RF_EVT_IFREQ_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS             	
#define RFLM_TDS_RF_EVT_TX_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_TYPE_MAX_TASKS                  TFW_RF_EVT_TX_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_TYPE_MAX_TASKS             			
#define RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_TASKS                        TFW_RF_EVT_RX_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_TASKS             				
#define RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_TASKS                              TFW_RF_EVT_RX_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_TASKS             						
#define RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS          TFW_RF_EVT_RX_TO_TX_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS             	
#define RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS            TFW_RF_EVT_RX_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS             	
#define RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_MAX_TASKS               TFW_RF_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_MAX_TASKS             		
#define RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_TASKS                TFW_RF_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_TASKS             		
#define RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_TASKS                              TFW_RF_EVT_TX_TO_RX_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_TASKS             						
#define RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_ACTIVATE_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS TFW_RF_EVT_TX_TO_RX_EVENT_RX_ACTIVATE_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS   
#define RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_MAX_TASKS          TFW_RF_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_MAX_TASKS             	
#define RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_TASKS           TFW_RF_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_TASKS             	
#define RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_TASKS                         TFW_RF_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_TASKS             					
#define RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS       TFW_RF_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS            
#define RFLM_TDS_RF_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_TYPE_MAX_TASKS                 TFW_RF_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_TYPE_MAX_TASKS             			
#define RFLM_TDS_RF_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_TYPE_MAX_TASKS           TFW_RF_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_TYPE_MAX_TASKS             	
#define RFLM_TDS_RF_EVT_BHO_STARTUP_EVENT_TX_ONE_TIME_INIT_SCRIPT_TYPE_MAX_TASKS                    TFW_RF_EVT_BHO_STARTUP_EVENT_TX_ONE_TIME_INIT_SCRIPT_TYPE_MAX_TASKS             			
#define RFLM_TDS_RF_EVT_DO_HDET_EVENT_HDET_WRITE_SCRIPT_TYPE_MAX_TASKS                              TFW_RF_EVT_DO_HDET_EVENT_HDET_WRITE_SCRIPT_TYPE_MAX_TASKS            						
#define RFLM_TDS_RF_EVT_DO_HDET_EVENT_HDET_READ_SCRIPT_TYPE_MAX_TASKS                               TFW_RF_EVT_DO_HDET_EVENT_HDET_READ_SCRIPT_TYPE_MAX_TASKS             						
#define RFLM_TDS_RF_EVT_DO_THERM_EVENT_THERM_WRITE_SCRIPT_TYPE_MAX_TASKS                            TFW_RF_EVT_DO_THERM_EVENT_THERM_WRITE_SCRIPT_TYPE_MAX_TASKS             					
#define RFLM_TDS_RF_EVT_DO_THERM_EVENT_THERM_READ_SCRIPT_TYPE_MAX_TASKS                             TFW_RF_EVT_DO_THERM_EVENT_THERM_READ_SCRIPT_TYPE_MAX_TASKS             						
#define RFLM_TDS_RF_EVT_DO_THERM_EVENT_TUNER_THERM_READ_SCRIPT_TYPE_MAX_TASKS                       TFW_RF_EVT_DO_THERM_EVENT_TUNER_THERM_READ_SCRIPT_TYPE_MAX_TASKS             				
#define RFLM_TDS_RF_EVT_DO_RX_PLL_READ_SCRIPT_TYPE_MAX_TASKS                                        TFW_RF_EVT_DO_RX_PLL_READ_SCRIPT_TYPE_MAX_TASKS             								
#define RFLM_TDS_RF_EVT_DO_TX_PLL_READ_SCRIPT_TYPE_MAX_TASKS                                        TFW_RF_EVT_DO_TX_PLL_READ_SCRIPT_TYPE_MAX_TASKS             								
#define RFLM_TDS_PA_STATE_GRFC_TYPE             TFW_PA_STATE_GRFC_TYPE
#define RFLM_TDS_PA_STATE_RFFE_NON_EXT_TYPE     TFW_PA_STATE_RFFE_NON_EXT_TYPE
#define RFLM_TDS_PA_STATE_RFFE_EXT_TYPE         TFW_PA_STATE_RFFE_EXT_TYPE
#define RFLM_TDS_APT_RFFE_NON_EXT_TYPE          TFW_APT_RFFE_NON_EXT_TYPE
#define RFLM_TDS_ASM_RFFE_NON_EXT_TYPE          TFW_ASM_RFFE_NON_EXT_TYPE
#define RFLM_TDS_TUNER_AOL_RFFE_NON_EXT_TYPE    TFW_TUNER_AOL_RFFE_NON_EXT_TYPE
#define RFLM_TDS_PA_STATE_INVALID_TYPE          TFW_PA_STATE_INVALID_TYPE

  // OFF-to-RX event
typedef tfw_off_to_rx_event_rx_tuning_script_prx_only_buf_x_t rflm_tds_off_to_rx_event_rx_tuning_script_prx_only_buf_x_t;
typedef tfw_off_to_rx_event_rx_tuning_script_prx_drx_buf_x_t rflm_tds_off_to_rx_event_rx_tuning_script_prx_drx_buf_x_t;
typedef tfw_off_to_rx_event_rx_enable_script_t rflm_tds_off_to_rx_event_rx_enable_script_t;
typedef tfw_off_to_rx_event_rx_activate_issue_seq_script_buf_x_t rflm_tds_off_to_rx_event_rx_activate_issue_seq_script_buf_x_t; 
  
  // RX-to-OFF event
typedef tfw_rx_to_off_event_rx_deactivate_issue_seq_script_buf_x_t rflm_tds_rx_to_off_event_rx_deactivate_issue_seq_script_buf_x_t;

  // OFF-to-TX event
typedef tfw_off_to_tx_event_tx_tuning_script_buf_x_t rflm_tds_off_to_tx_event_tx_tuning_script_buf_x_t;
typedef tfw_off_to_tx_event_tx_enable_script_t rflm_tds_off_to_tx_event_tx_enable_script_t;
typedef tfw_off_to_tx_event_tx_activate_issue_seq_script_buf_x_t rflm_tds_off_to_tx_event_tx_activate_issue_seq_script_buf_x_t;

  // IFREQ-to-TX event
typedef tfw_ifreq_to_tx_event_tx_tuning_script_buf_x_t rflm_tds_ifreq_to_tx_event_tx_tuning_script_buf_x_t;
typedef tfw_ifreq_to_tx_event_tx_enable_script_t rflm_tds_ifreq_to_tx_event_tx_enable_script_t;
typedef tfw_ifreq_to_tx_event_tx_activate_issue_seq_script_buf_x_t rflm_tds_ifreq_to_tx_event_tx_activate_issue_seq_script_buf_x_t;

  // TX-to-OFF event
typedef tfw_tx_to_off_event_tx_deactivate_issue_seq_script_buf_x_t rflm_tds_tx_to_off_event_tx_deactivate_issue_seq_script_buf_x_t;

  // RX-to-TX event
typedef tfw_rx_to_tx_event_tx_tuning_script_buf_x_t rflm_tds_rx_to_tx_event_tx_tuning_script_buf_x_t;
typedef tfw_rx_to_tx_event_tx_enable_script_t rflm_tds_rx_to_tx_event_tx_enable_script_t;                    
typedef tfw_rx_to_tx_event_rx_deactivate_issue_seq_script_buf_x_t rflm_tds_rx_to_tx_event_rx_deactivate_issue_seq_script_buf_x_t; 
typedef tfw_rx_to_tx_event_tx_activate_issue_seq_script_buf_x_t rflm_tds_rx_to_tx_event_tx_activate_issue_seq_script_buf_x_t;
  
  // TX-to-RX event
typedef tfw_tx_to_rx_event_rx_tuning_script_prx_only_buf_x_t rflm_tds_tx_to_rx_event_rx_tuning_script_prx_only_buf_x_t;   
typedef tfw_tx_to_rx_event_rx_tuning_script_prx_drx_buf_x_t rflm_tds_tx_to_rx_event_rx_tuning_script_prx_drx_buf_x_t;
typedef tfw_tx_to_rx_event_rx_enable_script_t rflm_tds_tx_to_rx_event_rx_enable_script_t;    
typedef tfw_tx_to_rx_event_rx_activate_tx_deactivate_issue_seq_script_buf_x_t rflm_tds_tx_to_rx_event_rx_activate_tx_deactivate_issue_seq_script_buf_x_t;
    
  // TX-to-TX event
typedef tfw_tx_to_tx_event_tx_activate_issue_seq_script_t rflm_tds_tx_to_tx_event_tx_activate_issue_seq_script_t;                    

  // BHO_startup event
typedef tfw_bho_startup_event_tx_one_time_init_script_t rflm_tds_bho_startup_event_tx_one_time_init_script_t;                     
  // BHO_cleanup event  
  
  // IFREQ_startup event
typedef tfw_ifreq_startup_event_rx_tuning_script_prx_drx_buf_x_t rflm_tds_ifreq_startup_event_rx_tuning_script_prx_drx_buf_x_t;
typedef tfw_ifreq_startup_event_rx_enable_script_t rflm_tds_ifreq_startup_event_rx_enable_script_t;                           
typedef tfw_ifreq_startup_event_rx_activate_issue_seq_script_buf_x_t rflm_tds_ifreq_startup_event_rx_activate_issue_seq_script_buf_x_t;

  // IFREQ_cleanup event  
typedef tfw_ifreq_cleanup_event_rx_tuning_script_prx_drx_t rflm_tds_ifreq_cleanup_event_rx_tuning_script_prx_drx_t;
typedef tfw_ifreq_cleanup_event_rx_deactivate_issue_seq_script_t rflm_tds_ifreq_cleanup_event_rx_deactivate_issue_seq_script_t;          
  
  // DO_HDET event
typedef tfw_do_hdet_event_hdet_write_script_t rflm_tds_do_hdet_event_hdet_write_script_t;                                
typedef tfw_do_hdet_event_hdet_read_script_t rflm_tds_do_hdet_event_hdet_read_script_t;                            
  
  // DO_THERM event
typedef tfw_do_therm_event_therm_write_script_t rflm_tds_do_therm_event_therm_write_script_t;                             
typedef tfw_do_therm_event_therm_read_script_t rflm_tds_do_therm_event_therm_read_script_t;                              

  // Tuner Temp Event
typedef tfw_do_therm_event_tuner_therm_read_script_t rflm_tds_do_therm_event_tuner_therm_read_script_t;

  // Get Pll Status for Dime
typedef tfw_do_rx_pll_status_read_script_t rflm_tds_do_rx_pll_status_read_script_t; 	                               
typedef tfw_do_tx_pll_status_read_script_t rflm_tds_do_tx_pll_status_read_script_t;

  // Info to update PA_STATE
typedef tfw_pa_state_update_info_t rflm_tds_pa_state_update_info_t;                                           

  // Info to update APT
typedef tfw_apt_update_info_t rflm_tds_apt_update_info_t;
  
  // Info to update Tuner AOL
typedef tfw_tuner_aol_update_info_t rflm_tds_tuner_aol_update_info_t;

  // Info to update ASM
typedef tfw_asm_update_info_t rflm_tds_asm_update_info_t;
  
typedef tfw_ccs_event_t rflm_tds_ccs_event_t;

typedef tfw_rx_tx_rf_dynamic_update_for_single_chain_t rflm_tds_rx_tx_rf_dynamic_update_for_single_chain_t;

typedef tfw_rx_tx_rf_dynamic_update_t rflm_tds_rx_tx_rf_dynamic_update_t;

typedef tfw_dynamic_update_info_e rflm_tds_dynamic_update_info_e;

typedef tfw_rf_sm_intf_t rflm_tds_rf_sm_intf_t;

typedef tfw_do_jdet_event_jdet_read_script_t rflm_tds_do_jdet_event_jdet_read_script_t;    

typedef tfw_do_ip2_cal_code_write_script_t rflm_tds_do_ip2_cal_code_write_script_t;  

#else /* !FEATURE_RFA_D3925 */

/* ***************************************************************************/
/*                                                                           */
/*                             USE RFLM/DM INTERFACE                         */
/*                                                                           */
/* ***************************************************************************/

typedef struct
{
  uint8 band;
  uint16 chan;
  uint32  freqInKHz;
} rflm_tds_rf_script_info_t;

/* Generic CCS Event typedef */
#define RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( CcsEventType, maxTasks, maxPayload ) \
	typedef struct tag_##CcsEventType \
	{ \
		rflm_ccs_rf_event_description_buffer_header_type header; \
		rf_ccs_event_task_description_type task[maxTasks]; \
		uint32 payload[maxPayload]; \
	} CcsEventType


/*---------------------------------------------------------------------------*/
/*                        OFF-to-RX Event Scripts Definitions                */
/*---------------------------------------------------------------------------*/

/* 
** OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X Type (X=0,1) 
*/ 
#define RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_MAX_TASKS             3
#define RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS 30
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS*2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_off_to_rx_event_rx_tuning_script_prx_only_buf_x_t, \
                               RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_PAYLOAD);

/* 
** OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X Type (X=0,1) 
*/ 
#define RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_TASKS             3
#define RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS 50
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS *2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_off_to_rx_event_rx_tuning_script_prx_drx_buf_x_t, \
                               RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_PAYLOAD);

/* 
** OFF_TO_RX_EVENT_RX_ENABLE_SCRIPT Type 
*/ 
#define RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_TASKS             1
#define RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 1
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_ENABLE_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS*2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_off_to_rx_event_rx_enable_script_t, \
                               RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_ENABLE_SCRIPT_TYPE_PAYLOAD);

/* 
** OFF_TO_RX_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X Type (X=0,1) shared for both PRX_ONLY and PRX_DRX
*/ 
#define RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS             17
#define RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS 10
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS*2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_off_to_rx_event_rx_activate_issue_seq_script_buf_x_t, \
                               RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_OFF_TO_RX_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_PAYLOAD);



/*---------------------------------------------------------------------------*/
/*                        RX-to-OFF Event Scripts Definitions                */
/*---------------------------------------------------------------------------*/

/* 
** RX_TO_OFF_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X Type (X=0,1) shared for both PRX_ONLY and PRX_DRX
*/ 
#define RFLM_TDS_RF_EVT_RX_TO_OFF_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS             17
#define RFLM_TDS_RF_EVT_RX_TO_OFF_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_RX_TO_OFF_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS 10
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_RX_TO_OFF_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_RX_TO_OFF_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_RX_TO_OFF_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS *2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_rx_to_off_event_rx_deactivate_issue_seq_script_buf_x_t, \
                               RFLM_TDS_RF_EVT_RX_TO_OFF_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_RX_TO_OFF_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_PAYLOAD);






/*---------------------------------------------------------------------------*/
/*                        TX-to-OFF Event Scripts Definitions                */
/*---------------------------------------------------------------------------*/

/* 
** TX_TO_OFF_EVENT_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X Type (X=0,1)  
*/ 
#define RFLM_TDS_RF_EVT_TX_TO_OFF_EVENT_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS             10
#define RFLM_TDS_RF_EVT_TX_TO_OFF_EVENT_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_TX_TO_OFF_EVENT_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS 20
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_TX_TO_OFF_EVENT_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_TX_TO_OFF_EVENT_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_TX_TO_OFF_EVENT_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS*2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_tx_to_off_event_tx_deactivate_issue_seq_script_buf_x_t, \
                               RFLM_TDS_RF_EVT_TX_TO_OFF_EVENT_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_TX_TO_OFF_EVENT_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_PAYLOAD);

/* 
** TX_TO_OFF_EVENT_TXRX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X Type (X=0,1)  
*/ 
#define RFLM_TDS_RF_EVT_TX_TO_OFF_EVENT_TXRX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS             16
#define RFLM_TDS_RF_EVT_TX_TO_OFF_EVENT_TXRX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_TX_TO_OFF_EVENT_TXRX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS 30
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_TX_TO_OFF_EVENT_TXRX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_TX_TO_OFF_EVENT_TXRX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_TX_TO_OFF_EVENT_TXRX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS*2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_tx_to_off_event_txrx_deactivate_issue_seq_script_buf_x_t, \
                               RFLM_TDS_RF_EVT_TX_TO_OFF_EVENT_TXRX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_TX_TO_OFF_EVENT_TXRX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_PAYLOAD);





/*---------------------------------------------------------------------------*/
/*                        OFF-to-TX Event Scripts Definitions                */
/*---------------------------------------------------------------------------*/

/* 
** OFF_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X Type (X=0,1) 
*/ 
#define RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_TASKS             3
#define RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS 40
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS*2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_off_to_tx_event_tx_tuning_script_buf_x_t, \
                               RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_PAYLOAD);

/* 
** OFF_TO_TX_EVENT_TX_ENABLE_SCRIPT Type 
*/ 
#define RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_TASKS             1
#define RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 1
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS*2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_off_to_tx_event_tx_enable_script_t, \
                               RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_PAYLOAD);

/* 
** OFF_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT Type 
*/ 
#define RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS             12
#define RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS 15
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS*2)
RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_off_to_tx_event_tx_activate_issue_seq_script_buf_x_t, \
                               RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_OFF_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_PAYLOAD);



/*---------------------------------------------------------------------------*/
/*                        IFREQ-to-TX Event Scripts Definitions                */
/*---------------------------------------------------------------------------*/

/* 
** IFREQ_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X Type (X=0,1) 
*/ 
#define RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_TASKS             3
#define RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS 40
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS*2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_ifreq_to_tx_event_tx_tuning_script_buf_x_t, \
                               RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_PAYLOAD);

/* 
** IFREQ_TO_TX_EVENT_TX_ENABLE_SCRIPT Type 
*/ 
#define RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_TASKS             1
#define RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 1
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS*2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_ifreq_to_tx_event_tx_enable_script_t, \
                               RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_PAYLOAD);

/* 
** IFREQ_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT Type 
*/ 
#define RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS             12
#define RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS 15
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS*2)
RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_ifreq_to_tx_event_tx_activate_issue_seq_script_buf_x_t, \
                               RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_IFREQ_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_PAYLOAD);





/*---------------------------------------------------------------------------*/
/*                        TX-to-TX Event Scripts Definitions                */
/*---------------------------------------------------------------------------*/

/* 
** TX_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT TYPE (POWER_L_TO_H and POWER_H_TO_L)
*/ 
#define RFLM_TDS_RF_EVT_TX_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_TYPE_MAX_TASKS             7
#define RFLM_TDS_RF_EVT_TX_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_TX_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 10
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_TX_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_TX_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_TX_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS*2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_tx_to_tx_event_tx_activate_issue_seq_script_t, \
                               RFLM_TDS_RF_EVT_TX_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_TX_TO_TX_EVENT_TX_ACTIVATE_ISSUE_SEQ_SCRIPT_TYPE_PAYLOAD);





/*---------------------------------------------------------------------------*/
/*                        RX-to-TX Event Scripts Definitions                */
/*---------------------------------------------------------------------------*/

/* 
** RX_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X Type (X=0,1) 
*/ 
#define RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_TASKS             3
#define RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS 40
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS*2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_rx_to_tx_event_tx_tuning_script_buf_x_t, \
                               RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_TUNING_SCRIPT_BUF_X_TYPE_PAYLOAD);

/* 
** RX_TO_TX_EVENT_TX_ENABLE_SCRIPT Type 
*/ 
#define RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_TASKS             1
#define RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 1
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS*2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_rx_to_tx_event_tx_enable_script_t, \
                               RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_ENABLE_SCRIPT_TYPE_PAYLOAD);

/* 
** RX_TO_TX_EVENT_RX_DEACTIVATE_TRIGGER_ISSUE_SEQ_SCRIPT_BUF_X Type (X=0,1) shared for both PRX_ONLY and PRX_DRX
*/ 
#define RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_RX_DEACTIVATE_TRIGGER_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS             10
#define RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_RX_DEACTIVATE_TRIGGER_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_RX_DEACTIVATE_TRIGGER_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS 20
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_RX_DEACTIVATE_TRIGGER_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_RX_DEACTIVATE_TRIGGER_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_RX_DEACTIVATE_TRIGGER_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS*2)
RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_rx_to_tx_event_rx_deactivate_trigger_issue_seq_script_buf_x_t, \
                               RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_RX_DEACTIVATE_TRIGGER_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_RX_DEACTIVATE_TRIGGER_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_PAYLOAD);



/* 
** RX_TO_TX_EVENT_TX_ACTIVATE_NON_TRIGGER_ISSUE_SEQ_SCRIPT_BUF_X Type (X=0,1)
*/ 
#define RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_ACTIVATE_NON_TRIGGER_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS             10
#define RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_ACTIVATE_NON_TRIGGER_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_ACTIVATE_NON_TRIGGER_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS 20
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_ACTIVATE_NON_TRIGGER_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_ACTIVATE_NON_TRIGGER_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_ACTIVATE_NON_TRIGGER_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS*2)
RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_rx_to_tx_event_tx_activate_non_trigger_issue_seq_script_buf_x_t, \
                               RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_ACTIVATE_NON_TRIGGER_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_RX_TO_TX_EVENT_TX_ACTIVATE_NON_TRIGGER_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_PAYLOAD);




/*---------------------------------------------------------------------------*/
/*                        TX-to-RX Event Scripts Definitions                */
/*---------------------------------------------------------------------------*/

/* 
** TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X Type (X=0,1) 
*/ 
#define RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_MAX_TASKS             3
#define RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS 30
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS*2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_tx_to_rx_event_rx_tuning_script_prx_only_buf_x_t, \
                               RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_ONLY_BUF_X_TYPE_PAYLOAD);

/* 
** TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X Type (X=0,1) 
*/ 
#define RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_TASKS             3
#define RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS 50
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS *2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_tx_to_rx_event_rx_tuning_script_prx_drx_buf_x_t, \
                               RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_PAYLOAD);

/* 
** TX_TO_RX_EVENT_RX_ENABLE_SCRIPT Type 
*/ 
#define RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_TASKS             1
#define RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 1
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_ENABLE_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS*2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_tx_to_rx_event_rx_enable_script_t, \
                               RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_ENABLE_SCRIPT_TYPE_PAYLOAD);

/* 
** TX_TO_RX_EVENT_TX_DEACTIVATE_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X Type (X=0,1) shared for both PRX_ONLY and PRX_DRX
*/ 
#define RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_ACTIVATE_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS             15
#define RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_ACTIVATE_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_ACTIVATE_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS 30
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_ACTIVATE_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_ACTIVATE_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_ACTIVATE_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS*2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_tx_to_rx_event_rx_activate_tx_deactivate_issue_seq_script_buf_x_t, \
                               RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_ACTIVATE_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_TX_TO_RX_EVENT_RX_ACTIVATE_TX_DEACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_PAYLOAD);



/*---------------------------------------------------------------------------*/
/*                        IFREQ START_UP Event Scripts Definitions                */
/*---------------------------------------------------------------------------*/

/* 
** IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X Type (X=0..1) 
*/ 
#define RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_TASKS             3
#define RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS 50
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS *2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_ifreq_startup_event_rx_tuning_script_prx_drx_buf_x_t, \
                               RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_BUF_X_TYPE_PAYLOAD);

/* 
** IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT Type 
*/ 
#define RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_TASKS             1
#define RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 1
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS*2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_ifreq_startup_event_rx_enable_script_t, \
                               RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_ENABLE_SCRIPT_TYPE_PAYLOAD);

/* 
** IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X Type (X=0..7)
*/ 
#define RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS             10
#define RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS 10
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_PAYLOAD \
            ((RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
            + RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS*2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_ifreq_startup_event_rx_activate_issue_seq_script_buf_x_t, \
                               RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_RX_ACTIVATE_ISSUE_SEQ_SCRIPT_BUF_X_TYPE_PAYLOAD);

/* 
** IFREQ_STARTUP_EVENT_TUNER_UPDATE_SCRIPT Type 
*/ 
#define RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_TUNER_UPDATE_SCRIPT_TYPE_MAX_TASKS                5
#define RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_TUNER_UPDATE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS   10
#define RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_TUNER_UPDATE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS   35
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_TUNER_UPDATE_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_TUNER_UPDATE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_TUNER_UPDATE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS*2)
      

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_ifreq_startup_event_tuner_update_script_t, \
                               RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_TUNER_UPDATE_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_IFREQ_STARTUP_EVENT_TUNER_UPDATE_SCRIPT_TYPE_PAYLOAD);

/* 
** IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX Type  
*/ 
#define RFLM_TDS_RF_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_TYPE_MAX_TASKS             3
#define RFLM_TDS_RF_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_TYPE_MAX_RFFE_TRANSACTIONS 50
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_TYPE_MAX_RFFE_TRANSACTIONS *2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_ifreq_cleanup_event_rx_tuning_script_prx_drx_t, \
                               RFLM_TDS_RF_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_IFREQ_CLEANUP_EVENT_RX_TUNING_SCRIPT_PRX_DRX_TYPE_PAYLOAD);


/* 
** IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT Type (X=0..7)
*/ 
#define RFLM_TDS_RF_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_TYPE_MAX_TASKS             7
#define RFLM_TDS_RF_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 20
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_TYPE_PAYLOAD \
            ((RFLM_TDS_RF_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
            + RFLM_TDS_RF_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS*2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_ifreq_cleanup_event_rx_deactivate_issue_seq_script_t, \
                               RFLM_TDS_RF_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_IFREQ_CLEANUP_EVENT_RX_DEACTIVATE_ISSUE_SEQ_SCRIPT_TYPE_PAYLOAD);

/* 
** HOME_RESTORE_EVENT_SCRIPT Type 
*/ 
#define RFLM_TDS_RF_EVT_HOME_RESTORE_EVENT_SCRIPT_TYPE_MAX_TASKS                5
#define RFLM_TDS_RF_EVT_HOME_RESTORE_EVENT_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS   10
#define RFLM_TDS_RF_EVT_HOME_RESTORE_EVENT_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS   35
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_HOME_RESTORE_EVENT_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_HOME_RESTORE_EVENT_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_HOME_RESTORE_EVENT_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS*2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_home_restore_event_script_t, \
                               RFLM_TDS_RF_EVT_HOME_RESTORE_EVENT_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_HOME_RESTORE_EVENT_SCRIPT_TYPE_PAYLOAD);
                               
/*---------------------------------------------------------------------------*/
/*                        Rx LNA Event Scripts Definitions                   */
/*---------------------------------------------------------------------------*/

/* 
** BUILD_RX_LNA_SCRIPT_BUF_X Type (X=0..23) 3(band) * 4(LNA gain states) * 2 (PRx or DRx)
*/ 

#define RFLM_TDS_RF_EVT_BUILD_RX_LNA_SCRIPT_BUF_X_TYPE_MAX_TASKS             2
#define RFLM_TDS_RF_EVT_BUILD_RX_LNA_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_BUILD_RX_LNA_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS 10
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_BUILD_RX_LNA_SCRIPT_BUF_X_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_BUILD_RX_LNA_SCRIPT_BUF_X_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_BUILD_RX_LNA_SCRIPT_BUF_X_TYPE_MAX_RFFE_TRANSACTIONS *2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_build_rx_lna_script_buf_x_t, \
                               RFLM_TDS_RF_EVT_BUILD_RX_LNA_SCRIPT_BUF_X_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_BUILD_RX_LNA_SCRIPT_BUF_X_TYPE_PAYLOAD);

/* 
** BUILD_RX_LNA_LATCH_SCRIPT Type 
*/ 

#define RFLM_TDS_RF_EVT_BUILD_RX_LNA_LATCH_SCRIPT_TYPE_MAX_TASKS             2
#define RFLM_TDS_RF_EVT_BUILD_RX_LNA_LATCH_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_BUILD_RX_LNA_LATCH_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 10
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_BUILD_RX_LNA_LATCH_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_BUILD_RX_LNA_LATCH_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_BUILD_RX_LNA_LATCH_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS *2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_build_rx_lna_latch_script_buf_x_t, \
                               RFLM_TDS_RF_EVT_BUILD_RX_LNA_LATCH_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_BUILD_RX_LNA_LATCH_SCRIPT_TYPE_PAYLOAD);



/*---------------------------------------------------------------------------*/
/*                        Wakeup Event Scripts Definitions                   */
/*---------------------------------------------------------------------------*/

/* 
** BUILD_WAKEUP_SCRIPT Type 
*/ 

#define RFLM_TDS_RF_EVT_BUILD_WAKEUP_SCRIPT_TYPE_MAX_TASKS             15
#define RFLM_TDS_RF_EVT_BUILD_WAKEUP_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_BUILD_WAKEUP_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 185
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_BUILD_WAKEUP_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_BUILD_WAKEUP_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_BUILD_WAKEUP_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS *2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_build_wakeup_script_t, \
                               RFLM_TDS_RF_EVT_BUILD_WAKEUP_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_BUILD_WAKEUP_SCRIPT_TYPE_PAYLOAD);

#define RFLM_TDS_RF_EVT_BUILD_TX_WAKEUP_WTR_SCRIPT_TYPE_MAX_TASKS             5
#define RFLM_TDS_RF_EVT_BUILD_TX_WAKEUP_WTR_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_BUILD_TX_WAKEUP_WTR_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 200
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_BUILD_TX_WAKEUP_WTR_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_BUILD_TX_WAKEUP_WTR_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_BUILD_TX_WAKEUP_WTR_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS *2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_build_tx_wakeup_wtr_script_t, \
                               RFLM_TDS_RF_EVT_BUILD_TX_WAKEUP_WTR_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_BUILD_TX_WAKEUP_WTR_SCRIPT_TYPE_PAYLOAD);

#define RFLM_TDS_RF_EVT_BUILD_TX_WAKEUP_NON_WTR_SCRIPT_TYPE_MAX_TASKS             15
#define RFLM_TDS_RF_EVT_BUILD_TX_WAKEUP_NON_WTR_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_BUILD_TX_WAKEUP_NON_WTR_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 60
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_BUILD_TX_WAKEUP_NON_WTR_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_BUILD_TX_WAKEUP_NON_WTR_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_BUILD_TX_WAKEUP_NON_WTR_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS *2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_build_tx_wakeup_non_wtr_script_t, \
                               RFLM_TDS_RF_EVT_BUILD_TX_WAKEUP_NON_WTR_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_BUILD_TX_WAKEUP_NON_WTR_SCRIPT_TYPE_PAYLOAD);


/*---------------------------------------------------------------------------*/
/*                        BHO START_UP Event Scripts Definitions             */
/*---------------------------------------------------------------------------*/

/* 
** BHO_STARTUP_EVENT_TX_ONE_TIME_INIT_SCRIPT Type 
*/ 
#define RFLM_TDS_RF_EVT_BHO_STARTUP_EVENT_TX_ONE_TIME_INIT_SCRIPT_TYPE_MAX_TASKS             7
#define RFLM_TDS_RF_EVT_BHO_STARTUP_EVENT_TX_ONE_TIME_INIT_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_BHO_STARTUP_EVENT_TX_ONE_TIME_INIT_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 100
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_BHO_STARTUP_EVENT_TX_ONE_TIME_INIT_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_BHO_STARTUP_EVENT_TX_ONE_TIME_INIT_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_BHO_STARTUP_EVENT_TX_ONE_TIME_INIT_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS *2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_bho_startup_event_tx_one_time_init_script_t, \
                               RFLM_TDS_RF_EVT_BHO_STARTUP_EVENT_TX_ONE_TIME_INIT_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_BHO_STARTUP_EVENT_TX_ONE_TIME_INIT_SCRIPT_TYPE_PAYLOAD);


/*---------------------------------------------------------------------------*/
/*                        DO_HDET Event Scripts Definitions                  */
/*---------------------------------------------------------------------------*/

/* 
** DO_HDET_EVENT_HDET_WRITE_SCRIPT Type 
*/ 
#define RFLM_TDS_RF_EVT_DO_HDET_EVENT_HDET_WRITE_SCRIPT_TYPE_MAX_TASKS             10
#define RFLM_TDS_RF_EVT_DO_HDET_EVENT_HDET_WRITE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_DO_HDET_EVENT_HDET_WRITE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 30
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_DO_HDET_EVENT_HDET_WRITE_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_DO_HDET_EVENT_HDET_WRITE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_DO_HDET_EVENT_HDET_WRITE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS *2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_do_hdet_event_hdet_write_script_t, \
                               RFLM_TDS_RF_EVT_DO_HDET_EVENT_HDET_WRITE_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_DO_HDET_EVENT_HDET_WRITE_SCRIPT_TYPE_PAYLOAD);


/* 
** DO_HDET_EVENT_HDET_READ_SCRIPT Type 
*/ 
#define RFLM_TDS_RF_EVT_DO_HDET_EVENT_HDET_READ_SCRIPT_TYPE_MAX_TASKS             1
#define RFLM_TDS_RF_EVT_DO_HDET_EVENT_HDET_READ_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_DO_HDET_EVENT_HDET_READ_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 1
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_DO_HDET_EVENT_HDET_READ_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_DO_HDET_EVENT_HDET_READ_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_DO_HDET_EVENT_HDET_READ_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS *2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_do_hdet_event_hdet_read_script_t, \
                               RFLM_TDS_RF_EVT_DO_HDET_EVENT_HDET_READ_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_DO_HDET_EVENT_HDET_READ_SCRIPT_TYPE_PAYLOAD);


/** DO_JDET_EVENT_JDET_READ_SCRIPT Type 
*/ 
#define RFLM_TDS_RF_EVT_DO_JDET_EVENT_JDET_READ_SCRIPT_TYPE_MAX_TASKS             1
#define RFLM_TDS_RF_EVT_DO_JDET_EVENT_JDET_READ_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_DO_JDET_EVENT_JDET_READ_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 2
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_DO_JDET_EVENT_JDET_READ_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_DO_JDET_EVENT_JDET_READ_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_DO_JDET_EVENT_JDET_READ_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS *2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_do_jdet_event_jdet_read_script_t, \
                               RFLM_TDS_RF_EVT_DO_JDET_EVENT_JDET_READ_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_DO_JDET_EVENT_JDET_READ_SCRIPT_TYPE_PAYLOAD);


/** DO_IP2_CAL_CODE_WRITE_SCRIPT Type 
*/ 
#define RFLM_TDS_RF_EVT_DO_IP2_CAL_CODE_WRITE_SCRIPT_TYPE_MAX_TASKS             1
#define RFLM_TDS_RF_EVT_DO_IP2_CAL_CODE_WRITE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_DO_IP2_CAL_CODE_WRITE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 2
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_DO_IP2_CAL_CODE_WRITE_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_DO_IP2_CAL_CODE_WRITE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_DO_IP2_CAL_CODE_WRITE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS *2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_do_ip2_cal_code_write_script_t, \
                               RFLM_TDS_RF_EVT_DO_IP2_CAL_CODE_WRITE_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_DO_IP2_CAL_CODE_WRITE_SCRIPT_TYPE_PAYLOAD);


/*---------------------------------------------------------------------------*/
/*                        DO_THERM Event Scripts Definitions                 */
/*---------------------------------------------------------------------------*/

/* 
** DO_THERM_EVENT_THERM_WRITE_SCRIPT Type 
*/ 
#define RFLM_TDS_RF_EVT_DO_THERM_EVENT_THERM_WRITE_SCRIPT_TYPE_MAX_TASKS             10
#define RFLM_TDS_RF_EVT_DO_THERM_EVENT_THERM_WRITE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_DO_THERM_EVENT_THERM_WRITE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 50
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_DO_THERM_EVENT_THERM_WRITE_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_DO_THERM_EVENT_THERM_WRITE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_DO_THERM_EVENT_THERM_WRITE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS *2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_do_therm_event_therm_write_script_t, \
                               RFLM_TDS_RF_EVT_DO_THERM_EVENT_THERM_WRITE_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_DO_THERM_EVENT_THERM_WRITE_SCRIPT_TYPE_PAYLOAD);


/* 
** DO_THERM_EVENT_THERM_READ_SCRIPT Type 
*/ 
#define RFLM_TDS_RF_EVT_DO_THERM_EVENT_THERM_READ_SCRIPT_TYPE_MAX_TASKS             10
#define RFLM_TDS_RF_EVT_DO_THERM_EVENT_THERM_READ_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_DO_THERM_EVENT_THERM_READ_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 50
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_DO_THERM_EVENT_THERM_READ_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_DO_THERM_EVENT_THERM_READ_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_DO_THERM_EVENT_THERM_READ_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS *2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_do_therm_event_therm_read_script_t, \
                               RFLM_TDS_RF_EVT_DO_THERM_EVENT_THERM_READ_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_DO_THERM_EVENT_THERM_READ_SCRIPT_TYPE_PAYLOAD);


/* 
** DO_THERM_EVENT_TUNER_THERM_READ_SCRIPT Type 
*/ 
#define RFLM_TDS_RF_EVT_DO_THERM_EVENT_TUNER_THERM_READ_SCRIPT_TYPE_MAX_TASKS             1
#define RFLM_TDS_RF_EVT_DO_THERM_EVENT_TUNER_THERM_READ_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_DO_THERM_EVENT_TUNER_THERM_READ_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 1
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_DO_THERM_EVENT_TUNER_THERM_READ_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_DO_THERM_EVENT_TUNER_THERM_READ_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_DO_THERM_EVENT_TUNER_THERM_READ_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS *2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_do_therm_event_tuner_therm_read_script_t, \
                               RFLM_TDS_RF_EVT_DO_THERM_EVENT_TUNER_THERM_READ_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_DO_THERM_EVENT_TUNER_THERM_READ_SCRIPT_TYPE_PAYLOAD);

/*---------------------------------------------------------------------------*/
/*                       GET PLL Event Scripts Definitions                   */
/*---------------------------------------------------------------------------*/

/* 
** DO_RX_PLL_READ_SCRIPT Type 
*/ 
#define RFLM_TDS_RF_EVT_DO_RX_PLL_READ_SCRIPT_TYPE_MAX_TASKS             1
#define RFLM_TDS_RF_EVT_DO_RX_PLL_READ_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_DO_RX_PLL_READ_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 1
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_DO_RX_PLL_READ_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_DO_RX_PLL_READ_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_DO_RX_PLL_READ_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS *2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_do_rx_pll_status_read_script_t, \
                               RFLM_TDS_RF_EVT_DO_RX_PLL_READ_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_DO_RX_PLL_READ_SCRIPT_TYPE_PAYLOAD);


//Get Pll Status for Dime
/* 
** DO_TX_PLL_READ_SCRIPT Type 
*/ 
#define RFLM_TDS_RF_EVT_DO_TX_PLL_READ_SCRIPT_TYPE_MAX_TASKS             1
#define RFLM_TDS_RF_EVT_DO_TX_PLL_READ_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_DO_TX_PLL_READ_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 1
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_DO_TX_PLL_READ_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_DO_TX_PLL_READ_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_DO_TX_PLL_READ_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS *2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_do_tx_pll_status_read_script_t, \
                               RFLM_TDS_RF_EVT_DO_RX_PLL_READ_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_DO_RX_PLL_READ_SCRIPT_TYPE_PAYLOAD);


/*---------------------------------------------------------------------------*/
/*                        AutoPin Scripts Definitions                  */
/*---------------------------------------------------------------------------*/

/* 
** AUTOPIN_WRITE_SCRIPT Type 
*/ 
#define RFLM_TDS_RF_EVT_AUTOPIN_WRITE_SCRIPT_TYPE_MAX_TASKS             20
#define RFLM_TDS_RF_EVT_AUTOPIN_WRITE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS 0
#define RFLM_TDS_RF_EVT_AUTOPIN_WRITE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS 60
/* SSBI: 2 address/data pairs per 32 bit word; ext-RFFE needs 2*32 bit word */
#define RFLM_TDS_RF_EVT_AUTOPIN_WRITE_SCRIPT_TYPE_PAYLOAD \
      ((RFLM_TDS_RF_EVT_AUTOPIN_WRITE_SCRIPT_TYPE_MAX_SSBI_TRANSACTIONS + 1)/2 \
      + RFLM_TDS_RF_EVT_AUTOPIN_WRITE_SCRIPT_TYPE_MAX_RFFE_TRANSACTIONS *2)

RFLM_TDS_GENERIC_CCS_EVENT_TYPEDEF( rflm_tds_autopin_write_script_t, \
                               RFLM_TDS_RF_EVT_AUTOPIN_WRITE_SCRIPT_TYPE_MAX_TASKS, \
                               RFLM_TDS_RF_EVT_AUTOPIN_WRITE_SCRIPT_TYPE_PAYLOAD);




/* PA state update information */
typedef enum
{
  RFLM_TDS_PA_STATE_GRFC_TYPE,
  RFLM_TDS_PA_STATE_RFFE_NON_EXT_TYPE,
  RFLM_TDS_PA_STATE_RFFE_EXT_TYPE,

  RFLM_TDS_APT_RFFE_NON_EXT_TYPE,

  RFLM_TDS_ASM_RFFE_NON_EXT_TYPE,

  RFLM_TDS_TUNER_AOL_RFFE_NON_EXT_TYPE,

  RFLM_TDS_PA_STATE_INVALID_TYPE,
} rflm_tds_dynamic_update_info_e;

typedef struct
{
  /* grfc bitmask to update */
  uint32 grfc_bitmask[RFLM_TDS_RX_TX_MAX_NUM_TX_GAIN_STATES][2];
  /* task number for each type of event */
  uint8  off_to_tx_event_tasknum[2];
  uint8  rx_to_tx_event_tasknum[2];
  uint8  tx_to_tx_event_tasknum[2];
} rflm_tds_pa_state_grfc_info_t;

typedef struct
{
  uint8  rffe_writes[RFLM_TDS_RX_TX_MAX_NUM_TX_GAIN_STATES][10];
  uint8  num_rffe_writes_for_each_state[RFLM_TDS_RX_TX_MAX_NUM_TX_GAIN_STATES];
  /* task number for each type of event */
  uint8  off_to_tx_event_tasknum;
  uint8  rx_to_tx_event_tasknum;
  uint8  tx_to_tx_event_tasknum;
} rflm_tds_pa_state_rffe_info_t;

typedef struct
{
  rflm_tds_dynamic_update_info_e update_type;

  union
  {
    /* GRFC control PA */
    rflm_tds_pa_state_grfc_info_t grfc_control_pa_info;

    /* RFFE control PA */
    rflm_tds_pa_state_rffe_info_t rffe_control_pa_info;
  };
} rflm_tds_pa_state_update_info_t;

/* APT update information */
typedef struct
{
  rflm_tds_dynamic_update_info_e update_type;

  /* task number for each type of event */
  uint8  off_to_tx_event_tasknum;
  uint8  rx_to_tx_event_tasknum;
  uint8  tx_to_tx_event_tasknum;
} rflm_tds_apt_update_info_t;

/* TUNER AOL update information */
typedef struct
{
  rflm_tds_dynamic_update_info_e update_type;

  /* rx0: task number for each type of event */
  /* Note: There are two types of scripts. Prxonly and prxdrx. 
   The tasknum might be different */
  uint8  prxonly_script_rx0_off_to_rx_event_tasknum; 
  uint8  prxdrx_script_rx0_off_to_rx_event_tasknum;
  uint8  prxonly_script_rx0_tx_to_rx_event_tasknum;
  uint8  prxdrx_script_rx0_tx_to_rx_event_tasknum;

  /* rx1: task number for each type of event */
  uint8  prxdrx_script_rx1_off_to_rx_event_tasknum;
  uint8  prxdrx_script_rx1_tx_to_rx_event_tasknum;

  /* Tx: task number for each type of event */
  uint8  tx_off_to_tx_event_tasknum;
  uint8  tx_rx_to_tx_event_tasknum;
  
} rflm_tds_tuner_aol_update_info_t;

/* ASM update information */
typedef struct
{
  rflm_tds_dynamic_update_info_e update_type;

  /* rx0: task number for each type of event */
  // For ASM, we need to support update upto to 3 cascading devices. 
  uint8  rx0_num_device; 
  // Note: There are two types of scripts. Prxonly and prxdrx. The tasknum might be different
  uint8  prxonly_script_rx0_off_to_rx_event_tasknum[3]; 
  uint8  prxdrx_script_rx0_off_to_rx_event_tasknum[3];
  uint8  prxonly_script_rx0_tx_to_rx_event_tasknum[3];
  uint8  prxdrx_script_rx0_tx_to_rx_event_tasknum[3];

  /* rx1: task number for each type of event */
  uint8  rx1_num_device;  
  uint8  prxdrx_script_rx1_off_to_rx_event_tasknum[3];  
  uint8  prxdrx_script_rx1_tx_to_rx_event_tasknum[3];

  /* Tx chain: task number for each type of event */
  uint8  tx_num_device;  
  uint8  tx_off_to_tx_event_tasknum[3];  
  uint8  tx_rx_to_tx_event_tasknum[3];
  
} rflm_tds_asm_update_info_t;

typedef enum
{
  RFLM_TDS_OFF_TO_RX,
  RFLM_TDS_RX_TO_OFF,
  RFLM_TDS_OFF_TO_TX,
  RFLM_TDS_TX_TO_OFF,
  RFLM_TDS_RX_TO_TX,
  RFLM_TDS_TX_TO_RX,
  RFLM_TDS_TX_TO_TX,
  RFLM_TDS_RX_TO_RX,

  RFLM_TDS_TXRX_TO_OFF,

  RFLM_TDS_MAX_EVENT,
  RFLM_TDS_NO_EVENT
} rflm_tds_event_e;

typedef struct
{
  int32 start_offset;
} rflm_tds_event_info_t;

typedef struct
{  
  rflm_ccs_rf_seq_t  offToTxEventTxActivateIssueSeqScript[RFLM_TDS_MAX_PINGPONG_BUFFERS];

  rflm_ccs_rf_seq_t  rxToTxEventRxDeactivateTriggerIssueSeqScriptPrxOnly[RFLM_TDS_MAX_PINGPONG_BUFFERS];
  rflm_ccs_rf_seq_t  rxToTxEventRxDeactivateTriggerIssueSeqScriptPrxDrx[RFLM_TDS_MAX_PINGPONG_BUFFERS];

  rflm_ccs_rf_seq_t  rxToTxEventTxActivateNonTriggerIssueSeqScriptPrxOnlyPowerLtoHBufX[RFLM_TDS_MAX_PINGPONG_BUFFERS];
  rflm_ccs_rf_seq_t  rxToTxEventTxActivateNonTriggerIssueSeqScriptPrxDrxPowerLtoHBufX[RFLM_TDS_MAX_PINGPONG_BUFFERS];

  rflm_ccs_rf_seq_t  txToOffEventTxDeactivateIssueSeqScript[RFLM_TDS_MAX_PINGPONG_BUFFERS];
} rflm_tds_preprocessing_issue_seq_t;


typedef struct 
{
  // OFF-to-RX event
  rflm_tds_off_to_rx_event_rx_tuning_script_prx_only_buf_x_t                        offToRxEventRxTuningScriptPrxOnlyBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];
  rflm_tds_off_to_rx_event_rx_tuning_script_prx_drx_buf_x_t                         offToRxEventRxTuningScriptPrxDrxBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];
  rflm_tds_off_to_rx_event_rx_enable_script_t                                       offToRxEventRxEnableScript[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];
  rflm_tds_off_to_rx_event_rx_activate_issue_seq_script_buf_x_t                     offToRxEventRxActivateIssueSeqScriptPrxOnlyBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS][RFLM_TDS_MAX_PINGPONG_BUFFERS];
  rflm_tds_off_to_rx_event_rx_activate_issue_seq_script_buf_x_t                     offToRxEventRxActivateIssueSeqScriptPrxDrxBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS][RFLM_TDS_MAX_PINGPONG_BUFFERS];
  	
  // RX-to-OFF event
  rflm_tds_rx_to_off_event_rx_deactivate_issue_seq_script_buf_x_t                   rxToOffEventRxDeactivateIssueSeqScriptPrxOnlyBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS][RFLM_TDS_MAX_PINGPONG_BUFFERS];
  rflm_tds_rx_to_off_event_rx_deactivate_issue_seq_script_buf_x_t                   rxToOffEventRxDeactivateIssueSeqScriptPrxDrxBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS][RFLM_TDS_MAX_PINGPONG_BUFFERS];

  // OFF-to-TX event
  rflm_tds_off_to_tx_event_tx_tuning_script_buf_x_t                                 offToTxEventTxTuningScriptBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];
  rflm_tds_off_to_tx_event_tx_enable_script_t                                       offToTxEventTxEnableScript[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];  
#if 0 // comment out for pre-processing issue_seq
  rflm_tds_off_to_tx_event_tx_activate_issue_seq_script_buf_x_t                     offToTxEventTxActivateIssueSeqScriptPowerLtoHBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS][RFLM_TDS_MAX_PINGPONG_BUFFERS];
#else
  rflm_tds_off_to_tx_event_tx_activate_issue_seq_script_buf_x_t                     offToTxEventTxActivateIssueSeqScriptPowerLtoHBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];
#endif

#if 0 // space holder for single PLL chip
  // IFREQ-to-TX event
  rflm_tds_ifreq_to_tx_event_tx_tuning_script_buf_x_t                               ifreqToTxEventTxTuningScriptBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];
  rflm_tds_ifreq_to_tx_event_tx_enable_script_t                                     ifreqToTxEventTxEnableScript[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];  
  rflm_tds_ifreq_to_tx_event_tx_activate_issue_seq_script_buf_x_t                   ifreqToTxEventTxActivateIssueSeqScriptPowerLtoHBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];
#endif

  // TX-to-OFF event
#if 0 // comment out for pre-processing issue_seq
  rflm_tds_tx_to_off_event_tx_deactivate_issue_seq_script_buf_x_t                   txToOffEventTxDeactivateIssueSeqScriptBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS][RFLM_TDS_MAX_PINGPONG_BUFFERS]; 
#else
  rflm_tds_tx_to_off_event_tx_deactivate_issue_seq_script_buf_x_t                   txToOffEventTxDeactivateIssueSeqScriptBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS]; 
#endif
  rflm_tds_tx_to_off_event_txrx_deactivate_issue_seq_script_buf_x_t                 txToOffEventTxRxDeactivateIssueSeqScriptBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS][RFLM_TDS_MAX_PINGPONG_BUFFERS]; 

  // RX-to-TX event
  rflm_tds_rx_to_tx_event_tx_tuning_script_buf_x_t                                  rxToTxEventTxTuningScriptBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];
  rflm_tds_rx_to_tx_event_tx_enable_script_t                                        rxToTxEventTxEnableScript[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];  
#if 0 // comment out for pre-processing issue_seq
  rflm_tds_rx_to_tx_event_rx_deactivate_trigger_issue_seq_script_buf_x_t            rxToTxEventRxDeactivateTriggerIssueSeqScriptPrxOnlyBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS][RFLM_TDS_MAX_PINGPONG_BUFFERS];
  rflm_tds_rx_to_tx_event_rx_deactivate_trigger_issue_seq_script_buf_x_t            rxToTxEventRxDeactivateTriggerIssueSeqScriptPrxDrxBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS][RFLM_TDS_MAX_PINGPONG_BUFFERS];

  rflm_tds_rx_to_tx_event_tx_activate_non_trigger_issue_seq_script_buf_x_t          rxToTxEventTxActivateNonTriggerIssueSeqScriptPrxOnlyPowerLtoHBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS][RFLM_TDS_MAX_PINGPONG_BUFFERS];
  rflm_tds_rx_to_tx_event_tx_activate_non_trigger_issue_seq_script_buf_x_t          rxToTxEventTxActivateNonTriggerIssueSeqScriptPrxDrxPowerLtoHBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS][RFLM_TDS_MAX_PINGPONG_BUFFERS];
#else
  rflm_tds_rx_to_tx_event_rx_deactivate_trigger_issue_seq_script_buf_x_t            rxToTxEventRxDeactivateTriggerIssueSeqScriptPrxOnlyBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];
  rflm_tds_rx_to_tx_event_rx_deactivate_trigger_issue_seq_script_buf_x_t            rxToTxEventRxDeactivateTriggerIssueSeqScriptPrxDrxBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];

  rflm_tds_rx_to_tx_event_tx_activate_non_trigger_issue_seq_script_buf_x_t          rxToTxEventTxActivateNonTriggerIssueSeqScriptPrxOnlyPowerLtoHBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];
  rflm_tds_rx_to_tx_event_tx_activate_non_trigger_issue_seq_script_buf_x_t          rxToTxEventTxActivateNonTriggerIssueSeqScriptPrxDrxPowerLtoHBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];
#endif
  
  // TX-to-RX event
  rflm_tds_tx_to_rx_event_rx_tuning_script_prx_only_buf_x_t                         txToRxEventRxTuningScriptPrxOnlyBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];
  rflm_tds_tx_to_rx_event_rx_tuning_script_prx_drx_buf_x_t                          txToRxEventRxTuningScriptPrxDrxBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];
  rflm_tds_tx_to_rx_event_rx_enable_script_t                                        txToRxEventRxEnableScript[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];  
  rflm_tds_tx_to_rx_event_rx_activate_tx_deactivate_issue_seq_script_buf_x_t        txToRxEventRxActivateTxDeactivateIssueSeqScriptPrxOnlyBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS][RFLM_TDS_MAX_PINGPONG_BUFFERS];
  rflm_tds_tx_to_rx_event_rx_activate_tx_deactivate_issue_seq_script_buf_x_t        txToRxEventRxActivateTxDeactivateIssueSeqScriptPrxDrxBufX[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS][RFLM_TDS_MAX_PINGPONG_BUFFERS];
    
  // TX-to-TX event
  rflm_tds_tx_to_tx_event_tx_activate_issue_seq_script_t                            txToTxEventTxActivateIssueSeqScriptPowerLtoH[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];  

  // BHO_startup event
  //rflm_tds_bho_startup_event_tx_one_time_init_script_t                                                        bhoStartupEventTxOneTimeInitScript;  
  
  // BHO_cleanup event  
  
  // IFREQ_startup event
  rflm_tds_ifreq_startup_event_rx_tuning_script_prx_drx_buf_x_t                     ifreqStartupEventRxTuningScriptPrxDrxBufX[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];
  rflm_tds_ifreq_startup_event_rx_enable_script_t                                   ifreqStartupEventRxEnableScript[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];
  rflm_tds_ifreq_startup_event_rx_activate_issue_seq_script_buf_x_t                 ifreqStartupEventRxActivateIssueSeqScriptPrxDrxBufX[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];
  
  // IFREQ_cleanup event  
  //rflm_tds_ifreq_cleanup_event_rx_tuning_script_prx_drx_t                                                    ifreqCleanupEventRxTuningScriptPrxDrx[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];
  //rflm_tds_ifreq_cleanup_event_rx_deactivate_issue_seq_script_t                                          ifreqCleanupEventRxDeactivateIssueSeqScriptPrxDrx[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];
  
  // IFREQ Tuner startup event
  rflm_tds_ifreq_startup_event_tuner_update_script_t                                ifreqStartupEventTunerUpdateScript[2];
  
  // Home Restore event
  rflm_tds_home_restore_event_script_t                                              homeRestoreEventScript[RFLM_TDS_MAX_NUM_HOME_RESTORE_SCRIPT_BUFFER];
  
  // DO_HDET event
  rflm_tds_do_hdet_event_hdet_write_script_t                                        doHdetEventHdetWriteScript;
  rflm_tds_do_hdet_event_hdet_read_script_t                                         doHdetEventHdetReadScript;
  
  // DO_THERM event
  rflm_tds_do_therm_event_therm_write_script_t                                      doThermEventThermWriteScript;
  rflm_tds_do_therm_event_therm_read_script_t                                       doThermEventThermReadScript;

  // Tuner Temp Event
  rflm_tds_do_therm_event_tuner_therm_read_script_t                                 doThermEventTunerThermReadScript;

  // Get Pll Status
  rflm_tds_do_rx_pll_status_read_script_t 	                                    doRxPllStatusReadScript;
  rflm_tds_do_tx_pll_status_read_script_t                                           doTxPllStatusReadScript;

  // Build Rx LNA scripts
  rflm_tds_build_rx_lna_script_buf_x_t                                              rxLnaScriptBufX[RFLM_TDS_RX_TX_MAX_NUM_RX_LIN_STATES][RFLM_TDS_RX_TX_MAX_NUM_BANDS][RFLM_TDS_RX_TX_MAX_NUM_RX_GAIN_STATES][RFLM_TDS_RX_TX_MAX_NUM_CHAINS];
  rflm_tds_build_rx_lna_latch_script_buf_x_t                                        rxLnaLatchPrxOnlyScript;
  rflm_tds_build_rx_lna_latch_script_buf_x_t                                        rxLnaLatchDrxOnlyScript;
  rflm_tds_build_rx_lna_latch_script_buf_x_t                                        rxLnaLatchPrxDrxScript;

  // Build wakeup scripts
  rflm_tds_build_wakeup_script_t                                                    rxWakeupScript;
  rflm_tds_build_tx_wakeup_wtr_script_t                                             txWakeupScriptWtrCommon;
  rflm_tds_build_tx_wakeup_wtr_script_t                                             txWakeupScriptWtrNonCommon;
  rflm_tds_build_tx_wakeup_non_wtr_script_t                                         txWakeupScriptNonWtr;

  // TDS Standalone Event Information
  rflm_tds_rf_script_info_t                                                         tdsEventInfo[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];

  // TDS Inter-freq Event Information
  rflm_tds_rf_script_info_t                                                         ifreqEventInfo[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];

  // TDS IRAT Event Information
  rflm_tds_rf_script_info_t                                                         iratEventInfo;

  // Info to update PA_STATE
  rflm_tds_pa_state_update_info_t                                                   paStateUpdateInfo[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];

  // Info to update APT
  rflm_tds_apt_update_info_t                                                        aptUpdateInfo[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];
  
  // Info to update Tuner AOL
  rflm_tds_tuner_aol_update_info_t                                                  tunerAolUpdateInfo[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];

  // Info to update ASM
  rflm_tds_asm_update_info_t                                                        asmUpdateInfo[RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];

  // Event info
  rflm_tds_event_info_t                                                             homeEventTimingPrxOnly[RFLM_TDS_MAX_EVENT][RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];
  rflm_tds_event_info_t                                                             homeEventTimingPrxDrx[RFLM_TDS_MAX_EVENT][RFLM_TDS_RX_TX_MAX_NUM_HOME_TARGET_BUFFERS];
  rflm_tds_event_info_t                                                             ifreqEventTiming[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];

  // JDET EVENT
  rflm_tds_do_jdet_event_jdet_read_script_t                                         doJdetEventJdetReadScript[6];

  // Pre-processed ISSUE_SEQ
  rflm_tds_preprocessing_issue_seq_t                                                preProcessedSeq;
  
  //IP2 cal code write
  rflm_tds_do_ip2_cal_code_write_script_t                                           doIp2CalCodeWriteScript[RFLM_TDS_RX_TX_MAX_NUM_BANDS][RFLM_TDS_RX_TX_MAX_NUM_RX_LIN_STATES];

  //AutoPin
  rflm_tds_autopin_write_script_t                                                   autopinWriteScript[2];

} rflm_tds_ccs_event_t;


typedef struct 
{
  // Build Rx LNA scripts
  rflm_tds_build_rx_lna_script_buf_x_t                                              rxLnaScriptBufX[RFLM_TDS_RX_TX_MAX_NUM_BANDS][RFLM_TDS_RX_TX_MAX_NUM_RX_GAIN_STATES];
  rflm_tds_build_rx_lna_latch_script_buf_x_t                                        rxLnaLatchPrxOnlyScript;

  // TDS IRAT Event Information
  rflm_tds_rf_script_info_t                                                         tdsIratEventInfo;  

} rflm_tds_meas_ccs_event_t;

typedef struct
{
  /*! \brief Number of device per chain. Chain is defined as either Rx0, Rx1 or Tx. Require to support multiple cascading devices  */
  uint8 num_device_per_chain;

  /*! \brief Number of writes (support up to 5 devices) */
  uint8 num_write[RFLM_TDS_RFC_MAX_DEVICE_INSTANCES_PER_BAND];

  /*! \brief Register values i.e., content to be updated (support up to 15 writes per device and up to 5 devices  */
  uint8 update_info_array[RFLM_TDS_RFC_MAX_DEVICE_INSTANCES_PER_BAND][RFLM_TDS_RX_TX_MAX_NUM_WRITES_UPDATE];

} rflm_tds_rx_tx_rf_dynamic_update_for_single_chain_t;

typedef struct 
{
  /*! \brief Update type  */
  rflm_tds_dynamic_update_info_e update_type;

  /*! \brief The buffer index of the script to be updated  */
  uint8 buf_index;

  /*! \brief The update info for Rx0  */
  rflm_tds_rx_tx_rf_dynamic_update_for_single_chain_t rx0_info;
  /*! \brief The update info for Rx0  */
  rflm_tds_rx_tx_rf_dynamic_update_for_single_chain_t rx1_info;
  /*! \brief The update info for Rx0  */
  rflm_tds_rx_tx_rf_dynamic_update_for_single_chain_t tx_info;

  /*! \brief The control version of this update. To ACK back to RF  */
  uint16 control_version;
  
} rflm_tds_rx_tx_rf_dynamic_update_t;

typedef struct {

  /*! \brief RF CCS Event Script Table  */
  rflm_tds_ccs_event_t ccsEventTable;
} rflm_tds_rf_sm_intf_t;

#endif /* !FEATURE_RFA_D3925 */


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RFLM_TDS_CCS_H */
