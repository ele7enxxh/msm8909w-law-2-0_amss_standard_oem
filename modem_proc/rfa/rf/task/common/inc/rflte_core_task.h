#ifndef RFLTE_CORE_TASK_H
#define RFLTE_CORE_TASK_H
/*! 
  @file
  rflte_core_task.h
 
  @brief
  This file includes functions pertaining to Tasks used for FW response 
  intelliceiver Operation 
*/ 
 
/*==============================================================================

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/inc/rflte_core_task.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 

==============================================================================*/

#include "comdef.h"
#include "msgr_types.h"
#include "rex.h"
#include "queue.h"
#include "rfcommon_locks.h"
#include "rf_dispatch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!< Signal for when commands available in  RF command queue. */
#define RFLTE_CORE_TASK_CMD_Q_SIG          0x0001

/*----------------------------------------------------------------------------*/
/*!< Signal for RF FWRSP to task. */
#define RFLTE_CORE_TASK_START_SIG     0x0002

/*----------------------------------------------------------------------------*/
/*! rflte_core command ID */
typedef uint8 rflte_core_task_cmd_id_type;

/*----------------------------------------------------------------------------*/
/*! rflte_core command type */
typedef uint8 rflte_core_task_type_id_type;

/*----------------------------------------------------------------------------*/
/*! rflte_core module ID */
typedef uint8 rflte_core_task_module_id_type;

/*----------------------------------------------------------------------------*/
/*! rflte_core Tech ID */
typedef uint8 rflte_core_task_tech_id_type;

/*----------------------------------------------------------------------------*/
/*! Maximum payload size for RF commands */
#define RFLTE_CORE_TASK_MAX_CMD_PAYLOAD_SIZE 72

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Maximum number of commands that RF queue can hold.

  @details
  The number of commands that can be handled by RF command queue at once. 
  If we get more than the number of command then we will drop those commands. 
  calling module should handle the case when the command is not accepted.
*/
#define RFLTE_CORE_TASK_CMD_QUEUE_SIZE 10

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The RF command related status enum. 

  @details
  This enum specifies the related states and status of the command in the queue.
*/
typedef enum
{
  RFLTE_CORE_TASK_CMD_BUF_EMPTY,
  /*!< Queue buffer is empty and not in use. */

  RFLTE_CORE_TASK_CMD_BUF_ALLOCATED,
  /*!< Buffer allocated for calling module to add command to queue. */

  RFLTE_CORE_TASK_CMD_QUEUED,
  /*!< Buffer filled with valid information and added to Queue. */

  RFLTE_CORE_TASK_CMD_PROCESSING,
  /*!< Command currently being processed. */

  RFLTE_CORE_TASK_CMD_NOT_SUPPORTED,
  /*!< Command not supported. */

  RFLTE_CORE_TASK_CMD_QUEUE_FULL,
  /*!< Command Queue full. */

  RFLTE_CORE_TASK_CMD_FAILURE,
  /*!< Command Failed during processing. */

  RFLTE_CORE_TASK_CMD_SUCCESS,
  /*!< Command Processing successful. */

} rflte_core_task_cmd_status_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The RF FWRSP command header. 

  @details
  The RF command header includes the module ID and command ID. This strucure is 
  supposed to be 32 bit for compatibilty with message router.
*/
typedef PACK(struct)
{
  /* NOTE: Dont not change the sequence of structures in the rf_cmd_hdr_type 
  structure. The following struct order is required. MSGR copies the UMID into 
  the rf cmd hdr in this order.*/
  rflte_core_task_cmd_id_type cmd_id;
  /*!< Command ID for handling the command */
   
  rflte_core_task_type_id_type type_id;
  /*!< Type ID of the command */
 
  rflte_core_task_module_id_type module_id;
  /*!< Module responsible for handling the command */    
 
  rflte_core_task_tech_id_type tech_id; 
  /*!< System level tech ID - usually message router tech ID. In most cases 
  ignored.*/
 
} rflte_core_task_cmd_hdr_type;

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

  q_link_type link;
  /*!<  Q linking element -  This element has to be the first element in this 
  data structure because of removal of self_ptr in q_link_type */

  rflte_core_task_cmd_hdr_type  cmd_hdr;
  /*!< Type of RF command */

} rflte_core_task_cmd_rex_hdr_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The structure holds details about RF command. 

  @details This structure holds details about RF command.
  It includes command header, payload, status, queue links.
*/
typedef struct
{
  rflte_core_task_cmd_rex_hdr_type rex_hdr;
  /*!< REX header */
  
  msgr_hdr_struct_type msgr_hdr;
 /*!< MSGR Header for targets using MSGR. IMPORTANT: This following field must 
 start on a 8-byte aligned address  */
  
  byte payload[ RFLTE_CORE_TASK_MAX_CMD_PAYLOAD_SIZE ];
  /*!< RF command payload */  
  
  /* ----- Other debug information for the command ----- */

  rflte_core_task_cmd_status_type  status;
  /*!< Command Status of the command */
  
  rex_sigs_type complete_sig;
  /*!< Signal to send upon execution of command */
  
  rex_tcb_type *task_to_sig;
  /*!< task to signal upon execution of command */
  
} rflte_core_task_cmd_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The RF FWRSP command queue data structure type. 

  @details
  This structure holds the storage for command queue and queue link strucutres.
*/
typedef struct 
{

  rflte_core_task_cmd_type q_items[ RFLTE_CORE_TASK_CMD_QUEUE_SIZE ];
  /*!< Storage for command items */
    
  q_type cmd_q;
  /*!< Queue of currently active RF commands */  

  q_type free_q;
  /*!< Queue of free command structures */  

} rflte_core_task_cmd_q_struct_type;

/*----------------------------------------------------------------------------*/
/*! Sturcture to Hold RF FWRSP task msgr related information. */
typedef struct
{

  msgr_client_t client;
  /*!< All RF interaction with the message router include this client as a 
  parameter. */

  msgr_id_t mb;
  /*!< his mailbox allows RF task to receive messages from a message 
  router based task.*/

} rflte_core_task_msgr_type;

/*----------------------------------------------------------------------------*/
/*! Structure to maintain the rflte_core Task housekeeping data */
typedef struct
{

  boolean is_task_running;
  /*!< Flag indicating, if the rflte_core Task is running */

} rflte_core_task_housekeeping_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure holding the RF FW response Task data */
typedef struct
{
  rflte_core_task_housekeeping_data_type housekeeping;
  /*!< Structure to maintain the rflte_core Task housekeeping data */
  
  rflte_core_task_cmd_q_struct_type rflte_core_task_cmd_q;
  /*!< Structure to hold RF FWRSP queue data strucutre */

  rflte_core_task_msgr_type rflte_core_msgr;
  /*!< Sturcture to Hold RF FWRSP task msgr related information */

  rf_dispatch_handle_type dispatch_handle;
  /*!< Structure to store the dispatch handle data */

} rflte_core_task_data_type;


/*----------------------------------------------------------------------------*/
void
rflte_core_task_signal_to_start_task
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rflte_core_task_is_task_running
(
  void
);

/*----------------------------------------------------------------------------*/
const rflte_core_task_msgr_type*
rflte_core_task_get_msgr_param
(
  void
);

/*----------------------------------------------------------------------------*/
rf_dispatch_handle_type *
rflte_core_task_get_dispatch_handle
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rflte_core_task_dispatch
(
  rflte_core_task_cmd_type *cmd
);

#ifdef __cplusplus
}
#endif

#endif /* RFLTE_CORE_TASK_H */

