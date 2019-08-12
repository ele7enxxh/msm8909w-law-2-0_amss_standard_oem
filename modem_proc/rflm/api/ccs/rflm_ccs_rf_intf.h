#ifndef RFLM_CCS_RF_INTF
#define RFLM_CCS_RF_INTF
/*!
  @file
  rflm_ccs_rf_intf.h

  @brief
  RF CCS software interface.
 
  @detail
  description of functions in this file
 
*/

/*==============================================================================

  Copyright (c) 2013 -2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/ccs/rflm_ccs_rf_intf.h#1 $

when         who     what, where, why
----------   ---     -----------------------------------------------------------
04/13/16     rs      KW error fix
10/20/14     hdz     Added support for thread save tq enqueue
08/20/14     rca     Support for callback invocation disregarding enqueue order
07/29/14     aw      Support priority update task enqueue.
07/23/14     aw      Support event late timing check
07/22/14     rca     THOR featurization
07/21/14     rca     Support for RFLM_CCS_SCRUTINY_MODE
06/11/14     ra      Fix  Bolt Plus featurization
06/10/14     jc      Add stall check API
06/03/14     ra      Add support target specific support for scratch array
05/21/14     hdz     Increased issue_seq payload size
12/02/13     ra      Support new API to obtain a CCS Task Queue Handle
09/05/13     ra      Keeping backward compatibility of RFLM_CCS_RF_INTF
09/04/13     ra      Moved support to CCS_RF_INTF
09/03/13     ra      Changed name from rflm_ccs_init to rflm_ccs_core_init
09/03/13     ra      Imported api and definitions
07/16/13     ra      Initial port to Bolt RF SW
==============================================================================*/
/*===========================================================================

                   INCLUDES

===========================================================================*/
#include "comdef.h"
#include "rflm_ccs_task_defs.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Number of SSBI channels supported.
*/
#define RFLM_CCS_RF_NUM_SSBI_CHAN            CCS_RF_NUM_SSBI_CHAN

/*! @brief Number of RFFE channels supported.
*/
#define RFLM_CCS_RF_NUM_RFFE_CHAN            CCS_RF_NUM_RFFE_CHAN

/*! @brief Maximum number of transactions supported per task. 
    The task field for num_transactions is 8 bits, but memory constraints will
    probably limit this lower.
*/
#define RFLM_CCS_RF_MAX_NUM_TRANSACTIONS CCS_RF_MAX_NUM_TRANSACTIONS



/*! @brief Number of RF task queues.
    First 4 are intended for RAT direct use, fifth is for RFSW commands (via FW).
*/
#define RFLM_CCS_RF_NUM_TASKQ                CCS_RF_NUM_TASKQ

/*! @brief Number of RF task queues which support a full feature set.
    The remainder have limited functionality, such as not supporting ISSUE_SEQ.
*/
#define RFLM_CCS_RF_NUM_FULL_TASKQ           CCS_RF_NUM_FULL_TASKQ           

/*! @brief Number of tasks on each task queue.
*/
#define RFLM_CCS_RF_TASKQ_NUM_ITEMS          CCS_RF_TASKQ_NUM_ITEMS          

/*! @brief Size of taskq data buffer, shared for all TQ.
    - 2kByte per pair of FW TQs
    - an extra 2k for IRAT TQ sharing
    - 1k for RFSW TQ
    - 2k for "on demand" buffer, when RATs need more memory
*/
#define RFLM_CCS_RF_TASKQ_DATA_BUF_WORDS     CCS_RF_TASKQ_DATA_BUF_WORDS     



/*! @brief Task types.
*/
#define RFLM_CCS_RF_TASK_NONE                CCS_RF_TASK_NONE                
#define RFLM_CCS_RF_TASK_SSBI_WRITE          CCS_RF_TASK_SSBI_WRITE          
#define RFLM_CCS_RF_TASK_SSBI_ACCESS         CCS_RF_TASK_SSBI_ACCESS         
#define RFLM_CCS_RF_TASK_RFFE_WRITE          CCS_RF_TASK_RFFE_WRITE          
#define RFLM_CCS_RF_TASK_RFFE_ACCESS         CCS_RF_TASK_RFFE_ACCESS            
#define RFLM_CCS_RF_TASK_RFFE_WRITE_EXT      CCS_RF_TASK_RFFE_WRITE_EXT      
#define RFLM_CCS_RF_TASK_RFFE_ACCESS_EXT     CCS_RF_TASK_RFFE_ACCESS_EXT      
#define RFLM_CCS_RF_TASK_GRFC_WRITE          CCS_RF_TASK_GRFC_WRITE          
#define RFLM_CCS_RF_TASK_ISSUE_SEQ           CCS_RF_TASK_ISSUE_SEQ           
#define RFLM_CCS_RF_TASK_MAX                 CCS_RF_TASK_MAX                 


