/*!
  @file
  rflm_tds_ccs.h

  @brief
  RFLM TDSCDMA CCS header file
  
  @detail
  Define type defintion and function prototype.
  
  
*/

/*==============================================================================

  Copyright (c) 2013, 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/rftech/tdscdma/inc/rflm_tds_ccs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/03/15   whz     Add type of AutoPin
10/06/15   whz     Add support to AutoPin script for TA/JO
12/17/14   ms      Add IP2 script support, lin state override support
12/05/14   jyu     Updated API interface to allow FW query status of last RF tune
12/02/14   jyu     Fixed timing error for RX-TX transition when Rx and 
                   Tx reference time are too close
11/03/14   jyu     Added RFLM/FED logging support
10/15/14   jyu     Support to pass taskInfo for CCS task debug
09/25/14   jyu     Added support to pre-process issue_seq scripts (for timeline saving)
03/15/14   jyu     Added support for head-start
01/24/14   jyu     Updated the code to get TQs
01/23/14   jyu     Added support to execute Rx wakeup/burst scripts
01/06/13   jyu     Added initial support for IRAT
12/15/13   jps     Fixes for RFLM based TXAGC override
11/13/13   jyu     Expose rflm_tds_ustmr_add_offset()
11/11/13   jps     Hookup TDS CCS in txagc override mode
10/16/13   jps     Add TXAGC seq CCS commit API
10/15/13   jps     Add TX TQ info data structure for common txagc
10/03/13   jps     Add utility API to return TQ memory usage
09/18/13   jyu     Initial version 
==============================================================================*/

#ifndef RFLM_TDS_CCS_H
#define RFLM_TDS_CCS_H


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "rflm_ccs_rf_intf.h"
#include "rflm_ccs_rf_event_intf.h"
#include "rflm_api_tds.h"


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

#define RFLM_TDS_USE_CCS_TQ_PAIR0                 0
#define RFLM_TDS_USE_CCS_TQ_PAIR1                 1

#define RFLM_TDS_8BIT_32BIT_SHFT                  2
#define RFLM_TDS_DIVISION_BY_FOUR_CARRYOVER_VAL   3

#define RFLM_TDS_MAX_NUM_TQ                       4
#define RFLM_TDS_MAX_NUM_TASK_PER_TQ              32
/* This Macro transfer Memory usage in bytes into number of 32 bits aligned memory chunk */
#define RFLM_TDS_CCS_MEM_USAGE(memsize) ((memsize+RFLM_TDS_DIVISION_BY_FOUR_CARRYOVER_VAL)>>RFLM_TDS_8BIT_32BIT_SHFT)

/* Max CCS memory (in bytes) allocated to RFLM TDS*/
#define RFLM_TDS_RF_CCS_MEM_SIZE_IN_BYTES         (CCS_RF_TASKQ_PAIRS_DATA_BUF_NUM_WORDS * 4) //3K memory


