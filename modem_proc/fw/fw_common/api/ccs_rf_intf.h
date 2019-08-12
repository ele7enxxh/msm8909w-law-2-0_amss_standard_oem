/*!
  @file
  ccs_rf_intf.h

  @brief
  CCS RF software interface.
 
*/

/*===========================================================================

  Copyright (c) 2012-2014 Qualcomm Technologies Incorporated. 
  All rights reserved.
  Qualcomm Confidential and Proprietary

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

#ifndef CCS_RF_INTF_H
#define CCS_RF_INTF_H


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*! @brief Version of the CCS RF module.
*/
#define CCS_RF_API_VERSION              1
#define CCS_RF_MINOR_VERSION            8
#define CCS_RF_VERSION                  ((CCS_RF_API_VERSION << 16) | CCS_RF_MINOR_VERSION)



/*! @brief Index of DSP-to-CCS interrupt used for RF commands. 
*/
#define CCS_RF_CMD_IRQ_IDX              1

/*! @brief Index of CCS-to-DSP interrupt used for RF responses. 
*/
#define CCS_RF_RSP_IRQ_IDX              1



/*! @brief Number of SSBI channels supported.
*/
#define CCS_RF_NUM_SSBI_CHAN            4

/*! @brief Number of RFFE channels supported.
*/
#define CCS_RF_NUM_RFFE_CHAN            5

/*! @brief Maximum number of transactions supported per task. 
    The task field for num_transactions is 8 bits, but memory constraints will
    probably limit this lower.
*/
#define CCS_RF_MAX_NUM_TRANSACTIONS     255



/*! @brief Number of RF task queues.
    First 4 are intended for RAT direct use, fifth is for RFSW commands (via FW).
*/
#define CCS_RF_NUM_TASKQ                5

/*! @brief Number of RF task queues which support a full feature set.
    The remainder have limited functionality, such as not supporting ISSUE_SEQ.
*/
#define CCS_RF_NUM_FULL_TASKQ           4

/*! @brief Number of tasks on each task queue.
*/
#define CCS_RF_TASKQ_NUM_ITEMS          16

/*! @brief Size of taskq data buffer, shared for all TQ.
*/
#define CCS_RF_TASKQ_DATA_BUF_WORDS     ((11*1024) / 4)


/*! @brief Task types.
*/
#define CCS_RF_TASK_NONE                0
#define CCS_RF_TASK_SSBI_WRITE          1
#define CCS_RF_TASK_SSBI_ACCESS         3
#define CCS_RF_TASK_RFFE_WRITE          4
#define CCS_RF_TASK_RFFE_WRITE_EXT      6
#define CCS_RF_TASK_GRFC_WRITE          8
#define CCS_RF_TASK_ISSUE_SEQ           9
#define CCS_RF_TASK_RFFE_ACCESS         10   
#define CCS_RF_TASK_RFFE_ACCESS_EXT     11 
#define CCS_RF_TASK_RFFE_READ_EXT       12
#define CCS_RF_TASK_ISSUE_SEQ_UPDATE    13
#define CCS_RF_TASK_MAX                 13


/*! @brief Task trigger types ("trig_type" field).
    IMMEDIATE: Task is triggered immediately when channel is available.
    ABSOLUTE:  Task will be started exactly at trigger time, or error .
    DYNAMIC:   Task will be started after trigger time, when channel is available.
*/
#define CCS_RF_TRIG_IMMEDIATE           0
#define CCS_RF_TRIG_ABSOLUTE            1
#define CCS_RF_TRIG_DYNAMIC             2

/*! @brief Trigger time (task "trig_time" field) precision: 
    Number of bits and bitmask, in UNIV_STMR units.
*/
#define CCS_RF_TRIG_TIME_BITS           22
#define CCS_RF_TRIG_TIME_MASK           ((1<<CCS_RF_TRIG_TIME_BITS)-1)

/*! @brief Highest task priority (task "priority" field).
*/
#define CCS_RF_TASK_PRIO_HIGHEST        15