/*! @brief Task trigger types ("trig_type" field).
    IMMEDIATE: Task is triggered immediately when channel is available.
    ABSOLUTE:  Task will be started exactly at trigger time, or error .
    DYNAMIC:   Task will be started after trigger time, when channel is available.
*/
#define RFLM_CCS_RF_TRIG_IMMEDIATE           CCS_RF_TRIG_IMMEDIATE           
#define RFLM_CCS_RF_TRIG_ABSOLUTE            CCS_RF_TRIG_ABSOLUTE            
#define RFLM_CCS_RF_TRIG_DYNAMIC             CCS_RF_TRIG_DYNAMIC             

/*! @brief Trigger time (task "trig_time" field) precision: 
    Number of bits and bitmask, in UNIV_STMR units.
*/
#define RFLM_CCS_RF_TRIG_TIME_BITS           CCS_RF_TRIG_TIME_BITS           
#define RFLM_CCS_RF_TRIG_TIME_MASK           CCS_RF_TRIG_TIME_MASK           

/*! @brief Highest task priority (task "priority" field).
*/
#define RFLM_CCS_RF_TASK_PRIO_HIGHEST        CCS_RF_TASK_PRIO_HIGHEST        

/*! @brief Task state (for ccs_rf_task_status_t.state)
    IDLE:      Task item unoccupied.
    PENDING:   Task placed on queue, but not serviced by CCS yet.
    SCHED:     Task is in CCS scheduler, but dispatch time not yet determined.
    ACTIVE:    Task has been committed to dispatch.
    INTERRUPT: Task completed, and generated interrupt.
    DONE:      Task completed and has no interrupt pending.
*/
#define RFLM_CCS_RF_TASK_STATE_IDLE          CCS_RF_TASK_STATE_IDLE          
#define RFLM_CCS_RF_TASK_STATE_PENDING       CCS_RF_TASK_STATE_PENDING       
#define RFLM_CCS_RF_TASK_STATE_SCHED         CCS_RF_TASK_STATE_SCHED         
#define RFLM_CCS_RF_TASK_STATE_ACTIVE        CCS_RF_TASK_STATE_ACTIVE        
#define RFLM_CCS_RF_TASK_STATE_INTERRUPT     CCS_RF_TASK_STATE_INTERRUPT     
#define RFLM_CCS_RF_TASK_STATE_DONE          CCS_RF_TASK_STATE_DONE          


/*! @brief CCS RF errors. 
    PARAM:    Error in task parameters.
    TIMING:   Task with ABSOLUTE timing could not satisfy its dispatch time.
    PREEMPT:  Task with PREEMPT_ERROR was interrupted. 
    PARITY:   RFFE read transaction got parity error.
    SEQ_LATE: An ISSUE_SEQ task could not be programmed by its start time.
*/
#define RFLM_CCS_RF_ERR_NONE                 CCS_RF_ERR_NONE                 
#define RFLM_CCS_RF_ERR_PARAM                CCS_RF_ERR_PARAM                
#define RFLM_CCS_RF_ERR_TIMING               CCS_RF_ERR_TIMING               
#define RFLM_CCS_RF_ERR_PREEMPT              CCS_RF_ERR_PREEMPT              
#define RFLM_CCS_RF_ERR_PARITY               CCS_RF_ERR_PARITY               
#define RFLM_CCS_RF_ERR_SEQ_LATE             CCS_RF_ERR_SEQ_LATE             


