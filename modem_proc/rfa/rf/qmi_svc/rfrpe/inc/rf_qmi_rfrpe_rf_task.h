#ifndef RF_QMI_RFRPE_RF_TASK_H
#define RF_QMI_RFRPE_RF_TASK_H

/*! 
  @file
  rf_qmi_rfrpe_rf_task.h
 
  @brief
  This file includes functions pertaining to Tasks used for RF Tuner Operation
*/
 
/*==============================================================================

  Copyright (c) 2012-14 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/qmi_svc/rfrpe/inc/rf_qmi_rfrpe_rf_task.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/20/14   aca     Export set scenario API
11/07/13   yb/ndb  Added the support for dynamic tune code size
02/11/13   aca     QMI response message
02/07/13   aca     RFRPE Task Registration
02/07/13   aca     DIME QMI call back fn registration
02/04/13   aca     DIME QMI server
09/10/12   gh      Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "msgr_types.h"
#include "rfm_device_types.h"
#include "rfcommon_time_profile.h"
#include "rf_cmd_interface.h"
#include "rf_qmi_rfrpe_svc.h"


/*==========================================================================*/
/* Enums and vars */
/*==========================================================================*/	

#define RF_QMI_MAX_FUNCTIONS 16

#define RF_QMI_MAX_SIZE_OF_EACH_TUNE_CODE 8

/*----------------------------------------------------------------------------*/
/* Command enums that functions can register for with the RFRPE server */
typedef enum
{
  RF_QMI_SET_SCENARIO_CMD,
  RF_QMI_GET_SCENARIO_CMD,
  RF_QMI_GET_PROVISION_TBL_CMD,
  RF_QMI_MAX_CMD
} rf_qmi_cmd_type_enum;

/*----------------------------------------------------------------------------*/
/* Expected function protoypes for the various supported commands */
typedef boolean (*rf_qmi_set_scenarios_fn)
                                        (
                                          void *class_obj_ptr,
                                          uint8 num_of_scenarios,
                                          uint8 *scenario_vals
                                        );
typedef boolean (*rf_qmi_get_scenarios_fn)
                                        (
                                          void *class_obj_ptr,
                                          uint8 *scenario_in_use,
                                          uint8 *nv_tune_code,
                                          uint8 *tune_code_size
                                        );
typedef boolean (*rf_qmi_get_provision_tbl_fn)
                                             (
                                               void *class_obj_ptr,
                                               uint32 *revision,
                                               uint32 *oem_string_len,
                                               uint16 *oem_string
                                             );
/*----------------------------------------------------------------------------*/
/* Union of function pointers that call back functions support*/
typedef union
{
  rf_qmi_set_scenarios_fn set_scenario_fp;
  rf_qmi_get_scenarios_fn get_scenario_fp;
  rf_qmi_get_provision_tbl_fn get_provision_table_fp;
} rf_qmi_cmd_handler_func_type;

/*----------------------------------------------------------------------------*/

typedef struct
{
	void* class_obj_ptr; /* Null if the call back fn is not a C++ member fn */
	rf_qmi_cmd_handler_func_type fn_ptr; /* union of the possible types of call back functions */
	boolean execution_status;

}rf_qmi_rfrpe_cb_fn_type;


typedef struct
{
  uint8 cmd; /* Command to be registered for */
  uint8 num_call_bk_fn; /* Number of call back functions registered */
  rf_qmi_rfrpe_cb_fn_type fn_list[RF_QMI_MAX_FUNCTIONS];  /*Array of call back functions */		

}rf_qmi_rfrpe_fn_list_type;

/* Statemachine to track RFRPE server */
typedef struct
{
  /* List of registered functions per command */
  rf_qmi_rfrpe_fn_list_type call_bk_fn[RF_QMI_MAX_CMD];

}rf_qmi_rfrpe_state_type;

/*==========================================================================*/
/* Function protoypes */
/*==========================================================================*/	

boolean
rf_qmi_rfrpe_rf_task_dispatch_init
(
  msgr_client_t *msgr_client, 
  msgr_id_t mailbox 
);

/*----------------------------------------------------------------------------*/
boolean
rf_qmi_rfrpe_rf_task_dispatch_deinit
(
  msgr_client_t *msgr_client
);
/*----------------------------------------------------------------------------*/

/* Function to be used by interested parties to register call back functions for specific commands*/
boolean
rf_qmi_register_function
(
  rf_qmi_cmd_type_enum cmd,
  rf_qmi_rfrpe_cb_fn_type fn

);
/*----------------------------------------------------------------------------*/
void 
rf_qmi_rfrpe_process_set_scenario_req
(
  rf_qmi_rfrpe_svc_set_scenario_req_s* set_scenario_payload
);

#endif /* RF_QMI_RFRPE_RF_TASK_H */