/*! @brief Signal definitions (task "done_signal" field). 
    These are bitmasks relative to the LSb of DONE_SIGNAL, and can be ORed together.
*/
#define CCS_RF_SIG_NONE                 0
#define CCS_RF_SIG_RXFE0                (0x1)
#define CCS_RF_SIG_RXFE1                (0x2)
#define CCS_RF_SIG_RXFE2                (0x4)
#define CCS_RF_SIG_RXFE3                (0x8)
#define CCS_RF_SIG_RXFE_ALL             (CCS_RF_SIG_RXFE0 | CCS_RF_SIG_RXFE1 | CCS_RF_SIG_RXFE2 | CCS_RF_SIG_RXFE3  )

/*! @brief Task state (for ccs_rf_task_status_t.state)
    IDLE:      Task item unoccupied.
    PENDING:   Task placed on queue, but not serviced by CCS yet.
    SCHED:     Task is in CCS scheduler, but dispatch time not yet determined.
    ACTIVE:    Task has been committed to dispatch.
    INTERRUPT: Task completed, and generated interrupt.
    DONE:      Task completed and has no interrupt pending.
*/
#define CCS_RF_TASK_STATE_IDLE          0
#define CCS_RF_TASK_STATE_PENDING       1
#define CCS_RF_TASK_STATE_SCHED         2
#define CCS_RF_TASK_STATE_ACTIVE        3
#define CCS_RF_TASK_STATE_INTERRUPT     4
#define CCS_RF_TASK_STATE_DONE          5


/*! @brief CCS RF errors. 
    PARAM:    Error in task parameters.
    INVALID_TYPE: Unsupported task type specified.
    TIMING:   Task with ABSOLUTE timing could not satisfy its dispatch time.
    PREEMPT:  Task with PREEMPT_ERROR was interrupted. 
    PARITY:   RFFE read transaction got parity error.
    SEQ_LATE: An ISSUE_SEQ task could not be programmed by its start time.
    NO_DISPATCH: Error in ISSUE_SEQ update.
    INVALID_NUM_TRANS: Invalid number of transactions specified
    INVALID_PTR: Invalid PDMEM pointer specified in task.
    INVALID_SSBI_CHAN: Invalid SSBI channel specified.
    INVALID_RFFE_CHAN: Invalid RFFE channel specified.
    INVALID_SEQ_RES: Resource mismatch in seq header and seq task.
    INVALID_SEQ_DMA: Invalid number of DMA descriptors in seq.
*/
#define CCS_RF_ERR_NONE                 0
#define CCS_RF_ERR_PARAM_INVALID_TYPE   1 
#define CCS_RF_ERR_TIMING               2
#define CCS_RF_ERR_PREEMPT              3
#define CCS_RF_ERR_PARITY               4
#define CCS_RF_ERR_SEQ_LATE             5
#define CCS_RF_ERR_NO_DISPATCH             6
#define CCS_RF_ERR_PARAM_INVALID_NUM_TRANS 7  
#define CCS_RF_ERR_PARAM_INVALID_PTR       8  
#define CCS_RF_ERR_PARAM_INVALID_SSBI_CHAN 9  
#define CCS_RF_ERR_PARAM_INVALID_RFFE_CHAN 10 
#define CCS_RF_ERR_PARAM_INVALID_SEQ_RES   11 
#define CCS_RF_ERR_PARAM_INVALID_SEQ_DMA   12 

/*! @brief Task resources.
    Each RF channel is a separate resource. GRFCs have no resource, since no conflict.
    Each TQ also is a resource.
*/
#define CCS_RF_RESOURCE_SSBI0         (0)
#define CCS_RF_RESOURCE_SSBI_LAST     (CCS_RF_RESOURCE_SSBI0+CCS_RF_NUM_SSBI_CHAN-1)
#define CCS_RF_RESOURCE_RFFE0      (6)
#define CCS_RF_RESOURCE_RFFE_LAST     (CCS_RF_RESOURCE_RFFE0+CCS_RF_NUM_RFFE_CHAN-1)
#define CCS_RF_RESOURCE_TQ0           (CCS_RF_RESOURCE_RFFE_LAST+1)
#define CCS_RF_RESOURCE_TQ_LAST       (CCS_RF_RESOURCE_TQ0+CCS_RF_NUM_TASKQ-1)
#define CCS_RF_RESOURCE_LAST          (CCS_RF_RESOURCE_TQ_LAST)