/*! @brief Task resources.
    Each RF channel is a separate resource. GRFCs have no resource, since no conflict.
    Each TQ also is a resource.
*/
#define RFLM_CCS_RF_RESOURCE_SSBI0      CCS_RF_RESOURCE_SSBI0      
#define RFLM_CCS_RF_RESOURCE_SSBI_LAST  CCS_RF_RESOURCE_SSBI_LAST  
#define RFLM_CCS_RF_RESOURCE_RFFE0      CCS_RF_RESOURCE_RFFE0      
#define RFLM_CCS_RF_RESOURCE_RFFE_LAST  CCS_RF_RESOURCE_RFFE_LAST  
#define RFLM_CCS_RF_RESOURCE_TQ0        CCS_RF_RESOURCE_TQ0        
#define RFLM_CCS_RF_RESOURCE_TQ_LAST    CCS_RF_RESOURCE_TQ_LAST    
#define RFLM_CCS_RF_RESOURCE_LAST       CCS_RF_RESOURCE_LAST       

#define RFLM_CCS_RF_TQ_CONCUR_CNT                                                 0
/* assuming number of tasks in one taskQ is 32 */
#define RFLM_CCS_RF_TQ_WRIDX                                                     27
#define RFLM_CCS_RF_INC_CONCURCNT_WRIDX   ((1<<RFLM_CCS_RF_TQ_CONCUR_CNT) + \
                                           (1<<RFLM_CCS_RF_TQ_WRIDX))

#define RFLM_CCS_RF_TQ_CONCUR_MASK                                           0xFFFF

/*! @brief wr_ctl type in interface.
*/
typedef struct
{
  uint32 concurrent_count : 16;   // tied to RFLM_CCS_RF_TQ_CONCUR_MASK
  uint32                  : 11;
  uint32 wr_idx           : 5;   // tied to RFLM_CCS_RF_TQ_WRIDX, determined by CCS_RF_TASKQ_NUM_ITEMS
} wr_ctl_t;

/*! @brief Common task fields.
*/
typedef ccs_rf_task_common_t rflm_ccs_rf_task_common_t;


/*! @brief Common task fields for SSBI and RFFE (not GRFC and not ISSUE_SEQ)
*/
typedef ccs_rf_task_common_non_grfc_t rflm_ccs_rf_task_common_non_grfc_t;


/*! @brief SSBI_WRITE task 
*/
typedef ccs_rf_task_ssbi_write_t rflm_ccs_rf_task_ssbi_write_t;


/*! @brief SSBI_ACCESS task 
*/
typedef ccs_rf_task_ssbi_access_t rflm_ccs_rf_task_ssbi_access_t;


/*! @brief RFFE_WRITE task 
*/
typedef ccs_rf_task_rffe_write_t rflm_ccs_rf_task_rffe_write_t;


/*! @brief RFFE_ACCESS task 
*/
typedef ccs_rf_task_rffe_access_t rflm_ccs_rf_task_rffe_access_t;


/*! @brief RFFE_WRITE_EXT task 
*/
typedef ccs_rf_task_rffe_write_ext_t rflm_ccs_rf_task_rffe_write_ext_t;


/*! @brief RFFE_ACCESS_EXT task 
*/
typedef ccs_rf_task_rffe_access_ext_t rflm_ccs_rf_task_rffe_access_ext_t;


/*! @brief GRFC_WRITE task 
*/
typedef ccs_rf_task_grfc_write_t rflm_ccs_rf_task_grfc_write_t;


/*! @brief ISSUE_SEQ task 
*/
typedef ccs_rf_task_issue_seq_t rflm_ccs_rf_task_issue_seq_t;


/*! @brief Union of all task types.
*/
typedef ccs_rf_task_t rflm_ccs_rf_task_t;



/*! @brief Task status information.
*/
typedef ccs_rf_task_status_t rflm_ccs_rf_task_status_t;


/*! @brief Task+status word pair.
*/
typedef ccs_rf_task_buf_item_t rflm_ccs_rf_task_buf_item_t;



/*! @brief Format of a single SSBI or RFFE transaction, used by reads and writes.
*/
typedef ccs_rf_transaction_ssbi_rffe_rw_t rflm_ccs_rf_transaction_ssbi_rffe_rw_t;

/*! @brief Format of an SSBI or RFFE transaction pair.
*/
typedef ccs_rf_transaction_ssbi_rffe_rw_pair_t rflm_ccs_rf_transaction_ssbi_rffe_rw_pair_t;

