

/*!
  @file
  rfgsm_data_manager.h

  @brief
  RF GSM core data manager to define the script, LUTs, vars and access funcs for
  managing RF data
*/

/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================
                 EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/rfgsm_data_manager.h#1 $
$DateTime: 2016/12/13 07:59:51 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/14/16   ase     Store and dump CCS build script errors
12/07/15   bp      Add API to set TX_STOP CCS event priority to be used in HMSC case
08/19/15   par     Deprecate slot mode control support
01/22/15   piy     Fixed bug in Device status read feature for Mon Burst
12/02/14   sml     Read WTR or RF front end devices status in between Rx/Tx 
                   burst along with GRFC status
11/17/14   sc      Read and calculate delay match DM1-3 in RFLM GSM TxFE config 
10/23/14   sc      Enable event type control from RFSW 
10/07/14   sc      Program all delay match settings in RFLM DRIF
09/19/14   sb      Add IP2 cal moving averaging + pass/fail status 
08/28/14   sc      Clean up Tx data initialisation to reduce wakeup timeline 
08/04/14   cj      Added rfgsm_core_dm_dynamic_common_event_wrapper
07/28/14   ec      Function prototypes for Jammer Detector support
07/25/14   sb      bug fixes for V1 IP2 cal functionality 
07/02/14   tsr     DR-DSDS: Add worst case rx alpha for IDLE mode RX 
07/01/14   tsr     DR-DSDS: Lower RX CCS event priority when GSM sub is in Idle mode 
06/26/14   ch      added rfgsm_core_dm_set_tx_power for CL tuner in RFLM 
06/19/14   sb      changes to support cal on c2 for DSDA 
05/30/14   ldg     Add slot mode control function
05/29/14   sb      IP2 cal bug fixes, updates 
05/08/14   sc      Add RFLM GSM init once API interface 
04/22/14   ch      Passing core_data to rfgsm_core_dm_create_event
04/16/14   sb      IP2 cal Interface and functionality changes 
04/10/14  tsr      Add RFLM RX gain and spectral inv coefficients for DRX 
04/07/14   sc      Clean event API
03/26/14   sb      Add support for IP2 cal for Dime PM 
03/19/14   sc      Add set RFLM DM device function
03/06/14   sc      Correct the PA ramp up table for accurate PA ramp adjust 
02/28/14   sc      Pass core data through mc to core layers 
02/05/14   sc      Use RFLM DM memory space for RF GSM data 
01/30/14   sc      Manage data table independent of device
01/22/14   sc      Remove rfm_dev dependency and unrequired APIs - part 1
01/21/14   ggs     Initial DPD and TXDCOC cal support for Bolt
01/15/14   sc      Remove unused APIs using FW shared memory 
12/13/13   sc      Add set predistortion flag API
12/10/13   sc      Add set envelope gain API functionality
12/09/13   sc      Remove unused monitor alpha update 
11/07/13   sc      Rx gain API using Rx burst type
11/05/13   ggs     Added rfgsm_core_get_tx_cal_dma_data_addr 
10/21/13   sc      Modify dynamic event building for scenario-specific wrappers 
10/21/13   sc      Mainline dynamic event building 
10/14/13   sc      Mainline core DM and RFLM code 
10/10/13   sc      Use RFLM band information to initialise PA ramp tables 
10/03/13   sc      RFLM DM interface data and dynamic event development
09/27/13   sc      Modify RFLM GSM interface data programming functionality
09/12/13   sc      Add dynamic CCS event creation APIs 
09/11/13   sc      Move calls to alloc/free DM mem to enter and exit mode 
09/11/13   sc      Correct Tx calibration data type name 
08/19/13   sc      Prepare for merge from core DM to RFLM
08/13/13   sc      Populate valid CCS event structure task and payload values
08/13/13   sc      Rename definitions ready for RFLM GSM merging
08/08/13   sc      Add master PA ramp table retrieval APIs and ramp definitions 
07/29/13   sc      Modify for integtration and allocation
01/19/12   sc      Initial version

===========================================================================*/

#ifndef RFGSM_DATA_MANAGER_H
#define RFGSM_DATA_MANAGER_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "rfcom.h"
#include "rfgsm_core.h"
#include "rf_hal_buffer.h"
#include "rflm_rf_event_intf.h"
#include "rfgsm_msg.h"

#include "rflm_api_gsm.h"
#include "rflm_gsm_dm.h"



/*===========================================================================

                                  FEATURES

===========================================================================*/



