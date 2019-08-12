#ifndef RFGSM_CORE_LOG_H
#define RFGSM_CORE_LOG_H
/*===========================================================================

           GSM  FTM   LOGGING    H E A D E R    F I L E

DESCRIPTION
  This file contains declarations associated with the GSM FTM logging.

REFERENCES
  None

Copyright (c) 2003-2007, 2009, 2010, 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2014, 2015, 2016            by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/* <EJECT> */
/*===========================================================================
Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/rfgsm_core_log.h#1 $
$DateTime: 2016/12/13 07:59:51 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/23/16   par     Add MC logging buffer for failing sequences
11/25/15   bp      FR25072: for device wakeup, vote for lowest/first supported band   
11/18/14   sw/sc   Deprecate legacy state machines and move cmd buff out of rfa    
02/10/12   sb      Changes to support FTM logging for GSM
06/03/12   sb      Included correct header file to enableGSM FTM logging
============================================================================*/
#include "rfa_variation.h"
#include "comdef.h"
#include "customer.h"
#include "target.h"
#include "queue.h"
#include "msg.h"
#include "rfgsm_core_types.h"
#include "rfcom.h"

#ifdef FTM_HAS_LOGGING
#include "ftm_log.h"
#endif




/*===========================================================================*/
/*                                                                           */
/*                              DEFINTIONS                                   */
/*                                                                           */
/*===========================================================================*/
#define RFGSM_LOG_BUFFERS   10

#define MAX_NUM_SLOTS 8

#define RFGSM_CMD_NUM_BUFFS       500
#define RFGSM_PROF_NUM_BUFFS      100
#define RFGSM_PROF_HIST_SIZE      10
#define PROF_LOG2_AVG_SIZE        2
#define RFGSM_NUM_MC_ERR_ENTRIES 50  

