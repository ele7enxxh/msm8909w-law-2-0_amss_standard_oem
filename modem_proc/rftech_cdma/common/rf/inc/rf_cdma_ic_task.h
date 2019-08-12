#ifndef RF_CDMA_IC_TASK_H
#define RF_CDMA_IC_TASK_H
/*! 
  @file
  rf_cdma_ic_task.h
 
  @brief
  This file includes functions pertaining to Tasks used for CDMA 
  intelliceiver Operation 
*/ 
 
/*==============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/inc/rf_cdma_ic_task.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
03/28/12   aro     Added commands to PAUSE, RESUME, and PM OVERRIDE
11/30/11   aki     Removed "rex_types.h" as it breaks off-target when included 
                   before rex.h.
10/26/11   aro     Implemented mechanism not to start iC task until a signal
                   is set during intelliceiver initialization
10/26/11   aro     Deleted obselete test function
10/25/11   aro     Added housekeeping variable to keep track of 1x and HDR
                   active device to be used during handling of iC Update rsp
10/25/11   aro     Renamed command ID enums
10/25/11   aro     Renamed the names of iC Task functions to maintain
                   consistency
10/25/11   aro     Implemented unified dispatch table to handle both REX and
                   MSGR based commands.
10/24/11   aro     Cleaned up unused timers
10/24/11   aro     Implemented an interface to put RF iC command to task Q
10/24/11   aro     Added interface to get the free command buffer
10/24/11   aro     Removed Watchdog implementation for RF iC Task
10/21/11   aro     Deleted extra space
10/21/11   aro     Added function to initialize the task queue commands
10/21/11   aro     Added function for REX MSGR initialization
10/21/11   aro     Created WATCHDOG initialization and kick functions
10/21/11   aro     Moved WATCHDOG variable out from the data structure
10/20/11   aro     Added Code to initialize watchdog timer
10/20/11   aro     Deleted prototype for rf_ic_task()
10/20/11   aro     Added iC Task data structure types
10/20/11   aro     Doxygen documentation Update
10/20/11   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "msgr_types.h"
#include "rex.h"
#include "queue.h"
#include "rfa_msgr.h"
#include "rfcommon_time_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!< Signal for when commands available in  RF command queue. */
#define RF_CDMA_IC_CMD_Q_SIG          0x0001

/*----------------------------------------------------------------------------*/
/*!< Signal for RF iC Task to task. */
#define RF_CDMA_IC_TASK_START_SIG    0x0002

/*----------------------------------------------------------------------------*/
/*!<  Signal for RF task clock wait. */
#define RF_CDMA_IC_CLK_WAIT_SIG       0X0020

/*----------------------------------------------------------------------------*/
/*! Maximum payload size for RF commands */
#define RF_CDMA_IC_MAX_CMD_PAYLOAD_SIZE 72

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Maximum number of commands that RF queue can hold.

  @details
  The number of commands that can be handled by RF command queue at once. 
  If we get more than the number of command then we will drop those commands. 
  calling module should handle the case when the command is not accepted.
*/
#define RF_CDMA_IC_CMD_QUEUE_SIZE 10

/*----------------------------------------------------------------------------*/
/*! Starting address of Intelliceiver cmd */
#define RF_CDMA_IC_REX_COMMAND_MIN (MSGR_MODULE_RF_1X << 4)

/*----------------------------------------------------------------------------*/
/*! Enumeration of RF CDMA Tasks Command IDs */
typedef enum
{

  RF_CDMA_IC_ENTER_STEADY_STATE_C = RF_CDMA_IC_REX_COMMAND_MIN,
  /*!< Command to enter Steady State mode for intelliceiver */

  RF_CDMA_IC_START_POLLING_C,
  /*!< Command to start polling for intelliceiver in steady state mode */

  RF_CDMA_IC_PAUSE_IC_C,
  /*!< Command to pause intelliceiver operation */

  RF_CDMA_IC_RESUME_IC_C,
  /*!< Command to resume intelliceiver operation */

  RF_CDMA_IC_OVERRIDE_PM_C,
  /*!< Command to override intelliceiver power mode */

  RF_CDMA_IC_CLEAR_OVERRIDE_PM_C,
  /*!< Command to clear override power mode */
  
  RF_CDMA_IC_COMMAND_MAX,
  /*!< Maximum enumeration for CDMA Task command */

} rf_cdma_ic_command_cmd_id_type;