/*! @brief Format of an SSBI_ACCESS or RFFE_ACCESS transaction.
*/
typedef ccs_rf_transaction_ssbi_rffe_access_t rflm_ccs_rf_transaction_ssbi_rffe_access_t;

/*! @brief Aliases for SSBI/RFFE transactions.
*/
typedef ccs_rf_transaction_ssbi_rffe_rw_t      rflm_ccs_rf_transaction_ssbi_rw_t;
typedef ccs_rf_transaction_ssbi_rffe_rw_t      rflm_ccs_rf_transaction_rffe_rw_t;
typedef ccs_rf_transaction_ssbi_rffe_rw_pair_t rflm_ccs_rf_transaction_ssbi_rw_pair_t;
typedef ccs_rf_transaction_ssbi_rffe_rw_pair_t rflm_ccs_rf_transaction_rffe_rw_pair_t;
typedef ccs_rf_transaction_ssbi_rffe_access_t  rflm_ccs_rf_transaction_ssbi_access_t;
typedef ccs_rf_transaction_ssbi_rffe_access_t  rflm_ccs_rf_transaction_rffe_access_t;


/*! @brief Format of RFFE extended transactions (read or write).
*/
typedef ccs_rf_transaction_rffe_ext_rw_t rflm_ccs_rf_transaction_rffe_ext_rw_t;



/*! @brief Single task queue interface.
*/
typedef ccs_rf_taskq_intf_t rflm_ccs_rf_taskq_intf_t;


/*! @brief CCS RF shared memory interface.
*/
typedef ccs_rf_intf_t rflm_ccs_rf_intf_t;


/*! @brief Transaction sequence header.
    For internal and sequence generator use only.
*/
typedef ccs_rf_seq_header_t rflm_ccs_rf_seq_header_t;


/*! @brief Maximum number of tasks allowed in one sequence.
*/
#define RFLM_CCS_SEQ_MAX_TASKS             16

/*! @brief Maximum number of transactions allowed in one sequence.
*/
#define RFLM_CCS_SEQ_MAX_TRANSACTIONS      32

/*! @brief Maximum size of a CCS sequence, in 32-bit words.
*/
#define RFLM_CCS_SEQ_MAX_SIZE_WORDS        256

/*! @brief Number of RF channels, including SSBI and RFFE.
*/
#define RFLM_CCS_RF_NUM_RF_CHANNELS     (CCS_RF_NUM_SSBI_CHAN+CCS_RF_NUM_RFFE_CHAN)

/*! @brief Size of issue_seq update table header in word.
*/
#define RFLM_CCS_RF_SEQ_UPDATE_TABLE_HEADER_SIZE_WORDS                  1

/*! @brief Sequence generator errors.
*/
typedef enum
{
  RFLM_CCS_SEQ_ERR_NONE = 0,   /*!< No error */
  RFLM_CCS_SEQ_ERR_NUM_TASKS,  /*!< Number of tasks is out of range */
  RFLM_CCS_SEQ_ERR_NUM_DMA,    /*!< Maximum number of DMA descriptors exceeded */
  RFLM_CCS_SEQ_ERR_NUM_FIXUP,  /*!< Maximum number of transaction or timing fixup items exceeded */
  RFLM_CCS_SEQ_ERR_TASK_TYPE,  /*!< Unsupported task type */
  RFLM_CCS_SEQ_ERR_PARAM,      /*!< Task parameter error */
  RFLM_CCS_SEQ_ERR_LATE,       /*!< An ABSOLUTE trigger time could not be satisfied */
  RFLM_CCS_SEQ_ERR_MON_SETUP,  /*!< Monitor setup margin was violated */
  RFLM_CCS_SEQ_ERR_DURATION,   /*!< Total duration of sequence is too long */
} rflm_ccs_rf_seq_err_e;

/*! @brief Scratch memory for CCS sequence generator, allowing it to be
    re-entrant.
*/
typedef struct
{
#if defined(FEATURE_RF_CCS_THOR) || defined(FEATURE_RF_CCS_BOLT_PLUS)
  uint32 scratch[288];
#else
  uint32 scratch[256];
#endif
} rflm_ccs_rf_seq_scratch_t;