/*---------------------------------------------------------------------------*/
typedef enum
{
  RFGSM_API_DEFAULT,

  /* RF State Machine Transitions */
  RFGSM_FSM_TRANSITION,
  RFGSM_FSM_ILLEGAL_TRANSITION,

  /* RF MC commands */
  RFGSM_MC_SET_BAND,
  RFGSM_MC_SET_TX_BAND,
  RFGSM_MC_TUNE_TO_CHAN,
  RFGSM_MC_SET_RX_GAIN,
  RFGSM_MC_SET_AMAM_PROFILE,
  RFGSM_MC_SETUP_RX_BURST,
  RFGSM_MC_POPULATE_RX_DEVICE_BUFF,
  RFGSM_MC_SETUP_TX_BURST,
  RFGSM_MC_COMPUTE_RX_POWER,
  RFGSM_MC_SETUP_STOP_ACQ,
  RFGSM_MC_UPDATE_FREQ_ERROR,
  RFGSM_MC_ENABLE_TX,
  RFGSM_MC_DISABLE_TX,
  RFGSM_MC_ENTER_MODE,
  RFGSM_MC_MEAS_IRAT_INFO_GET,
  RFGSM_MC_MEAS_ENTER,
  RFGSM_MC_MEAS_PRE_CONFIG,
  RFGSM_MC_MEAS_BUILD_SCRIPTS,
  RFGSM_MC_BUILD_RESET_SCRIPTS,
  RFGSM_MC_MEAS_EXIT,
  RFGSM_MC_THERM_READ_RSP,
  RFGSM_MC_DEVICE_READ_RSP,
  RFGSM_MC_EXIT_MODE,
  RFGSM_MC_SLEEP,
  RFGSM_MC_WAKEUP,

  /* RF task commands */
  RFA_RF_GSM_TX_BURST_REQUEST_START,
  RFA_RF_GSM_TX_BURST_REQUEST_STOP,
  RFA_RF_GSM_RX_BURST_REQUEST_START,
  RFA_RF_GSM_RX_BURST_REQUEST_STOP,
  RFA_RF_GSM_DRX_BURST_REQUEST_START,
  RFA_RF_GSM_DRX_BURST_REQUEST_STOP,
  RFA_RF_GSM_SET_TX_BAND_REQUEST_START,
  RFA_RF_GSM_SET_TX_BAND_REQUEST_STOP,
  RFA_RF_GSM_IDLE_FRAME_PROCESSING_REQUEST_START,
  RFA_RF_GSM_IDLE_FRAME_PROCESSING_REQUEST_STOP,
  RFA_RF_GSM_ENTER_MODE_REQUEST_START,
  RFA_RF_GSM_ENTER_MODE_REQUEST_STOP,
  RFA_RF_GSM_RX_ENTER_MODE_REQUEST_START,
  RFA_RF_GSM_RX_ENTER_MODE_REQUEST_STOP,
  RFA_RF_GSM_TX_ENABLE_REQUEST_START,
  RFA_RF_GSM_TX_ENABLE_REQUEST_STOP,
  RFA_RF_GSM_TX_DISABLE_REQUEST_START,
  RFA_RF_GSM_TX_DISABLE_REQUEST_STOP,
  RFA_RF_GSM_WAKEUP_REQUEST_START,
  RFA_RF_GSM_WAKEUP_REQUEST_STOP,
  RFA_RF_GSM_RX_WAKEUP_REQUEST_START,
  RFA_RF_GSM_RX_WAKEUP_REQUEST_STOP,
  RFA_RF_GSM_SLEEP_REQUEST_START,
  RFA_RF_GSM_SLEEP_REQUEST_STOP,
  RFA_RF_GSM_RX_SLEEP_REQUEST_START,
  RFA_RF_GSM_RX_SLEEP_REQUEST_STOP,
  RFA_RF_GSM_EXIT_MODE_REQUEST_START,
  RFA_RF_GSM_EXIT_MODE_REQUEST_STOP,
  RFA_RF_GSM_RX_EXIT_MODE_REQUEST_START,
  RFA_RF_GSM_RX_EXIT_MODE_REQUEST_STOP,
  RFA_RF_GSM_THERM_READ_RESPONSE,

#ifdef FEATURE_RF_ASDIV
  RFA_RF_GSM_ASD_READ_RESPONSE_START,
  RFA_RF_GSM_ASD_READ_RESPONSE_STOP,
#endif

  RFA_RF_GSM_SET_SAR_LIMIT_INDICATION_START,
  RFA_RF_GSM_SET_SAR_LIMIT_INDICATION_STOP,
  RFA_RF_GSM_CM_ENTER_REQUEST_START,
  RFA_RF_GSM_CM_ENTER_REQUEST_STOP,
  RFA_RF_GSM_CM_EXIT_REQUEST_START,
  RFA_RF_GSM_CM_EXIT_REQUEST_STOP,
  RFA_RF_GSM_PROCESS_BURST_METRICS_REQUEST,
  RFA_RF_GSM_DBG_START_IP2_CAL_REQUEST_START,
  RFA_RF_GSM_DBG_START_IP2_CAL_REQUEST_STOP,
  RFA_RF_GSM_DBG_INIT_MSM_IP2_CAL_REQUEST_START,
  RFA_RF_GSM_DBG_INIT_MSM_IP2_CAL_REQUEST_STOP,

  /*CCS event builder*/
  RFGSM_MDSP_CCS_TX_START_SBI_EVENT_BUILDER,
  RFGSM_MDSP_CCS_TX_TRANS_SBI_EVENT_BUILDER,
  RFGSM_MDSP_CCS_TX_STOP_SBI_EVENT_BUILDER,
  RFGSM_MDSP_CCS_TX_START_NONSBI_EVENT_BUILDER,
  RFGSM_MDSP_CCS_TX_TRANS_NONSBI_EVENT_BUILDER,
  RFGSM_MDSP_CCS_TX_STOP_NONSBI_EVENT_BUILDER,
  RFGSM_MDSP_CCS_RX_START_SBI_EVENT_BUILDER,
  RFGSM_MDSP_CCS_RX_STOP_SBI_EVENT_BUILDER,
  RFGSM_MDSP_CCS_RX_START_NONSBI_EVENT_BUILDER,
  RFGSM_MDSP_CCS_RX_STOP_NONSBI_EVENT_BUILDER,
  RFGSM_MC_BUILD_RX_BURST_CCS_EVENT,

  /*GL1_RF_MSG_ROUTER*/
  GL1_RF_MSG_R_RF_TASK_START,
  GL1_RF_MSG_R_RF_TASK_END,
  RF_GL1_MSG_CNF_SENT_SUCCESSFUL,
  RF_GL1_MSG_CNF_NOT_REQUIRED,

  /*RF_APPS RF MSG ROUTER*/
  RF_RFAPPS_MSG_CNF_SENT_SUCCESSFUL,
  RF_RFAPPS_MSG_CNF_NOT_REQUIRED,

  /*RF_APPS->RF_TASK Temp comp update*/
  RFA_RF_GSM_TEMP_COMP_UPDATE_REQUEST_START,
  RFA_RF_GSM_TEMP_COMP_UPDATE_REQUEST_STOP,

#ifdef FEATURE_RF_ASDIV
  /*GL1->RF_TASK Set Antenna*/
  RFA_RF_GSM_SET_ANTENNA_REQUEST_START,
  RFA_RF_GSM_SET_ANTENNA_REQUEST_STOP,
#endif

  RFGSM_MC_DBG_START_IP2_CAL_REQUEST,
  RFGSM_MC_DBG_INIT_MSM_IP2_CAL_REQUEST,

  /* Core Layer for sleep and wakeup*/
  RFGSM_CORE_RX_PWR_WAKEUP,
  RFGSM_CORE_RX_PWR_SLEEP,
  RFGSM_CORE_TX_PWR_WAKEUP,
  RFGSM_CORE_TX_PWR_SLEEP,

  RFGSM_MAX_API = 0xFFFF
} rfgsm_api_type;

