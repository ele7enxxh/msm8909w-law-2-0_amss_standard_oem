#ifndef RF_APPS_TASK_H
#define RF_APPS_TASK_H
/*! 
  @file
  rf_apps_task.h
 
  @brief
  This file includes functions pertaining to RF APPS Task
*/ 
 
/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header:

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/11/13   aro     Support to migrate WCDMA Temp Comp to RF Apps task
06/03/13   ndb     Added Tuner support for rf app task
01/09/13   sr      Added GSM tech support for rf app task.
04/18/12   hdz     Fixed compiler warnings
04/18/12   hdz     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "msgr_types.h"
#include "rex.h"
#include "queue.h"
#include "rf_dispatch.h"

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------------------------*/
/*!< Signal for when commands available in  RF command queue. */
#define RF_APPS_CMD_Q_SIG          0x0001

/*----------------------------------------------------------------------------*/
/*!< Signal for RF APPS to task. */
#define RF_APPS_TASK_START_SIG     0x0002

/*----------------------------------------------------------------------------*/
/*!< Signal to show RF APPS has started. */
#define RF_APPS_TASK_STARTED_SIG   0x0004


/*******************************************************************************
*  In RF APPS task, only rf_apps_cmd_id and rf_apps_tech_id are currently used
*  rf_apps_typs_id and rf_apps_module_id are not used. They are defined in order
*  to be compatible with RF_TASK
*******************************************************************************/
/*----------------------------------------------------------------------------*/
/*! RF_APPS command ID */
typedef uint8 rf_apps_cmd_id_type ;

/*----------------------------------------------------------------------------*/
/*! RF_APPS command type */
typedef uint8 rf_apps_type_id_type;

/*----------------------------------------------------------------------------*/
/*! RF_APPS module ID */
typedef uint8 rf_apps_module_id_type;

/*----------------------------------------------------------------------------*/
/*! RF_APPS Tech ID */
typedef uint8 rf_apps_tech_id_type;

/*! RF_APPS command size */
typedef uint32 rf_apps_cmd_size_type ;


/*----------------------------------------------------------------------------*/
/*! Maximum payload size for RF commands */
#define RF_APPS_MAX_CMD_PAYLOAD_SIZE 6

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Maximum number of commands that RF queue can hold.

  @details
  The number of commands that can be handled by RF command queue at once. 
  If we get more than the number of command then we will drop those commands. 
  calling module should handle the case when the command is not accepted.
*/
#define RF_APPS_CMD_QUEUE_SIZE 10

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The RF command related status enum. 

  @details
  This enum specifies the related states and status of the command in the queue.
*/
typedef enum
{
  RF_APPS_CMD_BUF_EMPTY,
  /*!< Queue buffer is empty and not in use. */

  RF_APPS_CMD_BUF_ALLOCATED,
  /*!< Buffer allocated for calling module to add command to queue. */

  RF_APPS_CMD_QUEUED,
  /*!< Buffer filled with valid information and added to Queue. */

  RF_APPS_CMD_PROCESSING,
  /*!< Command currently being processed. */

  RF_APPS_CMD_NOT_SUPPORTED,
  /*!< Command not supported. */

  RF_APPS_CMD_QUEUE_FULL,
  /*!< Command Queue full. */

  RF_APPS_CMD_FAILURE,
  /*!< Command Failed during processing. */

  RF_APPS_CMD_SUCCESS,
  /*!< Command Processing successful. */

} rf_apps_cmd_status_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  The RF tech enum. 

  @details
  This enum specifies the related states and status of the command in the queue.
  Curretnly enum only includes 1xHDR technology. One can add new technology in 
  the enum if needed following the format of
        RF_APPS_xxxxx(tech)_ID
