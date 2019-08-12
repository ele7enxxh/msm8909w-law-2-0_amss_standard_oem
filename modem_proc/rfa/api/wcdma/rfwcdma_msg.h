/*!
  @file
  rfwcdma_msg.h

  @brief
  Message router UMIDs header for RF WCDMA subsystem. This file defines
  interfaces in the RF task.

  @note
  This file must be named with a _msg.h suffix for the build environment
  to correctly process the messages defined within.

*/

/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header  : $
$DateTime: 2016/12/13 07:59:52 $
$Author  : $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/08/13   jj      add support for tx SAR_limit
04/30/12   ac      compiler warning fix
02/06/12   ka      Initial version

===========================================================================*/

#ifndef RF_WCDMA_MSG_H
#define RF_WCDMA_MSG_H

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "appmgr_msg.h"
#include "msgr.h"
#include "msgr_types.h"
#include "rfa_msgr.h"
#include "rfcom.h"
#include "rfmeas_wcdma.h"
#include "lm_types.h"

/*===========================================================================
                  EXTERNAL DEFINITIONS AND TYPES
===========================================================================*/


/*-----------------------------------------------------------------------*/
/*     MSGR SUPERVISORY ID'S                                             */
/*-----------------------------------------------------------------------*/

/*! @brief ID for RF to enter WCDMA mode of operation CMD and RSP
 */
#define RFA_RF_WCDMA_MSG_ID_ENTER_MODE			0xC1
#define RFA_RF_WCDMA_MSG_ID_EXIT_MODE		  	0xC2
#define RFA_RF_WCDMA_MSG_ID_CMD_DISPATCH                0xC3
#define RFA_RF_WCDMA_MSG_ID_TUNE_TO_MULTI_CHAN          0xC4
#define RFA_RF_WCDMA_MSG_ID_ENABLE_TX                   0xC5
#define RFA_RF_WCDMA_MSG_ID_DISABLE_TX                  0xC6
#define RFA_RF_WCDMA_MSG_ID_DO_TEMP_COMP                0xC7
#define RFA_RF_WCDMA_MSG_ID_SLEEP                       0xC8
#define RFA_RF_WCDMA_MSG_ID_WAKEUP                      0xC9
#define RFA_RF_WCDMA_MSG_ID_ENABLE_RX                   0xD1
#define RFA_RF_WCDMA_MSG_ID_DISABLE_RX                  0xD2
#define RFA_RF_WCDMA_MSG_ID_POWER_CLOCK_ON              0xD3
#define RFA_RF_WCDMA_MSG_ID_POWER_CLOCK_OFF             0xD4
#define RFA_RF_WCDMA_MSG_ID_INIT                        0xD5
#define RFA_RF_WCDMA_MSG_ID_INIT_TX                     0xD6
#define RFA_RF_WCDMA_MSG_ID_SET_LOWER_POWER_MODE        0xD7
#define RFA_RF_WCDMA_MSG_ID_SET_SAR_LIMIT               0xD8
#define RFA_RF_WCDMA_MSG_MAX_ID                         0xFFFF