/*! @brief Common task fields.
*/
typedef struct
{
  /* Word0 */
  uint32 trig_time     : 22; /*!< Trigger time */
  uint32 trig_type     : 2;  /*!< Trigger type (CCS_RF_TRIG_x) */
  uint32               : 4;
  uint32 type          : 4;  /*!< Task type (CCS_RF_TASK_x) */

  /* Word1 */
  uint32               : 8;
  uint32 mode          : 1;  /*!< mode = 0 legacy (tq used as resource), mode = 1 new (tq NOT used as resource) */  
  uint32 done_signal   : 7;  /*!< Signals to generate when task is complete */
  uint32 int_enable    : 1;  /*!< Generate interrupt when complete */
  uint32 preempt_error : 1;  /*!< Generate error if preempted */
  uint32               : 9;
  uint32 atomic        : 1;  /*!< Make task non-preemptable */
  uint32 priority      : 4;  /*!< Task priority */

  /* Word2 */
  uint32               : 32;

} ccs_rf_task_common_t;


/*! @brief Common task fields for SSBI and RFFE (not GRFC and not ISSUE_SEQ)
*/
typedef struct
{
  /* Word0 */
  uint32 trig_time     : 22; /*!< Trigger time */
  uint32 trig_type     : 2;  /*!< Trigger type (CCS_RF_TRIG_x) */
  uint32 channel       : 4;  /*!< SSBI/RFFE channel */
  uint32 type          : 4;  /*!< Task type (CCS_RF_TASK_x) */

  /* Word1 */
  uint32 num_transactions : 8; /*!< Number of transactions to issue */
  uint32 mode          : 1;  /*!< mode = 0 legacy (tq used as resource), mode = 1 new (tq NOT used as resource) */
  uint32 done_signal   : 7;  /*!< Signals to generate when task is complete */
  uint32 int_enable    : 1;  /*!< Generate interrupt when complete */
  uint32 preempt_error : 1;  /*!< Generate error if preempted */
  uint32               : 9;
  uint32 atomic        : 1;  /*!< Make task non-preemptable */
  uint32 priority      : 4;  /*!< Task priority */

  /* Word2 */
  uint32 transaction_ptr : 32; /*!< Pointer to transaction data */

} ccs_rf_task_common_non_grfc_t;


/*! @brief SSBI_WRITE task 
*/
typedef struct
{
  /* Word0 */
  uint32 trig_time     : 22; /*!< Trigger time */
  uint32 trig_type     : 2;  /*!< Trigger type (CCS_RF_TRIG_x) */
  uint32 channel       : 4;  /*!< SSBI channel */
  uint32 type          : 4;  /*!< Task type (CCS_RF_TASK_SSBI_WRITE) */

  /* Word1 */
  uint32 num_transactions : 8; /*!< Number of transactions to issue */
  uint32 mode          : 1;  /*!< mode = 0 legacy (tq used as resource), mode = 1 new (tq NOT used as resource) */
  uint32 done_signal   : 7;  /*!< Signals to generate when task is complete */
  uint32 int_enable    : 1;  /*!< Generate interrupt when complete */
  uint32 preempt_error : 1;  /*!< Generate error if preempted */
  uint32               : 9;
  uint32 atomic        : 1;  /*!< Make task non-preemptable */
  uint32 priority      : 4;  /*!< Task priority */

  /* Word2 */
  uint32 transaction_ptr : 32; /*!< Pointer to transaction data */

} ccs_rf_task_ssbi_write_t;