/*===========================================================================

                        EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define RFGSM_DEFAULT_NUM_RAMP_SAMPLES       0
#define RFGSM_DEFAULT_NUM_AMAM_SAMPLES       0
#define RFGSM_DEFAULT_NUM_AMPM_SAMPLES       0

#define RFGSM_RAMP_SATURATION_LOW_LIMIT      0
#define RFGSM_RAMP_SATURATION_HIGH_LIMIT     0x2FFF

#define RFGSM_CORE_DM_MIN_RAMP_UP_DELTA      -8 //qs
#define RFGSM_CORE_DM_MAX_RAMP_UP_DELTA       8 //qs

#define RFGSM_CORE_DM_MIN_RAMP_DOWN_DELTA    -8 //qs
#define RFGSM_CORE_DM_MAX_RAMP_DOWN_DELTA     8 //qs

#define RFGSM_CORE_DM_MAX_INTF_DATA_ENTRIES        RFLM_GSM_DM_MAX_INTF_DATA_ENTRIES
#define RFGSM_CORE_DM_BUFFER_ID_NOT_REQUIRED       RFLM_GSM_BUFFER_ID_NOT_REQUIRED
#define RFGSM_CORE_DM_BURST_NUM_NOT_REQUIRED       RFLM_GSM_BURST_NUM_NOT_REQUIRED

/*---------------------------------------------------------------------------*/

typedef enum
{
  RFGSM_CORE_DOUBLE_BUF0,
  RFGSM_CORE_DOUBLE_BUF1,
  RFGSM_CORE_MAX_DOUBLE_BUF,
}rfgsm_core_dm_double_buff;

/*---------------------------------------------------------------------------*/

typedef rflm_gsm_script_type rfgsm_core_dm_script_type;

/*---------------------------------------------------------------------------*/

typedef rflm_gsm_ccs_task_q rfgsm_core_dm_task_q;

/*===========================================================================

                      DATA MANAGER FUNCTION PROTOTYPES

===========================================================================*/
boolean rfgsm_core_dm_rflm_init( void );

/*---------------------------------------------------------------------------*/

boolean rfgsm_core_dm_rflm_deinit( void );

/*---------------------------------------------------------------------------*/

boolean rfgsm_core_dm_alloc_data_manager_mem( rflm_gsm_subscription_id sub_id );

/*---------------------------------------------------------------------------*/

rflm_gsm_tx_cal_data_type* rfgsm_core_dm_alloc_tx_cal_data_manager_mem( rfm_device_enum_type rfm_dev );

/*---------------------------------------------------------------------------*/

rflm_gsm_intf* rfgsm_core_dm_alloc_intf_data_mem( void );

/*---------------------------------------------------------------------------*/

boolean rfgsm_core_dm_free_data_manager_mem( rflm_gsm_subscription_id sub_id );

/*---------------------------------------------------------------------------*/

boolean rfgsm_core_dm_free_tx_cal_data_manager_mem( rfm_device_enum_type rfm_dev );

/*---------------------------------------------------------------------------*/

boolean rfgsm_core_dm_free_intf_data_mem( void );

/*---------------------------------------------------------------------------*/

rflm_gsm_tx_cal_data_type* rfgsm_core_dm_get_tx_cal_data_manager_addr( rfm_device_enum_type rfm_dev );

/*---------------------------------------------------------------------------*/

boolean rfgsm_core_dm_populate_enter_mode_intf_data( rfgsm_core_data_type *core_data, 
                                                     void *shared_mem_ptr );

boolean rfgsm_core_dm_populate_wakeup_intf_data( rfgsm_core_data_type *core_data, 
                                                    void *shared_mem_ptr );

boolean rfgsm_core_dm_populate_tx_band_intf_data( rfgsm_core_data_type *core_data, 
                                                  void *shared_mem_ptr );

boolean rfgsm_core_dm_populate_therm_read_intf_data( rfgsm_core_data_type *core_data, 
                                                     void *shared_mem_ptr );

boolean rfgsm_core_dm_populate_rx_burst_intf_data( rfgsm_core_data_type *core_data, 
                                                   void *shared_mem_ptr );

boolean rfgsm_core_dm_populate_tx_burst_intf_data( rfgsm_core_data_type *core_data, 
                                                   void *shared_mem_ptr, 
                                                   uint8 num_tx_slots );

/*---------------------------------------------------------------------------*/

boolean rfgsm_core_dm_set_rx_gain(rfgsm_core_data_type * core_data,
                                     rfgsm_rx_burst_type rx_burst_type,
                                     uint8 burst_num,
                                     uint16 gain);

/*---------------------------------------------------------------------------*/

boolean rfgsm_core_dm_set_rx_alpha( rflm_gsm_subscription_id sub_id,
                                    uint16 alpha,
									uint16 worst_case_rx_alpha);