/*! @brief A pregenerated RF sequence.
*/
typedef struct 
{
  uint32 start_time_ustmr;   /*!< Start time of sequence, in USTMR units modulo CCS_RF_TRIG_TIME_MASK */
  uint32 end_time_ustmr;     /*!< End time of sequence, in USTMR units modulo CCS_RF_TRIG_TIME_MASK */
  uint16 seq_size_bytes;     /*!< Size used in seq_data, in bytes */
  uint8 num_tasks;           /*!< Number of tasks in sequence */

  /*! Fixup table for transaction data. 
      First indices are per-task, providing an offset to the transactions
      for that task.
      byte_offset = fixup_table[fixup_table[task_idx]+transaction_idx]
      Byte offset is relative to start of "ccs_seq_header". */
  uint16 num_fixup_used;   /*!< Number of fixup entries used */
  uint16 fixup_table[RFLM_CCS_SEQ_MAX_TASKS+RFLM_CCS_SEQ_MAX_TRANSACTIONS]; 

  /*! CCS sequence data, variable length */
  ccs_rf_seq_header_t rflm_ccs_seq_header;  /*!< CCS sequence header */
  uint32 rflm_ccs_seq_data[RFLM_CCS_SEQ_MAX_SIZE_WORDS]; /*!< Internal sequence data */

} rflm_ccs_rf_seq_t;

// forward declarations
struct rflm_ccs_rf_tq_handle_s;
struct rflm_ccs_rf_event_header_task_s;

/*! @brief RF CCS task callback type.
*/
typedef void (*rflm_ccs_rf_task_callback_t)(struct rflm_ccs_rf_tq_handle_s *, uint32, ccs_rf_task_status_t, void *);

/*! @brief Event Time Check Config Type
*/
typedef struct rflm_ccs_rf_event_time_chk_config_s
{
  /*! @brief info for CCS event time check enable */
  boolean ccs_time_chk_en;
  /*! @brief reserved un-used bits */
  uint8 reserved_bits_8;
  /*! @brief reserved un-used bits */
  uint16 reserved_bits_16;
  /*! @brief expected event finish time used for delay reporting. 
            this is a 22-bit USTMR count number */
  uint32 exp_finish_time;
  /*! @brief absolute deadline for the event that start cause system issue 
            this is a 22-bit USTMR count number */
  uint32 evt_deadline_time;
  /*! @brief tag used for output warning and logging */
  uint32 tag0;
  /*! @brief tag used for output warning and logging */
  uint32 tag1;
} rflm_ccs_rf_event_time_chk_config_t;

/*! @brief event related info provided for callback functions
*/
typedef struct rflm_ccs_rf_event_late_chk_cb_info_s
{
  /*! @brief pointer of the event header  */
  struct rflm_ccs_rf_event_header_task_s * header_ptr;

  /*! @brief CCS event time check config */
  rflm_ccs_rf_event_time_chk_config_t * evt_chk_cfg_ptr;

  /*! @brief actual_end_time - exp_finish_time. if > 0, means later than 
          expected time. if < 0, means earlier than expected event finish time */
  int32 late_time;

   /*! @brief actual_end_time - deadline_finish_time. if > 0, means later than 
          deadline time. if < 0, means earlier than deadline event finish time */
  int32 late_time_deadline;

  /*! @brief indicator of actual event end time later than the event deadline.
          This means the event is too late and can cause system performance 
          issue */
  boolean violate_deadline;

} rflm_ccs_rf_event_late_chk_cb_info_t;

/*! @brief RF CCS event check callback type.
*/
typedef void (*rflm_ccs_rf_evt_chk_callback_t) (rflm_ccs_rf_event_late_chk_cb_info_t *);


/*! @brief Event Time Check Type
*/
typedef struct rflm_ccs_rf_event_time_chk_s
{
  /*! @brief event time check configs */
  rflm_ccs_rf_event_time_chk_config_t cfg;
  /*! @brief user callback */
  rflm_ccs_rf_evt_chk_callback_t cb_ptr;
} rflm_ccs_rf_event_time_chk_t;

