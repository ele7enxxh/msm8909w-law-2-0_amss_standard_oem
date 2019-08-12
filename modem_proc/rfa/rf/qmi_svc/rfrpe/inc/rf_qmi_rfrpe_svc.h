#ifndef RF_QMI_RFRPE_SVC_H
#define RF_QMI_RFRPE_SVC_H
/*!
   @file
   rf_qmi_rfrpe_svc.h

   @brief
   This file contains the function prototypes associated to RF Tuner QMI
   service for external modules to use
*/

/*===========================================================================

Copyright (c) 2012 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/qmi_svc/rfrpe/inc/rf_qmi_rfrpe_svc.h#1 $

when         who   what, where, why
--------   ---   -------------------------------------------------------------------
03/13/13   aro    Added RFRPE deinit to perform memory cleanup
02/07/13   aca    RFRPE Task Registration
02/07/13   aca    MSG param update
09/06/12   gh     Initial release

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfcom.h"

/*===========================================================================
                           GLOBAL VARS
===========================================================================*/

/*===========================================================================
                           DEFINITIONS
===========================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Max number of concurrent scenarios to program or are currently active
  Definition from RFRPE IDL
*/
#define RF_QMI_RFRPE_SVC_MAX_CONCURRENT_SCENARIOS 32

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Max length of the OEM string
  Definition from RFRPE IDL
*/
#define RF_QMI_RFRPE_SVC_MAX_OEM_STR_LENGTH       64

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Max length of the OEM string
  Definition from RFRPE IDL
*/
#define RF_QMI_RFRPE_RSP_NOT_NEEDED               0xFADE

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Error code enum to that is used by the tuner manager to notify QMI RFRPE
  server
*/
typedef enum
{
  RF_QMI_RFRPE_RESP_ERR_CODE_SUCCESS,
  RF_QMI_RFRPE_RESP_ERR_CODE_INTERNAL_ERROR,
  RF_QMI_RFRPE_RESP_ERR_CODE_BAD_PARAMETERS,
  RF_QMI_RFRPE_RESP_ERR_CODE_INVALID_SCENARIO_NUMBER,
  RF_QMI_RFRPE_RESP_ERR_CODE_CURRENT_SCENARIO_NOT_AVAILABLE,
  RF_QMI_RFRPE_RESP_ERR_CODE_REVISION_NOT_AVAILABLE,
  RF_QMI_RFRPE_RESP_ERR_CODE_OEM_STRING_NOT_AVAILABLE,
  RF_QMI_RFRPE_RESP_ERR_CODE_MAX
} rf_qmi_rfrpe_resp_err_code_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Struct def for interpreting the payload in the rf_cmd_type buffer associated
  to set_scenario request operations
*/
typedef struct
{
  void*  qmi_req_handle;
  uint8  scenario_len;
  uint8  scenarios[RF_QMI_RFRPE_SVC_MAX_CONCURRENT_SCENARIOS];
} rf_qmi_rfrpe_svc_set_scenario_req_s;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Struct def for interpreting the payload in the rf_cmd_type buffer associated
  to get_scenario operations
*/
typedef struct
{
  void*  qmi_req_handle;
  uint8 *scenario_val;	
} rf_qmi_rfrpe_svc_get_scenario_req_s;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Struct def for interpreting the payload in the rf_cmd_type buffer associated
  to get_revision_provision operations
*/
typedef struct
{
  void*  qmi_req_handle;
  uint8 *revision;
} rf_qmi_rfrpe_svc_get_revision_req_s;

/*===========================================================================
                           FUNCTION PROTOTYPES
===========================================================================*/

/*----------------------------------------------------------------------------*/
uint8   rf_qmi_rfrpe_svc_init(void);

/*----------------------------------------------------------------------------*/
uint8   rf_qmi_rfrpe_svc_deinit(void);

/*----------------------------------------------------------------------------*/
void rf_qmi_rfrpe_send_rf_task_set_scenario_cmd
(
 void*   req_handle,
 uint32  scenario_len,
 uint32* scenarios,
 uint32  max_scenario_len
);

/*----------------------------------------------------------------------------*/
void rf_qmi_rfrpe_send_rf_task_get_scenario_cmd(void* req_handle);

/*----------------------------------------------------------------------------*/
void rf_qmi_rfrpe_send_rf_task_get_revision_cmd(void* req_handle);

/*----------------------------------------------------------------------------*/
boolean rf_qmi_rfrpe_set_scenarios_resp
(
 void *qmi_handle,
 rf_qmi_rfrpe_resp_err_code_type err_code
);

/*----------------------------------------------------------------------------*/
boolean rf_qmi_rfrpe_get_scenarios_resp
(
 void *qmi_handle,
 uint32 num_of_scenarios,
 uint32 *scenario_vals,
 rf_qmi_rfrpe_resp_err_code_type err_code
);

/*----------------------------------------------------------------------------*/
boolean rf_qmi_rfrpe_get_provisioned_table_info_resp
(
 void *qmi_handle,
 uint32 revision_num,
 uint32 oem_name_len,
 uint16 *oem_name,
 rf_qmi_rfrpe_resp_err_code_type err_code
);

/*----------------------------------------------------------------------------*/
void rf_qmi_rfrpe_svc_disable_scenario_update(boolean disable_scenario_update);

#endif /* RF_QMI_RFRPE_SVC_H */
