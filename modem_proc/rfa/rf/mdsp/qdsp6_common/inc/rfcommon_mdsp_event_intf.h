/*!
  @file
  rfcommon_mdsp_event_intf.h

  @brief
  Interface for building and working with events
 
*/
#ifndef RFCOMMON_MDSP_EVENT_INTF_H
#define RFCOMMON_MDSP_EVENT_INTF_H

/*===========================================================================
Copyright (c) 2012 - ? by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/mdsp/qdsp6_common/inc/rfcommon_mdsp_event_intf.h#1 $
$DateTime: 2016/12/13 07:59:52 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/04/14   rmb     Ported rfcommon_mdsp_event_start_opt() on Bolt from Dime.
12/04/13   whz     Added event status_update API
10/01/13   ra      Add support to for new CCS event
04/15/13   jfc     Add additional event IO cases: num_tasks, num_trans
03/21/13   zhw     Added support for Antenna Tuner Open Loop common module 
11/14/12   jfc     Replace RFFE_READ with RFFE ACCESS (EXT included) 
10/30/12   jfc     Add event IO API 
10/09/12   jfc     Add event abort API 
08/08/12   jfc     Make task type access compatible with different interface versions 
08/06/12   jfc     Optimize event builder for time
07/20/12   jfc     Move RF event IDs to rf_event_intf.h; add priority_if_atomic field 
                    Change convert_to_ustmr signature 
07/19/12   jfc     Decouple CCS values in this file
07/18/12   jfc     Fix trigger type enum values to CCS values
07/11/12   jfc     Add time offset and time coversion
07/10/12   jfc     Export rfcommon_mdsp_event_builder_init()
07/09/12   spa     Updated rfcommon_mdsp_event_push to accept const pointer  
                   to resource script for rf_hal_buffer compatibility 
07/05/12   jfc     Fix prototype for rfcommon_mdsp_event_handle_fwipc()
06/06/12   Saul    CDMA. Sending scripts as Dime shared mem format.
                   MC now using mdsp events.
05/21/12   jfc     Add rfcommon_mdsp_event_recommit()
05/16/12   jfc     Tentatively remove int_enable and done_signal
05/13/12   jfc     Add interface for cleanup and FWIPC event handling
05/09/12   jfc     Add another error value to rfcommon_mdsp_event_error_type
05/07/12   jfc     Initial version
============================================================================*/

#include "rf_hal_bus_types.h"
#include "rflm_rf_event_intf.h"

#define RFCOMMON_MDSP_EVENT_ID_BASE_GSM     RFLM_RF_EVENT_ID_BASE_GSM    
#define RFCOMMON_MDSP_EVENT_ID_BASE_WCDMA   RFLM_RF_EVENT_ID_BASE_WCDMA  
#define RFCOMMON_MDSP_EVENT_ID_BASE_CDMA    RFLM_RF_EVENT_ID_BASE_CDMA   
#define RFCOMMON_MDSP_EVENT_ID_BASE_LTE     RFLM_RF_EVENT_ID_BASE_LTE    
#define RFCOMMON_MDSP_EVENT_ID_BASE_TDSCDMA RFLM_RF_EVENT_ID_BASE_TDSCDMA
#define RFCOMMON_MDSP_EVENT_ID_BASE_IRAT    RFLM_RF_EVENT_ID_BASE_IRAT  
#define RFCOMMON_MDSP_EVENT_ID_BASE_RFCMD   RFLM_RF_EVENT_ID_BASE_RFCMD
                                                                    
#define RFCOMMON_MDSP_EVENT_MAX_TECH_ID     RFLM_RF_EVENT_MAX_TECH_ID    


#define CCS_TASK_TRANS_PTR(t) ((t).task.common_non_grfc.transaction_ptr)
#define CCS_TASK_NUM_TRANS(t) ((t).task.common_non_grfc.num_transactions)
#define CCS_TASK_TYPE(t)      ((t).task.common.type)
#define CCS_TASK_STATUS(t)    ((t).status.status_word)
#define CCS_TASK_CHANNEL(t)   ((t).task.common_non_grfc.channel)
#define CCS_TASK_PRIORITY(t)  ((t).task.common.priority)
#define CCS_TASK_TRIG_TYPE(t) ((t).task.common.trig_type)
#define CCS_TASK_TRIG_TIME(t) ((t).task.common.trig_time)
#define CCS_TASK_PREEMPT_ERROR(t) ((t).task.common.preempt_error)

#define CCS_TASK_GRFC_MASK(t) ((t).task.grfc_write.grfc_bitmask)
#define CCS_TASK_GRFC_LEVEL(t) ((t).task.grfc_write.level)

