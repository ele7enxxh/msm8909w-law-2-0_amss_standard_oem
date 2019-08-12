#ifndef RFLM_RXAGC_API_H
#define RFLM_RXAGC_API_H
/*
  @file
  rflm_rxagc_api.h

  @brief
  This file contains RF FED RxAGC definitions, enumerations and prototypes that
  are exposed to external modules, such as RF SW and FW.

  @addtogroup RFLM_RXAGC
  @{
*/

/*=============================================================================

                R F  F E D RXAGC   H E A D E R   F I L E 
       
Copyright (c) 2014  by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

    This section contains comments describing changes made to this file.
    Notice that changes are listed in reverse chronological order.

    $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/rxagc/rflm_rxagc_api.h#1 $

when       who    what, where, why
--------   ---    -------------------------------------------------------------
07/09/14   sty    Changes made to use rflm_rxagc_cmn_lna_ccs_event_type 
                  instead of rfmeas_event_infor_type
05/29/14   sty    Added option to use events for LNA scripts
03/03/14   JJ     Added task_index to rflm_rxagc_cmn_schedule_task_t
02/07/14   JJ     Added RFLM_CMN_RXAGC_LNA_SCRIPT_SIZE_BYTES back
02/03/14   JJ     Removed a redundant variable 
12/04/13   vbh    Added enum for LNA indices 
11/21/13   sty    Added new member seq_size_in_bytes in 
                  rflm_rxagc_cmn_schedule_task_t
11/20/13   nsh    Return the number of valid cells/Add nb threshold
10/31/13   sar    Added support for RFLM_CMN_RXAGC_IS_CELL_VALID.
10/15/13   spa     Added calback and callback params for LNA event
10/08/13   spa    Added carrier-NB map to RxAGC static info
09/09/13   sar    Added support for Common RxAgc On Demand update commands.
09/04/13   gy     Added definition for rflm_rxagc_event_template_t
08/08/13   sty    Initial revision

===========================================================================*/

#include "rflm_api_cmn.h"
#include "rflm_ccs_irat_intf.h" /* for rfmeas_event_infor_type */

#ifdef __cplusplus
extern "C" {
#endif
/*----------------------------------------------------------------------------*/
/*!
  @brief
 
*/
#define RFLM_CMN_RXAGC_INVALID_CELL_ID                                    0xFFFF


/*----------------------------------------------------------------------------*/
/*!
  @brief
  typedef for Cell_ID
*/
typedef uint32 cell_id_t;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Each WB buffer can have a max of 11 cells - this is the worst-case scenario 
 (for TDS, where i-freq PRx/DRx may have upto 11 cells)
*/
#define RFLM_CMN_RXAGC_MAX_CELLS_PER_WB_BUFFER                                11

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Each WB buffer can have a max of 10 on demand updates/cmds 
 */
#define RFLM_CMN_RXAGC_MAX_ON_DEMAND_SCRIPT                                   10

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Max allowable tasks in the LNA event
 */
#define RFLM_CMN_RXAGC_MAX_LNA_TASKS                                           4

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  LNA SSBI, RFFE, GRFC numbers and event size
 */
#define RFLM_CMN_RXAGC_LNA_SCRIPT_NUM_OF_SSBI                                   5
#define RFLM_CMN_RXAGC_LNA_SCRIPT_NUM_OF_RFFE                                   5
 
/* data buffer size is 8 words =  32 bytes */
#define RFLM_CMN_RXAGC_LNA_SCRIPT_DATA_BUF_SIZE_WORDS     ((RFLM_CMN_RXAGC_LNA_SCRIPT_DATA_BUF_SIZE_BYTE + 3)>>2)
#define RFLM_CMN_RXAGC_LNA_SCRIPT_DATA_BUF_SIZE_BYTE      (RFLM_CMN_RXAGC_LNA_SCRIPT_NUM_OF_SSBI *4 + \
                                                           RFLM_CMN_RXAGC_LNA_SCRIPT_NUM_OF_RFFE *2)

