#ifndef RFLM_MEAS_H
#define RFLM_MEAS_H

/*!
  @file
  rflm_meas.h

  @brief
  Types for RF CCS events inside shared memory
 
*/

/*===========================================================================
Copyright (c) 2012 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/meas/rflm_meas.h#1 $
$DateTime: 2016/12/13 07:59:54 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/19/15   bp      Decrease the value of RFLM_MEAS_MAX_PRELOAD_TRANS from 175 to 155  
10/06/15   bp      Increase preload memory transaction size - RFLM_MEAS_MAX_PRELOAD_TRANS is made as 175
08/20/15   par     Increase preload memory transaction size
09/30/14   gvn     3xCA gaps support
08/15/14   gvn     Re-partition CCS memory for LTE tune-back
02/06/14   Saul    CCS. TQ Manager
01/06/14   gvn     Update timing info for LTE
12/11/13   pl      Fix Compiler Warning 
12/04/13   pl      move defined to header 
12/02/13   pl      Initial version
============================================================================*/

/*==============================================================================
                       Include
==============================================================================*/
#include "rflm_ccs_intf.h"
#include "rflm_ccs_irat_intf.h"
#include "rflm_dm_api.h"
#include <qurt.h>


/*===========================================================================

                   DEFINITIONS AND TYPES

===========================================================================*/
#define RFLM_MEAS_TQ_IDX 4

#define RFLM_MEAS_MAX_PRELOAD_TRANS     155
#define RFLM_MEAS_MAX_TRNSCVR_TRANS     150
#define RFLM_MEAS_MAX_NON_TRNSCVR_TRANS 60

/* Re-partition CCS for tune-back*/
#define RFLM_MEAS_MAX_TB_TRNSCVR_TRANS     240
#define RFLM_MEAS_MAX_TB_NON_TRNSCVR_TRANS 100

#define RFLM_MEAS_PRELOAD_MAX_BYTES_PER_TRANS 4
#define RFLM_MEAS_TRIG_MAX_BYTES_PER_TRANS 4
#define RFLM_TIME_TO_COPY_FROM_WXE_TO_HWREG 0.2

#define RFLM_MEAS_PRELOAD_SIZE_BYTE (RFLM_MEAS_MAX_PRELOAD_TRANS*RFLM_CCS_RF_IRAT_PRELOAD_TRANS_SIZE_BYTES)
#define RFLM_MEAS_TRNSCVR_SIZE_BYTE (RFLM_MEAS_MAX_TRNSCVR_TRANS*RFLM_CCS_RF_IRAT_SSBI_TRANS_SIZE_BYTES) 
#define RFLM_MEAS_NON_TRNSCVR_SIZE_BYTE (RFLM_MEAS_MAX_NON_TRNSCVR_TRANS*RFLM_CCS_RF_IRAT_RFFE_TRANS_SIZE_BYTES)

/* Different calculation for Tune-back since preload is not done during tune-back */
#define RFLM_MEAS_TB_TRNSCVR_SIZE_BYTE (RFLM_MEAS_MAX_TB_TRNSCVR_TRANS*RFLM_CCS_RF_IRAT_SSBI_TRANS_SIZE_BYTES) 
#define RFLM_MEAS_TB_NON_TRNSCVR_SIZE_BYTE (RFLM_MEAS_MAX_TB_NON_TRNSCVR_TRANS*RFLM_CCS_RF_IRAT_RFFE_TRANS_SIZE_BYTES)

#define RFLM_MEAS_PRELOAD_BUF_SIZE ((RFLM_MEAS_PRELOAD_SIZE_BYTE+3)>>2)
#define RFLM_MEAS_TRNSCVR_BUF_SIZE ((RFLM_MEAS_TRNSCVR_SIZE_BYTE+3)>>2)
#define RFLM_MEAS_NON_TRNSCVR_BUF_SIZE ((RFLM_MEAS_NON_TRNSCVR_SIZE_BYTE+3)>>2)

/* Different calculation for Tune-back since preload is not done during tune-back */
#define RFLM_MEAS_TB_TRNSCVR_BUF_SIZE ((RFLM_MEAS_TB_TRNSCVR_SIZE_BYTE+3)>>2)
#define RFLM_MEAS_TB_NON_TRNSCVR_BUF_SIZE ((RFLM_MEAS_TB_NON_TRNSCVR_SIZE_BYTE+3)>>2)