#define CCS_TASK_RFFE_SLAVE_ID(t)  ((t).task.rffe_write.slave_id)


/*! @brief Opaque internal structure to hold event state and data
  Pointer to handle is to be passed around and operated on via the 
  exported interfaces in this file
*/
typedef struct _event_internal_state_type rfcommon_mdsp_event_handle;


/*! @brief MDSP event error values
*/
typedef enum
{
  RFCOMMON_MDSP_EVENT_SUCCESS = 0,                  /*!< Operation completed successfully */
  RFCOMMON_MDSP_EVENT_FAIL_MEMORY_OVERFLOW = 1,     /*!< Allocated buffer is not large enough to hold event data */
  RFCOMMON_MDSP_EVENT_FAIL_EVENT_NOT_WRITEABLE = 2, /*!< Event is in a state not writeable by software */
  RFCOMMON_MDSP_EVENT_FAIL_EVENT_NOT_COMPLETE = 3,  /*!< Event has not been transacted */
  RFCOMMON_MDSP_EVENT_FAIL_BAD_PARAM = 4,           /*!< Bad parameter */
  RFCOMMON_MDSP_EVENT_FAIL_BAD_INTERNAL_STATE = 5,  /*!< Internal state is corrupted or unexpected */
  RFCOMMON_MDSP_EVENT_FAIL_OTHER = 6,               /*!< Other error */
} rfcommon_mdsp_event_error_type;

/*! @brief Possible IO operations that can be queried from an event handle */
typedef enum
{
  RFCOMMON_MDSP_EVENT_OUT_EVENT_ID,     /*!< Get the the event ID */
  RFCOMMON_MDSP_EVENT_OUT_STATUS,       /*!< Get the event status */
  RFCOMMON_MDSP_EVENT_OUT_SM_PTR,       /*!< Get a pointer to the shared memory buffer */
  RFCOMMON_MDSP_EVENT_OUT_PRIORITY,     /*!< Get the priority of the event */
  RFCOMMON_MDSP_EVENT_OUT_CALLBACK,     /*!< Get a pointer the callback function */
  RFCOMMON_MDSP_EVENT_OUT_MAKE_ATOMIC,  /*!< Get the make atomic flag value */
  RFCOMMON_MDSP_EVENT_OUT_RESPONSE_REQ, /*!< Get the response required flag value */
  RFCOMMON_MDSP_EVENT_OUT_NUM_TASKS,    /*!< Get the number of tasks in an event */
  RFCOMMON_MDSP_EVENT_OUT_NUM_TRANSACTIONS,/*!< Get the number of total non-GRFC transactions in event. Output requires uint32 type */
} rfcommon_mdsp_event_handle_io_enum;

/*! @brief MDSP event settings type
*/
typedef struct
{
  boolean make_atomic;              /*!< Make this event atomic (e.g. ISSUE_SEQ)*/
  boolean response_required;        /*!< Callback is required for this event */
  void (*callback_ptr) (uint32);    /*!< Pointer to callback function */
  int32 (*convert_time_func)(int32); /*!< Function pointer to convert tech specific units to USTMR (or whatever) */
  uint8 priority_if_atomic;         /*!< Specify the priority of the event if it is atomic; if event is not atomic, this field is ignored */
  uint8 max_tasks_allocated;        /*!< Specify the max number of tasks that have been allocated for this event (in terms of memory)
                                          Field is optional. Specify zero to get backwards compatibility. Otherwise an accurate value
                                          will speed up performance */
} rfcommon_mdsp_event_settings_type;


/*! @brief Trigger types enum 
*/
#define RFCOMMON_MDSP_TRANSACTION_TRIGGER_IMMEDIATE_VALUE   0
#define RFCOMMON_MDSP_TRANSACTION_TRIGGER_DYNAMIC_VALUE     2
typedef enum
{
  RFCOMMON_MDSP_TRANSACTION_TRIGGER_IMMEDIATE = RFCOMMON_MDSP_TRANSACTION_TRIGGER_IMMEDIATE_VALUE,
  RFCOMMON_MDSP_TRANSACTION_TRIGGER_DYNAMIC   = RFCOMMON_MDSP_TRANSACTION_TRIGGER_DYNAMIC_VALUE,
} rfcommon_mdsp_transaction_trigger_type;