/*----------------------------------------------------------------------------*/ 
/*! 
  @brief
  various commands supported by RxAGC module 
*/
typedef enum
{

  RFLM_CMN_RXAGC_ADD_CELLS=0,      ///< to add cells to a pre-existing WB buffer
  RFLM_CMN_RXAGC_DELETE_CELLS,          ///< to delete cells from a WB buffer
  RFLM_CMN_RXAGC_UPDATE_CELL_STATIC_INFO, ///< update info from NS for a cell
  RFLM_CMN_RXAGC_GET_CELL_STATIC_INFO, ///< cmd to return a copy of static info
  RFLM_CMN_RXAGC_SET_LNA_SCRIPTS_PTR,  ///< get pointer to CCS tasks for LNA                                      
  RFLM_CMN_RXAGC_SET_LNA_SCRIPTS_DATA = RFLM_CMN_RXAGC_SET_LNA_SCRIPTS_PTR,  
  RFLM_CMN_RXAGC_GET_LNA_SCRIPTS_PTR = RFLM_CMN_RXAGC_SET_LNA_SCRIPTS_PTR, 
                                       ///< get pointer to CCS tasks for LNA
  RFLM_CMN_RXAGC_CREATE_LNA_TASKS,     ///< create pre-compiled tasks
  RFLM_CMN_RXAGC_SCHEDULE_LNA_GAIN,    ///< schedules a CCS task for given LNA 
                                       ///< gain state
  RFLM_CMN_RXAGC_LATCH_WB_BUF_INFO,    ///< indication to RxAGC module that all 
                                       ///< info for WB buffer have been updated
  RFLM_CMN_RXAGC_GET_TECH_DATA_PTR,    ///< command to get a pointer to a 
                                       /// writeable chunk of memory

  RFLM_CMN_RXAGC_POPULATE_ON_DEMAND_SCRIPTS,
                      ///< used by DM when on-demand scripts are to be populated

  RFLM_CMN_RXAGC_IS_CELL_VALID,   
                      ///< used by cliente to query whether a cell with the id 
                      /// exists in RXAGC Module.
  RFLM_CMN_RXAGC_GET_NUM_VALID_CELLS
                      ///< Return the current valid cells
} rflm_cmn_rxagc_cmd_e;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Script names for Common RxAgc On Demand update commands. 
*/
typedef enum
{

  RFLM_CMN_RXAGC_RX_ON_SCRIPT = 0, ///< Switched AGC to RX ON.
  RFLM_CMN_RXAGC_RX_OFF_SCRIPT,    ///< Switched AGC to RX OFF.
  RFLM_CMN_RXAGC_TX_ON_SCRIPT,     ///< Switched AGC to TX ON.
  RFLM_CMN_RXAGC_TX_OFF_SCRIPT,    ///< Switched AGC to TX OFF.
  RFLM_CMN_RXAGC_INVALID_SCRIPT    ///< Upper bound for script names
                                              
} rflm_cmn_rxagc_script_name_e;


/*----------------------------------------------------------------------------*/
/*!
  @brief enum for LNA states
*/
typedef enum
{
  RFLM_RXAGC_LNA_STATE0 = 0,        ///< highest gain
  RFLM_RXAGC_LNA_STATE1,
  RFLM_RXAGC_LNA_STATE2,
  RFLM_RXAGC_LNA_STATE3,            
  RFLM_RXAGC_LNA_STATE4,            
  RFLM_RXAGC_LNA_STATE5,            ///< lowest gain             
  RFLM_CMN_RXAGC_MAX_LNA_STATES,   
} rflm_rxagc_lna_state_e;

/*----------------------------------------------------------------------------*/

/*!
  @brief enum for LNA indices
*/
typedef enum
{
  RFLM_RXAGC_LNA_INDEX0 = 0,        ///< device 0 (1c Prx)
  RFLM_RXAGC_LNA_INDEX1,
  RFLM_RXAGC_LNA_INDEX2,
  RFLM_RXAGC_LNA_INDEX3,            ///< device 3 (2c Drx)
  RFLM_CMN_RXAGC_MAX_LNA_INDICES,   
} rflm_rxagc_lna_index_e;

/*----------------------------------------------------------------------------*/
/*!@brief
  Enumeration of LNA Gain Steps
*/
typedef enum
{
  RFLM_RXAGC_LNA_GAIN_STEP_0_TO_1,
  /*!< Gain step from Gain state 0 to 1 */
  RFLM_RXAGC_LNA_GAIN_STEP_1_TO_2,
  /*!< Gain step from Gain state 1 to 2 */
  RFLM_RXAGC_LNA_GAIN_STEP_2_TO_3,
  /*!< Gain step from Gain state 2 to 3 */
  RFLM_RXAGC_LNA_GAIN_STEP_3_TO_4,
  /*!< Gain step from Gain state 3 to 4 */
  RFLM_RXAGC_LNA_GAIN_STEP_4_TO_5,
  /*!< Gain step from Gain state 4 to 5 */
  RFLM_RXAGC_LNA_GAIN_STEP_NUM
  /*!< Maximum number of Gain steps */
} rflm_rxagc_lna_gain_step_e;

