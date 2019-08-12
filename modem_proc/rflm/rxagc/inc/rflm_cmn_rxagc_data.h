#ifndef RFLM_CMN_RXAGC_DATA_H
#define RFLM_CMN_RXAGC_DATA_H
/*
  @file
  rflm_cmn_rxagc_data.h

  @brief
  This file contains internal data structures of the RxAGC module

  @details
  All interfaces that need to be local to RxAGC moudule are present in this 
  file

  @addtogroup RFLM_RXAGC
  @{
*/

/*=============================================================================

                R F L M CMN DATA RXAGC   H E A D E R   F I L E 
       
Copyright (c) 2014  by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

    $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/rxagc/inc/rflm_cmn_rxagc_data.h#1 $

when       who    what, where, why
--------   ---    -------------------------------------------------------------- 
11/25/14   pl     Add support for multiple tasks for RXAGC
07/21/14   zhw    Cmn RxAGC memory reduction
07/09/14   sty    Use rflm_rxagc_cmn_lna_ccs_event_type instead of 
                  rfmeas_event_infor_type
05/29/14   sty    Added option to use events for LNA scripts
03/03/14   JJ     Added ccs_task_idx into cell_info_struct
11/14/13   sar    Added prototype for rflm_cmn_rxagc_i_log_api_calls.
09/25/13   sar    Added prototype for rflm_cmn_rxagc_i_is_cell_valid.
09/11/13   sty    Deleted unused API - rflm_cmn_rxagc_i_get_wb_ptr
09/09/13   sar    Added support for Common RxAgc On Demand update commands.
07/29/13   sty    Initial revision

==============================================================================*/
#include "rflm_rxagc_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  maximum number of tasks for RXAGC
*/
#define RFLM_RXAGC_MAX_TASK_NUM                                             4

/*----------------------------------------------------------------------------*/
/*!
  @brief
 
*/
typedef enum
{
  RFLM_CMN_RXAGC_BUFF_NOT_ALLOCATED = 0,
  RFLM_CMN_RXAGC_BUFF_BUILDING,
  RFLM_CMN_RXAGC_BUFF_READY,
  RFLM_CMN_RXAGC_BUFF_MARKED_FOR_DEALLOC,
} rflm_cmn_rxagc_buffer_state_e;

/*----------------------------------------------------------------------------*/
/*!
  @brief
 
*/
typedef struct cell_info_struct
{
  cell_id_t cell_id; ///< uniquely identifies a cell in a given WB buffer 

  uint8 valid_lna_states; ///< holds valid number of LNA states for the cell 

  __rx_static_info_t static_info; ///< holds all configuration-related data 

  rflm_rxagc_cmn_lna_ccs_event_type* lna_task_ptr[RFLM_CMN_RXAGC_MAX_LNA_STATES]; 
  ///< points to the task info that clients need to populate

  rflm_ccs_rf_seq_t* pre_compiled_tasks[RFLM_CMN_RXAGC_MAX_LNA_STATES];
  ///< points to the pre-compiled issue-seq CCS task that needs to be executed 
  ///for the LNA to be set to a given gain state

  boolean tasks_valid; ///< flag to indicate if the pre-compiled tasks are valid

  uint32 ccs_task_idx; ///< holds the taskIdx of the CCS task that was scheduled

  boolean use_events;
  ///< Input: Set to TRUE if CCS events need to be used instead of Issue Seqs 
  ///  for scheduling LNA gain changes

}__cell_info_t;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Data structure that models a Wideband buffer. 
 
*/
typedef struct
{
  rflm_tech_id_t owner_tech;        ///< tech that allocated and owns buffer

  uint8 num_valid_cells;            ///< num of cells allocated

  __cell_info_t* cell_info[RFLM_CMN_RXAGC_MAX_CELLS_PER_WB_BUFFER];
           ///< cells in the buffer.. only the first "num_valid_cells" 
           /// entries in the cell_info[] array are valid 

  rflm_cmn_rxagc_buffer_state_e buffer_state; ///< current state of buffer - 
  /// used by the RxAGC module to decide if config steps on the WB buffer 
  /// have completed.

  /* Functionality not need for now. Stub out to save memory */
//  rflm_rxagc_cmn_on_demand_t 
//                          on_demand_script[RFLM_CMN_RXAGC_MAX_ON_DEMAND_SCRIPT];
//                                      ///< Array to hold On Demand Scripts.

  uint32* tech_data; 
  ///< this is a chunk of memory of size - RFLM_CMN_RXAGC_TECH_DATA_SIZE_BYTES 
  /// Techs may use this area for storing tech-specific data. The 
  /// client tech is completely responsible for this memory
   
  const uint32 tech_data_end_marker; 
  ///< marker to prevent clients from overwriting beyond the allowed size of
  /// tech_data

  /*------------------- internal ---------------------------------------------*/
    rflm_ccs_rf_task_t task_list[RFLM_RXAGC_MAX_TASK_NUM]; 
  ///< temp task list - needed to create CCS tasks

  rflm_ccs_rf_seq_scratch_t scratch_area; ///< temp scratch area - for CCS tasks

}__wb_buffer_info_t;

