/*!
  @file
  rflm_tds_mc.h

  @brief
  RFLM TDSCDMA main control header file
  
  @detail
  Define type defintion and function prototype.
  Interface with DM, RFLM-TDS other modules (e.g., RxAGC and TxAGC) and RFSW
  
*/

/*==============================================================================

  Copyright (c) 2013 - 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/rftech/tdscdma/inc/rflm_tds_mc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/11/15   whz     Add schedule period for AutoPin
10/06/15   whz     Add support to AutoPin script for TA
02/18/15   whz     Add wtrTriggerDelayTime for SAWless
01/13/15   jyu     Fix LNA gain stuck issue in N-FREQ
12/30/14   jak     Added RFNV_TDSCDMA_<band>_MPR_VAL_V2_I support
12/26/14   jz      Enable jdetInfo when it's sawless card
12/17/14   ms      Add IP2 script support, lin state override support
12/17/14   whz     Use txActualOutputPwrdB10 for HDET calculations
12/05/14   jyu     Updated API interface to allow FW query status of last RF tune
11/10/14   jyu     Added support to adjust "group" timing of TxAGC script
11/03/14   jyu     Added RFLM/FED logging support
11/03/14   jyu     Added support to override rxdEn flag in slot_update() for CMAPI type4
                   (RFSW/FED on RxD ON and FW on PRX ONLY mode)
10/15/14   jyu     Support to pass taskInfo for CCS task debug
10/15/14   jyu     Fixed the bug for partial Tx slot support
09/25/14   jyu     Added support to pre-process issue_seq scripts (for timeline saving)
09/24/14   ms      Schedule JDET read only if RFcard supports sawless feature
09/16/14   jyu     Support to pass WB ADC to FW
08/21/14   kb      PA state hack for RUMI
08/19/14   jyu     Need to release and request TQs when logic device change on wakeup
08/15/14   jyu     Avoid CCS TQ mem reset for BHO and DL->IFREQ transition
08/11/14   jyu     Added support to separate T2G and T2L script pointers
08/05/14   jhe     Separate mtpl for coex 
07/16/14   kb      Added parameter to save TxAGC during PDET trigger
06/15/14   jyu     Added statistics for CCS TQ memory usage
04/29/14   jyu     Added support to abort AOL in mc_exit_mode()
04/22/14   jhe     Add fbrx cal api
04/17/14   jyu     Added support to execute Tx wakeup script
04/11/14   jyu     Added support for head_start
04/10/14   jyu     Added support for Tuner Open Loop (CL)
04/09/14   jyu     Updated ASDiv FED implementation
04/09/14   jyu     Added support for Tuner Advanced Open Loop (AOL)
04/03/14   jyu     Added support for port A/B feature
03/28/14   jyu     Added support for double LNA latch
03/29/14   jyu     Added initial support for HDET/TDET and WTR/Tuner therm read
03/24/14   jyu     Added support for ASDiv
03/23/14   jyu     Added support for IFREQ-to-Tx script
02/27/14   jyu     Added support to detect if FW calling FED late
02/23/14   jyu     Added support for better handling RF exit_mode
                         in coordination of FW resource release
02/08/14   jyu     Added support for LNA latch script
02/05/14   jyu     Updated to release TQ pair
02/03/14   jyu     RxAGC timeline optimization
02/02/14   jyu     Added support for X2T IRAT
01/28/14   jps     Use one API to setup TX data
01/21/14   jps     Fix fbrx include file errors
01/16/14   jps     Added interface for FBRX 
01/16/14   jyu     Added support to execute rx_wakeup, rx_burst, rx_lna script
01/06/13   jyu     Added initial support for IRAT
12/24/13   nsh     Add support for TDS IRAT
12/11/13   jyu     Added support for LNA scripts
11/25/13   jps     Use only RF_TDSCDMA_INTEGRATE_COMMON_TXAGC to enable TXAGC 
11/25/13   jps     Remove unused debug code
11/13/13   nsh     FED RxAGC Support
10/31/13   jps     Update PA parameters to common TXAGC
10/30/13   jps     Updates and fixes for common TXAGC hookup
10/21/13   jps     Hook up drif configuration
10/22/13   jyu     Move definitions to allow sharing
10/18/13   jps     Enable txagc hook up
10/16/13   jps     Add TXAGC seq CCS commit API
10/16/13   jps     Add API to identify and extract txagc_params index based on 
                         current active TX slot
10/14/13   jps     Initial hook up with common TXAGC
09/30/13   jps     Fixed gtest build compilation bug 
09/19/13   jyu     Integrated CCS with TDS 
08/01/13   jyu     Added initial version of user data definition 
07/31/13   jyu     Initial version 
==============================================================================*/

#ifndef RFLM_TDS_MC_H
#define RFLM_TDS_MC_H


#define RF_TDSCDMA_INTEGRATE_COMMON_TXAGC

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "rflm_api_tds.h"
#include "rflm_tds_ccs.h"
#include "rflm_txagc.h"
#include "rflm_txagc_api.h"
#include "rflm_tds_dm_intf.h"
#include "rflm_api_fbrx_fw.h"

#include "rflm_dm_api.h"
#include "rflm_autopin.h"



/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

#define RFLM_TDS_MAX_NUM_LNA_GAIN_UPDATE_PER_SLOT        2
#define RFLM_TDS_MAX_NUM_HOME_TARGET_BUFFER              2

#define RFLM_TDS_MAX_NUM_EVENT_PER_SCHEDULE_POINT        10