/****************************************************************************
                            RF Task WCDMA messages
****************************************************************************/
/*! @brief WCDMA subsystem message IDs the RF task receives. 
	 This enum is never called by name. It just defines symbols.
*/
enum
{
   MSGR_DEFINE_UMID(RFA, RF_WCDMA, CNF, ENTER_MODE, RFA_RF_WCDMA_MSG_ID_ENTER_MODE, rfa_rf_wcdma_enter_mode_cnf_s),
   MSGR_DEFINE_UMID(RFA, RF_WCDMA, CNF, EXIT_MODE, RFA_RF_WCDMA_MSG_ID_EXIT_MODE, rfa_rf_wcdma_exit_mode_cnf_s),
   MSGR_DEFINE_UMID(RFA, RF_WCDMA, CNF, CMD_DISPATCH, RFA_RF_WCDMA_MSG_ID_CMD_DISPATCH, rfa_rf_wcdma_cmd_dispatch_cnf_s),
   MSGR_DEFINE_UMID(RFA, RF_WCDMA, CNF, TUNE_TO_MULTI_CHAN, RFA_RF_WCDMA_MSG_ID_TUNE_TO_MULTI_CHAN, rfa_rf_wcdma_tune_to_multi_chan_cnf_s),
   MSGR_DEFINE_UMID(RFA, RF_WCDMA, CNF, ENABLE_TX , RFA_RF_WCDMA_MSG_ID_ENABLE_TX, rfa_rf_wcdma_enable_tx_cnf_s),
   MSGR_DEFINE_UMID(RFA, RF_WCDMA, CNF, DISABLE_TX , RFA_RF_WCDMA_MSG_ID_DISABLE_TX, rfa_rf_wcdma_disable_tx_cnf_s),
   MSGR_DEFINE_UMID(RFA, RF_WCDMA, CNF, SLEEP, RFA_RF_WCDMA_MSG_ID_SLEEP , rfa_rf_wcdma_sleep_cnf_s),
   MSGR_DEFINE_UMID(RFA, RF_WCDMA, CNF, WAKEUP, RFA_RF_WCDMA_MSG_ID_WAKEUP, rfa_rf_wcdma_do_wakeup_cnf_s),
   MSGR_DEFINE_UMID(RFA, RF_WCDMA, CNF, DO_TEMP_COMP, RFA_RF_WCDMA_MSG_ID_DO_TEMP_COMP , rfa_rf_wcdma_do_temp_comp_cnf_s),
   MSGR_DEFINE_UMID(RFA, RF_WCDMA, CNF, ENABLE_RX, RFA_RF_WCDMA_MSG_ID_ENABLE_RX , rfa_rf_wcdma_enable_rx_cnf_s),
   MSGR_DEFINE_UMID(RFA, RF_WCDMA, CNF, DISABLE_RX, RFA_RF_WCDMA_MSG_ID_DISABLE_RX , rfa_rf_wcdma_disable_rx_cnf_s),
   MSGR_DEFINE_UMID(RFA, RF_WCDMA, CNF, POWER_CLOCK_ON, RFA_RF_WCDMA_MSG_ID_POWER_CLOCK_ON, rfa_rf_wcdma_power_clock_on_cnf_s),
   MSGR_DEFINE_UMID(RFA, RF_WCDMA, CNF, POWER_CLOCK_OFF, RFA_RF_WCDMA_MSG_ID_POWER_CLOCK_OFF, rfa_rf_wcdma_power_clock_off_cnf_s),
   MSGR_DEFINE_UMID(RFA, RF_WCDMA, CNF, INIT, RFA_RF_WCDMA_MSG_ID_INIT, rfa_rf_wcdma_init_cnf_s),
   MSGR_DEFINE_UMID(RFA, RF_WCDMA, CNF, INIT_TX, RFA_RF_WCDMA_MSG_ID_INIT_TX, rfa_rf_wcdma_init_tx_cnf_s),
   MSGR_DEFINE_UMID(RFA, RF_WCDMA, CNF, SET_LOWER_POWER_MODE, RFA_RF_WCDMA_MSG_ID_SET_LOWER_POWER_MODE, rfa_rf_wcdma_set_lower_power_mode_cnf_s),
};