/*----------------------------------------------------------------------------*/
void 
rflm_cmn_rxagc_i_init_module
(
  void
);

/*----------------------------------------------------------------------------*/
rflm_rxagc_event_template_t*
rflm_cmn_rxagc_i_allocate_new_wb_buffer
(
  const rflm_tech_id_t tech
);

/*----------------------------------------------------------------------------*/
rflm_err_t
rflm_cmn_rxagc_i_dealloc_wb_buffer
(
  rflm_rxagc_event_template_t* rxagc_ptr
);

/*----------------------------------------------------------------------------*/
rflm_err_t
rflm_cmn_rxagc_i_add_cells
(
  __wb_buffer_info_t* wb_ptr,
  const rflm_rxagc_cmn_add_delete_cells_t* cells_info
);

/*----------------------------------------------------------------------------*/
rflm_err_t
rflm_cmn_rxagc_i_delete_cells
(
  __wb_buffer_info_t* wb_ptr,
  const rflm_rxagc_cmn_add_delete_cells_t* cell_info
);

/*----------------------------------------------------------------------------*/
rflm_err_t
rflm_cmn_rxagc_i_create_pre_comp_tasks
(
  __wb_buffer_info_t* wb_ptr,
  const rflm_rxagc_cmn_create_lna_tasks_t* lna_data
);

/*----------------------------------------------------------------------------*/
rflm_err_t
rflm_cmn_rxagc_i_update_static_info
(
  __wb_buffer_info_t* wb_buffer_ptr,
  const rflm_cmn_rxagc_static_data_t* rxagc_data
);

/*----------------------------------------------------------------------------*/
rflm_err_t
rflm_cmn_rxagc_i_commit_wb_buffer
(
  __wb_buffer_info_t* wb_buffer_ptr
);

/*----------------------------------------------------------------------------*/
rflm_err_t
rflm_cmn_rxagc_i_get_lna_ptrs
(
  __wb_buffer_info_t* wb_ptr,
  rflm_rxagc_cmn_lna_ptrs_t* lna_data
);

/*----------------------------------------------------------------------------*/
rflm_err_t
rflm_cmn_rxagc_i_get_cell_data
(
  __wb_buffer_info_t* wb_ptr,
  rflm_cmn_rxagc_static_data_t* cell_data
);

/* Functionality not need for now. Stub out to save memory */
/*----------------------------------------------------------------------------*/
//rflm_err_t
//rflm_cmn_rxagc_i_populate_on_demand_script
//(
//  __wb_buffer_info_t* wb_ptr,
//  const rflm_rxagc_cmn_on_demand_t* on_demand_script_data
//);

/*---------------------------------------------------------------------------*/
rflm_err_t
rflm_cmn_rxagc_i_schedule_lna_task
(
  __wb_buffer_info_t* wb_ptr,
  rflm_rxagc_cmn_schedule_task_t* task_inputs
);

/*----------------------------------------------------------------------------*/
__cell_info_t*
rflm_cmn_rxagc_i_test_api_get_cell_ptr
(
  __wb_buffer_info_t* wb_ptr,
  cell_id_t cell_id_to_match
);

/*----------------------------------------------------------------------------*/
uint32*
rflm_cmn_rxagc_i_get_user_data_ptr
(
  __wb_buffer_info_t* wb_ptr
);

/*----------------------------------------------------------------------------*/
__cell_info_t*
rflm_cmn_rxagc_i_is_cell_valid
(
  __wb_buffer_info_t* wb_ptr,
  cell_id_t cell_id_to_match
);

/*----------------------------------------------------------------------------*/
void 
rflm_cmn_rxagc_i_log_api_calls(
  void* func_ptr,
  __wb_buffer_info_t* ptr_to_wb_buffer,
  rflm_cmn_rxagc_cmd_e cmd,
  void* payload
);
 
#ifdef __cplusplus
}
#endif

/*!   @} */

#endif /* #ifndef RFLM_CMN_RXAGC_DATA_H */
