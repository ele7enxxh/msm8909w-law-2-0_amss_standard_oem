/*! 
  @file
  rf_fed_lte_task.h
 
  @brief
  This file includes functions pertaining to Tasks used for FED LTE  
  Scheduling operation
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/inc/rf_fed_lte_task.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
02/26/12   kab     Initial Revision

==============================================================================*/

#ifndef RF_FED_LTE_TASK_H
#define RF_FED_LTE_TASK_H

/*==============================================================================

                           INCLUDE FILES

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

/*==============================================================================

                EXTERNAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*----------------------------------------------------------------------------*/
/*!< Signal for when commands available in  RF command queue. */
#define RF_FED_LTE_TASK_CMD_Q_SIG                0x0001

/*----------------------------------------------------------------------------*/
/*!< Signal for RF FWRSP to task. */
#define RF_FED_LTE_TASK_START_SIG           0x0002

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Maximum number of commands that RF queue can hold.

  @details
  The number of commands that can be handled by RF command queue at once. 
  If we get more than the number of command then we will drop those commands. 
  calling module should handle the case when the command is not accepted.
*/
#define RF_FED_LTE_CMD_QUEUE_SIZE               10

/*----------------------------------------------------------------------------*/
/*! Maximum payload size for RF commands */
#define RF_FED_LTE_MAX_CMD_PAYLOAD_SIZE         72

/*----------------------------------------------------------------------------*/



/*==============================================================================

              EXTERNAL DEFINITIONS AND TYPES : TYPEDEFS

==============================================================================*/

/*==============================================================================

              EXTERNAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/*----------------------------------------------------------------------------*/
/*! rf_fed_lte command ID */
typedef uint8 rf_fed_lte_cmd_id_type;

/*----------------------------------------------------------------------------*/
/*! rf_fed_lte command type */
typedef uint8 rf_fed_lte_type_id_type;

/*----------------------------------------------------------------------------*/
/*! rf_fed_lte module ID */
typedef uint8 rf_fed_lte_module_id_type;

/*----------------------------------------------------------------------------*/
/*! rf_fed_lte Tech ID */
typedef uint8 rf_fed_lte_tech_id_type;
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*!
  @brief
  The RF command related status enum. 

  @details
  This enum specifies the related states and status of the command in the queue.
*/
typedef enum
{
  RF_FED_LTE_CMD_BUF_EMPTY,
  /*!< Queue buffer is empty and not in use. */

  RF_FED_LTE_CMD_BUF_ALLOCATED,
  /*!< Buffer allocated for calling module to add command to queue. */

  RF_FED_LTE_CMD_QUEUED,
  /*!< Buffer filled with valid information and added to Queue. */

  RF_FED_LTE_CMD_PROCESSING,
  /*!< Command currently being processed. */

  RF_FED_LTE_CMD_NOT_SUPPORTED,
  /*!< Command not supported. */

  RF_FED_LTE_CMD_QUEUE_FULL,
  /*!< Command Queue full. */

  RF_FED_LTE_CMD_FAILURE,
  /*!< Command Failed during processing. */

  RF_FED_LTE_CMD_SUCCESS,
  /*!< Command Processing successful. */

} rf_fed_lte_cmd_status_type;

/*==============================================================================

            EXTERNAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The FED LTE Task command header. 

  @details
  The RF command header includes the module ID and command ID. This strucure is 
  supposed to be 32 bit for compatibilty with message router.
*/
typedef PACK(struct)
{
  /* NOTE: Dont not change the sequence of structures in the rf_cmd_hdr_type 
  structure. The following struct order is required. MSGR copies the UMID into 
  the rf cmd hdr in this order.*/
  rf_fed_lte_cmd_id_type cmd_id;
  /*!< Command ID for handling the command */
   
  rf_fed_lte_type_id_type type_id;
  /*!< Type ID of the command */
 
  rf_fed_lte_module_id_type module_id;
  /*!< Module responsible for handling the command */    
 
  rf_fed_lte_tech_id_type tech_id; 
  /*!< System level tech ID - usually message router tech ID. In most cases 
  ignored.*/
 
} rf_fed_lte_cmd_hdr_type;

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

  rf_fed_lte_cmd_hdr_type  cmd_hdr;
  /*!< Type of RF command */

} rf_fed_lte_cmd_rex_hdr_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The structure holds details about RF command. 

  @details This structure holds details about RF command.
  It includes command header, payload, status, queue links.
*/
typedef struct
{
  rf_fed_lte_cmd_rex_hdr_type rex_hdr;
  /*!< REX header */
  
  msgr_hdr_struct_type msgr_hdr;
 /*!< MSGR Header for targets using MSGR. IMPORTANT: This following field must 
 start on a 8-byte aligned address  */
  
  byte payload[ RF_FED_LTE_MAX_CMD_PAYLOAD_SIZE ];
  /*!< RF command payload */  
  
  /* ----- Other debug information for the command ----- */

  rf_fed_lte_cmd_status_type  status;
  /*!< Command Status of the command */
  
  rex_sigs_type complete_sig;
  /*!< Signal to send upon execution of command */
  
  rex_tcb_type *task_to_sig;
  /*!< task to signal upon execution of command */
  
} rf_fed_lte_cmd_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The FED LTE task command queue data structure type. 

  @details
  This structure holds the storage for command queue and queue link strucutres.
*/
typedef struct 
{

  rf_fed_lte_cmd_type q_items[ RF_FED_LTE_CMD_QUEUE_SIZE ];
  /*!< Storage for command items */
    
  q_type cmd_q;
  /*!< Queue of currently active RF commands */  

  q_type free_q;
  /*!< Queue of free command structures */  

} rf_fed_lte_cmd_q_struct_type;

/*----------------------------------------------------------------------------*/
/*! Sturcture to Hold FED LTE task msgr related information. */
typedef struct
{

  msgr_client_t client;
  /*!< All RF interaction with the message router include this client as a 
  parameter. */

  msgr_id_t mb;
  /*!< his mailbox allows RF task to receive messages from a message 
  router based task.*/

} rf_fed_lte_task_msgr_type;

/*----------------------------------------------------------------------------*/
/*! Structure to maintain the FED LTE task housekeeping data */
typedef struct
{

  boolean is_task_running;
  /*!< Flag indicating, if the fed_lte_schedule Task is running */

} rf_fed_lte_housekeeping_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure holding the FED LTE Task data */
typedef struct
{
  rf_fed_lte_housekeeping_data_type housekeeping;
  /*!< Structure to maintain the fed_lte_schedule Task housekeeping data */
  
  rf_fed_lte_cmd_q_struct_type rf_fed_lte_cmd_q;
  /*!< Structure to hold FED LTE queue data strucutre */

  rf_fed_lte_task_msgr_type rf_fed_lte_msgr;
  /*!< Sturcture to Hold FED LTE task msgr related information */

  rf_dispatch_handle_type dispatch_handle;
  /*!< Structure to store the dispatch handle data */

} rf_fed_lte_task_data_type;

/*----------------------------------------------------------------------------*/

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*----------------------------------------------------------------------------*/
void
rf_fed_lte_signal_to_start_task
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rf_fed_lte_is_task_running
(
  void
);

/*----------------------------------------------------------------------------*/
const rf_fed_lte_task_msgr_type*
rf_fed_lte_get_msgr_param
(
  void
);

/*----------------------------------------------------------------------------*/
rf_dispatch_handle_type *
rf_fed_lte_task_get_dispatch_handle
(
  void
);

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* RF_FED_LTE_TASK_H */