typedef enum
{
  RFGSM_MC_RET_ERROR,                   //RFGSM_MC_ERROR
  RFGSM_MC_RET_SUCCESS,                 //RFGSM_MC_SUCCESS
  RFGSM_MC_RET_INVALID_PARAM,           //RFGSM_MC_INVALID_PARAM
  RFGSM_MC_RET_CORE_API_FAILURE,        //RFGSM_MC_CORE_API_FAILURE
  RFGSM_MC_RET_INVALID_MC_STATE,        //RFGSM_MC_INVALID_MC_STATE
  RFGSM_MC_RET_ILLEGAL_STATE_TRANSITION,//RFGSM_MC_ILLEGAL_STATE_TRANSITION
  RFGSM_MC_RET_STATE_ERROR,             //RFGSM_MC_STATE_ERROR
  RFGSM_MC_RET_STATUS_MAX,              //RFGSM_MC_STATUS_MAX
}rfgsm_mc_ret_status_type;

/*---------------------------------------------------------------------------*/
/* Define execution times per subscription? */
typedef struct
{
  rf_task_num_type rf_task;
  uint32           pcycles;
}rfgsm_prof_data_type;

/*---------------------------------------------------------------------------*/
typedef struct
{
  rfgsm_api_type            api;        /* For debugging */
  uint32                    api_cnt;    /* Number of times function is hit */
  rfgsm_prof_data_type      worst_case; /* Worst Case Execution Time */
  rfgsm_prof_data_type      best_case;  /* Best Case Execution Time */
  rfgsm_prof_data_type      avg_case;   /* Average Case Execution Time */
  uint8                     index;      /* Index into pcycles buffer */
  uint32                    pcycles[RFGSM_PROF_HIST_SIZE];
}rfgsm_prof_type;

/*---------------------------------------------------------------------------*/
typedef struct
{
  rfgsm_api_type     rfm_api;
  uint8              state;
  uint8              rf_task;
  uint16             cmd_proc_start_qs;
  uint32             args[2];
  uint32             current_frame_number;
  uint32             start_time_ustmr;
  int32              time_in_func_us;
  unsigned long long start_time_pcycles;
  int32              time_in_func_pcycles;
}rfgsm_cmd_type;