/*----------------------------------------------------------------------------*/
/*! Enumeration indicating the type of source from which the command is 
received */
typedef enum
{

  RF_CDMA_IC_REX, /*!< Enum to indicate that the message is from REX */

  RF_CDMA_IC_MSGR  /*!< Enum to indicate that the message is from MSGR */

} rf_cdma_ic_cmd_source_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The RF command related status enum. 

  @details
  This enum specifies the related states and status of the command in the queue.
*/
typedef enum
{
  RF_CDMA_IC_CMD_BUF_EMPTY,
  /*!< Queue buffer is empty and not in use. */

  RF_CDMA_IC_CMD_BUF_ALLOCATED,
  /*!< Buffer allocated for calling module to add command to queue. */

  RF_CDMA_IC_CMD_QUEUED,
  /*!< Buffer filled with valid information and added to Queue. */

  RF_CDMA_IC_CMD_PROCESSING,
  /*!< Command currently being processed. */

  RF_CDMA_IC_CMD_NOT_SUPPORTED,
  /*!< Command not supported. */

  RF_CDMA_IC_CMD_QUEUE_FULL,
  /*!< Command Queue full. */

  RF_CDMA_IC_CMD_FAILURE,
  /*!< Command Failed during processing. */

  RF_CDMA_IC_CMD_SUCCESS,
  /*!< Command Processing successful. */

} rf_cdma_ic_cmd_status_e_type;

/*----------------------------------------------------------------------------*/
/*! Intelliceiver command ID */
typedef uint8 rf_cdma_ic_cmd_id_type;

/*----------------------------------------------------------------------------*/
/*! The command size for the data associated with the command. The maximum 
size is defined by #RF_CDMA_IC_MAX_CMD_PAYLOAD_SIZE */
typedef uint32 rf_cdma_ic_cmd_payload_size_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The RF CDMA iC command header. 

  @details
  The RF command header includes the module ID and command ID. This strucure is 
  supposed to be 32 bit for compatibilty with message router.
*/
typedef PACK(struct)
{
  /* NOTE: Dont not change the sequence of structures in the rf_cmd_hdr_type 
  structure. The following struct order is required. MSGR copies the UMID into 
  the rf cmd hdr in this order.*/
  rf_cdma_ic_cmd_id_type cmd_id;
  /*!< Command ID for handling the command */
   
  uint8 type_id;
  /*!< Type ID of the command */
 
  uint8 module_id;
  /*!< Module responsible for handling the command */    
 
  uint8 tech_id; 
  /*!< System level tech ID - usually message router tech ID. In most cases 
  ignored.*/
 
} rf_cdma_ic_cmd_hdr_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The strucutre holds details about the RF header. 

  @details This structure holds details about RF command header.
  It includes command header the link and cmd_hdr type. This size of 
  this structure provides the REX MSGR Interworking code the offset to 
  the MSGR structure. 
*/
typedef struct ALIGN(8)
{

  q_link_type            link;
  /*!<  Q linking element -  This element has to be the first element in this 
  data structure because of removal of self_ptr in q_link_type */

  rf_cdma_ic_cmd_hdr_type  cmd_hdr;
  /*!< Type of RF command */

} rf_cdma_ic_cmd_rex_hdr_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The structure holds details about RF command. 

  @details This structure holds details about RF command.
  It includes command header, payload, status, queue links.
*/
typedef struct
{
  rf_cdma_ic_cmd_rex_hdr_type   rex_hdr;
  /*!< REX header */
  
  msgr_hdr_struct_type   msgr_hdr;
 /*!<  MSGR Header for targets using MSGR. IMPORTANT: This following field must 
 start on a 8-byte aligned address  */
  
  byte                   payload[RF_CDMA_IC_MAX_CMD_PAYLOAD_SIZE];
  /*!< RF command payload */  
  
  /* Other debug information for the command */
  rf_time_tick_type  alloc_time; 
  /*!< time when CMD was added to queue */
 
  rf_cdma_ic_cmd_status_e_type  status;
  /*!< Command Status of the command */
  
  rex_sigs_type          complete_sig;
  /*!< Signal to send upon execution of command */
  
  rex_tcb_type         * task_to_sig;
  /*!< task to signal upon execution of command */
  
} rf_cdma_ic_cmd_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The RF CDMA iC command queue data structure type. 

  @details
  This structure holds the storage for command queue and queue link strucutres.