#ifndef FEATURE_JOLOKIA_MODEM
/*! @brief CCS RF task queue handle. Not user-accessible.
*/
typedef struct rflm_ccs_rf_tq_handle_s
{
  ccs_rf_taskq_intf_t *intf;  /*!< TQ interface in CCS memory */
  uint32 active_mask;         /*!< Mask of active/pending tasks */
  uint32 wr_idx;               /*!< Next index to write a task */
  uint8 tq_idx;               /*!< TQ index for reverse lookup */

  uint32 no_cb_order_tasks;   /*!< Mask of tasks that require cb regardless of order */

  /*! Interrupt callbacks. */
  struct rflm_ccs_rf_tq_handle_cb_s {
    rflm_ccs_rf_task_callback_t func; 
    void *arg;
  } callback[CCS_RF_TASKQ_NUM_ITEMS];

   /*!< event time check config*/
  rflm_ccs_rf_event_time_chk_t evt_chk[CCS_RF_TASKQ_NUM_ITEMS];

  wr_ctl_t wr_ctl;

}rflm_ccs_rf_tq_handle_t;
#else
/*! @brief CCS RF task queue handle. Not user-accessible.
*/
typedef struct rflm_ccs_rf_tq_handle_s
{
  ccs_rf_taskq_intf_t *intf;  /*!< TQ interface in CCS memory */
  uint32 active_mask;         /*!< Mask of active/pending tasks */
  uint8 wr_idx;               /*!< Next index to write a task */
  uint8 tq_idx;               /*!< TQ index for reverse lookup */

  uint32 concurrent;          /*!< Counter for concurrency checks */
  uint32 no_cb_order_tasks;   /*!< Mask of tasks that require cb regardless of order */

  /*! Interrupt callbacks. */
  struct rflm_ccs_rf_tq_handle_cb_s {
    rflm_ccs_rf_task_callback_t func; 
    void *arg;
  } callback[CCS_RF_TASKQ_NUM_ITEMS];

   /*!< event time check config*/
  rflm_ccs_rf_event_time_chk_t evt_chk[CCS_RF_TASKQ_NUM_ITEMS];

}rflm_ccs_rf_tq_handle_t;

#endif

/*! @brief CCS RF issue_seq delta table used in issue_seq update task.
*/
typedef struct rflm_ccs_rf_issue_seq_delta_table_s
{
  /*!< header section */
  uint32 seq_ptr;                /*!< pointer to the template seq ptr */
  /*!< Data section */
  uint32 * table_entry_ptr;      /*!< pointer to delta data to update issue_seq */
  /*!< Offset section */
  /*!< The entry_offset[i] is used to store address offset of table_entry[i] from the beginning of seq_ptr. 
       Its unit is the number of uint32.
       For example, seq_ptr header is 3 uint32, so the first field of the 1st DMAC is 3.
  */
  uint8 * entry_offset_ptr;    /*!< pointer to offset table*/                                                                                   
}rflm_ccs_rf_issue_seq_delta_table_t;


/*! @brief CCS RF callback timing log
*/
typedef struct rflm_ccs_callback_timing_log_s
{
  uint32 cb_start_time;          /* in ustmr count   */
  uint32 cb_duration;            /* in ustmr count   */
  uint32 cb_ptr;    		     /* function pointer */
  uint32 event_id;               /* For rf event, set to event_id; for non-rf-event, set to task_idx*/
} rflm_ccs_callback_timing_log_t;

/*==============================================================================
                       Export CCS Driver API
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/* GRFC direct access functions */
extern void rflm_ccs_grfc_set
(
  uint32 grfc_mask 
);

extern void rflm_ccs_grfc_clear
(
  uint32 grfc_mask 
);

extern uint32 rflm_ccs_grfc_read(void);


/* CCS RF interface functions */
extern uint32 rflm_ccs_rf_tq_enqueue_ext
(
  rflm_ccs_rf_tq_handle_t *tq, 
  ccs_rf_task_t *task,   
  rflm_ccs_rf_task_callback_t callback, 
  void *callback_arg,
  boolean no_cb_order
);

extern uint32 rflm_ccs_enqueue_priority_update_task ( 
  /*!< the label of the task to change, the valid range is [0:30] */
  uint32 target_task_label,
  /*!< the new priority of the task to change */
  uint32 target_task_priority,
  /*!< trigger time */
  uint32 trig_time,
  /*!< taskq to enqueue */
  rflm_ccs_rf_tq_handle_t *tq_ptr
);          

extern ccs_rf_task_status_t rflm_ccs_rf_get_task_status
(
  rflm_ccs_rf_tq_handle_t *tq, 
  uint32 task_idx 
);

