#ifndef RF_DISPATCH_H
#define RF_DISPATCH_H

/*! 
  @file
  rf_dispatch.h
 
  @brief
  This file includes common dispatch implementation for all task used by RFSW
*/
 
/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/inc/rf_dispatch.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/15/13   aro     Added critical section to synchronize the dipatching and
                   deregistration
04/15/13   aro     Added CID dispatch logging
04/04/13   aro     Changed enum
03/22/13   zhw     Update rf_dispatch_command API
01/16/13   aro     Renamed handler to handle
01/16/13   aro     Renamed dispatch_handler to dispatch_handle
01/15/13   aro     Renamed UMID to CID
01/15/13   aro     Documentation Update
01/15/13   aro     Updated registration interface to pass dispatch handle
01/09/13   aro     Added registration action mask in command registration
01/09/13   aro     Renamed UMID to CID
01/08/13   aro     Added for Return value in secondary dispatch deinit interface
01/08/13   aro     Organized secondary dispatch data container
01/07/13   aro     Interface to get CID information
01/07/13   aro     Renamed function pointer type name
01/07/13   aro     Type name update
01/07/13   aro     Command dispatch function for spawned tasks
01/07/13   aro     Initial support for secondary dispatching
01/07/13   aro     variable to indicate if the CID is registered wih MSGR
01/07/13   aro     Flag indicating if a task is registered to the dispatcher
01/07/13   aro     Implementation of CID registration function
01/07/13   aro     Interface to perform CID deregistration
01/07/13   aro     Interface to perform CID registration
01/07/13   aro     Structure to hold message router data and rex data
01/04/13   aro     Added data structure for dispatch table
01/04/13   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "msgr_types.h"
#include "rex.h"
#include "queue.h"
#include "rfcommon_time_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Maximum length of Task name */
#define RF_DISPATCH_TASK_NAME_LEN 20

/*----------------------------------------------------------------------------*/
/*! @brief Type for a 32-bit command Message ID (CID (Command ID)). For 
message router based command, this will be CID (Command ID) */
typedef uint32 rf_dispatch_cid_type;

/*----------------------------------------------------------------------------*/
/*! @brief Type for a TECH. For message router based command, this will be 
bits 23:16 in CID (Command ID) */
typedef uint16 rf_dispatch_module_type;

/*----------------------------------------------------------------------------*/
/*! List of enumeration to indicate the type of task dispatch */
typedef enum
{
  RF_TASK_DISPATCH,
  /*!< RF Task Dispatch */

  RF_TASK_2_DISPATCH,
  /*!< RF Task 2 Dispatch */

  RF_FWRSP_TASK_DISPATCH,
  /*!< RF FW response Task Dispatch */

  RF_APPS_TASK_DISPATCH,
  /*!< RF Apps Task Dispatch */

  RF_IC_TASK_DISPATCH,
  /*!< RF intelliceiver Task Dispatch */

  RF_MAX_DISPATCH,
  /*!< Max number of Task Dispatch supporte */

} rf_dispatch_task_enum;

/*----------------------------------------------------------------------------*/
/*! Enumeration listing the types of RF Dispatch Error */
typedef enum
{

  RF_DISPATCH_SUCCESS = 0,
  /*!< Enum indicating the successful RF dipatching of the command*/

  RF_DISPATCH_FAILURE = 1,
  /*!< Enum indicating that failed RF dipatching of the command */

  RF_DISPATCH_NO_HANDLER = 2
  /*!< Enum indicating that failed RF dipatching of the command due to
  absense of handler function */

} rf_dispatch_error_code_enum;

/*----------------------------------------------------------------------------*/
/*! List of enumeration indicating the the type of additional third party 
registration to be done by the Dispatch module */
typedef enum
{

  RF_DISPATACH_MSGR_REG = 0,
  /*!< Enumeration indicating Message router registration */

} rf_dispatch_registration_action_enum;

/*----------------------------------------------------------------------------*/
/*! 16-bit mask indicating the type of additional third party registration 
to be done on the command ID by RF dispatch module. Each bit field is defined 
by #rf_dispatch_registration_action_enum. For instance, if bit 0 of this
is set (RF_DISPATACH_MSGR_REG), then this means that the RF dispatch module
will perform message router initialization internally. */ 
typedef uint16 rf_dispatch_registration_action;

/*----------------------------------------------------------------------------*/
/*! Forward declaration of Command ID info type */
struct rf_dispatch_cid_info_type_t;

/*----------------------------------------------------------------------------*/
/*! Forward declaration of secondary dispatcher data type */
struct rf_dispatch_cid_sec_dispatch_type_t;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer for the first-level dispatch handler function associated with 
  each command.

  @param cmd_ptr
  Pointer to command payload
 
  @param cid_info
  Pointer to command ID info
 
  @param cb_data
  Opaque Pointer to the command ID callback data