/*---------------------------------------------------------------------------*/

boolean rfgsm_core_dm_set_hmsc_tx_stop_priority( rflm_gsm_subscription_id sub_id,
                                                 uint8 priority );

/*---------------------------------------------------------------------------*/

boolean rfgsm_core_dm_set_spectral_inv( rfgsm_core_data_type* core_data,
                                        rfgsm_rx_burst_type rx_burst_type,
                                        uint8 burst_num,
                                        uint32 rsb_coefA,
                                        uint32 rsb_coefB);

/*---------------------------------------------------------------------------*/

uint16* rfgsm_core_dm_get_amam_table_addr( rflm_gsm_subscription_id sub_id, 
                                           rfgsm_core_dm_double_buff ping_pong_id );

/*---------------------------------------------------------------------------*/

uint16* rfgsm_core_dm_get_ampm_table_addr( rflm_gsm_subscription_id sub_id, 
                                           rfgsm_core_dm_double_buff ping_pong_id );

/*---------------------------------------------------------------------------*/

uint16* rfgsm_core_dm_get_pa_ramp_up_table_addr( rflm_gsm_subscription_id sub_id,
                                                 rfgsm_core_dm_double_buff ping_pong_id,
                                                 uint8 slot );

/*---------------------------------------------------------------------------*/

uint16* rfgsm_core_dm_get_pa_ramp_down_table_addr( rflm_gsm_subscription_id sub_id,
                                                   rfgsm_core_dm_double_buff ping_pong_id,
                                                   uint8 slot );

/*---------------------------------------------------------------------------*/

uint16* rfgsm_core_dm_get_master_pa_ramp_up_table_addr( rflm_gsm_subscription_id sub_id,
                                                        rflm_gsm_band_type tx_band );

/*---------------------------------------------------------------------------*/

uint16* rfgsm_core_dm_get_master_pa_ramp_down_table_addr( rflm_gsm_subscription_id sub_id,
                                                          rflm_gsm_band_type tx_band );

/*---------------------------------------------------------------------------*/

boolean rfgsm_core_dm_set_amam_ampm_tables( rflm_gsm_subscription_id sub_id,
                                            uint8 burst_buffer_id,
                                            rfgsm_core_dm_double_buff ping_pong_id );

/*---------------------------------------------------------------------------*/

uint64* rfgsm_core_get_tx_cal_dma_data_addr( rfm_device_enum_type rfm_dev );

/*---------------------------------------------------------------------------*/

boolean rfgsm_core_dm_set_ramp_tables( rflm_gsm_subscription_id sub_id,
                                       uint8 burst_buffer_id,
                                       rfgsm_core_dm_double_buff ping_pong_id,
                                       uint8 num_tx_slots );

/*---------------------------------------------------------------------------*/

boolean rfgsm_core_dm_set_ramp_deltas( rflm_gsm_subscription_id sub_id,
                                       int16 pa_ramp_up_delta,
                                       int16 pa_ramp_down_delta );

/*---------------------------------------------------------------------------*/

boolean rfgsm_core_dm_set_tx_alpha( rflm_gsm_subscription_id sub_id,
                                    uint16 tx_alpha );

/*---------------------------------------------------------------------------*/

boolean rfgsm_core_dm_set_tx_power( rflm_gsm_subscription_id sub_id,                                    
                                    uint8 burst_buffer_id,
                                    uint8 slot_num,
                                    int32 txPower);
/*---------------------------------------------------------------------------*/

boolean rfgsm_core_dm_set_tx_path_delay( rflm_gsm_subscription_id sub_id,
                                         uint8 burst_buffer_id,
                                         int64 path_delay_val );

/*---------------------------------------------------------------------------*/

boolean rfgsm_core_dm_set_kv_data( rflm_gsm_subscription_id sub_id,
                                   uint16 num_kv_bits,
                                   uint32 tx_kv_data_sequence[10] );

/*---------------------------------------------------------------------------*/

boolean rfgsm_core_dm_set_env_gain_val( rflm_gsm_subscription_id sub_id,
                                        uint8 burst_buffer_id,
                                        uint8 slot_num,
                                        uint16 env_gain );

/*---------------------------------------------------------------------------*/

boolean rfgsm_core_dm_set_predist_flag( rflm_gsm_subscription_id sub_id,
                                        uint8 burst_buffer_id,
                                        uint8 slot_num,
                                        boolean enabled );

/*---------------------------------------------------------------------------*/

boolean rfgsm_core_dm_set_program_amam_ampm_tables_flag( rflm_gsm_subscription_id sub_id,
                                                         uint8 burst_buffer_id,
                                                         boolean flag );