/*---------------------------------------------------------------------------*/
typedef struct
{
  uint16          index;
  rfgsm_cmd_type  entry[RFGSM_CMD_NUM_BUFFS];
  rfgsm_prof_type time_prof[RFGSM_PROF_NUM_BUFFS];
} rfgsm_cmd_buffer_type;

typedef struct
{ 
  rfgsm_api_type           rfm_api;
  uint8                    rf_task;
  rfgsm_mc_ret_status_type rfgsm_mc_status;
  uint32                   current_frame_number;
  uint32                   args[2];
}rfgsm_mc_err_log_type;

typedef struct
{
  uint8                  index;
  rfgsm_mc_err_log_type  entry[RFGSM_NUM_MC_ERR_ENTRIES];
}rfgsm_mc_err_buffer_type;

/*****************************************************************************/
/*************************** PROFILING TYPES/MACROS **************************/
/*****************************************************************************/

/******************************* STORE PCYCLES *******************************/
/* For DEBUG purposes  */
#define RFGSM_PROF_DATA(h, x, f)                      \
{                                                     \
  ASSERT(x<RFGSM_PROF_NUM_BUFFS);                     \
  h.api = (rfgsm_api_type) x;                         \
  h.api_cnt++;                                        \
                                                      \
  /* Store the pcycles */                             \
  h.pcycles[h.index++] = f;                           \
  h.index %= RFGSM_PROF_HIST_SIZE;                    \
}

/*********************** STORE BEST CASE EXECUTION TIME***********************/
#ifndef MIN
  #define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif /* MIN */

#define RFGSM_PROF_BEST_CASE(h, f, t)                 \
{                                                     \
  /* Best case execution time */                      \
  h.best_case.rf_task = (rf_task_num_type) t;         \
  if (!h.best_case.pcycles)       /* First Time */    \
  {                                                   \
    h.best_case.pcycles = f;                          \
  }                                                   \
  h.best_case.pcycles = MIN(f, h.best_case.pcycles);  \
}

/*********************** STORE WORST CASE EXECUTION TIME**********************/
#ifndef MAX
  #define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif /* MAX */

#define RFGSM_PROF_WORST_CASE(h, f, t)                \
{                                                     \
  /* Worst case execution time */                     \
  h.worst_case.rf_task = (rf_task_num_type) t;        \
  h.worst_case.pcycles = MAX(f, h.worst_case.pcycles);\
}

/********************** STORE AVERAGE CASE EXECUTION TIME*********************/
/* Running average */
#define AVERAGE(p1, p2) \
  (((p1) > (p2)) ? ((p2)+(((p1)-(p2))>>PROF_LOG2_AVG_SIZE)) : ((p2)-(((p2)-(p1))>>PROF_LOG2_AVG_SIZE)))

#define RFGSM_PROF_AVG_CASE(h, f, t)                  \
{                                                     \
  /* Average case execution time */                   \
  h.avg_case.rf_task = (rf_task_num_type) t;          \
  if (f) {                                            \
    h.avg_case.pcycles = AVERAGE(f, h.avg_case.pcycles);\
  }                                                   \
}

