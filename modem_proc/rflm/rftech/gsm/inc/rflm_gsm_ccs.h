/*=============================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/rftech/gsm/inc/rflm_gsm_ccs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/07/15   bp      Add new API to set a new task priority
10/16/14   sc      Enable non-issue sequence event tasks to be atomic
10/06/14   sc      Enable dynamic task queue management from RFSW
08/07/14   sc      Use device from FW for TQ pair assignment
07/24/14   cj      Added changes for Tx sharing feature
07/28/14   ec      Add API to process Jammer Detector CCS events
07/14/14   sc      Remove unused/unrequired functionality
06/24/14   ch      Add PA Ramp up delta start offset to CL tuner trigger time
05/29/14   sb      IP2 cal bug fixes, updates
03/31/14   ch      Created dedicated CL tuner CCS event processing
03/26/14   sb      Add support for IP2 cal for Dime PM
03/12/14   sc      Add disTx/Rx support
02/24/14   sc      Add therm read abort handling
02/06/14   sc      Pass intf idx to process GSM CCS event APIs
12/13/13   cj      Code cleanup for Bolt IRAT
12/09/13   sc      Move logging to common trace buffer
12/09/13   cj      Added IRAT CCS structure initialization and release
11/19/13   cj      Added rflm_gsm_process_irat_ccs_event API
10/31/13   sc      Mutex the variables
10/30/13   sc      Bug fix for CCs mem size ptr manipulation
10/21/13   sc      Bug fixes and logging enhancement
10/03/13   sc      Alloc/Free TQ mem management
09/13/13   sc      Add init and release of CCS mem and TQs
09/09/13   sc      Clean up header includes
09/03/13   sc      Update headers and type resource
08/30/13   sc      Update to use RFLM task struct def
08/28/13   sc	   Initial version of RFLM GSM CCS
=============================================================================*/

#ifndef RFLM_GSM_CCS_H
#define RFLM_GSM_CCS_H


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"
#include "rflm_api_cmn.h"
#include "rflm_ccs_rf_intf.h"
#include "rflm_ccs_rf_event_intf.h"
#include "rflm_cmn_intf.h"
#include "rflm_gsm_dm.h"



/*=============================================================================

                     RFLM GSM CCS DEFINITIONS AND TYPES

=============================================================================*/
/*! @enum rflm_gsm_event_process_status                                      */
/*! @brief A enum for status granularity of static ccs event process         */
typedef enum
{
  RFLM_GSM_CCS_EVENT_DEFAULT,
  RFLM_GSM_CCS_EVENT_SUCCESS,
  RFLM_GSM_CCS_EVENT_HEADER_ERROR,
  RFLM_GSM_CCS_EVENT_STATUS_ERROR,
  RFLM_GSM_CCS_EVENT_TASK_ERROR,
  RFLM_GSM_CCS_EVENT_PROCESS_FAILED,
  RFLM_GSM_CCS_EVENT_FAILED,
  RFLM_GSM_CCS_EVENT_PENDING,
}rflm_gsm_event_process_status;
                         

typedef enum
{
  RFLM_GSM_IP2_CAL_SETUP_EVENT,
  RFLM_GSM_IP2_CAL_STEP_EVENT,
  RFLM_GSM_IP2_CAL_CLEANUP_EVENT
}rflm_gsm_ip2_cal_event_type;

#ifdef FEATURE_TX_SHARING
typedef struct
{

  rflm_ccs_rf_event_description_buffer_header_type *event_header_ptr;
  ccs_rf_task_buf_item_t *event_task_ptr;
  rflm_ustmr_t trigger_time;
  uint32 rflm_gsm_intf_idx;
  uint32 rflm_handle_id;
  boolean intf_idx_valid;
}rflm_gsm_ccs_event_process_data;
#endif

/*============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================== 

                     RFLM GSM CCS RELATED API PROTOTYPES

==============================================================================*/
rflm_gsm_event_process_status rflm_gsm_process_ccs_event
( 
  rflm_ccs_rf_event_description_buffer_header_type *event_header_ptr,
  rflm_ccs_rf_task_buf_item_t *event_task_ptr,
  rflm_ustmr_t trigger_time,
  uint32 rflm_gsm_intf_idx,
  rflm_gsm_ccs_task_q tq,
  rflm_gsm_ccs_event_type event_type
);