/*----------------------------------------------------------------------------*/
/*! @brief 
    LNA CCS event type
*/
typedef struct
{
  /*! @brief Header and tasks for LNA update event */
  rflm_ccs_rf_event_header_task_type header_task;
  /*! @brief Data buffer for the tasks  */
  uint32 data_buf[RFLM_CMN_RXAGC_LNA_SCRIPT_DATA_BUF_SIZE_WORDS];
} ALIGN(4) rflm_rxagc_cmn_lna_ccs_event_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  all info pertaining to a cell. The cell models the LNA - so this struct needs
  to have all info pertaining to the LNA
 
*/
typedef struct
{
  /*! tech ID */
  rflm_tech_id_t owner_tech;
  
  /*! Wideband index for which the RxAGC is to be configured */
  uint8 wideband_index;

  /*! Stores mapping of carrier to Narrow band index */
  uint8 carrier_to_nb_map[RFLM_CMN_RXAGC_MAX_NB_PER_WB];

  /*! Wideband index for which the RxAGC is to be configured */
  int rise_thresholds[RFLM_CMN_RXAGC_MAX_LNA_STATES];

  /*! Wideband index for which the RxAGC is to be configured */
  int fall_thresholds[RFLM_CMN_RXAGC_MAX_LNA_STATES];

  /*! Narrow band index for which the RxAGC is to be configured, 
      right now this interface is for TDS only */
  int rise_thresholds_nb[RFLM_CMN_RXAGC_MAX_LNA_STATES];

  /*! Narrow index for which the RxAGC is to be configured, 
      right now this interface is for TDS only */
  int fall_thresholds_nb[RFLM_CMN_RXAGC_MAX_LNA_STATES];

  /*! Wideband index for which the RxAGC is to be configured */
  int lna_gain_offsets[RFLM_CMN_RXAGC_MAX_LNA_STATES];

  /*! CSR LNA Phase offset per LNA gain state */
  uint32 lna_phase_offsets[RFLM_CMN_RXAGC_MAX_LNA_STATES];

  /* todo read from RxLM */
  int32 dvga_gain_offset[RFLM_CMN_RXAGC_MAX_NB_PER_WB];

  /*! LNA offset - per nb i/f. For 1x use index 0. 
      For DO, index this with carrier index.
      The offset is applied to the assigned narrowband of the carrier 
      automatically. */
  int16 cal_gain_offset[RFLM_CMN_RXAGC_MAX_NB_PER_WB][RFLM_CMN_RXAGC_MAX_LNA_STATES];

  int16 bypass_timer_val[RFLM_RXAGC_LNA_GAIN_STEP_NUM];
  /*!< LNA Bypass Timer Value per LNA Gain Step. Units is in microseconds */

  int16 nonbypass_timer_val[RFLM_RXAGC_LNA_GAIN_STEP_NUM];
  /*!< LNA non-Bypass Timer Value per LNA Gain Step. Units is in microseconds */
}__rx_static_info_t;

/*----------------------------------------------------------------------------*/
/*! @brief 
    Input parameters for updating static data 
    This needs to be exposed to tech MC layers since they need to be able to
    fill in the data that is needed to create a cell
*/
typedef struct
{

  cell_id_t cell_id;  ///< Input: used to identify a cell in a given WB buffer 

  __rx_static_info_t cell_static_info; ///< Output: holds config info for cell 

} rflm_cmn_rxagc_static_data_t;

/*----------------------------------------------------------------------------*/
/*! @brief 
*/
typedef struct
{

  void* tech_data_ptr; ///< Output: container that holds the start address of 
                       /// tech-specific data area. Clients may now cast this
                       /// pointer to their own user-defined struct

} rflm_cmn_rxagc_tech_data_t;

/*----------------------------------------------------------------------------*/
/*! @brief 
    Data structure exposed to DM
*/
typedef struct
{ 

  void* wb_buff_ptr;                  ///< pointer to a valid WB buffer

}rflm_rxagc_event_template_t;

/*----------------------------------------------------------------------------*/
/*! @brief 
    Input parameters for rflm_cmn_rxagc_ctrl
*/
typedef struct
{
  rflm_ustmr_t   action_time;         ///< Time for rf on/off to take effect
  boolean        immediate_flag;      ///< Flag for immediate action_time
  boolean        rf_on;               ///< TRUE=on, FALSE=off
} rflm_rxagc_cmn_ctrl_in_t;