extern boolean rflm_ccs_rf_get_task_stall_status
(
  rflm_ccs_rf_tq_handle_t *tq,
  uint32 task_idx,
  uint32 seq_ptr
);


extern rflm_ccs_rf_seq_err_e rflm_ccs_rf_seq_create
(
  rflm_ccs_rf_seq_t *seq,
  uint32 tq_idx,  
  const ccs_rf_task_t tasks[], 
  uint32 num_tasks,     
  rflm_ccs_rf_seq_scratch_t *scratch 
);

#ifdef RFLM_CCS_SCRUTINY_MODE
extern void rflm_ccs_rf_seq_dma_link_write
(
  void *ccs_dest_ptr,
  rflm_ccs_rf_seq_t *seq
);

#define rflm_ccs_rf_seq_write_to_ccs(ccs_dest_ptr, ccs_dest_num_bytes, seq) \
  do { \
    rflm_ccs_rf_seq_dma_link_write((void *)(ccs_dest_ptr), (rflm_ccs_rf_seq_t *)(seq)); \
    RFLM_DIAG_TRACE_HIGH(RFLM_DIAG_MSG_SSID_CCS,"seq_size_bytes :%d |ccs_dest_num_bytes %d ",((rflm_ccs_rf_seq_t *)(seq))->seq_size_bytes,(ccs_dest_num_bytes)); \
    if (((rflm_ccs_rf_seq_t *)(seq))->seq_size_bytes > (ccs_dest_num_bytes)) \
    { \
      RFLM_DIAG_ERR_FATAL( "rflm_ccs_rf_seq_write_to_ccs: seq_size_bytes > ccs_dest_num_bytes"); \
    } \
    rflm_ccs_write_memory((ccs_dest_ptr),&(((rflm_ccs_rf_seq_t *)(seq))->rflm_ccs_seq_header), ((rflm_ccs_rf_seq_t *)(seq))->seq_size_bytes); \
  } while(0)
#else /* RFLM_CCS_SCRUTINY_MODE */
extern void rflm_ccs_rf_seq_write_to_ccs
(
  void *ccs_dest_ptr,         
  uint32 ccs_dest_size,       
  rflm_ccs_rf_seq_t *seq
);
#endif /* !RFLM_CCS_SCRUTINY_MODE */

extern uint32 rflm_ccs_rf_seq_prepare
(
  uint32 *buf,                 
  uint32 buf_bytes,            
  void *ccs_dest_ptr,          
  rflm_ccs_rf_seq_t *seq  
);

extern void rflm_ccs_rf_isr
(
  void *arg
);


/* These functions are intended to be called by CRM only */
extern void rflm_ccs_core_init(void);


extern rflm_ccs_rf_tq_handle_t * rflm_ccs_rf_tq_request
(
  uint32 tq_idx
);

extern rflm_ccs_rf_tq_handle_t * rflm_ccs_rf_tq_release
(
  rflm_ccs_rf_tq_handle_t *tq
);

extern rflm_ccs_rf_tq_handle_t * rflm_ccs_rf_tq_get_handle
(
  uint32 tq_idx
);

extern boolean rflm_ccs_rf_tq_is_idle
(
  rflm_ccs_rf_tq_handle_t *tq  
);

extern uint32 rflm_ccs_enqueue_issue_seq_update_task
(
     rflm_ccs_rf_tq_handle_t *tq,                       /*!< taskq */                
     ccs_rf_task_t * task,                              /*!< issue_seq update task */
     rflm_ccs_rf_task_callback_t callback,              /*!< call back */
     void *callback_arg,                                /*!< callback args */
     uint32 * delta_table_ptr,                          /*!< Q6 delta table ptr */
     uint32 * ccs_dst_ptr,                              /*!< CCS target address - word aligned */
     uint32 ccs_dest_num_bytes                          /*!< allocated CCS memory size in bytes */
);

#ifdef __cplusplus
}  // extern "C"
#endif 

#define rflm_ccs_rf_tq_enqueue(tq,task,callback,callback_arg,...) rflm_ccs_rf_tq_enqueue_ext(tq,task,callback,callback_arg,(FALSE, ## __VA_ARGS__))

#endif /* RFLM_CCS_RF_INTF */