/*! @brief Settings for the script pushed into an event 
*/
typedef struct
{
  rfcommon_mdsp_transaction_trigger_type trigger_type;  /*!< Script trigger type */
  uint8 priority;                   /*!< Script priority */
  boolean preempt_error;            /*!< Preempt error */
  //boolean int_enable;               /*!< Enable interrupts */
  //uint8 done_signal;                /*!< Done signal mask */
  boolean use_ssbi_access;          /*!< Applicable only to SSBI transactions. If set, then SSBI_ACCESS task type is used.
                                         So reads and writes in the same script can be in a single script.
                                         Otherwise, reads and writes will cause multiple tasks to be created. This flag should
                                         only be set if there are SSBI reads and writes that are issued back to back; if the
                                         transactions are spaced out in time, then they are separate tasks anyway so there is no
                                         need for the ACCESS type.
                                     */
  int32 time_offset;                 /*!< Time offset to apply to all transactions. Units should be same as the transactions */
} rfcommon_mdsp_transaction_settings_type;


/*!
  @brief Initialize the event builder 
  */
boolean rfcommon_mdsp_event_builder_init
(
  void
);

/*!
  @brief Start event building

  @details
  This function initiates the event building process

  @param event_id 
  The event ID to start

  @param shared_mem_buf_ptr
  Address of the shared memory buffer

  @param shared_mem_max_size_bytes
  Size in bytes of the shared memory buffer

  @param settings
  Pointer to settings held in a structure

  @param handle
  Pointer to pointer to hold event handle. If call is successful,
  *handle will be a pointer to opaque struct

  @return
  Returns an enum status value. Unlikely to fail since this function
  triggers mostly internal operations.
 
*/
rfcommon_mdsp_event_error_type rfcommon_mdsp_event_start 
(
  uint32 event_id, 
  void * shared_mem_buf_ptr, 
  uint32 shared_mem_max_size_bytes, 
  rfcommon_mdsp_event_settings_type* settings,
  rfcommon_mdsp_event_handle** handle
);

/*!
  @brief Start event building

  @details
  This function initiates the event building process

  @param event_id 
  The event ID to start

  @param shared_mem_buf_ptr
  Address of the shared memory buffer

  @param shared_mem_max_size_bytes
  Size in bytes of the shared memory buffer

  @param settings
  Pointer to settings held in a structure

  @param handle
  Pointer to pointer to hold event handle. If call is successful,
  *handle will be a pointer to opaque struct

  @param enable_rffe_task_merge
  Allow RFFE tasks to be merged

  @return
  Returns an enum status value. Unlikely to fail since this function
  triggers mostly internal operations.
 
*/

rfcommon_mdsp_event_error_type rfcommon_mdsp_event_start_opt
(
  uint32 event_id, 
  void * shared_mem_buf_ptr, 
  uint32 shared_mem_max_size_bytes, 
  rfcommon_mdsp_event_settings_type* settings,
  rfcommon_mdsp_event_handle** handle,
  boolean enable_rffe_task_merge
);


/*!
  @brief Push transactions from script into event

  @details
  This function pushes transactions into an event. Each transaction
  is processed and placed into either a new or existing task in the
  event.

  @param handle
  Pointer to the event handle that was created from _start() function

  @param trans_script
  Pointer struct of a resource (RFFE, SSBI, GRFC) script

  @param params
  Pointer to settings for this script held in a structure

  @return
  Returns an enum status value.
  RFCOMMON_MDSP_EVENT_FAIL_MEMORY_OVERFLOW is a possibility 

*/
rfcommon_mdsp_event_error_type rfcommon_mdsp_event_push 
( 
  rfcommon_mdsp_event_handle* handle, 
  const rf_hal_bus_resource_script_type* trans_script, 
  rfcommon_mdsp_transaction_settings_type* params,
  boolean force_new_task
);


/*!
  @brief Commit the event

  @details
  Finalize the tasks and transactions in this event. The final
  data will be written into the shared memory buffer.

  @param handle
  Pointer to the event handle that was created from _start() function

  @return
  Returns an enum status value. 
  RFCOMMON_MDSP_EVENT_FAIL_MEMORY_OVERFLOW is a possibility 
*/
rfcommon_mdsp_event_error_type rfcommon_mdsp_event_commit 
(
  rfcommon_mdsp_event_handle* handle
);


/*!
  @brief Re-commit the event

  @details
  Reset the event to committed state without modifying any settings or data.
  The event shall be in the COMPLETE state

  @param handle
  Pointer to the event handle that was created from _start() function

  @return
  Returns an enum status value. 
  
*/
rfcommon_mdsp_event_error_type rfcommon_mdsp_event_recommit 
(
  rfcommon_mdsp_event_handle* handle
);