#define RFLM_MEAS_TOTAL_BUF_SIZE_BYTE 2040
#define RFLM_MEAS_TOTAL_BUF_SIZE (RFLM_MEAS_TOTAL_BUF_SIZE_BYTE >> 2)


/* NOTE : --> PLEASE UPDATE THIS COMMENT WHEN THE ABOVE SIZES CHANGES <--
   This structure has a total size of 2040 bytes 
   preload : floor( ((150*8)+3)/4 ) = 300 unit32 = 1200 bytes
   transceiver :  2 * floor( ((150*2)+3)/4 ) = 2 * 75 unit32  = 600 bytes
   non-transceiver : 2 * floor( ((60*2)+3)/4 ) = 2 * 30 unit32  = 240 bytes 

   For tune-back,  
   New calc w/o using ping-pong buffers: just use 1 chunk of memory and manage by offseting

   transceiver :  floor( ((720 *2)+3)/4 ) = 360 unit32  = 1440 bytes
   non-transceiver : floor( ((300 *2)+3)/4 ) = 150 unit32  = 600 bytes 

   old:
   transceiver :  3 * floor( ((240*2)+3)/4 ) = 3 * 120 unit32  = 1440 bytes
   non-transceiver : 3 * floor( ((100*2)+3)/4 ) = 3 * 50 unit32  = 600 bytes */

//#define RFLM_MEAS_POLICY_CHK_DEBUG_EN  1

#define RFLM_MEAS_MAX_RESOURCE_TABLE_ENTRY 16  
#define RFLM_MEAS_MAX_RESOURCE_NUM_PER_EVENT 1

#define RFLM_MEAS_EVENT_DEPENDENCY_NA -1


typedef uint32 (*policy_check_func_ptr_t)(rflm_dm_script_meta_s *);

typedef enum
{
    RFLM_MEAS_RES_STATE_UNINIT = 0,
    RFLM_MEAS_RES_STATE_PRELOAD_ENQ,
    RFLM_MEAS_RES_STATE_PRELOAD_DONE,
    RFLM_MEAS_RES_STATE_TRIGGER_ENQ,
    RFLM_MEAS_RES_STATE_TRIGGER_DONE,
    RFLM_MEAS_RES_STATE_PRELOAD_ENQ_TRIGGER_ENQ,
    RFLM_MEAS_RES_STATE_PRELOAD_DONE_TRIGGER_ENQ,
}rflm_meas_resource_state_e;

typedef enum
{
    RFLM_MEAS_RES_UPDATE_EVT_WXE_PRELOAD_ENQ = 0,
    RFLM_MEAS_RES_UPDATE_EVT_WXE_TRIGGER_ENQ,
    RFLM_MEAS_RES_UPDATE_EVT_WXE_PRELOAD_ISR,
    RFLM_MEAS_RES_UPDATE_EVT_WXE_TRIGGER_ISR
}rflm_meas_resource_update_event_e;

typedef enum
{
    RFLM_MEAS_ERR_NONE = 0,
    RFLM_MEAS_ERR_TRIGGER_LOCK_BUF = 0x1,                           // trigger is locking the buffer, can not enqueue another preload
    RFLM_MEAS_ERR_PRELOAD_DIFF_PRIORITY_LOCK_BUF = 0x2,             // preload with diff priority is locking the buffer, can not enqueue another preload
    RFLM_MEAS_ERR_TRIGGER_ON_UNINIT_BUF = 0x4,                  // Trigger on uninitialized buffer, can not enqueue a trigger
    RFLM_MEAS_ERR_TRIGGER_PRELOAD_DIFF_PRIORITY = 0x8,              // preload and trigger have different priorities, can not enqueue a trigger
    RFLM_MEAS_ERR_PRELOAD_TRIGGER_MISMATCH = 0x10,              // trigger on wrong preload, can not enqueue a trigger  
    RFLM_MEAS_ERR_SCRIPT_PAYLOAD_PTR_INVALID = 0x20,                // script buf payload ptr is invalid
    RFLM_MEAS_ERR_EMPTY_BIT_MASK = 0x40,                            // event has empty bit mask
    RFLM_MEAS_ERR_INVALID_EVENT_TYPE = 0x80,                    // event type is not preload nor trigger
    RFLM_MEAS_ERR_INVALIDATE_RESOURCE_WITH_PENDING_CMD = 0x100,     // invalidate a resource with pending cmd
        RFLM_MEAS_ERR_OVERWRITE_RESOURCE_WITH_PENDING_CMD = 0x200       // overwrite a resource state with pending cmd
}rflm_meas_wxe_preload_trigger_error_e;