/******************************** DEBUG MACRO ********************************/
#define RFGSM_DEBUG_EN_Q(handle, api, task, start_qs, arg1, arg2, frame_num, start_ustmr, dur_us, start_pcycles, dur_pcycles) \
{ \
  handle->cmd_buffs.index++; \
  handle->cmd_buffs.index = handle->cmd_buffs.index % RFGSM_CMD_NUM_BUFFS;                      \
  handle->cmd_buffs.entry[handle->cmd_buffs.index].rfm_api              = (rfgsm_api_type)(api);\
  handle->cmd_buffs.entry[handle->cmd_buffs.index].state                = handle->curr_state; \
  handle->cmd_buffs.entry[handle->cmd_buffs.index].rf_task              = (rf_task_num_type)(task); \
  handle->cmd_buffs.entry[handle->cmd_buffs.index].cmd_proc_start_qs    = (start_qs);           \
  handle->cmd_buffs.entry[handle->cmd_buffs.index].args[0]              = (uint32)(arg1);       \
  handle->cmd_buffs.entry[handle->cmd_buffs.index].args[1]              = (uint32)(arg2);       \
  handle->cmd_buffs.entry[handle->cmd_buffs.index].current_frame_number = (int32)(frame_num);   \
  handle->cmd_buffs.entry[handle->cmd_buffs.index].start_time_ustmr     = (int32)(start_ustmr); \
  handle->cmd_buffs.entry[handle->cmd_buffs.index].time_in_func_us      = (uint32)(dur_us);     \
  handle->cmd_buffs.entry[handle->cmd_buffs.index].start_time_pcycles   = (unsigned long long)start_pcycles; \
  handle->cmd_buffs.entry[handle->cmd_buffs.index].time_in_func_pcycles = (int32)(dur_pcycles); \
  RFGSM_PROF_DATA( handle->cmd_buffs.time_prof[api], api, dur_pcycles );                        \
  RFGSM_PROF_AVG_CASE( handle->cmd_buffs.time_prof[api], dur_pcycles, task );                   \
  RFGSM_PROF_BEST_CASE( handle->cmd_buffs.time_prof[api], dur_pcycles, task );                  \
  RFGSM_PROF_WORST_CASE( handle->cmd_buffs.time_prof[api], dur_pcycles, task );                 \
}

/******************************** DEBUG MACRO FOR MC ERROR LOGGING ********************************/
#define RFGSM_DEBUG_MC_ERR_LOG_Q(handle, api, task, mc_status, frame_num, arg1, arg2)             \
{                                                                                                 \
  if (TRUE == rfgsm_mc_err_logging_enable)                                                        \
  {                                                                                               \
    handle->index++;                                                                              \
    handle->index                                      = handle->index % RFGSM_NUM_MC_ERR_ENTRIES;\
    handle->entry[handle->index].rfm_api               = (rfgsm_api_type)(api);                   \
    handle->entry[handle->index].rf_task               = (rf_task_num_type)(task);                \
    handle->entry[handle->index].rfgsm_mc_status       = (rfgsm_mc_ret_status_type)(mc_status);   \
    handle->entry[handle->index].current_frame_number  = (uint32)(frame_num);                     \
    handle->entry[handle->index].args[0]               = (uint32)(arg1);                          \
    handle->entry[handle->index].args[1]               = (uint32)(arg2);                          \
  }                                                                                               \
}

#ifdef FTM_HAS_LOGGING
/*---------------------------------------------------------------------------*/
// Rf to FTM_TASK message structure
typedef struct
{
  q_link_type            link;
  rfgsm_core_tx_log_type msg;
} rfgsm_core_log_data;

/*---------------------------------------------------------------------------*/
typedef struct
{
  q_type              rfgsm_log_q;
  q_type              rfgsm_log_free_q;
  rfgsm_core_log_data rfgsm_buffers[ RFGSM_LOG_BUFFERS];
} rfgsm_log_info_type;

/*---------------------------------------------------------------------------*/
typedef PACK(struct)
{
  ftm_log_gen_type       header;
  rfgsm_core_tx_log_type data;
} rfgsm_core_log_type;

/*---------------------------------------------------------------------------*/
/*!
  @details
  This function initializes resources for the FTM task queue
  Rx burst.
  @param void
  @retval void
*/
void rfgsm_core_log_q_init( void);

/*---------------------------------------------------------------------------*/
/*!
  @details
  This function "Allocates" a buffer for RFGSM to send messages to FTM and returns a pointer for the free buffer.
  @param void
  @retval void*
*/
void *rfgsm_core_log_get_buf( void);

/*---------------------------------------------------------------------------*/
/*!
  @details
  This function sends a populated buffer (message structure) to ftm task.
  @param void
  @retval void*
*/
void rfgsm_core_log_send_to_ftm_task( rfm_device_enum_type rfm_dev );

/*---------------------------------------------------------------------------*/
/*!
  @details
  This function dequeues items (head first) from the FTM task buffer and populates log packet before sending it to DiaG.
  @param void
  @retval void*
*/
void rfgsm_core_log_handle_q_from_rf( void );

/*---------------------------------------------------------------------------*/
#endif /* FTM_HAS_LOGGING */
#endif 