/*! @brief SSBI_ACCESS task 
*/
typedef struct
{
  /* Word0 */
  uint32 trig_time     : 22; /*!< Trigger time */
  uint32 trig_type     : 2;  /*!< Trigger type (CCS_RF_TRIG_x) */
  uint32 channel       : 4;  /*!< SSBI channel */
  uint32 type          : 4;  /*!< Task type (CCS_RF_TASK_SSBI_ACCESS) */

  /* Word1 */
  uint32 num_transactions : 8; /*!< Number of transactions to issue */
  uint32 mode          : 1;  /*!< mode = 0 legacy (tq used as resource), mode = 1 new (tq NOT used as resource) */
  uint32 done_signal   : 7;  /*!< Signals to generate when task is complete */
  uint32 int_enable    : 1;  /*!< Generate interrupt when complete */
  uint32 preempt_error : 1;  /*!< Generate error if preempted */
  uint32               : 9;
  uint32 atomic        : 1;  /*!< Make task non-preemptable */
  uint32 priority      : 4;  /*!< Task priority */

  /* Word2 */
  uint32 transaction_ptr : 32; /*!< Pointer to transaction data */

} ccs_rf_task_ssbi_access_t;


/*! @brief RFFE_WRITE task 
*/
typedef struct
{
  /* Word0 */
  uint32 trig_time     : 22; /*!< Trigger time */
  uint32 trig_type     : 2;  /*!< Trigger type (CCS_RF_TRIG_x) */
  uint32 channel       : 4;  /*!< RFFE channel */
  uint32 type          : 4;  /*!< Task type (CCS_RF_TASK_RFFE_WRITE) */

  /* Word1 */
  uint32 num_transactions : 8; /*!< Number of transactions to issue */
  uint32 mode          : 1;  /*!< mode = 0 legacy (tq used as resource), mode = 1 new (tq NOT used as resource) */
  uint32 done_signal   : 7;  /*!< Signals to generate when task is complete */
  uint32 int_enable    : 1;  /*!< Generate interrupt when complete */
  uint32 preempt_error : 1;  /*!< Generate error if preempted */
  uint32               : 2;
  uint32 slave_id      : 4;  /*!< RFFE slave ID, for all transactions */
  uint32               : 3;
  uint32 atomic        : 1;  /*!< Make task non-preemptable */
  uint32 priority      : 4;  /*!< Task priority */

  /* Word2 */
  uint32 transaction_ptr : 32; /*!< Pointer to transaction data */

} ccs_rf_task_rffe_write_t;


/*! @brief RFFE_ACCESS task 
*/
typedef struct
{
  /* Word0 */
  uint32 trig_time     : 22; /*!< Trigger time */
  uint32 trig_type     : 2;  /*!< Trigger type (CCS_RF_TRIG_x) */
  uint32 channel       : 4;  /*!< RFFE channel */
  uint32 type          : 4;  /*!< Task type (CCS_RF_TASK_RFFE_ACCESS) */

  /* Word1 */
  uint32 num_transactions : 8; /*!< Number of transactions to issue */
  uint32 mode          : 1;  /*!< mode = 0 legacy (tq used as resource), mode = 1 new (tq NOT used as resource) */
  uint32 done_signal   : 7;  /*!< Signals to generate when task is complete */
  uint32 int_enable    : 1;  /*!< Generate interrupt when complete */
  uint32 preempt_error : 1;  /*!< Generate error if preempted */
  uint32               : 9;
  uint32 atomic        : 1;  /*!< Make task non-preemptable */
  uint32 priority      : 4;  /*!< Task priority */

  /* Word2 */
  uint32 transaction_ptr : 32; /*!< Pointer to transaction data */

} ccs_rf_task_rffe_access_t;