typedef struct 
{
    uint32 preload_seq_id;                  
    rflm_meas_resource_state_e state; // resource state
    uint32 priority;                  // preload/trigger pair priority
    uint32 pending_preload_cnt;
    uint32 pending_trigger_cnt;
}rflm_meas_resource_entry_s;

/*==============================================================================
                       Export CCS MEAS Driver API
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/* Get IRAT specific TQ */
extern rflm_ccs_rf_tq_handle_t* rflm_meas_get_tq( void );

/* Get IRAT TQ start address */
extern uint32* rflm_meas_get_irat_ccs_start_addr
(
  void
);

/* Get Preload TQ buffer address */
extern uint32* rflm_meas_get_preload_addr
(
  uint32 handle_id
);

/* Get trigger TQ buffer address */
extern uint32* rflm_meas_get_trnscvr_addr
(
  uint32 handle_id
);

/* Get non-TRX TQ buffer address */
extern uint32* rflm_meas_get_non_trnscvr_addr
(
  uint32 handle_id
);

/* Get trigger TQ buffer address */
extern uint32* rflm_meas_get_tb_trnscvr_addr
(
  uint32 handle_id
);

/* Get non-TRX TQ buffer address */
extern uint32* rflm_meas_get_tb_non_trnscvr_addr
(
  uint32 handle_id
);


/////////////////////////////////////////////////////////////////////////////
// Policy checking APIs
/////////////////////////////////////////////////////////////////////////////
/*===========================================================================

  FUNCTION:  rflm_meas_enqueue_policy_check
 
===========================================================================*/
/*!
    @brief
    The wrapper funciton performs:
    1) policy check on an event to decide if it can be enquened.
    2) enqueue an event if policy check Ok.

    @detail

    @return
    rflm_ccs_event_info_s:  ccs_event_info
*/
/*=========================================================================*/
extern rflm_ccs_event_info_s rflm_meas_enqueue_policy_check
(
  /*!< Head pointer for event (including header and tasks) */
  rflm_ccs_rf_event_header_task_type *task_p,
  /*!< Start time of script, if time needs to be immediate, 
    pass RFLM_CCS_RF_EVENT_TIME_IMMEDIATE */
  uint32 start_offset,
  /*!< CCS desination memory for SSBI/ RFFE addr/data information */
  uint32 *ccs_dest_ptr,
  /*!< Length of destination CCS memory in bytes */
  uint32 avail_length_bytes,
  /*!< Callback function for task completion */
  rflm_ccs_rf_task_callback_t user_callback,            // user callback 
  /*!< Argument to callback function */
  void *user_callback_arg,                              // user callback_arg  
  /*!< TQ for programming the CCS RF tasks */
  rflm_ccs_rf_tq_handle_t *tq,
  /*< script payload pointer */
  uint32 * script_payload_ptr, 
  /*! policy check function*/
  policy_check_func_ptr_t policy_check_func_ptr,
  /*! error code for enqueue check: 0 - no error and enqueue */
  uint32 * error_code
);

/*===========================================================================

  FUNCTION:  rflm_meas_rf_event_callback

===========================================================================*/
/*!
    @brief
    The funciton is the callback function for RF event IRQ
  
    @detail
    - update done time to used resource
    - check lateness of an event

    @input
    struct rflm_ccs_rf_tq_handle_s *:  tq_handle, 
    uint32:  task_idx
    ccs_rf_task_status_t: task_status
    void *: cb_args
 
    @return
    boolean: TRUE - update is ok
         FALSE - update fails
 
    -- Rflm/ccs/rf_ccs/src/rflm_ccs_rf_event.c
    // QUESTION: is this the script buffer payload pointer here? 
    rflm_ccs_enqueue_common_rf_event()
    {
       rflm_ccs_handle_common_rf_event{event header is used as callback arguments}
    }
    -- Rflm/ccs/rf_ccs/src/rflm_ccs.cc
    rflm_ccs_rf_tq_enqueue() (to enqueue one task)
    rflm_ccs_rf_isr()
 
*/
/*=========================================================================*/
extern void rflm_meas_rf_event_callback
(
   struct rflm_ccs_rf_tq_handle_s * tq_handle, 
   uint32 task_idx, 
   ccs_rf_task_status_t task_status, 
   void * cb_args
);
                        