/*----------------------------------------------------------------------------*/
/*! @brief 
    Input parameters for adding a new cell to an existing WB buffer 
*/
typedef struct
{
  uint8 num_cells;
  ///< cells to be created / deleted
  ///< the number of cells to be created in a pre-existing Wb buffer.
  ///< For CDMA/WCDMA/TDS : num_cells is 1
  ///< For LTE:             num_cells is 2 (current and neighbor)
  ///< For TDS:             num_cells can be upto 11 in case of i-freq

  cell_id_t cell_ids[RFLM_CMN_RXAGC_MAX_CELLS_PER_WB_BUFFER];   
  ///< array that contains the cell_IDs (only num_cells entries are valid)

  boolean use_events;
  ///< Input: Set to TRUE if CCS events need to be used instead of Issue Seqs 
  ///  for scheduling LNA gain changes

} rflm_rxagc_cmn_add_delete_cells_t;

/*----------------------------------------------------------------------------*/
/*! @brief 
    Output parameters for rflm_cmn_rxagc_ctrl
*/
typedef struct
{
  rflm_ustmr_t   est_done_time;      ///< estimated completion time  
  rflm_err_t     result;             ///< API result 
} rflm_rxagc_cmn_ctrl_out_t;

/*----------------------------------------------------------------------------*/
/*!
  @brief rflm rxagc cmn ccs data buffer type
*/
typedef union
{
  /*! @brief EXT RFFE write type */
  rflm_ccs_rf_transaction_rffe_ext_rw_t rffe_ext_data;

  /*! @brief normal RFFE/SSBI type */
  rflm_ccs_rf_transaction_ssbi_rffe_rw_pair_t rffe_ssbi_data;
    
}rflm_rxagc_ccs_data_buff_t;

/*----------------------------------------------------------------------------*/
/*!  @brief structure of rxagc event data type.  */
typedef struct
{
  /*! @brief CCS info to be populated by client */
  rfmeas_event_infor_type ccs_task_info;              

  /*! @brief  holds the task definition/type and also a pointer to 
      ccs_data_buff below */
  rflm_ccs_rf_task_t  task_buff[RFLM_CCS_RF_TASKQ_NUM_ITEMS];

}rflm_rxagc_ccs_task_t;

/*----------------------------------------------------------------------------*/
/*! @brief 
    Input and output parameters for querying the LNA pointers for a given
    cell in a WB buffer
*/
typedef struct
{
  cell_id_t cell_id;       
  ///< Input: cell for which LNA pointers are being queried
  
  rflm_rxagc_cmn_lna_ccs_event_type* lna_task_info[RFLM_CMN_RXAGC_MAX_LNA_STATES];   
  ///< Output: array that holds the pointers to ccs task info

} rflm_rxagc_cmn_lna_ptrs_t;

/*----------------------------------------------------------------------------*/
/*! @brief 
    Input and output parameters for querying the LNA pointers for a given
    cell in a WB buffer
*/
typedef struct
{
  cell_id_t cell_id;       
  ///< Input: cell for which LNA tasks are to be created

  uint8 tq_index;
  ///< Input: task queue index

} rflm_rxagc_cmn_create_lna_tasks_t;

/*----------------------------------------------------------------------------*/
/*! @brief 
    Input and output parameters for querying the LNA pointers for a given
    cell in a WB buffer
*/
typedef struct
{
  cell_id_t cell_id;       
  ///< Input: cell for which LNA pointers are being queried

  rflm_rxagc_lna_state_e gain_state;
  ///< Input: LNA gain state to which the LNA is to be programmed
  ///< todo - look for bad LNA gain state

  rflm_ccs_rf_tq_handle_t* task_q_index;
  ///< Input: Task Q index

  uint32 ustmr_action_time;
  ///< Input: action time in ustmr units when the task needs to be scheduled
  ///< todo - look for invalid action time 
  
  uint32 ccs_mem_ptr;
  ///< Input: LNA tasks will be be copied into this mem location in the CCS 
  /// memory map where the LNA task will be 

  uint32 seq_size_in_bytes;
  ///< Input: size allocated (in bytes) for the LNA task
  
  void* rxagc_done_callback;
  ///< Input: API called when task is complete

  void* callback_params;
   ///< Input: pointer to arguments for callback API

  uint32 task_index;
  ///< Output: this holds the task index of the CCS task scheduled for the 
  ///requested LNA update in CCS Task Queue
} rflm_rxagc_cmn_schedule_task_t;

/*----------------------------------------------------------------------------*/
/*! @brief 
    Input parameters for populating on-demand scripts
*/
typedef struct
{
  rflm_cmn_rxagc_script_name_e rxagc_script_name;
  ///< Struct member to hold Common RF AGC On Demand Script Name.

  rflm_rxagc_ccs_task_t* ccs_task;
  ///< Struct member to hold pointer to the On Demand Task.
  
} rflm_rxagc_cmn_on_demand_t;

#ifdef __cplusplus
}
#endif

/*!   @} */

#endif /* RFLM_RXAGC_API_H */