/*! @brief RFFE_EXT task 
*/
typedef struct
{
  /* Word0 */
  uint32 trig_time     : 22; /*!< Trigger time */
  uint32 trig_type     : 2;  /*!< Trigger type (CCS_RF_TRIG_x) */
  uint32 channel       : 4;  /*!< RFFE channel */
  uint32 type          : 4;  /*!< Task type (CCS_RF_TASK_RFFE_WRITE_EXT) */

  /* Word1 */
  uint32 num_transactions : 8; /*!< Number of transactions to issue */
  uint32 mode          : 1;  /*!< mode = 0 legacy (tq used as resource), mode = 1 new (tq NOT used as resource) */
  uint32 done_signal   : 7;  /*!< Signals to generate when task is complete */
  uint32 int_enable    : 1;  /*!< Generate interrupt when complete */
  uint32 preempt_error : 1;  /*!< Generate error if preempted */
  uint32               : 9;
  uint32 atomic        : 1;  /*!< Make task non-preemptable */
  uint32 priority      : 4;  /*!< Task priority */

  /* Word2 */
  uint32 transaction_ptr : 32; /*!< Pointer to transaction data */

} ccs_rf_task_rffe_ext_t;


/*! @brief RFFE_WRITE_EXT task 
*/
typedef ccs_rf_task_rffe_ext_t ccs_rf_task_rffe_write_ext_t;


/*! @brief RFFE_READ_EXT task 
*/
typedef ccs_rf_task_rffe_ext_t ccs_rf_task_rffe_read_ext_t;


/*! @brief RFFE_ACCESS_EXT task 
*/
typedef struct
{
  /* Word0 */
  uint32 trig_time     : 22; /*!< Trigger time */
  uint32 trig_type     : 2;  /*!< Trigger type (CCS_RF_TRIG_x) */
  uint32 channel       : 4;  /*!< RFFE channel */
  uint32 type          : 4;  /*!< Task type (CCS_RF_TASK_RFFE_READ_EXT) */

  /* Word1 */
  uint32 num_transactions : 8; /*!< Number of transactions to issue */
  uint32 mode          : 1;  /*!< mode = 0 legacy (tq used as resource), mode = 1 new (tq NOT used as resource) */
  uint32 done_signal   : 7;  /*!< Signals to generate when task is complete */
  uint32 int_enable    : 1;  /*!< Generate interrupt when complete */
  uint32 preempt_error : 1;  /*!< Generate error if preempted */
  uint32               : 9;
  uint32 atomic        : 1;  /*!< Make task non-preemptable */
  uint32 priority      : 4;  /*!< Task priority */

  /* Word2 */
  uint32 transaction_ptr : 32; /*!< Pointer to transaction data */

} ccs_rf_task_rffe_access_ext_t;


/*! @brief GRFC_WRITE task 
*/
typedef struct
{
  /* Word0 */
  uint32 trig_time     : 22; /*!< Trigger time */
  uint32 trig_type     : 2;  /*!< Trigger type (CCS_RF_TRIG_x) */
  uint32 level         : 1;  /*!< 1=set high, 0=set low */
  uint32               : 3;
  uint32 type          : 4;  /*!< Task type (CCS_RF_TASK_GRFC_WRITE) */

  /* Word1 */
  uint32               : 8; 
  uint32 mode          : 1;  /*!< mode = 0 legacy (tq used as resource), mode = 1 new (tq NOT used as resource) */
  uint32 done_signal   : 7;  /*!< Signals to generate when task is complete */
  uint32 int_enable    : 1;  /*!< Generate interrupt when complete */
  uint32               : 11; 
  uint32 priority      : 4;  /*!< Task priority */

  /* Word2 */
  uint32 grfc_bitmask  : 32; /*!< GRFC[31:0] control bits */

} ccs_rf_task_grfc_write_t;