*/
typedef void (*rf_dispatch_cid_handler_fp_type)( void* cmd_ptr,
                              struct rf_dispatch_cid_info_type_t *cid_info,
                              void *cb_data );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer to perform second level dispatching deinitialization

  @param sec_info
  Pointer to secondary dispatching data
*/
typedef boolean (*rf_dispatch_sec_dispatch_deinit_fp_type)( 
                 struct rf_dispatch_cid_sec_dispatch_type_t *sec_info);

/*----------------------------------------------------------------------------*/
/*! Structure to store data pertaining to secondary dispatching */
typedef struct rf_dispatch_cid_sec_dispatch_type_t
{
  rf_dispatch_sec_dispatch_deinit_fp_type deinit_fp;
  /*!< function pointer to perform second level dispatching deinitialization */

  void *dispatch_data;
  /*!< Pointer to the memory where data related to second level dispatching 
  can be stored. An used case of this would be sequence_number based 
  dispatching used by RF-FWRSP task to achieve robust RF_FW synchronization. */ 

} rf_dispatch_cid_sec_dispatch_type;

/*----------------------------------------------------------------------------*/
/*! Max length of circular buffer to hold dispatch data */
#define RF_DISPATCH_CID_DBG_MAX_LEN 10

/*----------------------------------------------------------------------------*/
/*! Structure to hold RF Dispatch debug data for the UMID */
typedef struct
{
  uint8 curr_idx;
  /*!< variable indicating the last index where the debug log was written */

  rf_time_tick_type dispatch_time[RF_DISPATCH_CID_DBG_MAX_LEN];
  /*!< Circular buffer to hold command dispatch times */

} rf_dispatch_cid_debug_log_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold the callback data related to a given callback
 
  @details
  This structure holds all callback data related to a given CID (Command ID). The
  cid_callback is the function call when first-level dispatching is to be
  done. With this funciton, cb_data is passed as a parameter. The callback data
  will have SequenceNumber(SNUM) based callback information. SNUM based
  callbacks are used as second-level dispatching.
*/
typedef struct rf_dispatch_cid_info_type_t
{
  uint16 index_num;
  /*!< Variable to keep track the index number of the CID (Command ID) in 
  the CID (Command ID) list  array */

  rf_dispatch_cid_type cid;
  /*!< Command ID */

  boolean msgr_registered;
  /*!< Flag indicating if this CID (Command ID) is registered with MSGR */
  
  rf_dispatch_cid_handler_fp_type cid_cb;
  /*!< callback function to be called by dispatcher for the given 
  CID (Command ID) */

  void *cid_cb_data;
  /*!< Opaque Pointer to the command ID callback data*/

  rf_dispatch_cid_debug_log_type dbg_data;
  /*!< Structure to hold RF Dispatch debug data for the UMID */

  rf_dispatch_cid_sec_dispatch_type secondary;
  /*!< Structure to store data pertaining to secondary dispatching */

} rf_dispatch_cid_info_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold list of CID (Command ID)s for the given tech
 
  @details
  This structure is used to hold the list of registered CID (Command ID)s for
  the given tech. Tech is defined by the 4 MSB bits of the
  CID (Command ID) [Bit 31:28]. Container for cid_list is dynamicall allocated 
  (and reallocated) based on the number of CID (Command ID) commands registered 
  for the given tech.
*/
typedef struct
{
  uint8 tech;
  /*!< Technology for which the CID (Command ID) list is populated. Tech 
  corresponds to 4 MSB bits of CID (Command ID) (Bit 31:28)*/

  uint8 num_of_cids;
  /*!< Number of CID (Command ID) entries */

  rf_dispatch_cid_info_type *cid_list;
  /*!< Pointer to the memory which holds the list of CID (Command ID) data. 
  This will be dyanically allocated when the registration is done. The size 
  of this structure will grow as new CID (Command ID)s are being registered. */ 

} rf_dispatch_cid_list_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure defining the singly linked node containing list of cids for a given
  technology.
 
  @details
  This structure holds the definition of a singly linked list node containing 
  the CID (Command ID)s callback information for a given tech.
 
  Method to find the callback data for a give CID (Command ID):
  - When the MSGR message is received by a task, the dispatcher will
    extract the MSGR Tech (bit 31:28) and traverse through the linked list
    to find the node corresponding to the tech.
  - In the tech node, it will then find the entry of the CID (Command ID).
  - Dispatcher will then call the callback function for the CID (Command ID)
    and will also pass the callback_data. This callback data will hold the
    SequenceNumber (SNUM) data. This sequence number data will be handled
    individual CID (Command ID)s handler unquiely.
*/
typedef struct rf_dispatch_tech_node
{
  rf_dispatch_cid_list_type tech_cids;
  /*!< Structure containing the  data payload for the node. This payload 
  contains the list of CID (Command ID)s and their corresponding dispatch 
  related data. */

  struct rf_dispatch_tech_node *next;
  /*!< Pointer to Previous Node in the linked list */

} rf_dispatch_tech_node_type;