*/ 
typedef enum
{
  RF_APPS_1XHDR_ID = 0,
  /*!< ID of 1xHDR for RF APPS Task. */

  RF_APPS_GSM_ID,
  /*!< ID of GSM for RF APPS Task. */

  RF_APSS_TUNER_ID,
  /*!< ID of TUNER for RF APPS Task. */

  RF_APPS_WCDMA_ID,
  /*!< WCDMA ID for RF APPS Task. */

  RF_APPS_COMMON_ID,
  /*!< COMMON ID for RF APPS Task. */

  RF_APPS_MAX_TECH_ID,
  /*! Maximun RF tech ID. */

} rf_apps_tech_enum_type;

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
  rf_apps_cmd_id_type cmd_id;
  /*!< Command ID for handling the command */

  rf_apps_type_id_type type_id;
  /*!< Type ID of the command */
 
  rf_apps_module_id_type module_id;
  /*!< Module responsible for handling the command */    

  rf_apps_tech_enum_type tech_id;
  /*!< RF tech id of the cmd */
} rf_apps_cmd_hdr_type;

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

  rf_apps_cmd_hdr_type  cmd_hdr;
  /*!< Type of RF command */

} rf_apps_cmd_rex_hdr_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The structure holds details about RF command. 

  @details This structure holds details about RF command.
  It includes command header, payload, status, queue links.
*/
typedef struct
{
  rf_apps_cmd_rex_hdr_type rex_hdr;
  /*!< REX header */
  
  msgr_hdr_struct_type msgr_hdr;
 /*!< MSGR Header for targets using MSGR. IMPORTANT: This following field must 
 start on a 8-byte aligned address  */
  
  uint8 payload[RF_APPS_MAX_CMD_PAYLOAD_SIZE];
  /*!< RF command payload */  

  rf_apps_cmd_status_type  status;
  /*!< Command Status of the command */
  
  rex_sigs_type complete_sig;
  /*!< Signal to send upon execution of command */
  
  rex_tcb_type *task_to_sig;
  /*!< task to signal upon execution of command */
  
} rf_apps_cmd_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The RF APPS command queue data structure type. 

  @details
  This structure holds the storage for command queue and queue link strucutres.
*/
typedef struct 
{

  rf_apps_cmd_type q_items[ RF_APPS_CMD_QUEUE_SIZE ];
  /*!< Storage for command items */
    
  q_type cmd_q;
  /*!< Queue of currently active RF commands */  

  q_type free_q;
  /*!< Queue of free command structures */  

} rf_apps_cmd_q_struct_type;

/*----------------------------------------------------------------------------*/
/*! Sturcture to Hold RF APPS task msgr related information. */
typedef struct
{

  msgr_client_t client;
  /*!< All RF interaction with the message router include this client as a 
  parameter. */

  msgr_id_t mb;
  /*!< his mailbox allows RF task to receive messages from a message 
  router based task.*/

} rf_apps_task_msgr_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The command dispatcher status type. 
*/
typedef enum
{

  RF_APPS_CMD_REG_FAILURE,
  /*!< Command not registered. It will not be dispatched. */

  RF_APPS_CMD_REG_SUCCESS
   /*!< Command registered successfully. */

} rf_apps_dispatch_status_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer for the handler function associated with each command.

  @details
  The function pointer prototype associated with each command.

  @param payload
  The data associated with the command. The size of data is specified in
  payload_size.
*/
typedef void (*rf_apps_cmd_handler_func_type)( rf_apps_cmd_type* cmd_ptr );

/*----------------------------------------------------------------------------*/
void
rf_apps_signal_to_start_task
(
  void
);

/*----------------------------------------------------------------------------*/
void
rf_apps_wait_till_task_started
(
  rex_tcb_type* rex_tcb
);

/*----------------------------------------------------------------------------*/
rf_apps_dispatch_status_type
rf_apps_dispatch_register
(
  rf_apps_cmd_handler_func_type callback_fn,
  rf_apps_tech_enum_type rf_tech_id
);

/*----------------------------------------------------------------------------*/
void
rf_apps_dispatch_deregister
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rf_apps_is_task_running
(
  void
);

/*----------------------------------------------------------------------------*/
const rf_apps_task_msgr_type*
rf_apps_get_msgr_param
(
  void
);

/*----------------------------------------------------------------------------*/
rf_dispatch_handle_type *
rf_apps_task_get_dispatch_handle
(
  void
);

/*----------------------------------------------------------------------------*/
rf_apps_cmd_type* rf_apps_get_buf
( 
  void
);

/*----------------------------------------------------------------------------*/
void rf_apps_cmd_put
(
   rf_apps_cmd_type* cmd 
);

#ifdef __cplusplus
}
#endif

#endif /* RF_APPS_TASK_H */