/*===========================================================================

  FUNCTION:  rflm_meas_init_resource_table
 
===========================================================================*/
/*!
    @brief
    The funciton init resource table

    @detail
    input all resource id and init all states to INIT 

    @return
        
*/
/*=========================================================================*/
extern void rflm_meas_init_resource_table(void);

/*===========================================================================

  FUNCTION:  rflm_meas_wxe_preload_trigger_enqueue_check
 
===========================================================================*/
/*!
    @brief
    The funciton check if a wxe preload/trigger event can be enqueued and
    returns an error code.
  
    @detail
    Assumption:
    Before calling enqueue function, the event has already been store into
    script buffer and associated with a handle.

    event_id is used as script_id which can be used with handle_id to locate
    the script_buf_ptr which stores the event contents.
 
    handle- |
            |- script[event_id of preload], store preload event contents
            |- script[event_id of trigger], store trigger event contents

    @input
    rflm_dm_script_meta_s *:  script_meta_ptr

    @return
    uint32: error message bit mask
            0 - no error, ok to enqueue this event
            non-zero, error, can not enqueue this event.
        
*/
/*=========================================================================*/
extern uint32 rflm_meas_wxe_preload_trigger_enqueue_check
(
   rflm_dm_script_meta_s * script_meta_ptr
);


/*===========================================================================

  FUNCTION:  rflm_meas_invalidate_resource

===========================================================================*/
/*!
    @brief
    The funciton invalidate resources based on bit mask for power collapsing
    clear all contents in the resource table for this resource.
  
    @detail

    @input
    uint32: bit mask
 
    @return
    uint32: error code. 0 - no error
        
*/
/*=========================================================================*/
extern uint32 rflm_meas_invalidate_resource
(
   uint32 bit_mask
);

/*===========================================================================

  FUNCTION:  rflm_meas_overwrite_resource_state

===========================================================================*/
/*!
    @brief
    The function overwrites resource state.
    This function needs to be used very carefully.
    (It will overwrite the state machine). 
    (tune-back preload is done by device, does not go through policy check).
  
    @detail

    @input
    uint32: bit mask
    rflm_meas_resource_state_e: overwrite state
 
    @return
    uint32: error code. 0 -  no error
        
*/
/*=========================================================================*/
extern uint32 rflm_meas_overwrite_resource_state
(                                     
   uint32 bit_mask,
   rflm_meas_resource_entry_s overwrite_entry
);


/*===========================================================================


  FUNCTION:  rflm_meas_print_resource_table

===========================================================================*/
/*!
    @brief
    The funciton print resource table

    @return
*/
/*=========================================================================*/
extern void rflm_meas_print_resource_table(void);


/*===========================================================================

  FUNCTION:  rflm_meas_get_resource_state

===========================================================================*/
/*!
    @brief
    The function returns resource state based on its index.
  
    @detail

    @input
    int32: resource index
 
    @return
    rflm_meas_resource_entry_s:  resource state
        
*/
/*=========================================================================*/
extern rflm_meas_resource_entry_s rflm_meas_get_resource_state
(                                     
   int32 res_idx
);

#if (RFLM_FEATURE_BUILD_MODE==RFLM_FEATURE_BUILD_FW_LIB)
/*----------------------------------------------------------------------------*/
extern volatile boolean rflm_ccs_tq_manager_enable_debug;

/*----------------------------------------------------------------------------*/
rflm_ccs_event_info_s
rflm_ccs_tq_manager_enqueue_common_rf_event
(
  rflm_ccs_rf_tq_handle_t *tq,
  rflm_ccs_rf_event_header_task_type *event_ptr,
  uint32 start_offset_time,
  rflm_ccs_rf_task_callback_t callback, 
  void *callback_arg,
  boolean is_irat
);

/*----------------------------------------------------------------------------*/
void
rflm_ccs_tq_manager_enqueue_stored_event
(
  rflm_ccs_rf_tq_handle_t *tq
);

/*----------------------------------------------------------------------------*/
void
rflm_ccs_tq_manager_handle_event_done
(
  rflm_ccs_rf_tq_handle_t *tq,
  rflm_ccs_rf_event_header_task_type *event_ptr
);
#endif //#if (RFLM_FEATURE_BUILD_MODE==RFLM_FEATURE_BUILD_FW_LIB)

#ifdef __cplusplus
}  // extern "C"
#endif 


#endif /* RFLM_MEAS_H*/