/*----------------------------------------------------------------------------*/
/*! Sturcture to Hold task msgr related information. */
typedef struct
{

  msgr_client_t client;
  /*!< All RF interaction with the message router include this client as a 
  parameter. */

  msgr_id_t mailbox;
  /*!< his mailbox allows tasks to receive messages from a message 
  router based task.*/

} rf_dispatch_msgr_data_type;

/*----------------------------------------------------------------------------*/
/*! Sturcture to hold rex task related information. This information will be 
fetch from individual task */
typedef struct
{
  rex_tcb_type *rex_tcb;
  /*!< Pointer to task TCB */

  rex_sigs_type rex_sig;
  /*!< Rex signal to be used to signal the task */

  q_type *cmd_q;
  /*!< Pointer to rex command queue */

  q_type *free_q;
  /*!< Pointer to rex free queue */

  uint16 header_size;
  /*!< Size of the rex command header. This will be used as offset to msgr_hdr 
  by the msgr module */

  uint16 q_link_size;
  /*!< Size of the Q link header. This will be used as offset to 
  legacy cmd_type by the msgr module */

  uint16 command_size;
  /*!< maximum size of the task command. */

} rf_dispatch_rex_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer to extract CID (Command ID) from the command

  @param cmd_ptr
  Pointer to data structure containing the command data sent to the task
 
  @param cid
  Pointer to store the extarcted CID (Command ID)
 
  @return
  Flag indicating the status of CID (Command ID) extraction
*/
typedef boolean (*rf_dispatch_extract_cid_fp_type)
( 
  void *cmd_ptr,
  rf_dispatch_cid_type *cid
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold dispatcher information for a task
*/
typedef struct
{
  char task_name[RF_DISPATCH_TASK_NAME_LEN];
  /*!< String representing Task name */

  uint32 task_uid;
  /*!< Unique ID for this Taskp.  Lower 32-bits of standard
  MD5 message digest of name string. */

  boolean dispatch_init_done;
  /*!< Flag indicating, if the task is registered with the dispatcher */

  boolean debug_mode;
  /*!< Flag indicating if the debug mode is to be enabled. This mode will 
  print extra F3 messages to aid debugging */
  
  rf_dispatch_tech_node_type *head_node;
  /*!< Pointer to the head node of the linked list containing the tech 
  specific CID (Command ID)s */

  boolean rex_msgr_reg_done;
  /*!< Flag indicating if the rex_msgr registration is done */
  
  rf_dispatch_msgr_data_type msgr_data;
  /*!< Sturcture to Hold task msgr related information. */

  rf_dispatch_rex_data_type rex_data;
  /*!< Sturcture to hold rex task related information. This information will be 
  fetch from individual task */

  rf_dispatch_extract_cid_fp_type extract_cid_fp;
  /*!< Funtion pointer to extract CID (Command ID) from command data */
  
} rf_dispatch_handle_type;

/*----------------------------------------------------------------------------*/
/*! Structure used by Tasks to push the dispatch handle init data */
typedef struct
{
  char *task_name;
  /*!< String representing Task name */

  uint32 task_uid;
  /*!< Unique ID for this Taskp.  Lower 32-bits of standard
  MD5 message digest of name string. */

  rf_dispatch_rex_data_type rex_data;
  /*!< Sturcture to hold rex task related information. This information will be 
  fetch from individual task */

  rf_dispatch_extract_cid_fp_type extract_cid_fp;
  /*!< Funtion pointer to extract CID (Command ID) from command data */

  boolean debug_mode;
  /*!< Flag indicating if the debug mode is to be enabled. This mode will 
  print extra F3 messages to aid debugging */
  
} rf_dispatch_init_handle_data_type;

/*----------------------------------------------------------------------------*/
boolean
rf_dispatch_init_handle
(
  rf_dispatch_handle_type *dispatch_handle,
  rf_dispatch_init_handle_data_type init_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_dispatch_register_command
(
  rf_dispatch_handle_type *dispatch_handle,
  rf_dispatch_module_type module_self,
  rf_dispatch_cid_type cid,
  rf_dispatch_cid_handler_fp_type cid_handler,
  void *cb_data,
  rf_dispatch_registration_action reg_action
);

/*----------------------------------------------------------------------------*/
boolean
rf_dispatch_deregister_command
(
  rf_dispatch_handle_type *dispatch_handle,
  rf_dispatch_module_type module_self,
  rf_dispatch_cid_type cid
);

/*----------------------------------------------------------------------------*/
rf_dispatch_cid_info_type*
rf_dispatch_get_cid_info
(
  rf_dispatch_handle_type *dispatch_handle,
  rf_dispatch_cid_type cid
);

/*----------------------------------------------------------------------------*/
rf_dispatch_error_code_enum
rf_dispatch_command
(
  rf_dispatch_handle_type *dispatch_handle,
  void *cmd_ptr
);

#ifdef __cplusplus
}
#endif

#endif /* RF_DISPATCH_H */


