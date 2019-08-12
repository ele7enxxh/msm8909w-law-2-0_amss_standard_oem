#ifndef RF_CMD_INTERFACE_H
#define RF_CMD_INTERFACE_H
/*!
  @file
  RF Command interface file.

  @defgroup RF_TASK_CMD_interface
  @ingroup RF_TASK
  @{
*/
/*===========================================================================

Copyright (c) 2009 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/inc/rf_cmd_interface.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/30/13   kab     Increased RF_MAX_CMD_PAYLOAD_SIZE to 550 bytes to support 
                   LTE scouting before actual RF tune request.
09/09/13   svi     Increased Max MSGR Mailbox size for RFA to 200 bytes
04/17/13   aro     Cleaned up unused time profiler code
06/15/12   pl      Increase RF_MAX_CMD_PAYLOAD_SIZE from 72 to 128 to accommodate for 
                   revised LTE Message interface to support CA
03/02/11   tnt     Increase RF_MAX_CMD_PAYLOAD_SIZE from 50 to 72 to accommodate for new
                   LM fields in rfa_rf_lte_l2l_build_scripts_req_s
13/01/09   pl      provide permanent fix for address alignment in rf_cmd_type
10/09/09   pl/mkv  Added reserved of size uint32 in rf_cmd_rex_hdr_type to fix 
                   the alignment issue on QDSP6. 
09/25/09   aro     Fixed "PACKED" usage to be compatible with QDSP6
09/11/09   tws     Add support for sending a signal back to calling task when
                   execution complete.
08/26/09   mkv     Integrating REX MSGR Interworking into RFA2. 
                   Adding the rf_cmd_struct_type back into the .h file
                   Changed rf_cmd_id_type to uint8 and added rf_cmd_id_type to MSGR UMID style.
07/21/09   mkv     Added a function to return address of the command queues (REX MSGR Interworking).   
05/04/09   ans     Initial Release.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "queue.h"
#include "rf_rex_msgr_interface.h"
#include "rf_task_common_services.h"
#include "rfcommon_time_profile.h"

/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/
/*!

  @brief
  Maximum number of commands that RF queue can hold.

  @details
  The number of commands that can be handled by RF command queue at once. 
  If we get more than the number of command then we will drop those commands. 
  calling module should handle the case when the command is not accepted.
*/

#define RF_CMD_QUEUE_SIZE 10

/*!

  @brief
  Maximum payload size for RF commands.

  @details
  Maximum payload size for RF commands.
*/

#define RF_MAX_CMD_PAYLOAD_SIZE 550

/*!
  @brief
  System level tech ID. Usually ignored.

  @details The system level tech ID which is usually a message router based tech ID. it will usually 
  be one fixed tech ID for RFA. For non-message router client it will be zero.

  Note: RFA will not be able to receive any messages client with tech-id zero in message router system.

*/
typedef uint8 rfa_system_tech_id_type ;

/*!
  @brief
  The RF module ID. 

  @details The RF module ID, Examples are LTE, 1x, UMTS. This has to be mapped to 
  message router RF module ID for message router based clients. 

  For now we use rfm_mode_enum_type structure for the same. It can be made it's own 
  enum in future.

*/
typedef uint8 rf_module_id_type ;

/*!
  @brief
  The RF command type ID. 

  @details The RF command type ID. RF command type ID is per command and can be CMD, RSP etc.
*/
typedef uint8 rf_type_id_type ;

/*!
  @brief
  The RF command ID. 

  @details The RF command ID. RF command ID is per module. LTE and 1X may have same
  command ID which can represent different commands. 
*/
typedef uint8 rf_cmd_id_type ;