/*! @brief WCDMA subsystem message IDs the RF task receives. 
	 This enum is never called by name. It just defines symbols.
*/
enum
{	
  MSGR_DEFINE_UMID(RFA, RF_WCDMA, REQ, ENTER_MODE, RFA_RF_WCDMA_MSG_ID_ENTER_MODE, rfa_rf_wcdma_enter_mode_req_s),
  MSGR_DEFINE_UMID(RFA, RF_WCDMA, REQ, EXIT_MODE, RFA_RF_WCDMA_MSG_ID_EXIT_MODE, rfa_rf_wcdma_exit_mode_req_s),
  MSGR_DEFINE_UMID(RFA, RF_WCDMA, REQ, CMD_DISPATCH, RFA_RF_WCDMA_MSG_ID_CMD_DISPATCH, rfa_rf_wcdma_cmd_dispatch_req_s),
  MSGR_DEFINE_UMID(RFA, RF_WCDMA, REQ, TUNE_TO_MULTI_CHAN, RFA_RF_WCDMA_MSG_ID_TUNE_TO_MULTI_CHAN, rfa_rf_wcdma_tune_to_multi_chan_req_s),
  MSGR_DEFINE_UMID(RFA, RF_WCDMA, REQ, ENABLE_TX , RFA_RF_WCDMA_MSG_ID_ENABLE_TX, rfa_rf_wcdma_enable_tx_req_s),
  MSGR_DEFINE_UMID(RFA, RF_WCDMA, REQ, DISABLE_TX , RFA_RF_WCDMA_MSG_ID_DISABLE_TX, rfa_rf_wcdma_disable_tx_req_s),
  MSGR_DEFINE_UMID(RFA, RF_WCDMA, REQ, SLEEP, RFA_RF_WCDMA_MSG_ID_SLEEP , rfa_rf_wcdma_sleep_req_s),
  MSGR_DEFINE_UMID(RFA, RF_WCDMA, REQ, WAKEUP, RFA_RF_WCDMA_MSG_ID_WAKEUP, rfa_rf_wcdma_do_wakeup_req_s),
  MSGR_DEFINE_UMID(RFA, RF_WCDMA, REQ, ENABLE_RX, RFA_RF_WCDMA_MSG_ID_ENABLE_RX , rfa_rf_wcdma_enable_rx_req_s),
  MSGR_DEFINE_UMID(RFA, RF_WCDMA, REQ, DISABLE_RX, RFA_RF_WCDMA_MSG_ID_DISABLE_RX , rfa_rf_wcdma_disable_rx_req_s),
  MSGR_DEFINE_UMID(RFA, RF_WCDMA, REQ, POWER_CLOCK_ON, RFA_RF_WCDMA_MSG_ID_POWER_CLOCK_ON, rfa_rf_wcdma_power_clock_on_req_s),
  MSGR_DEFINE_UMID(RFA, RF_WCDMA, REQ, POWER_CLOCK_OFF, RFA_RF_WCDMA_MSG_ID_POWER_CLOCK_OFF, rfa_rf_wcdma_power_clock_off_req_s),
  MSGR_DEFINE_UMID(RFA, RF_WCDMA, REQ, INIT, RFA_RF_WCDMA_MSG_ID_INIT, rfa_rf_wcdma_init_req_s),
  MSGR_DEFINE_UMID(RFA, RF_WCDMA, REQ, INIT_TX, RFA_RF_WCDMA_MSG_ID_INIT_TX, rfa_rf_wcdma_init_tx_req_s),
  MSGR_DEFINE_UMID(RFA, RF_WCDMA, REQ, SET_LOWER_POWER_MODE, RFA_RF_WCDMA_MSG_ID_SET_LOWER_POWER_MODE, rfa_rf_wcdma_set_lower_power_mode_req_s),
  MSGR_DEFINE_UMID(RFA, RF_WCDMA, IND, SET_SAR_LIMIT, RFA_RF_WCDMA_MSG_ID_SET_SAR_LIMIT, rfa_rf_wcdma_set_sar_limit_ind_s),
};



/*===========================================================================
                   EXTERNAL DEFINITIONS AND TYPES
===========================================================================*/

/*! @todo: This might need to be moved to a common file so that L1 can see it 
           and it might be shared with other targets.*/
typedef enum
{
  RFA_RF_WCDMA_SUCCESS=1,
  RFA_RF_WCDMA_STATE_MACHINE_ERROR=2,
  RFA_RF_WCDMA_EXECUTION_ERROR=3,
  RFA_RF_WCDMA_CMD_OUTCOME_MAX=4,
} rfa_rf_wcdma_cmd_outcome_t;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfcom_device_enum_type device;
  uint32                 rxlm_buf_idx;
} rfa_rf_wcdma_enter_mode_req_s;