/*! @brief ISSUE_SEQ task 
*/
typedef struct
{
  /* Word0 */
  uint32 trig_time     : 22; /*!< Trigger time */
  uint32 trig_type     : 2;  /*!< Trigger type (CCS_RF_TRIG_x) */
  uint32               : 4;
  uint32 type          : 4;  /*!< Task type (CCS_RF_TASK_ISSUE_SEQ) */

  /* Word1 */
  uint32               : 8; 
  uint32 mode          : 1;  /*!< mode = 0 legacy (tq used as resource), mode = 1 new (tq NOT used as resource) */
  uint32               : 7;  /*!< common_t/non_grfc_common_t done_signal field */
  uint32 int_enable    : 1;  /*!< Generate interrupt when complete */
  uint32               : 11; 
  uint32 priority      : 4;  /*!< Task priority */

  /* Word2 */
  uint32 seq_ptr       : 32; /*!< Pointer to task sequence */

} ccs_rf_task_issue_seq_t;


/*! @brief ISSUE_SEQ update task 
*/
typedef struct
{
  /* Word0 */
  uint32 trig_time     : 22; /*!< Trigger time */
  uint32 trig_type     : 2;  /*!< Trigger type (CCS_RF_TRIG_x) */
  uint32               : 4;
  uint32 type          : 4;  /*!< Task type (CCS_RF_TASK_ISSUE_SEQ) */

  /* Word1 */
  uint32               : 8;  /*!< non_grfc_common_t num_transactions field */
  uint32 mode          : 1;  /*!< mode = 0 legacy (tq used as resource), mode = 1 new (tq NOT used as resource) */
  uint32               : 7;  /*!< common_t/non_grfc_common_t done_signal field */
  uint32 int_enable    : 1;  /*!< Generate interrupt when complete */
  uint32               : 1;  /*!< common_t/non_grfc_common_t preempt_error field */
  uint32 update_count  : 8;
  uint32 dispatch      : 1;
  uint32               : 1;  /*!< common_t atomic field */
  uint32 priority      : 4;  /*!< Task priority */

  /* Word2 */
  uint32 update_ptr    : 32; /*!< Pointer to update sequence */

} ccs_rf_task_issue_seq_update_t;


/*! @brief Union of all task types.
*/
typedef union
{
  ccs_rf_task_common_t          common;
  ccs_rf_task_common_non_grfc_t common_non_grfc;
  ccs_rf_task_ssbi_write_t      ssbi_write;
  ccs_rf_task_ssbi_access_t     ssbi_access;
  ccs_rf_task_rffe_write_t      rffe_write;
  ccs_rf_task_rffe_access_t     rffe_access;
  ccs_rf_task_rffe_write_ext_t  rffe_write_ext;
  ccs_rf_task_rffe_read_ext_t   rffe_read_ext;
  ccs_rf_task_rffe_ext_t        rffe_ext;
  ccs_rf_task_rffe_access_ext_t rffe_access_ext;
  ccs_rf_task_grfc_write_t      grfc_write;
  ccs_rf_task_issue_seq_t       issue_seq;
  ccs_rf_task_issue_seq_update_t issue_seq_update;
  uint32                        task_word[3];

} ccs_rf_task_t;



/*! @brief Task status information.
*/
typedef union
{
  struct {
    uint32 end_time : 22;  /*!< Task completion time */
    uint32          : 2;   
    uint32 error    : 4;   /*!< Task error (CCS_RF_ERR_x) */
    uint32 state    : 4;   /*!< Task state (CCS_RF_TASK_STATE_x) */
  };

  uint32 status_word;

} ccs_rf_task_status_t;


/*! @brief Task+status word pair.
*/
typedef struct
{
  ccs_rf_task_t        task;    /*!< Task parameters */
  ccs_rf_task_status_t status;  /*!< Task status */

} ccs_rf_task_buf_item_t;



/*! @brief Format of a single SSBI or RFFE transaction, used by reads and writes.
*/
typedef union
{
  struct {
    uint8 data;     /*!< SSBI/RFFE data (output for writes, input for reads) */
    uint8 addr;     /*!< SSBI/RFFE address (for RFFE, auto-determines transaction type) */
  };
  uint16 transaction_hword;
} ccs_rf_transaction_ssbi_rffe_rw_t;