/*=============================================================================
 
                            DYNAMIC mDSP INTERFACE
 
=============================================================================*/

uint8 rfgsm_core_dm_create_event( rfgsm_core_data_type *core_data,
                                  void **event_handle,
                                  rflm_gsm_data_type *table_ptr,
                                  rfgsm_core_dm_script_type script_type,                                  
                                  uint8 burst_num,
                                  rfgsm_core_dm_task_q task_q_id );

boolean rfgsm_core_dm_push_script( void* event_handle,
                                   rf_hal_bus_resource_type bus_resource,
                                   rf_buffer_intf* script,
                                   uint8 script_type,
                                   uint8 task_q_id ,
								   boolean lower_rx_ccs_prio,
								   boolean ssbi_protocol_supported);

boolean rfgsm_core_dm_commit_event( void* event_handle );

void rfgsm_core_dm_cleanup_event( void** event_handle );

boolean rfgsm_core_dm_dynamic_enter_event_wrapper( rfgsm_core_data_type *core_data );
boolean rfgsm_core_dm_dynamic_wakeup_event_wrapper( rfgsm_core_data_type *core_data );
boolean rfgsm_core_dm_dynamic_tx_band_event_wrapper( rfgsm_core_data_type *core_data );
boolean rfgsm_core_dm_dynamic_rx_event_wrapper( rfgsm_core_data_type *core_data,
                                                rfgsm_rx_burst_type rx_burst_type,
                                                uint8 rx_burst_num );
boolean rfgsm_core_dm_dynamic_tx_event_wrapper( rfgsm_core_data_type *core_data,
                                                uint8 num_tx_slots );
boolean rfgsm_core_dm_dynamic_therm_event_wrapper( rfgsm_core_data_type *core_data );

boolean rfgsm_core_dm_set_ip2_cal_info(   uint8 sub_id, 
                                          uint16 step_duration_qs, 
                                          uint16 dc_settling_time_qs, 
                                          uint16 ip2_cal_alpha,
                                          uint8 total_cal_steps);

boolean rfgsm_core_dm_set_update_ip2_cal_grid_info(   uint8 sub_id, 
                                                      rfgsm_ip2_cal_grid_info_type* const common_grid_info);

#ifdef FEATURE_TX_SHARING
boolean rfgsm_core_dm_dynamic_common_event_wrapper( rfgsm_core_data_type* core_data,
                                                    rfgsm_core_dm_script_type script_type,
                                                    rflm_gsm_ccs_task_q task_q,
                                                    rf_buffer_intf** ccs_scripts,
                                                    size_t num_scripts,
                                                    void** event_handle_ptr,
                                                    uint8 burst_num );


#endif

boolean rfgsm_core_dm_device_status_read(uint8 sub_id, uint8 buff_id, rf_burst_type burst_type, 
                                         uint8 event_type, uint8 event_tag, uint8 burst_num);

boolean rfgsm_core_dm_get_last_ip2_cal_iter_result(rflm_gsm_subscription_id sub_id, int32* min_im2_ptr, uint8* step_idx_ptr);


boolean rfgsm_core_dm_dynamic_ip2_cal_setup_event_wrapper( rfgsm_core_data_type *core_data_ptr );

boolean rfgsm_core_dm_dynamic_ip2_cal_step_event_wrapper( rfgsm_core_data_type *core_data_ptr );


boolean rfgsm_core_dm_dynamic_ip2_cal_cleanup_event_wrapper( rfgsm_core_data_type *core_data_ptr);

boolean rfgsm_core_dm_init_ip2_cal_data(rfgsm_core_data_type *core_data_ptr);

boolean rfgsm_core_dm_clean_event( rfgsm_core_data_type* core_data, rfgsm_core_dm_script_type script_type );

boolean rfgsm_core_dm_set_ftm_sub_id(rfm_device_enum_type rfm_dev, rflm_gsm_subscription_id sub_id);

boolean rfgsm_core_dm_populate_ip2_intf_data( rfgsm_core_data_type *core_data,
                                              rfgsm_rx_burst_type rx_burst_type,
                                              uint8 burst_num,
                                              uint8 intf_idx );

boolean rfgsm_core_dm_dynamic_jdet_read_event_wrapper( rfgsm_core_data_type *core_data_ptr, uint8 burst_num, rflm_gsm_script_type jdet_script_type );

boolean rfgsm_core_dm_init_jdet_read_data(rfgsm_core_data_type *core_data_ptr);

/*---------------------------------------------------------------------------*/

#endif /* RFGSM_DATA_MANAGER_H */