typedef struct
{
  msgr_hdr_struct_type       hdr;
  /*echo back the input*/
  rfcom_device_enum_type     device;
  rfa_rf_wcdma_cmd_outcome_t req_result;
  uint32                     rxlm_buf_idx;
} rfa_rf_wcdma_enter_mode_cnf_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfcom_device_enum_type device;
} rfa_rf_wcdma_exit_mode_req_s;

typedef struct
{
  msgr_hdr_struct_type       hdr;
  /*echo back the input*/
  rfcom_device_enum_type     device;
  rfa_rf_wcdma_cmd_outcome_t req_result;
} rfa_rf_wcdma_exit_mode_cnf_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfcom_device_enum_type device;
  int32 cmd;
  void *data;
} rfa_rf_wcdma_cmd_dispatch_req_s;

typedef struct
{
  msgr_hdr_struct_type       hdr;
  /*echo back the input*/
  rfcom_device_enum_type     device;
  int32 cmd;
  void *data;
  rfa_rf_wcdma_cmd_outcome_t req_result;
} rfa_rf_wcdma_cmd_dispatch_cnf_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfcom_device_enum_type device;
  uint16 channel[RFCOM_MAX_CARRIERS];
  rfcom_multi_carrier_hspa_id_type num_chan;
  rfm_synth_tune_enum_type synths;
  const void *user_data_ptr;
  rfm_cb_handler_type cb_handler;
  uint32 rxlm_buf_idx;
} rfa_rf_wcdma_tune_to_multi_chan_req_s;

typedef struct
{
  msgr_hdr_struct_type       hdr;
  /*echo back the input*/
  rfcom_device_enum_type     device;
  uint16 channel[RFCOM_MAX_CARRIERS];
  rfcom_multi_carrier_hspa_id_type num_chan;
  rfm_synth_tune_enum_type synths;
  const void *user_data_ptr;
  rfm_cb_handler_type cb_handler;
  uint32 rxlm_buf_idx;
  rfa_rf_wcdma_cmd_outcome_t req_result;
} rfa_rf_wcdma_tune_to_multi_chan_cnf_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfcom_device_enum_type device;
  const void *user_data_ptr;
  rfm_cb_handler_type cb_handler;
  uint32 txlm_buf_idx;
} rfa_rf_wcdma_enable_tx_req_s;

typedef struct
{
  msgr_hdr_struct_type       hdr;
  /*echo back the input*/
  rfcom_device_enum_type     device;
  const void *user_data_ptr;
  rfm_cb_handler_type cb_handler;
  uint32 txlm_buf_idx;
  rfa_rf_wcdma_cmd_outcome_t req_result;
} rfa_rf_wcdma_enable_tx_cnf_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfcom_device_enum_type device;
  uint32 txlm_buf_idx;
} rfa_rf_wcdma_disable_tx_req_s;

typedef struct
{
  msgr_hdr_struct_type       hdr;
  /*echo back the input*/
  rfcom_device_enum_type     device;
  uint32 txlm_buf_idx;
  rfa_rf_wcdma_cmd_outcome_t req_result;
} rfa_rf_wcdma_disable_tx_cnf_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfcom_device_enum_type device;
  const void *user_data_ptr;
  rfm_cb_handler_type cb_handler;
  uint32 rxlm_buf_idx;
} rfa_rf_wcdma_sleep_req_s;

typedef struct
{
  msgr_hdr_struct_type       hdr;
  /*echo back the input*/
  rfcom_device_enum_type     device;
  const void *user_data_ptr;
  rfm_cb_handler_type cb_handler;
  uint32 rxlm_buf_idx;
  rfa_rf_wcdma_cmd_outcome_t req_result;
} rfa_rf_wcdma_sleep_cnf_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfcom_device_enum_type device;
  const void *user_data_ptr;
  rfm_cb_handler_type cb_handler;
  uint32 rxlm_buf_idx;
} rfa_rf_wcdma_wakeup_req_s;

typedef struct
{
  msgr_hdr_struct_type       hdr;
  /*echo back the input*/
  rfcom_device_enum_type     device;
  const void *user_data_ptr;
  rfm_cb_handler_type cb_handler;
  uint32 rxlm_buf_idx;
  rfa_rf_wcdma_cmd_outcome_t req_result;
} rfa_rf_wcdma_wakeup_cnf_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfcom_device_enum_type device;
  const void *user_data_ptr;
  rfm_cb_handler_type cb_handler;
} rfa_rf_wcdma_enable_rx_req_s;