/*!
  @brief
  The RF command header. 

  @details The RF command header includes the module ID and command ID.

  This strucure is supposed to be 32 bit for compatibilty with message router.
*/
typedef PACK(struct)
{
    /*NOTE: Dont not change the sequence of structures in the rf_cmd_hdr_type structure.
      The following struct order is required. MSGR copies the UMID into the rf cmd hdr in this order.*/
    /*@todo Mani to understand behaviour and confirm with MCS team if this OS/Compiler dependent.*/
   /*!< Command ID for handling the command */
   rf_cmd_id_type cmd_id;
   
   /*!< Type ID of the command */
   rf_type_id_type type_id;

   /*!< Module responsible for handling the command */    
   rf_module_id_type module_id;

   /*!< System level tech ID - usually message router tech ID. In most cases ignored.*/
   rfa_system_tech_id_type tech_id; 
 
   
} rf_cmd_hdr_type;


/*!
  @brief
  The RF command related status enum. 

  @details
  This enum specifies the related states and status of the command in the queue.

*/
typedef enum
{
   RF_CMD_BUF_EMPTY,     /*!< Queue buffer is empty and not in use. */
   RF_CMD_BUF_ALLOCATED, /*!< Buffer allocated for calling module to add command to queue. */
   RF_CMD_QUEUED,        /*!< Buffer filled with valid information and added to Queue. */
   RF_CMD_PROCESSING,    /*!< Command currently being processed. */
   RF_CMD_NOT_SUPPORTED, /*!< Command not supported. */
   RF_CMD_QUEUE_FULL,    /*!< Command Queue full. */
   RF_CMD_FAILURE,       /*!< Command Failed during processing. */
   RF_CMD_SUCCESS,       /*!< Command Processing successful. */
} rf_cmd_status_e_type;

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
   /* Q linking element

      This element has to be the first
      element in this data structure
      because of removal of self_ptr
      in q_link_type
    */
   q_link_type            link;

   /* Type of RF command */
   rf_cmd_hdr_type        cmd_hdr;
}rf_cmd_rex_hdr_type;

/*!
  @brief
  The structure holds details about RF command. 

  @details This structure holds details about RF command.
  It includes command header, payload, status, queue links.

*/
typedef struct
{
   /* REX header */
   rf_cmd_rex_hdr_type   rex_hdr;

   /*  MSGR Header for targets using MSGR 
    * !!! IMPORTANT: This following field must start
    * on a 8-byte aligned address*/
   rf_msgr_hdr_struct_type   msgr_hdr;

   byte                   payload[RF_MAX_CMD_PAYLOAD_SIZE];
   /*!< RF command payload */  

   /* Other debug information for the command */
   rf_time_tick_type                 alloc_time; 
   /*!< time when CMD was added to queue */
     
   
   rf_cmd_status_e_type   status;
   /*!< Command Status of the command */

   rex_sigs_type          complete_sig;
   /*!< Signal to send upon execution of command */

   rex_tcb_type         * task_to_sig;
   /*!< task to signal upon execution of command */

} rf_cmd_type;

/*!
  @brief
  The RF command queue data structure type. 

  @details This structure holds the storage for command queue and queue link strucutres.
           Do not move this structure definition to the rf_cmd_inteface.c file. 
           REX MSGR Interworking code requires this structure.


*/
typedef struct 
{

  rf_cmd_type    q_items[ RF_CMD_QUEUE_SIZE ];
  /*!< Storage for command items */
    
  q_type                cmd_q;
  /*!< Queue of currently active RF commands */  

  q_type                free_q;
  /*!< Queue of free command structures */  
} rf_cmd_struct_type;

/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/

void rf_cmd_init(rf_task_num_type task_num );

rf_cmd_type* rf_cmd_get_buf( rf_task_num_type task_num );

void rf_cmd_put( rf_task_num_type task_num, rf_cmd_type* cmd );

rf_cmd_type* rf_cmd_get( rf_task_num_type task_num );

void rf_cmd_free ( rf_task_num_type task_num, rf_cmd_type* cmd );

int rf_num_of_cmds_in_q( rf_task_num_type task_num );

#endif /* RF_CMD_INTERFACE_H */
/*! @} */