/*! @brief Format of an SSBI or RFFE transaction pair.
*/
typedef union
{
  ccs_rf_transaction_ssbi_rffe_rw_t transaction[2];  /*!< Pair of transactions */
  uint32 transaction_word;
} ccs_rf_transaction_ssbi_rffe_rw_pair_t;

/*! @brief Format of an SSBI_ACCESS or RFFE_ACCESS transaction.
*/
typedef union
{
  struct {
    uint32 data      : 8;  /*!< SSBI data (output for writes, input for reads) */
    uint32 addr      : 8;  /*!< SSBI address */
    uint32 mask      : 8;  /*!< Bitmask (1=modify) for masked writes only */
    uint32 slave_id  : 4;  /*!< RFFE slave ID (ignored for SSBI) */
    uint32 half_rate : 1;  /*!< Perform read at half rate (RFFE read only) */
    uint32 rd_delay  : 1;  /*!< Insert extra cycle of delay before read-back */
    uint32 mode      : 2;  /*!< Transaction mode (see below) */
  #define CCS_RF_TRANSACTION_ACCESS_MODE_WRITE_MASKED 1
  #define CCS_RF_TRANSACTION_ACCESS_MODE_WRITE        2
  #define CCS_RF_TRANSACTION_ACCESS_MODE_READ         3
  };
  uint32 transaction_word;
} ccs_rf_transaction_ssbi_rffe_access_t;

/*! @brief Aliases for SSBI/RFFE transactions.
*/
typedef ccs_rf_transaction_ssbi_rffe_rw_t      ccs_rf_transaction_ssbi_rw_t;
typedef ccs_rf_transaction_ssbi_rffe_rw_t      ccs_rf_transaction_rffe_rw_t;
typedef ccs_rf_transaction_ssbi_rffe_rw_pair_t ccs_rf_transaction_ssbi_rw_pair_t;
typedef ccs_rf_transaction_ssbi_rffe_rw_pair_t ccs_rf_transaction_rffe_rw_pair_t;
typedef ccs_rf_transaction_ssbi_rffe_access_t  ccs_rf_transaction_ssbi_access_t;
typedef ccs_rf_transaction_ssbi_rffe_access_t  ccs_rf_transaction_rffe_access_t;


/*! @brief Format of RFFE extended transactions (read or write).
*/
typedef union
{
  struct {
    uint32 addr        : 16;  /*!< RFFE address */
    uint32 mode        : 2;   /*!< Transaction mode (see below) */
  #define CCS_RF_TRANSACTION_ACCESS_EXT_MODE_WRITE     2
  #define CCS_RF_TRANSACTION_ACCESS_EXT_MODE_READ      3
    uint32 num_data    : 4;   /*!< Number of data bytes (0=1 byte, 3=4 bytes, 15=16 bytes) */
    uint32 slave_id    : 4;   /*!< RFFE slave ID */
    uint32 half_rate   : 1;   /*!< Half-rate read */
    uint32 rd_delay    : 1;   /*!< Insert extra cycle of delay before read-back */
    uint32 type        : 2;   /*!< Transaction type (see below) */
  #define CCS_RF_TRANSACTION_RFFE_EXT_TYPE_REG0      0
  #define CCS_RF_TRANSACTION_RFFE_EXT_TYPE_REG       1
  #define CCS_RF_TRANSACTION_RFFE_EXT_TYPE_EXT       2
  #define CCS_RF_TRANSACTION_RFFE_EXT_TYPE_EXT_LONG  3
    uint32             : 2;   /*!< ** MUST be set to 0 */

    uint8 data[4];            /*!< Transaction data */
  };

  uint32 transaction_word[2];

} ccs_rf_transaction_rffe_ext_rw_t;