#define RFLM_TDS_ASSERT_ARGS( expr, ... ) \
  if ( EXPECT( !(expr), 0 ) ) \
  { \
    RFLM_DIAG_ERR_FATAL("Assertion (" #expr ") failed (see args)", ## __VA_ARGS__); \
  }

#define RFLM_TDS_CONVERT_USEC_TO_CX1(x) (x*864/675)


#define RFLM_RXAGC_TDS_FALL_G0_TO_G1 0
#define RFLM_RXAGC_TDS_FALL_G0_TO_G2 1  
#define RFLM_RXAGC_TDS_FALL_G0_TO_G3 2
#define RFLM_RXAGC_TDS_FALL_G1_TO_G2 3
#define RFLM_RXAGC_TDS_FALL_G1_TO_G3 4
#define RFLM_RXAGC_TDS_FALL_G2_TO_G3 5


#define RFLM_RXAGC_TDS_RISE_G1_TO_G0 0
#define RFLM_RXAGC_TDS_RISE_G2_TO_G0 1  
#define RFLM_RXAGC_TDS_RISE_G2_TO_G1 2
#define RFLM_RXAGC_TDS_RISE_G3_TO_G0 3
#define RFLM_RXAGC_TDS_RISE_G3_TO_G1 4
#define RFLM_RXAGC_TDS_RISE_G3_TO_G2 5

#define RFLM_TDS_MAX_PWR_DB10      230
#define RFLM_TDS_MIN_PWR_DB10     -550
#define RFLM_TDS_DEFAULT_IQ_GAIN   622
#define RFLM_TDS_DEFAULT_ENV_SCALE 622

extern boolean is_sawless_enabled;

extern uint8 tds_override_rx_lin_state_val;

typedef enum
{
  RFLM_TDS_UPDATE_RX_HANDLE_AFTER_SLOT_UPDATE,
  RFLM_TDS_UPDATE_RX_HANDLE_AFTER_EXEC_RF_SCRIPT,
  RFLM_TDS_UPDATE_RX_HANDLE_AFTER_RXAGC_CALC,
  RFLM_TDS_UPDATE_RX_HANDLE_AFTER_RXAGC_GET_LNAOFFSET,
  RFLM_TDS_UPDATE_RX_HANDLE_AFTER_RXAGC_UPDATE,

  RFLM_TDS_UPDATE_TX_HANDLE_AFTER_SLOT_UPDATE,
  RFLM_TDS_UPDATE_TX_HANDLE_AFTER_EXEC_RF_SCRIPT,
  RFLM_TDS_UPDATE_TX_HANDLE_AFTER_TXAGC_CALC,
  RFLM_TDS_UPDATE_TX_HANDLE_AFTER_TXAGC_GET_MTPL,

  RFLM_TDS_UPDATE_HANDLE_MAX_NUM
} rflm_tds_handle_user_data_update_t;

typedef enum
{
  /*---------------------------------------
   Adding new script ID here,    
  */  
  RFLM_TDS_DM_PRELOAD_SCRIPT,
  RFLM_TDS_DM_TRIGGER_SCRIPT,
  RFLM_TDS_DM_NONTRX_SCRIPT,

  RFLM_TDS_T2L_DM_PRELOAD_SCRIPT,
  RFLM_TDS_T2L_DM_TRIGGER_SCRIPT,
  RFLM_TDS_T2L_DM_NONTRX_SCRIPT,
  
  RFLM_TDS_TUNER_AOL_EVENT = RFLM_MAX_NUM_HANDLE_SCRIPT_BUF -3,
  RFLM_TDS_TUNER_CL_EVENT = RFLM_MAX_NUM_HANDLE_SCRIPT_BUF -2,
  RFLM_TDS_SCRIPT_ASD = RFLM_MAX_NUM_HANDLE_SCRIPT_BUF -1,
  RFLM_TDS_MAX_SCRIPT_ID = RFLM_MAX_NUM_HANDLE_SCRIPT_BUF,
}rflm_tds_script_e;


typedef enum
{
  RFLM_TDS_SCHEDULE_ASDIV,
  RFLM_TDS_SCHEDULE_AOL,
  RFLM_TDS_SCHEDULE_CL
}rflm_tds_schedule_script_e;



/* ***************************************************************************
**                            RFLM TDS State Machine
** **************************************************************************/
typedef enum
{
  /* IFREQ */
  RFLM_TDS_IFREQ_STARTUP_RX_TUNING,
  RFLM_TDS_IFREQ_STARTUP_RX_ENABLE,
  RFLM_TDS_IFREQ_STARTUP_RX_ACTIVATE,
  RFLM_TDS_IFREQ_CLEANUP_RX_TUNING,  
  RFLM_TDS_IFREQ_CLEANUP_RX_DEACTIVATE,  

  RFLM_TDS_IFREQE_MAX_SCRIPT
} rflm_tds_ifreq_script_e;

typedef enum
{
  STARTUP_SCRIPT,
  CLEANUP_SCRIPT
} rflm_tds_irat_script_e;

typedef struct
{
  /* TDS standalone script ptr array */
  rflm_ccs_rf_event_header_task_type* rfScriptPtrArray[RFLM_TDS_STANDALONE_MAX_SCRIPT][RFLM_TDS_MAX_NUM_HOME_TARGET_BUFFER][RFLM_TDS_MAX_PINGPONG_BUFFERS];
  /* IFREQ script ptr array */
  rflm_ccs_rf_event_header_task_type* ifreqScriptPtrArray[RFLM_TDS_IFREQE_MAX_SCRIPT][RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];
  /* LNA script ptr array */
  rflm_ccs_rf_event_header_task_type* rxLnaScriptPtrArray[RFLM_TDS_NUM_LIN_STATES][RFLM_TDS_RX_TX_MAX_NUM_BANDS][RFLM_TDS_RX_TX_MAX_NUM_RX_GAIN_STATES][RFLM_TDS_RX_TX_MAX_NUM_CHAINS];
  /* LNA w/ Latch script ptr array */
  rflm_ccs_rf_event_header_task_type* rxLnawLatchScriptPtrArray[RFLM_TDS_NUM_LIN_STATES][RFLM_TDS_RX_TX_MAX_NUM_BANDS][RFLM_TDS_RX_TX_MAX_NUM_RX_GAIN_STATES][RFLM_TDS_RX_TX_MAX_NUM_CHAINS];
  /* LNA latch script ptr */
  rflm_ccs_rf_event_header_task_type* rxLnaLatchPrxOnlyScriptPtr;
  rflm_ccs_rf_event_header_task_type* rxLnaLatchDrxOnlyScriptPtr;
  rflm_ccs_rf_event_header_task_type* rxLnaLatchPrxDrxScriptPtr;
  /* BHO one-time init */
  rflm_ccs_rf_event_header_task_type* bhoOnetimeInitScriptPtr;
  /* TDS Event Info */
  rflm_tds_rf_script_info_t* tdsEventInfoPtr;
  /* IFREQ Event Info */
  rflm_tds_rf_script_info_t* ifreqEventInfoPtr;
  /* IRAT Event Info */
  rflm_tds_rf_script_info_t* iratEventInfoPtr;
  /* PDET write/read */
  rflm_ccs_rf_event_header_task_type* pdetWriteScriptPtr;
  rflm_ccs_rf_event_header_task_type* pdetReadScriptPtr;
  /* Temp write/read */
  rflm_ccs_rf_event_header_task_type* tempWriteScriptPtr;
  rflm_ccs_rf_event_header_task_type* tempReadScriptPtr;
  /* RF interal event */
  rflm_ccs_rf_event_header_task_type* rxWakeupScriptPtr;
  rflm_ccs_rf_event_header_task_type* txWakeupScriptPtr;

  rflm_ccs_rf_event_header_task_type* jdetReadScriptPtr;
  rflm_ccs_rf_event_header_task_type* homeRestoreScriptPtr[RFLM_TDS_MAX_NUM_HOME_RESTORE_SCRIPT_BUFFER];
  rflm_ccs_rf_event_header_task_type* ip2CalCodeWriteScriptPtr[RFLM_TDS_RX_TX_MAX_NUM_BANDS][RFLM_TDS_NUM_LIN_STATES];

  /* AutoPin write event*/
  rflm_ccs_rf_event_header_task_type* autopinWriteScriptPtrArray[2];

} rflm_tds_rf_script_ptr_t;

typedef struct
{
  uint32 enqueueStartTime;
  uint32 enqueueEndTime;
  int32  enqueueDurTime;

  uint32 execStartTime;
  uint32 execEndTime;
  int32  execDurTime;

  boolean execInProgress;
} rflm_tds_script_status_t;

typedef struct 
{
  rflm_tds_script_status_t rxWakeupScript;
  rflm_tds_script_status_t rxBurstScript;
  rflm_tds_script_status_t rxLnaScript;
  rflm_tds_script_status_t txWakeupScript;
  rflm_tds_script_status_t txBurstScript;
  rflm_tds_script_status_t rxOffScript;
  rflm_tds_script_status_t txOffScript;
  rflm_tds_script_status_t bhoOneTimeInitScript;
  rflm_tds_script_status_t dlToIfreqScript;
} rflm_tds_script_summary_t;


typedef struct
{
  uint32 txLmHandle;
  /*! @brief input to PA STM         */
  rflm_txagc_pa_input_t  pa_input[RFLM_TDS_MAX_NUM_TX_SLOTS]; 
  /*! @brief input to txagc lookup   */
  rflm_txagc_agc_input_t txagc_input[RFLM_TDS_MAX_NUM_TX_SLOTS];  
  /*! @brief input to build script   */
  rflm_txagc_script_input_t script_settings[RFLM_TDS_MAX_NUM_TX_SLOTS];
  /*! @brief txagc param data buff   */
  rflm_txagc_params_t txagc_params[RFLM_TDS_MAX_NUM_TX_SLOTS];
} rflm_tds_txagc_params_s;

typedef struct
{
  int riseThreshold[RFLM_CMN_RXAGC_MAX_LNA_STATES];
  int fallThreshold[RFLM_CMN_RXAGC_MAX_LNA_STATES];
} rflm_tds_rxagc_swpt_t;

typedef struct
{
  rflm_handle_rx_t rxLmHandle[RFLM_TDS_NUM_RX_CHAINS];  
  uint8 homeTargetScriptBufIndex; 
  rflm_tds_rxagc_swpt_t swpt[RFLM_TDS_MODE_INVALID][RFLM_TDS_MAX_PWR_EST_TYPE];
  int32 lnaOffset[RFLM_TDS_NUM_RX_CHAINS][RFLM_CMN_RXAGC_MAX_LNA_STATES];
  boolean infoReady;   
} rflm_tds_home_rxagc_params_t;

typedef struct
{  
  rflm_handle_rx_t rxLmHandle[RFLM_TDS_NUM_RX_CHAINS];  
  rflm_tds_rxagc_swpt_t swpt[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS][RFLM_TDS_MAX_PWR_EST_TYPE];
  int32 lnaOffset[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS][RFLM_TDS_NUM_RX_CHAINS][RFLM_CMN_RXAGC_MAX_LNA_STATES];
  boolean infoReady;   
} rflm_tds_ifreq_rxagc_params_t;

typedef struct
{
  rflm_handle_rx_t rxLmHandle;  
  rflm_tds_rxagc_swpt_t swpt[RFLM_TDS_MODE_INVALID][RFLM_TDS_MAX_PWR_EST_TYPE];
  int32 lnaOffset[RFLM_CMN_RXAGC_MAX_LNA_STATES];
  boolean infoReady;   
} rflm_tds_irat_rxagc_params_t;

typedef struct
{
  rflm_tds_home_rxagc_params_t home[RFLM_TDS_MAX_NUM_HOME_TARGET_BUFFER];
  rflm_tds_ifreq_rxagc_params_t ifreq;
  rflm_tds_irat_rxagc_params_t irat;
} rflm_tds_rxagc_params_t;

typedef struct
{
  uint8 execBufIdx[RFLM_TDS_MAX_NUM_HOME_TARGET_BUFFER];
  uint8 prepBufIdx[RFLM_TDS_MAX_NUM_HOME_TARGET_BUFFER];
  boolean rxOnlyScptBuilding[RFLM_TDS_MAX_NUM_HOME_TARGET_BUFFER][RFLM_TDS_MAX_PINGPONG_BUFFERS];
  boolean rxTxScptBuilding[RFLM_TDS_MAX_NUM_HOME_TARGET_BUFFER][RFLM_TDS_MAX_PINGPONG_BUFFERS];
} rflm_tds_pingpong_buffer_info_t;

typedef struct
{
  uint8 execBufIdx;
  uint8 prepBufIdx; 
} rflm_tds_home_restore_scpt_info_t;

typedef struct
{
  uint16  singleRunCcsMemoryUseInByte;
  uint16  singleRunUpperCcsMemoryUseInByte;
  uint16  singleRunBottomCcsMemoryUseInByte;
  uint32  accuCcsMemoryUseInByte;
  uint16  accuCcsMemoryCalcRun;
  uint16  maxSingleRunCcsMemoryUseInByte;
  uint16  avgSingleRunCcsMemoryUseInByte;
  uint16  minSingleRunCcsMemoryUseInByte;
} rflm_tds_tq_memory_use_stat_t;

typedef struct
{
  /* CCS interface structure */
  rflm_tds_rf_ccs_intf_s rfCcsIntf;
  /* CCS resources lock state */
  boolean ccsRfTqLocked;
  /* Task ID for last issue seq event */
  uint32  lastIseqTaskIdx;  
  /* end_time of the last task */
  uint32  lastTaskEndTime;  
  /* CCS TQ memory use stat */
  rflm_tds_tq_memory_use_stat_t ccsMemStat; 
} rflm_tds_ccs_control_info_t;

/* Info shared for WTR HDET and Tuner TDET */
typedef struct
{
  uint16 lastSchedSfm;                              
  boolean enabled;
  boolean inProcess;
  int16 triggerPwrdBm10;                  /* unit: dbm10 */
  int16 rflmTxagcdbm10;                  /* unit: dbm10 */
  uint16 triggerPeriod;                   /* unit: no. of subframe */
  rflm_tds_slot_num_e triggerScheSlotNum;
  boolean triggerExecuted;
  boolean readBackReady;
} rflm_tds_pdet_sched_info_t;

/* Info for WTR temp read */
typedef struct
{
  uint16 lastSchedSfm;                              
  boolean enabled;
  boolean inProcess;
  uint16 triggerPeriod;                   /* unit: no. of subframe */
  rflm_tds_slot_num_e triggerScheSlotNum;
  boolean triggerExecuted;
  boolean readBackReady;
} rflm_tds_therm_read_sched_info_t;

/* Info for Tuner temp read */
typedef struct
{
  uint16 lastSchedSfm;                              
  boolean enabled;
  boolean inProcess;
  uint16 triggerPeriod;                   /* unit: no. of subframe */
  rflm_tds_slot_num_e triggerScheSlotNum;
  boolean triggerExecuted;
  boolean readBackReady;
} rflm_tds_tuner_therm_read_sched_info_t;

/* Info for JDET read */
typedef struct
{
  uint16 lastSchedSfm;                              
  boolean enabled;
  boolean inProcess;
  uint16 triggerPeriod;                 
  rflm_tds_slot_num_e triggerScheSlotNum;
  boolean triggerExecuted;
  boolean readBackReady;
  uint32 jdetValue; 
} rflm_tds_jdet_sched_info_t;

/* Info for ASDiv */
typedef struct
{
  uint16 lastSchedSfm;                              
  boolean enabled;
  boolean scptReady;
  rflm_handle_rx_t rxLmHandle[2];
  uint32  refTime;  
} rflm_tds_asd_sched_info_t;

/* Info for AOL */
typedef struct
{
  uint16 lastSchedSfm;                              
  boolean enabled;
  boolean scptReady;
  rflm_handle_rx_t rxLmHandle[2];
  uint32  refTime;  
} rflm_tds_aol_sched_info_t;

/* Info for CL */
typedef struct
{
  uint16 lastSchedSfm;                              
  boolean enabled;
  boolean scptReady;
  rflm_handle_rx_t rxLmHandle[2];
  uint32  refTime;  
  int16   triggerPwrdBm10; 
  int16   actualPwrdBm10; 
} rflm_tds_cl_sched_info_t;

typedef struct
{
  boolean enabled;
  uint16  timeAdvancedInUsec;
  uint16  currSfm;
  rflm_tds_slot_num_e schedSlot;
  boolean tuningSched;
  boolean tuningCompleted;
  uint32  tuningEndTime;
  boolean toSchedTxRxOffScpt;
} rflm_tds_head_start_info_t;

typedef struct
{
  boolean toSchedScript;
  boolean scptCompleted;
  uint32  flagSetTime;
  uint32  execCompleteTime;
} rflm_tds_tx_wakeup_script_exec_info_t;

typedef struct
{
  uint8 rx0Idx;
  uint8 rx1Idx;
} rflm_tds_wb_adc_info_t;

/* Info for partial Tx slot */
typedef struct
{
  boolean scptInProcess;  
} rflm_tds_tx_partial_slot_info_t;


/* Info for CCS execution debug */
typedef struct
{
  // TDS always hold a pair of TQs, e.g., "logical" TQ0 and TQ1. 
  // Physically, they can be TQ0/1 or TQ2/3. Below index used to record the physical TQ index. 
  uint8 tq0Index; // physical TQ indix for "logical" TQ0 
  uint8 tq1Index; // physical TQ indix for "logical" TQ1

  uint32 currTaskIndex[RFLM_TDS_MAX_NUM_TQ];  

  rflm_tds_ccs_exec_single_task_info_t info[RFLM_TDS_MAX_NUM_TQ][RFLM_TDS_MAX_NUM_TASK_PER_TQ];  

  uint32 lastTaskIndex[RFLM_TDS_TS6][RFLM_TDS_MAX_NUM_TQ];   
} rflm_tds_ccs_exec_info_t;

typedef struct
{
  /* TxAGC "group" timing */
  int32 txAgcGroupOffsetUsec;
  /* FTM RF mode timing adjustment */
  int32 txAgcFtmRfTimeAdjustUsec;  
} rflm_tds_timing_control_info_t;

typedef struct
{
  rflm_autopin_get_run_decision_in_t run_decision_in;
  rflm_autopin_get_run_decision_out_t run_decision_out;
  boolean enabled;
  boolean inProcess;
  uint16 lastSchedSfm;                              
  uint16 triggerPeriod;                   /* unit: no. of subframe */
  rflm_tds_slot_num_e triggerScheSlotNum;
  boolean triggerExecuted;
  boolean readBackReady;
} rflm_tds_autopin_info_t;

typedef struct
{
  /* logic device */
  uint8	device;
  /* RxD enable */
  uint8	rxdEn;
  /* RxD override (for CMAPI) */
  uint8	rxdEnOverride;
  /* home buffer index */
  uint8	homeBufIdx;
  /* home Rx buffer index (for BHO, Rx and Tx might not change buffer index at the same time) */
  uint8	rxHomeBufIdx;  
  /* RF FTM mode */
  boolean isRfFtmTestMode;
  /* Timing Control Info */
  rflm_tds_timing_control_info_t timingCtrInfo;
  /* RF script ptr definitions */
  rflm_tds_rf_script_ptr_t script;
  /* CCS control info */
  rflm_tds_ccs_control_info_t ccsInfo;
  /* dm_tech_buff_ptr */
  rflm_tds_ccs_event_t*   rflmTdsDmTechBuffPtr;
  /* dm_tech_buff_ptr */
  rflm_tds_meas_ccs_event_t*   rflmTdsDmMeasBuffPtr;
  /* RF script status (enqueue time and exec time) */
  rflm_tds_script_summary_t scriptSummary;
  /* RxAGC information */
  rflm_tds_rxagc_params_t rxAgc[RFLM_TDS_NUM_LIN_STATES]; 

  /* PDET schedule info */
  rflm_tds_pdet_sched_info_t pdetInfo;
  /* Therm Read schedule info */
  rflm_tds_therm_read_sched_info_t thermInfo;
  /* Tuner Therm Read schedule info */
  rflm_tds_tuner_therm_read_sched_info_t tunerThermInfo;
  /* ASDiv schedule info */
  rflm_tds_asd_sched_info_t asdInfo;
  /* AOL schedule info */
  rflm_tds_aol_sched_info_t aolInfo;
  /* CL schedule info */
  rflm_tds_cl_sched_info_t clInfo;
  /* Ping-pong buffer info */
  rflm_tds_pingpong_buffer_info_t ppInfo;
  /* Head_Start Information */
  volatile rflm_tds_head_start_info_t  headStartInfo;
  /* Tx wakeup script execution information */
  volatile rflm_tds_tx_wakeup_script_exec_info_t txWakeupScptInfo;
  /* Home restore script info */
  rflm_tds_home_restore_scpt_info_t homeRestoreScptInfo;
  /* Partial slot info */
  rflm_tds_tx_partial_slot_info_t txPartialSlotInfo;
  /* CCS task info */
  rflm_tds_ccs_exec_info_t ccsTaskInfo;

  /* FW request/release FED resource */
  boolean fwRequestedResource;
  boolean grantedFwResourceRequest;

  rflm_tds_jdet_sched_info_t jdetInfo;

  volatile rflm_tds_wb_adc_info_t wbAdcInfo;

  uint32 wtrTriggerDelayTime;

  rflm_tds_autopin_info_t autopinInfo;
} rflm_tds_rf_mode_ctl_s;

typedef struct
{
  boolean isRfTune;
  uint32  expectedCompleteTime;
  uint32  actualCompleteTime;
  int32   rfTuneLateBy;
  int32   rfTuneLateByUsec;
} rflm_tds_single_slot_rf_tune_info_t;

extern rflm_tds_rf_mode_ctl_s rflmTdsRfModeCtlState;
extern boolean rflm_tds_disable_slot_update; 
extern boolean rflm_tds_use_on_rumi;
extern rflm_tds_single_slot_rf_tune_info_t rflmTdsRfTuneInfo[RFLM_TDS_TS_INVALID];
extern boolean is_sawless_enabled;



/* ***************************************************************************
**                            RFLM TDS Rx Handle User Data
** Note: 
**   This data definition shared between Rx Home/Target, IFREQ, IRAT handles 
** **************************************************************************/

typedef enum
{
  // Rx handle type
  RFLM_TDS_HOME_FREQ_PRX_HANDLE,
  RFLM_TDS_HOME_FREQ_DRX_HANDLE,
  RFLM_TDS_TARGET_FREQ_PRX_HANDLE,
  RFLM_TDS_TARGET_FREQ_DRX_HANDLE,
  RFLM_TDS_IFREQ_PRX_HANDLE,
  RFLM_TDS_IFREQ_DRX_HANDLE,
  RFLM_TDS_IRAT_PRX_HANDLE,
  RFLM_TDS_IRAT_DRX_HANDLE,

  // Tx handle type
  RFLM_TDS_HOME_FREQ_TX_HANDLE,
  RFLM_TDS_TARGET_FREQ_TX_HANDLE,

  RFLM_TDS_MAX_NUM_HANDLE
} rflm_tds_handle_t;

/* Type definition for state variables */
typedef struct
{
  rflm_tds_handle_t handleType;
  uint8             band;
  uint16            channel;
  uint32            freqInKHz;
  uint8             bufferIndex;
} rflm_tds_rx_handle_state_t;

typedef struct
{
  rflm_tds_handle_t handleType;
  uint8             ifreqScriptBugIndex;
  uint8             band[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];
  uint16            channel[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];
  uint32            freqInKHz[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];
} rflm_tds_ifreq_rx_handle_state_t;

/* Type definition for home/target rxLM handle (primary chain, i.e., Rx0) */
typedef struct 
{
  // OFF-to-RX transition
  void* offToRxEventRxTuningScriptPrxOnlyScriptPtr;
  void* offToRxEventRxEnableScriptPtr;
  void* offToRxEventRxActivateIssueSeqScriptPrxOnlyScriptPtr;
  // RX-to-OFF transition
  void* rxToOffEventRxDeactivateIssueSeqScriptPrxOnlyScriptPtr;
} rflm_tds_rx_home_target_prxonly_script_ptr_array_t;

/* Type definition for home/target rxLM handle (diversity chain, i.e., Rx1) */
typedef struct 
{
  // OFF-to-RX transition
  void* offToRxEventRxTuningScriptPrxDrxScriptPtr;
  void* offToRxEventRxEnableScriptPtr;
  void* offToRxEventRxActivateIssueSeqScriptPrxDrxScriptPtr;
  // RX-to-OFF transition
  void* rxToOffEventRxDeactivateIssueSeqScriptPrxDrxScriptPtr;
} rflm_tds_rx_home_target_prxdrx_script_ptr_array_t;

/* Type definition for ifreq rxLM handle (primary/diversity chain, i.e., Rx0/Rx1, share the same) */
typedef struct 
{
  // IFREQ Start-up transiton
  void* ifreqStartupEventRxTuningScriptPrxDrxScriptPtr[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];
  void* ifreqStartupEventRxEnableScriptPtr[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];
  void* ifreqStartupEventRxActivateIssueSeqScriptPrxDrxPtr[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];
  // IFREQ Clean-up transiton
  void* ifreqCleanupEventRxTuningScriptPrxDrxScriptPtr[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];
  void* ifreqCleanupEventRxDeactivateIssueSeqScriptPrxDrx[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS]; 
} rflm_tds_rx_ifreq_prxdrx_script_ptr_array_t;


typedef struct
{
  uint16 currSfm;                             ///< current subframe #
  rflm_tds_slot_num_e currSlot;               ///< current slot #
  uint8 numofEvt;                             ///< Num of state info for this scheduling point, up to 3 events
  rflm_tds_slot_update_slot_cfg_t slot[RFLM_TDS_MAX_NUM_SLOT_PER_SCHEDULE_POINT];  
                                              ///< Slot config info
} rflm_tds_user_data_general_schedule_info_t;

typedef struct
{
  rflm_tds_rx_pwr_est_e pwrEstType;           ///< Power estimate type
  int32 dBFS256[RFLM_TDS_MAX_NUM_LNA_GAIN_UPDATE_PER_SLOT];                           
                                              ///< dBFS in S16Q8 format
                                              ///< support non-TS0 and TS0, up to 2 updates/slot
  uint8 prevGainState[RFLM_TDS_MAX_NUM_LNA_GAIN_UPDATE_PER_SLOT];                     
                                              ///< Previous Rx gain state
                                              ///< support non-TS0 and TS0, up to 2 updates/slot
                                              ///<
                                              ///< This information will be updated after the new 
                                              ///< calculation, at that time, the definition of 
                                              ///< previous gain state and next gain state is clear
  uint8 nextGainState[RFLM_TDS_MAX_NUM_LNA_GAIN_UPDATE_PER_SLOT];                     
                                              ///< Next Rx gain state
                                              ///< support non-TS0 and TS0, up to 2 updates/slot
  int32 lnaOffset[RFLM_TDS_MAX_NUM_LNA_GAIN_UPDATE_PER_SLOT];
                                              ///< LNA offset for the NEXT Rx gain state
                                              ///< support non-TS0 and TS0, up to 2 updates/slot

  uint16 lnaUpdateScheduledAtSfm[RFLM_TDS_MAX_NUM_LNA_GAIN_UPDATE_PER_SLOT];             
                                              ///< LNA UPDATE scheduled at which subframe
                                              ///< support non-TS0 and TS0, up to 2 updates/slot
  rflm_tds_slot_num_e lnaUpdateScheduledAtSlot[RFLM_TDS_MAX_NUM_LNA_GAIN_UPDATE_PER_SLOT];
                                              ///< LNA UPDATE scheduled at which slot scheduling point
                                              ///< support non-TS0 and TS0, up to 2 updates/slot

  uint16 lnaTriggerScheduledAtSfm[RFLM_TDS_MAX_NUM_LNA_GAIN_UPDATE_PER_SLOT];             
                                              ///< LNA TRIGGER scheduled at which subframe
                                              ///< support non-TS0 and TS0, up to 2 updates/slot
  rflm_tds_slot_num_e lnaTriggerScheduledAtSlot[RFLM_TDS_MAX_NUM_LNA_GAIN_UPDATE_PER_SLOT];
                                              ///< LNA TRIGGER scheduled at which slot scheduling point
                                              ///< support non-TS0 and TS0, up to 2 updates/slot
} rflm_tds_user_data_home_target_rxagc_t;


typedef struct
{
  rflm_tds_rx_pwr_est_e pwrEstType;           ///< Power estimate type
  uint8 lastIfreqLnaUpdateBufferIndex;        ///< The last nbr buffer index whose LNA
                                              ///< got updated
  int32 dBFS256[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];                           
                                              ///< dBFS in S16Q8 format
                                              ///< support up to 11 neighbors
  uint8 prevGainState[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];                     
                                              ///< Previous Rx gain state
                                              ///< support up to 11 neighbors
                                              ///<
                                              ///< This information will be updated after the new 
                                              ///< calculation, at that time, the definition of 
                                              ///< previous gain state and next gain state is clear
  uint8 nextGainState[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];                     
                                              ///< Next Rx gain state
                                              ///< support up to 11 neighbors
  int32 lnaOffset[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];
                                              ///< LNA offset for the NEXT Rx gain state
                                              ///< support up to 11 neighbors

  uint16 lnaUpdateScheduledAtSfm[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];             
                                              ///< LNA UPDATE scheduled at which subframe
                                              ///< support up to 11 neighbors
  rflm_tds_slot_num_e lnaUpdateScheduledAtSlot[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];
                                              ///< LNA UPDATE scheduled at which slot scheduling point
                                              ///< support up to 11 neighbors

  uint16 lnaTriggerScheduledAtSfm[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];             
                                              ///< LNA TRIGGER scheduled at which subframe
                                              ///< support up to 11 neighbors
  rflm_tds_slot_num_e lnaTriggerScheduledAtSlot[RFLM_TDS_RX_TX_MAX_NUM_INTER_FREQ_BUFFERS];
                                              ///< LNA TRIGGER scheduled at which slot scheduling point
                                              ///< support up to 11 neighbors
} rflm_tds_user_data_ifreq_rxagc_t;


typedef struct
{
  // state variables
  union
  {
    rflm_tds_rx_handle_state_t                          homeTargetRx;
    rflm_tds_ifreq_rx_handle_state_t                    ifreqRx;
  } handleState;
  
  // General scheduling info (mainly for debug purpose)
  rflm_tds_user_data_general_schedule_info_t            generalScheduleInfo;

  // script ptr array
  union
  {
    rflm_tds_rx_home_target_prxonly_script_ptr_array_t  homeTargetPrxOnlyScript;
    rflm_tds_rx_home_target_prxdrx_script_ptr_array_t   homeTargetPrxDrxScript;
    rflm_tds_rx_ifreq_prxdrx_script_ptr_array_t         ifreqPrxDrxScript;
  } rxScriptPtrArray;

  // tech specific rxagc information (mainly for debug purpose)
  union
  {
    rflm_tds_user_data_ifreq_rxagc_t                    homeTagetRxagc;
    rflm_tds_user_data_ifreq_rxagc_t                    ifreqRxagc;
  } rxagc;

} rflm_tds_rxlm_handle_user_data_t;




/* ***************************************************************************
**                            RFLM TDS Tx Handle User Data
** Note: 
**   This data definition shared between Tx Home/Target handles  
** **************************************************************************/

/* Type definition for state variables */
typedef struct
{
  rflm_tds_handle_t     handleType;
  uint8                 band;
  uint16                channel;
  uint32                freqInKHz;
  uint8                 bufferIndex;

  boolean               hdetEnabled;
  boolean               FBRxenabled;
  boolean               TempReadEnabled;
} rflm_tds_tx_handle_state_t;


typedef struct
{
  // OFF-to-Tx transition
  void* offToTxEventTxTuningScriptPtr;
  void* offToTxEventTxEnableScriptPtr;  
  void* offToTxEventTxActivateIssueSeqScriptPowerLtoHScriptPtr;

  // IFREQ-to-Tx transition
  void* ifreqToTxEventTxTuningScriptPtr;
  void* ifreqToTxEventTxEnableScriptPtr;  
  void* ifreqToTxEventTxActivateIssueSeqScriptPowerLtoHScriptPtr;

  // Tx-to-OFF transition
  void* txToOffEventTxDeactivateIssueSeqScriptPtr; 

  // Rx-to-Tx transition
  void* rxToTxEventTxTuningScriptPtr;
  void* rxToTxEventTxEnableScriptPtr;  
  void* rxToTxEventRxDeactivateIssueSeqScriptPrxOnlyScriptPtr;
  void* rxToTxEventRxDeactivateIssueSeqScriptPrxDrxScriptPtr;
  void* rxToTxEventTxActivateIssueSeqScriptPowerLtoHScriptPtr;
  
  // Rx-to-Tx transition
  void* txToRxEventRxTuningScriptPrxOnlyScriptPtr;
  void* txToRxEventRxTuningScriptPrxDrxScriptPtr;
  void* txToRxEventRxEnableScriptPtr;  
  void* txToRxEventRxActivateTxDeactivateIssueSeqScriptPrxOnlyScriptPtr;
  void* txToRxEventRxActivateTxDeactivateIssueSeqScriptPrxDrxScriptPtr;
    
  // Tx-to-Tx transition
  void* txToTxEventTxActivateIssueSeqScriptPowerLtoHScriptPtr;  

  // BHO one-time init
  void* bhoStartupEventTxOneTimeInitScriptPtr;

  // HDET
  void* doHdetEventHdetWriteScriptPtr;
  void* doHdetEventHdetReadScriptPtr;
  
  // Therm read
  void* doThermEventThermWriteScriptPtr;
  void* doThermEventThermReadScriptPtr;

  // Tuner therm read
  void* doThermEventTunerThermReadScriptPtr;
} rflm_tds_tx_script_ptr_array_t;

typedef struct
{
  int32 mtplS16Q9;                            ///< MTPL, unit of dBm in 16Q9 format
  int32 sgtdsBackOffMtplS16Q9;                ///< sgtds MTPL, unit of dBm in 16Q9 format
  int32 minPwrS16Q9;                          ///< Min pwr, unit of dBm in 16Q9 format
  uint32 iqGainNominal[RFLM_TXAGC_MAX_XPT_MODE];
  uint32 envScale;
  int16 mpr16Q9[RFLM_TDS_RX_TX_MAX_NUM_MPR_BINS];	      ///< MPR V2, unit of dBm in 16Q9 format  
} rflm_tds_tx_data_s;

typedef struct
{
  int16 txPwrdB10[RFLM_TDS_MAX_NUM_TX_SLOTS];///< Desired output power in unit??? support up to 7 slots
  boolean txPwrValid[RFLM_TDS_MAX_NUM_TX_SLOTS];
                                              ///< Indicate if Tx pwr is valid, support up to 7 slots
  int32 digitalBackoffS16Q9[RFLM_TDS_MAX_NUM_TX_SLOTS];               
                                              ///< Digital pwr backoff, unit of dB in 16Q9 format
                                              ///< for example, -5<->-5*256   
                                              ///< support up to 7 slots
  uint16 txagcUpdateScheduledAtSfm;           ///< TxAGC UPDATE scheduled at which subframe
  rflm_tds_slot_num_e txagcUpdateScheduledAtSlot;
                                              ///< TxAGC UPDATE scheduled at which slot scheduling point
  uint16 txagcTriggerScheduledAtSfm[RFLM_TDS_MAX_NUM_TX_SLOTS];             
                                              ///< LNA TRIGGER scheduled at which subframe
                                              ///< support up to 7 tx slots
  rflm_tds_slot_num_e txagcTriggerScheduledAtSlot[RFLM_TDS_MAX_NUM_TX_SLOTS];
                                              ///< LNA TRIGGER scheduled at which slot scheduling point
                                              ///< support up to t tx slots
  uint16 mtplMinPwrQueryAtSfm;                ///< MTPL/min power query at which subframe
  rflm_tds_slot_num_e mtplMinPwrQueryAtSlot;
                                              ///< MTPL/min power query at which slot scheduling point
  rflm_tds_txagc_params_s rflmTdsTxagcParams; ///< TXAGC parameters
  rflm_tds_slot_num_e activeTxSlot;           ///< placeholder for active TX slot during slot_update
  rflm_tds_tx_data_s txData;
  rflm_txagc_pa_state_data_t preSubframePaState[RFLM_TDS_MAX_NUM_TX_SLOTS];  
                                              ///< save pa state for previous subframe 
  int16 txActualOutputPwrdB10[RFLM_TDS_MAX_NUM_TX_SLOTS]; //actual TX power used to compare HDET
  //boolean isSingleTxSlot;                     ///< if there is only one tx slot in the subframe
} rflm_tds_user_data_txagc_t;

typedef struct
{
  // state variables
  rflm_tds_tx_handle_state_t handleState;

  // General scheduling info (mainly for debug purpose)
  rflm_tds_user_data_general_schedule_info_t            generalScheduleInfo;

  // script ptr array
  rflm_tds_tx_script_ptr_array_t txScriptPtrArray;

  // txagc
  rflm_tds_user_data_txagc_t  txAGC;
} rflm_tds_txlm_handle_user_data_t;

typedef struct
{
  uint8   device;
  uint32  techBufferSize;
} rflm_tds_enter_mode_in_t;

typedef struct
{
  boolean success;
  void*   dmTechBuffPtr;
} rflm_tds_enter_mode_out_t;


/*! @brief  Input parameters for rflm_tds_schedule_rf_internal_event API */
typedef enum
{
  RFLM_TDS_RX_WAKEUP_SCRIPT,
  RFLM_TDS_RX_BURST_SCRIPT,
  RFLM_TDS_RX_LNA_SCRIPT,
  RFLM_TDS_TX_WAKEUP_SCRIPT,
  RFLM_TDS_TX_BURST_SCRIPT,
  RFLM_TDS_RX_OFF_SCRIPT,
  RFLM_TDS_TX_OFF_SCRIPT,
} rflm_tds_internal_script_type_e;

typedef enum
{
  RFLM_TDS_EXECUTE_SCRIPT,
  RFLM_TDS_CHECK_SCRIPT_STATUS,
} rflm_tds_internal_action_e;

typedef struct
{
  boolean callInWakeup;
  boolean divEn;
} rflm_tds_rx_wakeup_script_t;

typedef struct
{
  boolean divEn;
  uint8 bufId;
  uint8 gainState[RFLM_TDS_NUM_RX_CHAINS];
  rflm_handle_rx_t rxLmHandle[RFLM_TDS_NUM_RX_CHAINS];
  boolean callInCalMode;
  rflm_tds_rx_lin_state_e linState;

} rflm_tds_rx_burst_script_t;

typedef struct
{
  uint8 band;
  uint8 gainState;
  rflm_tds_rx_chain_e rxChain;
  boolean callInCalMode;
  rflm_tds_rx_lin_state_e linState;
} rflm_tds_rx_lna_script_t;

typedef struct
{
  boolean callInCalMode;
} rflm_tds_tx_wakeup_script_t;

typedef struct
{
  boolean callInCalMode;
  uint8 bufId;
  int16 txPwrdBmQ9;
  rflm_handle_tx_t txLmHandle;
} rflm_tds_tx_burst_script_t;


typedef struct
{
  rflm_ustmr_t                      effectTime;      ///< Time for script to take effect
  rflm_tds_internal_script_type_e   scriptType;      ///< Script type to execute
  rflm_tds_internal_action_e        actionCmd;       ///< Execuate script or check status
  union
  {
    rflm_tds_rx_wakeup_script_t     rxWakeupScript;
    rflm_tds_rx_burst_script_t      rxBurstScript;
    rflm_tds_rx_lna_script_t        rxLnaScript;
    rflm_tds_tx_wakeup_script_t     txWakeupScript;
    rflm_tds_tx_burst_script_t      txBurstScript;
    rflm_tds_rx_burst_script_t      rxOffScript;
    rflm_tds_tx_burst_script_t      txOffScript;
  } scriptInfo;                               ///< Pass additional info for the script execution
  boolean                           scriptExecComplete;
} rflm_tds_schedule_rf_internal_event_in_t;

typedef struct
{
  uint8 priDevice;
  uint8 secDevice;
  boolean rxdEn;
} rflm_tds_mc_wakeup_in_t;

typedef struct
{
  uint8 priDevice;
  uint8 secDevice;
  boolean rxdEn;
} rflm_tds_mc_sleep_in_t;


typedef struct
{
  boolean ifreqEn;
  boolean rxdEn;
  uint8   bufId;                // ifreqScriptID for ifreq and bufID for non-ifreq
  uint8   ppBufIdx;
  boolean isDlToIfreqTrans;

  boolean disableRxAgc;

  uint32  freqInKHz;

  uint32  rx0LmHandle;
  uint32  rx1LmHandle;

  uint8   linState;
  
  uint8   rx0GainState;
  uint8   rx1GainState;
  
  uint32  currSlotRefTime;      // Off-rx: pass two slot information. currSlot and nextSlot
  uint32  nextSlotRefTime;
  int32   refTimeShiftUsec;
  uint32  updatedRefTime;       // Only change the refTime is different than what FW pass is for TS6(DL)->TS0(IFREQ) transition
                                // where updatedRefTime = currSlotRefTime - refTimeShit
  int32   prxLnaRefTimeShiftUsec;
  int32   drxLnaRefTimeShiftUsec;
  int32   rxLnaLatchRefTimeShiftUsec;

  uint32  timeAdvanceRequired;   // Advance time required to call FED to complete RF script execution
  uint32  timeAdvanceShortBy;    // Short by amount of time. 0 means no late

  rflm_tds_single_script_info rxTuneScpt;
  rflm_tds_single_script_info rxPrxLnaScpt;
  rflm_tds_single_script_info rxDrxLnaScpt;
  rflm_tds_single_script_info rxRxLnaLatchScpt;
  rflm_tds_single_script_info rxEnableScpt;
  rflm_tds_single_script_info rxActivateScpt;

  uint32  timeProfileUsec;
}rflm_tds_log_off_rx_event_t;

typedef struct
{
  boolean ifreqEn;
  boolean rxdEn;
  uint8   bufId;                // ifreqScriptID for ifreq and bufID for non-ifreq
  uint8   ppBufIdx;

  uint32  freqInKHz;

  uint32  rx0LmHandle;
  uint32  rx1LmHandle;
 
  uint32  refTime;
  
  uint32 timeAdvanceRequired;   // Advance time required to call FED to complete RF script execution
  uint32 timeAdvanceShortBy;    // Short by amount of time. 0 means no late. 

  rflm_tds_single_script_info homeRestoreScpt;
  rflm_tds_single_script_info rxDeactivateScpt;

  uint32  timeProfileUsec;
}rflm_tds_log_rx_off_event_t;


typedef struct
{ 
  uint8   bufId;                
  uint8   ppBufIdx;
  
  boolean isFtmRFTestMode;
  int16   txAgcAdvanceUsec;
  
  boolean isTxPartialSlot;
  uint16  partialSlotLengthCx1;

  rflm_tds_slot_update_tx_type_e txSlotType;

  boolean disableTxAgc;
  
  uint32  freqInKHz;

  uint8   txLmHandle;
  
  uint32  refTime;
  
  uint32 timeAdvanceRequired;   // Advance time required to call FED to complete RF script execution
  uint32 timeAdvanceShortBy;    // Short by amount of time. 0 means no late. 

  rflm_tds_single_script_info txTuneScpt;
  rflm_tds_single_script_info txEnableScpt;
  rflm_tds_single_script_info txActivateScpt;
  rflm_tds_single_script_info txAgcScpt;

  uint32  timeProfileUsec;
}rflm_tds_log_off_tx_event_t;

typedef struct
{
  uint8   bufId;                
  uint8   ppBufIdx;

  boolean isTxPartialSlot;
  uint16  partialSlotLengthCx1;

  rflm_tds_slot_update_tx_type_e txSlotType;
  
  uint32  freqInKHz;

  uint8   txLmHandle;

  boolean toSchedTxRxOff;
  
  rflm_tds_slot_type_e slotType;
  boolean takeRefTimeFromTxCfg;
  boolean slotTypeWrong;
  uint32  refTime;

  uint32 timeAdvanceRequired;   // Advance time required to call FED to complete RF script execution
  uint32 timeAdvanceShortBy;    // Short by amount of time. 0 means no late. 

  rflm_tds_single_script_info txDeactivateScpt;

  uint32  timeProfileUsec;
}rflm_tds_log_tx_off_event_t;

typedef struct
{ 
  boolean ifreqEn;
  boolean rxdEn;
  uint8   rxBufId;                // ifreqScriptID for ifreq and bufID for non-ifreq
  uint8   txBufId;
  uint8   ppBufIdx;

  boolean isTxPartialSlot;
  uint16  partialSlotLengthCx1;

  rflm_tds_slot_update_tx_type_e txSlotType;
  
  uint32  freqInKHz;

  uint32  rx0LmHandle;
  uint32  rx1LmHandle;
  uint8   txLmHandle;

  boolean disableTxAgc;

  boolean isFtmRFTestMode;
  int32   txAgcAdvanceUsec;
  
  uint32  rxRefTime;
  uint32  txRefTime;

  uint32 timeAdvanceRequired;   // Advance time required to call FED to complete RF script execution
  uint32 timeAdvanceShortBy;    // Short by amount of time. 0 means no late. 

  rflm_tds_single_script_info txTuneScpt;
  rflm_tds_single_script_info homeRestoreScpt;
  rflm_tds_single_script_info txEnableScpt;
  rflm_tds_single_script_info rxDeactivateScpt;
  rflm_tds_single_script_info txActivateScpt;
  rflm_tds_single_script_info txAgcScpt;

  uint32  timeProfileUsec;
}rflm_tds_log_rx_tx_event_t;

typedef struct
{ 
  uint8   bufId;                
  uint8   ppBufIdx;
  boolean isFtmRFMode;

  boolean isTxPartialSlot;
  uint16  partialSlotLengthCx1;
  rflm_tds_slot_update_tx_type_e txSlotType;

  boolean headStartScheduled;
  int32   headStartStartRefTimeToRxStart;
  
  uint32  freqInKHz;

  uint8   txLmHandle;

  boolean disableTxAgc;
  
  uint32  refTime;
  
  boolean isFtmRFTestMode;
  int32   txAgcAdvanceUsec;

  uint32 timeAdvanceRequired;   // Advance time required to call FED to complete RF script execution
  uint32 timeAdvanceShortBy;    // Short by amount of time. 0 means no late. 

  rflm_tds_single_script_info txActivateScpt;
  rflm_tds_single_script_info txAgcScpt;

  uint32  timeProfileUsec;
}rflm_tds_log_tx_tx_event_t;

typedef struct
{
  boolean rxdEn;
  uint8   bufId;                // ifreqScriptID for ifreq and BufID for non-ifreq
  uint8   ppBufIdx;

  uint32  freqInKHz;

  uint8   linState;

  boolean disableRxAgc;

  rflm_tds_slot_update_tx_type_e txSlotType;
  boolean isTxPartialSlot;
  uint16  partialSlotLengthCx1;

  uint32  rx0LmHandle;
  uint32  rx1LmHandle;
  uint32  txLmHandle;
  
  uint8   rx0GainState;
  uint8   rx1GainState;
  
  uint32  refTime;
  int32   prxLnaRefTimeShiftUsec;
  int32   drxLnaRefTimeShiftUsec;
  
  uint32 timeAdvanceRequired;   // Advance time required to call FED to complete RF script execution
  uint32 timeAdvanceShortBy;    // Short by amount of time. 0 means no late

  boolean headStartEnabled;
  boolean headStartToSchedule;
  int32   headStartExecStartDeltaUsec;

  rflm_tds_single_script_info rxTuneScpt;
  rflm_tds_single_script_info rxPrxLnaScpt;
  rflm_tds_single_script_info rxDrxLnaScpt;
  rflm_tds_single_script_info rxRxLnaLatchScpt;
  rflm_tds_single_script_info rxEnableScpt;
  rflm_tds_single_script_info txDeactRxActivateScpt;

  uint32  timeProfileUsec;
}rflm_tds_log_tx_rx_event_t;

typedef struct
{  
  uint32  currSlotRefTime;      // Off-rx: pass two slot information. currSlot and nextSlot
  uint32  nextSlotRefTime;
  int32   refTimeShift;
  uint32  updatedRefTime;       // Only change the refTime is different than what FW pass is for TS6(DL)->TS0(IFREQ) transition
                                // where updatedRefTime = currSlotRefTime - refTimeShit

  uint32 timeAdvanceRequired;   // Advance time required to call FED to complete RF script execution
  uint32 timeAdvanceShortBy;    // Short by amount of time. 0 means no late. 

  rflm_tds_single_script_info scpt;
}rflm_tds_log_asdiv_event_t;


typedef struct
{  
  uint32  currSlotRefTime;      // Off-rx: pass two slot information. currSlot and nextSlot
  uint32  nextSlotRefTime;
  int32   refTimeShift;
  uint32  updatedRefTime;       // Only change the refTime is different than what FW pass is for TS6(DL)->TS0(IFREQ) transition
                                // where updatedRefTime = currSlotRefTime - refTimeShit

  uint32 timeAdvanceRequired;   // Advance time required to call FED to complete RF script execution
  uint32 timeAdvanceShortBy;    // Short by amount of time. 0 means no late. 

  rflm_tds_single_script_info scpt;
}rflm_tds_log_tuner_aol_event_t;

typedef struct
{  
  uint32  currSlotRefTime;      // Off-rx: pass two slot information. currSlot and nextSlot
  uint32  nextSlotRefTime;
  int32   refTimeShift;
  uint32  updatedRefTime;       // Only change the refTime is different than what FW pass is for TS6(DL)->TS0(IFREQ) transition
                                // where updatedRefTime = currSlotRefTime - refTimeShit

  uint32 timeAdvanceRequired;   // Advance time required to call FED to complete RF script execution
  uint32 timeAdvanceShortBy;    // Short by amount of time. 0 means no late. 

  rflm_tds_single_script_info scpt;
}rflm_tds_log_tuner_cl_event_t;

typedef struct
{  
  uint32  currSlotRefTime;      // Off-rx: pass two slot information. currSlot and nextSlot
  uint32  nextSlotRefTime;
  int32   refTimeShift;
  uint32  updatedRefTime;       // Only change the refTime is different than what FW pass is for TS6(DL)->TS0(IFREQ) transition
                                // where updatedRefTime = currSlotRefTime - refTimeShit

  uint32 timeAdvanceRequired;   // Advance time required to call FED to complete RF script execution
  uint32 timeAdvanceShortBy;    // Short by amount of time. 0 means no late. 

  rflm_tds_single_script_info scpt;
}rflm_tds_log_fbrx_event_t;



typedef struct
{
  boolean enableLog;
  uint16 currSfm;                              ///< current subframe #
  rflm_tds_slot_num_e currSlot;                ///< current slot #
  uint8 numofSlot;                             ///< Num of state info for this scheduling point, up to 3 events
  rflm_tds_slot_update_slot_cfg_t slot[RFLM_TDS_MAX_NUM_SLOT_PER_SCHEDULE_POINT];  
  uint8 numofEvent;

  rflm_tds_event_e eventName[RFLM_TDS_MAX_NUM_EVENT_PER_SCHEDULE_POINT];
  
  union
  {
    rflm_tds_log_off_rx_event_t offRx;
    rflm_tds_log_off_tx_event_t offTx;    
    rflm_tds_log_rx_off_event_t rxOff;
    rflm_tds_log_tx_off_event_t txOff;
    
    rflm_tds_log_tx_rx_event_t txRx;
    rflm_tds_log_rx_tx_event_t rxTx;
    rflm_tds_log_tx_tx_event_t txTx;

    rflm_tds_log_asdiv_event_t asdiv;
    rflm_tds_log_tuner_aol_event_t aol;
    rflm_tds_log_tuner_cl_event_t cl;
    rflm_tds_log_fbrx_event_t fbrx;
  } eventInfo[RFLM_TDS_MAX_NUM_EVENT_PER_SCHEDULE_POINT];


} rflm_tds_log_debug_slot_update_t;



extern boolean rflm_tds_enable_fbrx;
extern boolean rflm_tds_debug_txagc;
extern boolean rflm_tds_debug_pdet;
extern boolean rflm_tds_debug_therm;

extern uint32 jdetValue[2][7];
extern uint8 currSfIndex;
extern uint8 prevSfIndex;




/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif


/*==============================================================================
 rflm_tds_event_scheduler
 
  @brief
    API to schedule events
 
  @details
    API to schedule events
 
==============================================================================*/

rflm_err_t rflm_tds_event_scheduler
( 
  rflm_tds_slot_update_in_t* in,
  rflm_tds_slot_update_out_t* out
);


/*==============================================================================
 rflm_tds_exec_rf_script_dispatch
 
  @brief
    API to dispatch rf script
 
  @details
    API to dispatch rf script
 
==============================================================================*/

rflm_err_t rflm_tds_exec_rf_script_dispatch
( 
  rflm_tds_exec_rf_script_in_t  *in,
  rflm_tds_exec_rf_script_out_t *out 
);

void rflm_tds_mc_enter_mode
(
   rflm_tds_enter_mode_in_t* in, 
   rflm_tds_enter_mode_out_t* out
);

void rflm_tds_mc_exit_mode
(
   boolean waitForFwRelease
);

void rflm_tds_meas_enter_mode
(
   rflm_tds_enter_mode_in_t* in, 
   rflm_tds_enter_mode_out_t* out
);

void rflm_tds_meas_exit_mode(void);

rflm_err_t rflm_tds_schedule_off_rx
( 
  rflm_tds_slot_num_e currSlot,
  rflm_tds_slot_update_slot_cfg_t currSlotCfg,
  rflm_tds_slot_update_slot_cfg_t slotCfg,
  rflm_ccs_rf_task_callback_t lastEventCallback,
  void *callbackArg 
);

rflm_err_t rflm_tds_schedule_off_tx
( 
  rflm_tds_slot_num_e currSlot,
  rflm_tds_slot_update_slot_cfg_t slotCfg,
  rflm_tds_slot_update_out_t* out_config,
  uint8 out_config_idx,
  rflm_ccs_rf_task_callback_t lastEventCallback,
  void *callbackArg   
);

rflm_err_t rflm_tds_schedule_rx_off
( 
  rflm_tds_slot_num_e currSlot,
  rflm_tds_slot_update_slot_cfg_t slotCfg,
  rflm_ccs_rf_task_callback_t lastEventCallback,
  void *callbackArg 
);

rflm_err_t rflm_tds_schedule_tx_off
( 
  rflm_tds_slot_num_e currSlot,
  rflm_tds_slot_update_slot_cfg_t slotCfg,
  rflm_ccs_rf_task_callback_t lastEventCallback,
  void *callbackArg 
);

rflm_err_t rflm_tds_schedule_rx_tx
( 
  rflm_tds_slot_num_e currSlot,
  rflm_tds_slot_update_slot_cfg_t rxSlotCfg,
  rflm_tds_slot_update_slot_cfg_t txSlotCfg,
  rflm_tds_slot_update_out_t* out_config,
  uint8 out_config_idx
);

rflm_err_t rflm_tds_schedule_tx_rx
( 
  rflm_tds_slot_num_e currSlot,
  rflm_tds_slot_update_slot_cfg_t txSlotCfg,
  rflm_tds_slot_update_slot_cfg_t rxSlotCfg,
  boolean toSchedHs
);

rflm_err_t rflm_tds_schedule_tx_tx
( 
  rflm_tds_slot_num_e currSlot,
  rflm_tds_slot_update_slot_cfg_t slotCfg,
  rflm_tds_slot_update_out_t* out_config,
  uint8 out_config_idx
);

rflm_err_t rflm_tds_schedule_irat_scripts
( 
  rflm_tds_slot_num_e currSlot,
  rflm_tds_slot_update_slot_cfg_t slotCfg,
  rflm_tds_irat_script_e script_type,
  uint32 script_exec_bitmask,
  void (*rflm_tds_callback_func)(void *)
);

rflm_err_t rflm_tds_schedule_rf_internal_event
( 
  rflm_tds_schedule_rf_internal_event_in_t* in
);

rflm_err_t rflm_tds_update_handle_user_data
(
  rflm_tds_handle_user_data_update_t  update_type, 
  void* in,
  void* out,
  rflm_tds_rxlm_handle_user_data_t* rxUserDataPtr,
  rflm_tds_txlm_handle_user_data_t* txUserDataPtr
);

uint32 rflm_tds_mc_util_get_curr_tq_mem_usage(void);

rflm_err_t rflm_tds_mc_txagc_calc
( 
  rflm_tds_txagc_calc_in_t  *in,
  rflm_tds_txagc_calc_out_t *out 
);

uint32* rflm_tds_mc_txagc_update
(
  uint32 *ccsDestPtr,
  rflm_ccs_rf_task_callback_t callbackFunctionPtr,   
  uint32 actTime,
  rflm_drif_out_cfg_t* txagc_update_drif_out_cfg,
  boolean* txagc_fbrx_enable,
  int32* txagc_fbrx_trigger_offset,
  rflm_fbrx_dynamic_param_t* txagc_fbrx_dyn,
  rflm_fbrx_pwr_dependent_param_t* txagc_fbrx_pwr,
  rflm_tds_ccs_exec_single_task_info_t* taskInfoPtr
);

rflm_err_t rflm_tds_mc_txagc_get_mtpl
(
  rflm_tds_txagc_get_mtpl_in_t *in,
  rflm_tds_txagc_get_mtpl_out_t *out 
);

rflm_err_t rflm_tds_mc_txagc_get_mpr
(
  rflm_tds_txagc_get_mpr_in_t *in,
  rflm_tds_txagc_get_mpr_out_t *out 
);

void rflm_tds_tx_data_init(void);

void rflm_tds_set_tx_data
(
  int32 mtpldBm10,
  int32 sgtdsMtpldBm10,
  int32 minPwrdBm10,
  int32 iqGainApt,
  int32 iqGainEpt,
  int32 iqGainEt,
  int32 envScale
);

void rflm_tds_set_mtpl(int32 mtpldBm10, int32 sgtdsMtpldBm10);

void rflm_tds_set_mpr(int16* mpr_ptr );

void rflm_tds_mc_set_active_tx_slot(rflm_tds_slot_num_e activeTxSlot);

uint8 rflm_tds_mc_get_active_tx_slot(void);

rflm_err_t rflm_tds_mc_rxagc_calc
( 
  rflm_tds_rxagc_calc_in_t  *in,
  rflm_tds_rxagc_calc_out_t *out 
);

void rflm_tds_dm_tech_buffer_script_ptr_init(void);

void rflm_tds_dm_tech_buffer_script_ptr_deinit(void);

void* rflm_tds_dm_init
( 
  uint32 tech_buffer_size
);

void rflm_tds_dm_exit
( 
  void
);

rflm_rxagc_lna_state_e rflm_tds_mc_rxagc_calc_single_chain(rflm_handle_rx_t rxLMHandle,
	                                           rflm_tds_rx_mode_e mode,
	                                           rflm_tds_rx_pwr_est_e pwrEstType,
	                                           int32 dbFS256,
	                                           rflm_rxagc_lna_state_e currGainState,
	                                           uint8 ifreqScriptBufIndex,
                                             rflm_tds_rx_lin_state_e lin_state);

rflm_rxagc_lna_state_e rflm_tds_mc_rxagc_gain_update_algo( int32 rssi_dbFS256,
	                                                   rflm_rxagc_lna_state_e currGainState,
	                                                   int * rise_threshold,
	                                                   int * fall_threshold);

int8 rflm_tds_srch_cell_idx(rflm_handle_rx_t rxLMHandle,uint8 cell_id);

rflm_err_t rflm_tds_exec_asd_aol_cl_script(rflm_tds_schedule_script_e scptId);

void rflm_tds_pdet_scheduler(rflm_tds_slot_update_in_t* in);

void rflm_tds_exec_pdet_trigger_script(rflm_tds_slot_update_in_t* in);

void rflm_tds_exec_pdet_read_script(rflm_tds_slot_update_in_t* in);

void rflm_tds_jdet_scheduler(rflm_tds_slot_update_in_t* in);

void rflm_tds_exec_jdet_read_script(rflm_tds_slot_update_in_t* in);

void rflm_tds_therm_scheduler(rflm_tds_slot_update_in_t* in);

void rflm_tds_exec_therm_trigger_script(rflm_tds_slot_update_in_t* in);

void rflm_tds_exec_therm_read_script(rflm_tds_slot_update_in_t* in);

boolean rflm_tds_aol_cl_allocate_memory(uint32 handle_id, boolean isAol);

boolean rflm_tds_aol_cl_deallocate_memory(uint32 handle_id, boolean isAol);

extern void (*rflm_tds_irat_cleanup_event_fw_callback_func)(void *);

rflm_err_t rflm_tds_mc_fbrx_cal_update( rflm_handle_tx_t handle,
                                        rflm_tds_fbrx_cal_update_in_t  *in,
                                        rflm_tds_fbrx_cal_update_out_t *out );

void rflm_tds_abort_tuner_open_loop_update(rflm_handle_rx_t rxlmHandle);

void rflm_tds_config_tuner_close_loop_threshold(rflm_handle_rx_t rxlmHandle);

void rflm_tds_mc_fbrx_enable(boolean flag);

void rflm_tds_mc_sleep(rflm_tds_mc_sleep_in_t* in);

void rflm_tds_mc_wakeup(rflm_tds_mc_wakeup_in_t* in);

void rflm_tds_mc_issue_seq_preprocessing
(
  rflm_ccs_rf_event_header_task_type *evtPtr,
  rflm_ccs_rf_seq_t* ccsSeqPtr
);

void rflm_tds_mc_issue_seq_preprocessing_bho(void);

void rflm_tds_autopin_scheduler(rflm_tds_slot_update_in_t *in);

void rflm_tds_autopin_trigger_script(rflm_tds_slot_update_in_t* in);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RFLM_TDS_MC_H */