*/
typedef struct 
{

  rf_cdma_ic_cmd_type q_items[ RF_CDMA_IC_CMD_QUEUE_SIZE ];
  /*!< Storage for command items */
    
  q_type cmd_q;
  /*!< Queue of currently active RF commands */  

  q_type free_q;
  /*!< Queue of free command structures */  

} rf_cdma_ic_cmd_q_struct_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer for the handler function associated with each command.

  @details
  The function pointer prototype associated with each command.

  @param cmd
  The command for which dispatcher is called.  

  @param cmd_payload_size
  The size of the message associated with the command.  

  @param payload
  The data associated with the command. The size of data is specified in 
  payload_size.
*/
typedef void (*rf_cdma_ic_cmd_handler_type)
(
  rf_cdma_ic_cmd_id_type ic_cmd, 
  rf_cdma_ic_cmd_payload_size_type cmd_payload_size, 
  rf_cdma_ic_cmd_type* cmd_ptr
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure for command dispatcher table entry.

  @details
  Structure for command dispatcher table entry.
  Stores the command ID, function pointer to handler and 
  payload size.
*/
typedef struct
{

  rf_cdma_ic_cmd_source_type cmd_src;
  /*!< Variable indicating the type of source from which the command is 
  received */

  rf_cdma_ic_cmd_id_type cmd_id;
  /*!< Command ID for the command. This is same as index of the table. 
  Stored for validation or hash table if needed. */

  msgr_umid_type cdma_umid;
  /*!< This field defines the MSGR UMID, if the command is sent through MSGR. 
  For REX commands, this field is insignificant. */

  rf_cdma_ic_cmd_handler_type cmd_handler;
  /*!< function pointer to the handler function */

  rf_cdma_ic_cmd_payload_size_type payload_size;
  /*!< Command palyoad size - stored for validation */

} rf_cdma_ic_cmd_dispatch_tbl_type;

/*----------------------------------------------------------------------------*/
/*! Data structure to hold RF_IC_TASK dispatch tabel */
typedef struct
{
  boolean init_done;
  /*!< Flag indicating, if the dispatch table is initialized successfully */

  rf_cdma_ic_cmd_dispatch_tbl_type *dispatch_tbl;
  /*!< Table pointer for command disptcher. Index zero equals min_cmd_id */

} rf_cdma_ic_dispatch_tbl_type;

/*----------------------------------------------------------------------------*/
/*! Sturcture to Hold RF CDMA IC task msgr related information. */
typedef struct
{

  msgr_client_t  client;
  /*!< All RF interaction with the message router include this client as a 
  parameter. */

  msgr_id_t mb;
  /*!< his mailbox allows RF task to receive messages from a message 
  router based task.*/

} rf_cdma_ic_task_msgr_type;

/*----------------------------------------------------------------------------*/
/*! Structure to maintain the iC Task housekeeping data */
typedef struct
{
  boolean is_task_running;
  /*!< Flag indicating, if the CDMA iC Task is running */

  rfm_device_enum_type onex_ic_active_dev;
  /*!< Variable to store the active device on which 1x intelliceiver is running 
  currently running. */

  rfm_device_enum_type hdr_ic_active_dev;
  /*!< Variable to store the active device on which HDR intelliceiver is running 
  currently running. */

} rf_cdma_ic_housekeeping_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure holding the CDMA intelliceiver Task data */
typedef struct
{

  rf_cdma_ic_housekeeping_data_type housekeeping;
  /*!< Structure to maintain the iC Task housekeeping data */

  rf_cdma_ic_dispatch_tbl_type ic_dispatch_tbl;
  /*!< Data structure to hold RF_IC_TASK dispatch tabel */

  rf_cdma_ic_cmd_q_struct_type ic_cmd_q;
  /*!< Structure to hold RF CDMA iC queue data strucutre */

  rf_cdma_ic_task_msgr_type ic_msgr;
  /*!< Sturcture to Hold RF CDMA IC task msgr related information */

} rf_cdma_ic_task_data_type;

/*----------------------------------------------------------------------------*/
boolean 
rf_cdma_ic_put_command
(
  rf_cdma_ic_cmd_type* cmd
);

/*----------------------------------------------------------------------------*/
rf_cdma_ic_cmd_type*
rf_cdma_get_command_buffer
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rf_cdma_ic_init_dispatcher
(
  void
);

/*----------------------------------------------------------------------------*/
void
rf_cdma_ic_signal_to_start_task
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* RF_CDMA_IC_TASK_H */