/*! @brief Single task queue interface.
*/
typedef struct
{
  volatile uint32 enable;       /*!< Enable flag, controlled by DSP */
  volatile uint32 wr_idx;       /*!< Next buffer write location */
  volatile uint32 rd_idx;       /*!< Next buffer execute location */
  volatile uint32 active_tasks; /*!< Bitmask of tasks currently active, in some stage */
  volatile uint32 int_pending;  /*!< Bitmask of interrupts pending */
  volatile uint32 int_clear;    /*!< Written by DSP to clear interrupts */

  ccs_rf_task_buf_item_t task_buf[CCS_RF_TASKQ_NUM_ITEMS]; /*!< Task buffer */

} ccs_rf_taskq_intf_t;


/*! @brief Global flags */
typedef union
{
  uint32 _options;
  struct {
    uint32 ignore_parity  : 1;   /*!< When set, parity read errors are ignored.
                                      Useful for testing without RF devices connected. */ 
    uint32                : 31;
  };
} ccs_rf_intf_options_t;


/*! @brief CCS RF shared memory interface.
*/
typedef struct
{
  /*! Version of the CCS RF module */
  uint32 version;         

  /*! Global flags */
  ccs_rf_intf_options_t options;

  uint32 debug;             /*!< Enable internal debug features. */

  /*! Task queues */
  ccs_rf_taskq_intf_t taskq[CCS_RF_NUM_TASKQ]; 

  /*! Task transaction data buffer, shared by all task queues */
  uint32 data_buf[CCS_RF_TASKQ_DATA_BUF_WORDS];

  uint8 resource_to_dma_chan[(CCS_RF_RESOURCE_RFFE_LAST+1+3)&(~3)];

} ccs_rf_intf_t;


/*! @brief Transaction sequence header.
    For internal and sequence generator use only.
*/
typedef struct
{
  uint32 start_time_ustmr;  /*!< Reference start time of sequence, in USTMR units */
  uint16 duration_ustmr;    /*!< Duration of sequence, in USTMR units */
  uint16 resources;         /*!< Resources used by sequence */
  uint8 num_dma;            /*!< Number of DMA descriptors */
  uint8 num_timing_fixups;  /*!< Number of timing fixups */

  /* Remainder of sequence is variable-length:
     dmac_descriptor_t dma[num_dma];
     uint8 timing_fixup[num_timing_fixups];
   */

} ccs_rf_seq_header_t;



/*! @brief Format of RFFE extended write transaction control.
*/
typedef union
{
  struct {
    uint32 addr        : 16;  /*!< RFFE address */
    uint32 mode        : 2;   /*!< Transaction mode (see below) */
  #define CCS_RF_TRANSACTION_ACCESS_EXT_MODE_WRITE     2
  #define CCS_RF_TRANSACTION_ACCESS_EXT_MODE_READ      3
    uint32 num_data    : 4;   /*!< Number of data bytes (0=1 byte, 3=4 bytes, 15=16 bytes) */
    uint32 slave_id    : 4;   /*!< RFFE slave ID */
    uint32 half_rate   : 1;   /*!< Half-rate read */
    uint32 rd_delay    : 1;   /*!< Insert extra cycle of delay before read-back */
    uint32 type        : 2;   /*!< Transaction type (see below) */
  #define CCS_RF_TRANSACTION_RFFE_EXT_TYPE_REG0      0
  #define CCS_RF_TRANSACTION_RFFE_EXT_TYPE_REG       1
  #define CCS_RF_TRANSACTION_RFFE_EXT_TYPE_EXT       2
  #define CCS_RF_TRANSACTION_RFFE_EXT_TYPE_EXT_LONG  3
    uint32             : 2;   /*!< ** MUST be set to 0 */

  };

  uint32 transaction_ctl;/*!< Transaction Control */

} ccs_rf_transaction_rffe_ext_control_rw_t;

/*! @brief Format of RFFE extended write transactions data.
*/
typedef union
{
  uint8 data[4];            
  uint32 transaction_data;/*!< Transaction Data */
} ccs_rf_transaction_rffe_ext_data_rw_t;


typedef struct
{
  uint32 seq_ptr;
} ccs_rf_seq_update_header_t;


#endif /* CCS_RF_INTF_H */