typedef enum
{
  RFLM_TDS_DEFAULT,

  /* OFF-TO-RX */
  RFLM_TDS_OFF_TO_RX_RX_TUNING_PRX_ONLY,
  RFLM_TDS_OFF_TO_RX_RX_TUNING_PRX_DRX,
  RFLM_TDS_OFF_TO_RX_RX_ENABLE,
  RFLM_TDS_OFF_TO_RX_RX_ACTIVATE_PRX_ONLY,
  RFLM_TDS_OFF_TO_RX_RX_ACTIVATE_PRX_DRX,

  /* RX-TO-OFF */
  RFLM_TDS_RX_TO_OFF_RX_DEACTIVATE_PRX_ONLY,
  RFLM_TDS_RX_TO_OFF_RX_DEACTIVATE_PRX_DRX,

  /* OFF-TO-TX */
  RFLM_TDS_OFF_TO_TX_TX_TUNING,
  RFLM_TDS_OFF_TO_TX_TX_ENABLE,
  RFLM_TDS_OFF_TO_TX_TX_ACTIVATE,

  /* TX-TO-OFF */
  RFLM_TDS_TX_TO_OFF_TX_DEACTIVATE,
  RFLM_TDS_TX_TO_OFF_TXRX_DEACTIVATE,

  /* TX-TO-RX */
  RFLM_TDS_TX_TO_RX_RX_TUNING_PRX_ONLY,
  RFLM_TDS_TX_TO_RX_RX_TUNING_PRX_DRX,
  RFLM_TDS_TX_TO_RX_RX_ENABLE,
  RFLM_TDS_TX_TO_RX_TX_DEACTIVATE_RX_ACTIVATE_PRX_ONLY,
  RFLM_TDS_TX_TO_RX_TX_DEACTIVATE_RX_ACTIVATE_PRX_DRX,

  /* RX-TO-TX */
  RFLM_TDS_RX_TO_TX_TX_TUNING,
  RFLM_TDS_IFREQ_TO_TX_TX_TUNING,
  RFLM_TDS_RX_TO_TX_TX_ENABLE,
  RFLM_TDS_RX_TO_TX_RX_DEACTIVATE_PRX_ONLY,
  RFLM_TDS_RX_TO_TX_RX_DEACTIVATE_PRX_DRX,
  RFLM_TDS_RX_TO_TX_TX_ACTIVATE_PRX_ONLY,
  RFLM_TDS_RX_TO_TX_TX_ACTIVATE_PRX_DRX,
  
  /* TX-TO-TX */
  RFLM_TDS_TX_TO_TX_TX_ACTIVATE,

  /* Rx LNA*/
  RFLM_TDS_PRX_LNA,
  RFLM_TDS_DRX_LNA,
  RFLM_TDS_RX_LNA_LATCH,

  /* TxAGC */
  RFLM_TDS_TXAGC,

  /* Home Restore */
  RFLM_TDS_HOME_RESTORE,

  /* PDET */
  RFLM_TDS_PDET_TRIGGER,
  RFLM_TDS_PDET_READBACK,

  /* THERM */
  RFLM_TDS_THERM_TRIGGER,
  RFLM_TDS_THERM_READBACK,

  /* ASDIV/TUNER */
  RFLM_TDS_ASDIV,
  RFLM_TDS_TUNER_AOL,
  RFLM_TDS_TUNER_CL,

  /* FBRX LDO ON/OFF */
  RFLM_TDS_FBRX_LDO_ON,
  RFLM_TDS_FBRX_LDO_OFF,

  /* JDET */
  RFLM_TDS_JDET_READ,

  RFLM_TDS_ONE_TIME_BHO,
  RFLM_TDS_TX_WAKEUP,

  /* IFREQ */
  RFLM_TDS_IFREQ_RX_TUNING,
  RFLM_TDS_IFREQ_RX_ACTIVATE,

  /* IP2 cal code*/
  RFLM_TDS_IP2_CAL_CODE,

  /* AutoPin */
  RFLM_TDS_AUTOPIN_TRIGGER,

  RFLM_TDS_STANDALONE_MAX_SCRIPT 
} rflm_tds_rf_script_e;

typedef struct
{  
  uint32 enqueueTime;
  uint32 callbackTime;
  boolean inProcess; 
  uint8 associatedTaskStartIdx;
  rflm_tds_rf_script_e scriptType;
  uint32 scriptID;
} rflm_tds_ccs_exec_single_task_info_t;

/*! @brief Structure to send CCS RF task related information to techs as a pair */
typedef struct 
{
  /*! @brief CCS RF TQ 0 taskQ pointer */
  rflm_ccs_rf_tq_handle_t *tq0_handle;
  /*! @brief CCS RF TQ 1 taskQ pointer */
  rflm_ccs_rf_tq_handle_t *tq1_handle;
  /*! @brief CCS RF TQ data memory pointer. The 2 tasks Q above
  need to share the data memory below, caller can partition it appropriately. */
  uint32 *tq_data_ptr;
  /*! @brief Length (in bytes) of the data memory */
  uint16 tq_data_bytes;
  /*! @brief TQ index corresponding to tq0_handle */
  uint8 tq0_index;
  /*! @brief TQ index corresponding to tq1_handle */
  uint8 tq1_index;
} rflm_tds_ccs_rf_pair_info_s;


/*! @brief tdscdma ccs taskQ structure for common txagc */
typedef struct 
{
  /*! @brief CCS RF TQ  taskQ pointer */
  rflm_ccs_rf_tq_handle_t *tq_handle;
  /*! @brief CCS RF TQ data memory pointer */
  uint32 *tq_data_ptr;
  /*! @brief Length (in bytes) of the data memory */
  uint16 tq_data_bytes;
  /*! @brief TQ index corresponding to tq_handle */
  uint8 tq_index;
} rflm_tds_ccs_tq_t;

typedef struct
{
  uint32 *currPtr;
  uint32 *maxPtr;
  uint32 *minPtr;
} rflm_tds_rf_ccs_slot_dest_ptr_t;

typedef struct
{
  rflm_tds_rf_ccs_slot_dest_ptr_t  slotCcsDest;
} rflm_tds_rf_ccs_dest_ptr_set_t;

/* rflm tds ccs interface structure */
typedef struct 
{
  rflm_tds_ccs_rf_pair_info_s     ccsRfPairPtr;
  uint32                          tqPairIdx;
  boolean                         isUpPchSlot;
  rflm_tds_rf_ccs_dest_ptr_set_t  prev;
  rflm_tds_rf_ccs_dest_ptr_set_t  curr;
} rflm_tds_rf_ccs_intf_s;

typedef struct
{
  boolean scheduled;  
  uint32 tqMemStartAddr;
  uint32 tqMemEndAddr;
  uint8  tqIdx;
  uint8  tqTaskStartIdx;
  uint8  tqTaskEndIdx;

  uint8  numTask;
  int32  scptStartDelta;

  uint32 enqueueCompleteTime;
  uint32 triggerTimeSentToCcs;  // Trigger time sent to CCS (normally, walltime)
  int32  scriptStartDelta;      // relative time delta built into the script
} rflm_tds_single_script_info;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