/*!
  @brief Abort the event

  @details
  Call this function if an event has been committed or is building, but the event shared memory status
  needs to be reset. This function will set the event status back to IDLE.
 
  Caution must be exercised when calling this function if the event state is COMMITTED
  since FW might be reading the shared memory

  @param handle
  Pointer to the event handle that was created from _start() function

  @return
  Returns an enum status value. 
  
*/
rfcommon_mdsp_event_error_type rfcommon_mdsp_event_abort 
(
  void* sm_ptr
);


/*! 
  @brief Update the status of the event

  @details
  Call this function if an event has been completed, but the event shared memory status
  needs to be reset. This function will set the event status back to COMMITTED.
 
  Caution must be exercised when calling this function if the event state is COMMITTED
  since FW might be reading the shared memory

  @param handle
  Pointer to the event handle that was created from _start() function

  @return
  Returns an enum status value. 
  
*/
rfcommon_mdsp_event_error_type rfcommon_mdsp_event_status_update 
(
  void* sm_ptr
);


/*! 
  @brief Perform IO query from handle
 
  @param handle
  Pointer to the event handle that was created from event_start() function
 
  @param operation
  Enum of what operation to perform
 
  @param data
  Pointer to allocated memory that is appropriate to hold the field requested
 
  @return
  Returns an enum status value of the operation
*/
rfcommon_mdsp_event_error_type rfcommon_mdsp_event_handle_io
(
  rfcommon_mdsp_event_handle* handle,
  rfcommon_mdsp_event_handle_io_enum operation,
  void* data
);

/*!
  @brief Query SSBI read data from event

  @details
  May be called after event enters COMPLETE state. This function 
  will fetch the SSBI read data.

  @param event_id 
  The event ID

  @param buf_ptr
  Pointer to memory allocated for SSBI transactions

  @param max_num
  Max number SSBI transactions the buffer can hold

  @param num_retrieved
  Pointer to variable to hold the number of transactions returned

  @return
  Returns an enum status value.
  RFCOMMON_MDSP_EVENT_FAIL_EVENT_NOT_COMPLETE is a possibility
*/
rfcommon_mdsp_event_error_type rfcommon_mdsp_event_query_ssbi_data 
(
  rfcommon_mdsp_event_handle* handle, 
  rf_hal_bus_sbi_type* buf_ptr, 
  uint16 max_num, 
  uint16* num_retrieved
);


/*!
  @brief Query RFFE read data from event

  @details
  May be called after event enters COMPLETE state. This function 
  will fetch the RFFE read data.

  @param event_id 
  The event ID

  @param buf_ptr
  Pointer to memory allocated for RFFE transactions

  @param max_num
  Max number RFFE transactions the buffer can hold

  @param num_retrieved
  Pointer to variable to hold the number of transactions returned

  @return
  Returns an enum status value.
  RFCOMMON_MDSP_EVENT_FAIL_EVENT_NOT_COMPLETE is a possibility
*/
rfcommon_mdsp_event_error_type rfcommon_mdsp_event_query_rffe_data 
(
  rfcommon_mdsp_event_handle* handle,
  rf_hal_bus_rffe_type* buf_ptr, 
  uint16 max_num, 
  uint16* num_retrieved
);


/*!
  @brief Query task status data

  @details
  May be called after event enters COMPLETE state. This function 
  will fetch the task return status of each task in the event.

  @param event_id 
  The event ID

  @param buf_ptr
  Pointer to memory allocated for status words

  @param max_num
  Max number of status words buf_ptr can holder

  @param num_retrieved
  Pointer to variable to hold the number of words returned

  @return
  Returns an enum status value.
  RFCOMMON_MDSP_EVENT_FAIL_EVENT_NOT_COMPLETE is a possibility
*/
rfcommon_mdsp_event_error_type rfcommon_mdsp_event_query_task_status 
( 
  rfcommon_mdsp_event_handle* handle,
  uint32* buf_ptr, 
  uint16 max_num, 
  uint16* num_retrieved
);

/*!
  @brief Do final cleanup for event

  @param handle
  A pointer to pointer to handle. Memory for handle will be freed
  Then the handle pointer will be set to NULL

  @return void
  */
void rfcommon_mdsp_event_cleanup
(
  rfcommon_mdsp_event_handle** handle
);


/*!
  @brief Handle FWIPC to handle event response 

  @details FW will send a interrupt to SW to inform it that some event
  has finished. SW will check all pending events and call the callback
  functions if there is data to be queried
  
  @return void
  */
void rfcommon_mdsp_event_handle_fwipc
(
  void
);

#endif /* RFCOMMON_MDSP_EVENT_INTF_H */