typedef struct
{
  msgr_hdr_struct_type       hdr;
  /*echo back the input*/
  rfcom_device_enum_type     device;
  const void *user_data_ptr;
  rfm_cb_handler_type cb_handler;
  rfa_rf_wcdma_cmd_outcome_t req_result;
} rfa_rf_wcdma_enable_rx_cnf_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfcom_device_enum_type device;
} rfa_rf_wcdma_disable_rx_req_s;

typedef struct
{
  msgr_hdr_struct_type       hdr;
  /*echo back the input*/
  rfcom_device_enum_type     device;
  rfa_rf_wcdma_cmd_outcome_t req_result;
} rfa_rf_wcdma_disable_rx_cnf_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfcom_device_enum_type device;
} rfa_rf_wcdma_power_clock_on_req_s;

typedef struct
{
  msgr_hdr_struct_type       hdr;
  /*echo back the input*/
  rfcom_device_enum_type     device;
  rfa_rf_wcdma_cmd_outcome_t req_result;
} rfa_rf_wcdma_power_clock_on_cnf_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfcom_device_enum_type device;
} rfa_rf_wcdma_power_clock_off_req_s;

typedef struct
{
  msgr_hdr_struct_type       hdr;
  /*echo back the input*/
  rfcom_device_enum_type     device;
  rfa_rf_wcdma_cmd_outcome_t req_result;
} rfa_rf_wcdma_power_clock_off_cnf_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfcom_device_enum_type device;
  rex_tcb_type *caller_tcb_ptr;
  rex_sigs_type task_nv_wait_sig;
  void (*task_wait_func_ptr)( rex_sigs_type );
  boolean common_init_successful;
} rfa_rf_wcdma_init_req_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfcom_device_enum_type device;
  rex_tcb_type *caller_tcb_ptr;
  rex_sigs_type task_nv_wait_sig;
  void (*task_wait_func_ptr)( rex_sigs_type );
  boolean common_init_successful;
  rfa_rf_wcdma_cmd_outcome_t req_result;
} rfa_rf_wcdma_init_cnf_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
} rfa_rf_wcdma_init_tx_req_s;

typedef struct
{
  msgr_hdr_struct_type       hdr;
  /*echo back the input*/
  rfa_rf_wcdma_cmd_outcome_t req_result;
} rfa_rf_wcdma_init_tx_cnf_s;

typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfcom_lpm_config_enum_type lpm_config;
  uint32 c0_rxlm_buf_idx;
  uint32 c1_rxlm_buf_idx;
} rfa_rf_wcdma_set_lower_power_mode_req_s;

typedef struct
{
  msgr_hdr_struct_type       hdr;
  rfcom_lpm_config_enum_type lpm_config;
  uint32 c0_rxlm_buf_idx;
  uint32 c1_rxlm_buf_idx;
  rfa_rf_wcdma_cmd_outcome_t req_result;
} rfa_rf_wcdma_set_lower_power_mode_cnf_s;

/*=============================================================================
                   RFA_RF_WCDMA_MSG_ID_SET_SAR_LIMIT
=============================================================================*/
/*  RFA_RF_WCDMA_SET_SAR_LIMIT_IND  */
/*! @struct rfa_rf_wcdma_set_sar_limit_ind_s*/
/*! @brief This structure is the payload for confirmation that RF has received
 *  a sar limit update
 */
typedef struct
{
  /*! MSGR details related to each UMID i.e priority, sending module etc. */
  msgr_hdr_struct_type		 hdr;
  /*! SAR limit params to be passed from MCS to RF*/
  rfcom_device_enum_type	 device;
  /*! Max Tx limit in dB10 */
  int16                    sar_tx_limit;
}rfa_rf_wcdma_set_sar_limit_ind_s;


#endif /* RF_WCDMA_MSG_H */