void rflm_tds_rf_ccs_tq_lock(uint8 device);

void rflm_tds_rf_ccs_tq_free(uint32 tqPairIdx);

void rflm_tds_rf_ccs_task_check
( 
  rflm_tds_slot_num_e currSlot, 
  uint32 tqPairIdx
);


void rflm_tds_rf_ccs_tq_reset(uint32 tqPairIdx);

void rflm_tds_rf_event_callback(rflm_ccs_rf_tq_handle_t *tq, uint32 task_idx, ccs_rf_task_status_t status, void *arg);

void rflm_tds_rf_tune_event_callback(rflm_ccs_rf_tq_handle_t *tq, uint32 task_idx, ccs_rf_task_status_t status, void *arg);

void rflm_tds_pdet_trigger_callback(rflm_ccs_rf_tq_handle_t *tq, uint32 task_idx, ccs_rf_task_status_t status, void *arg);

void rflm_tds_therm_trigger_callback(rflm_ccs_rf_tq_handle_t *tq, uint32 task_idx, ccs_rf_task_status_t status, void *arg);

void rflm_tds_pdet_read_callback(rflm_ccs_rf_tq_handle_t *tq, uint32 task_idx, ccs_rf_task_status_t status, void *arg);

void rflm_tds_jdet_read_callback(rflm_ccs_rf_tq_handle_t *tq, uint32 task_idx, ccs_rf_task_status_t status, void *arg);

void rflm_tds_therm_read_callback(rflm_ccs_rf_tq_handle_t *tq, uint32 task_idx, ccs_rf_task_status_t status, void *arg);

void rflm_tds_rf_last_event_callback(rflm_ccs_rf_tq_handle_t *tq, uint32 task_idx, ccs_rf_task_status_t status, void *arg);

void rflm_tds_hs_tuning_event_callback(rflm_ccs_rf_tq_handle_t *tq, uint32 task_idx, ccs_rf_task_status_t status, void *arg);

void rflm_tds_irat_cleanup_event_callback(rflm_ccs_rf_tq_handle_t *tq, uint32 task_idx, ccs_rf_task_status_t status, void *arg);

void rflm_tds_exec_rf_internal_script_callback(rflm_ccs_rf_tq_handle_t *tq, uint32 task_idx, ccs_rf_task_status_t status, void *arg);

void rflm_tds_autopin_trigger_callback(rflm_ccs_rf_tq_handle_t *tq, uint32 task_idx, ccs_rf_task_status_t status, void *arg);

uint32* rflm_tds_ccs_issue_seq
(
  rflm_ccs_rf_event_header_task_type *evtPtr,
  uint32 *ccsDestPtr,
  rflm_ccs_rf_tq_handle_t *ccsTqPtr,
  rflm_ccs_rf_task_callback_t callback,
  uint32 actTime,
  void *callbackArg,
  rflm_ccs_rf_seq_t* rfScriptCcsSeqPtr,
  rflm_tds_ccs_exec_single_task_info_t* taskInfoPtr,
  rflm_tds_single_script_info * scriptInfoPtr
);

void rflm_tds_ccs_issue_seq_preprocessing_bho(void);

void rflm_tds_ccs_issue_seq_preprocessing_normal
(
  rflm_ccs_rf_event_header_task_type *evtPtr,
  rflm_ccs_rf_seq_t* ccsSeqPtr
);

uint32* rflm_tds_ccs_event_commit
(
  rflm_ccs_rf_event_header_task_type *evtScptPtr,
  uint32 *ccsDestPtr,
  rflm_ccs_rf_tq_handle_t *tqPtr,
  uint32 actTime,
  void *callbackFunctionPtr,
  rflm_tds_ccs_exec_single_task_info_t* taskInfoPtr,
  rflm_tds_single_script_info * scriptInfoPtr,
  void* arg
);

uint32* rflm_tds_ccs_commit_txagc_seq
(
  rflm_ccs_rf_seq_t *issue_seq_ptr,
  rflm_ccs_rf_task_callback_t callbackFunctionPtr,
  uint32 *ccsDestPtr,
  uint32 ustmr_action_time,
  rflm_tds_ccs_exec_single_task_info_t* taskInfoPtr,
  rflm_tds_single_script_info * scriptInfoPtr
);

uint32 rflm_tds_ccs_get_txagc_seq_action_time(void);

uint32 rflm_tds_ccs_util_get_curr_tq_mem_usage(void);

rflm_tds_ccs_tq_t* rflm_tds_get_ccs_tx_tq(void);
uint32 rflm_tds_ustmr_add_offset(uint32 time0, int32 offset);

uint32 rflm_tds_get_ccs_mem_addr(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RFLM_TDS_CCS_H */