rflm_gsm_event_process_status rflm_gsm_process_cltuner_event
( 
  rflm_cmn_tuner_event_data_type *tuner_event_ptr,  
  int32 power_level,
  rflm_ustmr_t trigger_time,
  int16 start_offset,
  uint32 rflm_gsm_intf_idx
);

rflm_gsm_event_process_status rflm_gsm_process_therm_read_abort
(
  rflm_ccs_rf_event_description_buffer_header_type *event_header_ptr_a,
  rflm_ccs_rf_event_description_buffer_header_type *event_header_ptr_b,
  uint32 rflm_gsm_intf_idx
);

void rflm_gsm_ccs_combine_events(rflm_ccs_rf_event_description_buffer_header_type *event_header1,
                                 ccs_rf_task_buf_item_t *event_task1,
                                 rflm_ccs_rf_event_description_buffer_header_type *event_header2,
                                 ccs_rf_task_buf_item_t *event_task2,
                                 rflm_ccs_rf_event_description_buffer_header_type *event_header_combined,
                                 ccs_rf_task_buf_item_t *event_task_combined);


/*---------------------------------------------------------------------------*/

void rflm_gsm_ccs_change_event_priority( ccs_rf_task_buf_item_t *task_ptr, 
                                         uint32 num_tasks, uint8 new_priority );

/*---------------------------------------------------------------------------*/

boolean rflm_gsm_ccs_init_ccs_info( rflm_gsm_subscription_id sub_id, uint32 device_id );

/*---------------------------------------------------------------------------*/

void rflm_gsm_ccs_release_ccs_info( rflm_gsm_subscription_id sub_id, uint32 device_id );

 /*---------------------------------------------------------------------------*/

 rflm_err_t rflm_gsm_rf_send_irat_script( rflm_handle_rx_t rflm_handle,
                                        rflm_gsm_irat_ccs_input_type *script_vars_in,
                                        rflm_gsm_irat_ccs_output_type *script_vars_out,
										uint32* temp_script_buf );

 /*---------------------------------------------------------------------------*/
boolean rflm_gsm_init_ip2_cal_step_info_tbl(rflm_gsm_subscription_id sub_id );

boolean rflm_gsm_reset_ip2_cal_step_info_tbl(rflm_gsm_subscription_id sub_id);

void rflm_gsm_execute_ip2_cal_steps
( 
 rflm_gsm_ip2_cal_step_ccs_input_type *ip2_cal_info,
 rflm_gsm_ip2_cal_step_ccs_output_type *ret_info,
 rflm_gsm_subscription_id sub_id
);

rflm_gsm_event_process_status rflm_gsm_process_ip2_ccs_event
( 
 uint32 curr_index,
 uint32 curr_trigger_time,
 rflm_gsm_subscription_id sub_id,
 rflm_gsm_ip2_cal_event_type event_type
);

rflm_gsm_event_process_status rflm_gsm_process_jdet_read_ccs_event
(
 uint32 read_index,
 uint32 curr_trigger_time,
 uint32 rflm_intf_idx,
 rflm_gsm_fw_cb_type fw_callback,
 void* fw_jdet_cb_arg
 );
#ifdef FEATURE_TX_SHARING
rflm_err_t rflm_gsm_prog_tx_enter_ccs_script( rflm_handle_tx_t rflm_handle,
                                                 rflm_gsm_init_ccs_input_type *script_vars_in,
                                                 rflm_gsm_init_ccs_output_type *script_vars_out );
rflm_err_t rflm_gsm_prog_tx_enable_ccs_script( rflm_handle_tx_t rflm_handle,
                                                 rflm_gsm_init_ccs_input_type *script_vars_in,
                                                 rflm_gsm_init_ccs_output_type *script_vars_out );
rflm_err_t rflm_gsm_prog_tx_disable_ccs_script( rflm_handle_tx_t rflm_handle,
                                                 rflm_gsm_init_ccs_input_type *script_vars_in,
                                                 rflm_gsm_init_ccs_output_type *script_vars_out );
rflm_gsm_event_process_status rflm_gsm_process_ccs_event_v2( rflm_gsm_ccs_event_process_data *in);
#endif
/*============================================================================*/
#ifdef __cplusplus
} // extern "C"
#endif
/*============================================================================*/
#endif /* RFLM_GSM_CCS_H */

