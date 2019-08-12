#ifndef RF_FWRSP_TASK_H
#define RF_FWRSP_TASK_H
/*! 
  @file
  rf_fwrsp_task.h
 
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/inc/rf_fwrsp_task.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
08/24/14   kb      Fix compiler warning
04/05/13   spa     Make rf_fwrsp_dispatch non static temporarily to fix warning
01/16/13   aro     Renamed dispatch_handler to dispatch_handle
01/15/13   aro     Updated registration interface to pass dispatch handle
01/08/13   aro     Rex data query and UMID extraction functions added
12/05/12   aro     Doc Update
09/10/12   aro     Feature to have static/dynamic callbakc registration
09/10/12   aro     MSGR request pointer added as argument in SNUM dispatch
08/22/12   aro     Coverted SNUM linked list with dynamic memory allocation
                   to support packing in UMID list after command de-registration
                   (memory footprint reduction)
08/22/12   aro     Added UMID index number
08/16/12   aro     Generic FW response handler function
08/16/12   aro     Added common function to extract sequence number from
                   MSGR Payload
08/16/12   aro     Updated SNUM callback function prototype to pass SNUM data
08/15/12   aro     Compiler warning fix
08/15/12   aro     Added deregistration of SNUM callback functions at the end
                   of SNUM dispatching
08/15/12   aro     SNUM push implemented at the end of SNUM dispatching
08/15/12   aro     SNUM dispatcher function
08/15/12   aro     Updated SNUM dispatcher registration to be done using
                   SNUMPool implementation
08/14/12   aro     Interfaces to handle SNUM doubly linked list
08/14/12   aro     Implementation for SNUM disptacher registration
08/14/12   aro     Implementation to perform UMID deregistration
08/14/12   aro     Enhanced UMID registration with error checkin
08/14/12   aro     Function to traverse the linked list and to add new node
08/08/12   aro     Defined interfaces to command registration
08/08/12   aro     Moved FWRSP data structure to header file
08/07/12   aro     Added type definition for new RF_FWRSP task dispatcher able 
                   to handle snum based dispatching 
03/21/12   aro     Added function to get constant pointer to MSGR parameter 
                   used by RF_FWRSP_TASK
03/20/12   aro     Interface to return the running state RF_FWRSP task
03/20/12   aro     Added interface to perform UMID based dispatcher 
                   registration 
03/20/12   aro     Added interface to perform UMID based dispatcher 
                   de-registration
03/20/12   aro     Added status command structure
03/20/12   aro     Support to wait and start the task
03/20/12   aro     Added initial revision of function to spawn RF_FWRSP task
03/20/12   aro     Data structure to hold RF_FWRSP task
03/20/12   aro     Added MSGR registration data structure
03/20/12   aro     Definition for RF_FWRSP Command and RF_FWRSP queue
03/20/12   aro     Initial Revision.

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
#define RF_FWRSP_CMD_Q_SIG          0x0001

/*----------------------------------------------------------------------------*/
/*!< Signal for RF FWRSP to task. */
#define RF_FWRSP_TASK_START_SIG     0x0002

/*----------------------------------------------------------------------------*/
/*! RF_FWRSP command ID */
typedef uint8 rf_fwrsp_cmd_id_type;

/*----------------------------------------------------------------------------*/
/*! RF_FWRSP command type */
typedef uint8 rf_fwrsp_type_id_type;

/*----------------------------------------------------------------------------*/
/*! RF_FWRSP module ID */
typedef uint8 rf_fwrsp_module_id_type;

/*----------------------------------------------------------------------------*/
/*! RF_FWRSP Tech ID */
typedef uint8 rf_fwrsp_tech_id_type;

/*----------------------------------------------------------------------------*/
/*! Maximum payload size for RF commands */
#define RF_FWRSP_MAX_CMD_PAYLOAD_SIZE 72

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Maximum number of commands that RF queue can hold.

  @details
  The number of commands that can be handled by RF command queue at once. 
  If we get more than the number of command then we will drop those commands. 
  calling module should handle the case when the command is not accepted.
*/
#define RF_FWRSP_CMD_QUEUE_SIZE 10

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The RF command related status enum. 

  @details
  This enum specifies the related states and status of the command in the queue.
*/
typedef enum
{
  RF_FWRSP_CMD_BUF_EMPTY,
  /*!< Queue buffer is empty and not in use. */

  RF_FWRSP_CMD_BUF_ALLOCATED,
  /*!< Buffer allocated for calling module to add command to queue. */

  RF_FWRSP_CMD_QUEUED,
  /*!< Buffer filled with valid information and added to Queue. */

  RF_FWRSP_CMD_PROCESSING,
  /*!< Command currently being processed. */

  RF_FWRSP_CMD_NOT_SUPPORTED,
  /*!< Command not supported. */

  RF_FWRSP_CMD_QUEUE_FULL,
  /*!< Command Queue full. */

  RF_FWRSP_CMD_FAILURE,
  /*!< Command Failed during processing. */

  RF_FWRSP_CMD_SUCCESS,
  /*!< Command Processing successful. */

} rf_fwrsp_cmd_status_type;

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
  rf_fwrsp_cmd_id_type cmd_id;
  /*!< Command ID for handling the command */
   
  rf_fwrsp_type_id_type type_id;
  /*!< Type ID of the command */
 
  rf_fwrsp_module_id_type module_id;
  /*!< Module responsible for handling the command */    
 
  rf_fwrsp_tech_id_type tech_id; 
  /*!< System level tech ID - usually message router tech ID. In most cases 
  ignored.*/
 
} rf_fwrsp_cmd_hdr_type;

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

  rf_fwrsp_cmd_hdr_type  cmd_hdr;
  /*!< Type of RF command */

} rf_fwrsp_cmd_rex_hdr_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The structure holds details about RF command. 

  @details This structure holds details about RF command.
  It includes command header, payload, status, queue links.
*/
typedef struct
{
  rf_fwrsp_cmd_rex_hdr_type rex_hdr;
  /*!< REX header */
  
  msgr_hdr_struct_type msgr_hdr;
 /*!< MSGR Header for targets using MSGR. IMPORTANT: This following field must 
 start on a 8-byte aligned address  */
  
  byte payload[ RF_FWRSP_MAX_CMD_PAYLOAD_SIZE ];
  /*!< RF command payload */  
  
  /* ----- Other debug information for the command ----- */

  rf_fwrsp_cmd_status_type  status;
  /*!< Command Status of the command */
  
  rex_sigs_type complete_sig;
  /*!< Signal to send upon execution of command */
  
  rex_tcb_type *task_to_sig;
  /*!< task to signal upon execution of command */
  
} rf_fwrsp_cmd_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The RF FWRSP command queue data structure type. 

  @details
  This structure holds the storage for command queue and queue link strucutres.
*/
typedef struct 
{

  rf_fwrsp_cmd_type q_items[ RF_FWRSP_CMD_QUEUE_SIZE ];
  /*!< Storage for command items */
    
  q_type cmd_q;
  /*!< Queue of currently active RF commands */  

  q_type free_q;
  /*!< Queue of free command structures */  

} rf_fwrsp_cmd_q_struct_type;

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

} rf_fwrsp_task_msgr_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  The command dispatcher status type. 
*/
typedef enum
{

  RF_FWRSP_CMD_REG_FAILURE,
  /*!< Command not registered. It will not be dispatched. */

  RF_FWRSP_CMD_REG_SUCCESS
   /*!< Command registered successfully. */

} rf_fwrsp_dispatch_status_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer for the first-level dispatch handler function associated with 
  each command.

  @param cmd_ptr
  Pointer to RF_FWRSP command payload
*/
typedef void (*rf_fwrsp_cb_handler_func_type)( rf_fwrsp_cmd_type* cmd_ptr );

/*----------------------------------------------------------------------------*/
/*! Forward declaration of UMID info type */
struct rf_fwrsp_umid_info_type_t;

/*----------------------------------------------------------------------------*/
/*! Forward declaration of SNUM dispatch info type */
struct rf_fwrsp_snum_dispatch_info_type_t;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer for the first-level dispatch handler function associated with 
  each command.

  @param cmd_ptr
  Pointer to RF_FWRSP command payload
*/
typedef void (*rf_fwrsp_umid_handler_func_type)( rf_fwrsp_cmd_type* cmd_ptr,
                                 struct rf_fwrsp_umid_info_type_t *umid_info );


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function pointer for the sequence number based second-level dispatch handler 
  function associated with each command.

  @param cmd_ptr
  Pointer to RF_FWRSP command payload
 
  @param snum_data
  Pointer to the opaque data pertaining to sequence number
*/
typedef boolean (*rf_fwrsp_snum_handler_func_type)( 
                   rf_fwrsp_cmd_type *req_ptr,
                   const struct rf_fwrsp_snum_dispatch_info_type_t *snum_data );

/*----------------------------------------------------------------------------*/
/*! Maximum Number of techs supported by UMID. Tech is defined by Bit [31:24] 
in a given UMID. Thus the Maximum number of techs would be 2^8 = 256 */
#define RF_FWRSP_NUM_OF_UMID_TECHS 256

/*----------------------------------------------------------------------------*/
/*! Structure to maintain the RF_FWRSP Task housekeeping data */
typedef struct
{

  boolean is_task_running;
  /*!< Flag indicating, if the RF_FWRSP Task is running */

} rf_fwrsp_housekeeping_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to hold callabck functions for module based command
  dispatching. Techs refers to the 8-bit technology field [Bit 31:24] in UMID 
  message, whereas Module refers to 8-bit module field [Bit 23:16] in UMID 
  message
*/
typedef struct
{
  uint8 module_id;
  /*!< Field to store the module_id */

  rf_fwrsp_cb_handler_func_type module_based_cb;
  /*!< Callback function pertaining to a module based dispatching */

} rf_fwrsp_module_table_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to hold Tech specific callabck functions.
*/
typedef struct
{
  uint8 tech_id;
  /*!< Field to store the tech_id */

  rf_fwrsp_cb_handler_func_type tech_based_cb;
  /*!< Callback function pertaining to a tech based dispatching from UMID. This 
  callback function will be executed, only if there is no module registration 
  for this tech (i.e. there is no module-based dispatching ). Module 
  registration for the tech will be checked by looking at the module_table 
  pointer.*/

  rf_fwrsp_module_table_type *module_table;
  /*!< Pointer to module table pertaining to the tech. The memory for this 
  field will dynamically allocated when a registration is requested for 
  each Tech and Module together. */ 

  uint64 module_063_000_reg_mask;
  /*!< Mask to keep track of registration of Modules IDs 0 through 63 */

} rf_fwrsp_tech_table_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure to hold al  Tech specific callabck functions. Techs refers to the 
  8-bit technology field in UMID message.
*/
typedef struct
{
  boolean first_registration;
  /*!< Flag indicating, if first registration is done or not. */

  uint8 max_tech_id;
  /*!< Variable to keep track of maximum tech ID number which has requested 
  registration till now. This will be used to alloc/realloc memory pointed 
  by tech_data */ 

  rf_fwrsp_tech_table_type *tech_data;
  /*!< 
  @brief
  Pointer to the memeory whoch hold tech data for several tech. 
   
  @details 
  This chunk of memory grows dynamically based on the maximum tech id that 
  requests registration with RF_FWRSP_TASK. For instance, assume that following 
  tech_ids request for registration is particular order. 
  -# Tech ID 5 
  -# Tech ID 3 
  -# Tech ID 8. 
   
  In the above sequence, when Tech ID 5 requests registration, tech_data will 
  be allocated with the size of 5 * sizeof(rf_cmd_tech_data_type). For the 
  second registraiton, since 3 is less than 5, no re-allocation is done. 
  However, when Tech ID 8 requests registration, re-allocation will be 
  done with new size of  8 * sizeof(rf_cmd_tech_data_type).   
  */ 

  uint64 tech_063_000_reg_mask;
  /*!< Mask to keep track of registration of Tech IDs 0 through 63 */

  uint64 tech_127_064_reg_mask;
  /*!< Mask to keep track of registration of Tech IDs 127 through 64 */

  uint64 tech_191_128_reg_mask;
  /*!< Mask to keep track of registration of Tech IDs 191 through 128 */

  uint64 tech_255_192_reg_mask;
  /*!< Mask to keep track of registration of Tech IDs 255 through 192 */

} rf_fwrsp_dispatch_table_type;

/*----------------------------------------------------------------------------*/
/*! Enumeration to be used when there is no valid SNUM */
#define RF_FWRSP_NO_SNUM 0

/*----------------------------------------------------------------------------*/
/*! Enumeration to indicate the maximum number of concurrent MSGR messages */
#define RF_FWRSP_MAX_CONCURRENT_RSP 3

/*----------------------------------------------------------------------------*/
/*! Semaphore token ID type */
typedef uint8 rf_fwrsp_snum_item_id_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure definition containing information for second-level dispatching based 
  on sequence number.
 
  @details
  This structure holds information for second-level dispatching based on
  sequence number. 
*/
typedef struct rf_fwrsp_snum_dispatch_info_type_t
{
  rf_fwrsp_snum_item_id_type item_id;
  /*!< Semaphore token ID type */

  uint32 snum;
  /*!< SequenceNumber corresponding to the MSGR message*/

  boolean static_cb_registration;
  /*!< Flag indicating, if the callback registered should be automatically 
  deregistered at the end of the dispatching. FALSE will force the auto 
  deregistration, where as TRUE will ensure no auto-deregistration. 
  Inc ase of TRUE, client should explicity perform deregistration */ 

  rf_fwrsp_snum_handler_func_type snum_cb;
  /*!< Callback function pertaining to the sequence number */

  void *snum_cb_data;
  /*!< Pointer to the opaque data pertaining to the sequence number */

} rf_fwrsp_snum_dispatch_info_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure definition for sequence number doubly linked list containing 
  information for second-level dispatching based on sequence number.
 
  @details
  This structure holds information for second-level dispatching based on
  sequence number. 
*/
typedef struct rf_fwrsp_snum_node
{
  rf_fwrsp_snum_dispatch_info_type snum_info;
  /*!< Structure to hold information for second-level dispatching based on
  sequence number. */

  struct rf_fwrsp_snum_node *prev;
  /*!< Pointer to Previous Node in the linked list */

  struct rf_fwrsp_snum_node *next;
  /*!< Pointer to Next Node in the linked list */

} rf_fwrsp_snum_node_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold sequence number Pool data */
typedef struct
{
  rf_fwrsp_snum_dispatch_info_type no_snum_info;
  /*!< Structure to hold information for second-level dispatching for the 
  message without valid sequence number. */

  rf_fwrsp_snum_node_type *item_list;
  /*!< SNUM items where each entry will correspond to one 
  SNUM entry */

  rf_fwrsp_snum_node_type *sentinel_spare_items;
  /*!< 
  @brief 
  Sentinel for Spare items 
   
  @details 
  This list contains unallocated items. Based on client usage, 
  item status from this list will be removed and put in sentinel_used 
  list. 
  */

  rf_fwrsp_snum_node_type *sentinel_used_items;
  /*!< 
  @brief 
  Sentinel for Used items 
   
  @details 
  This list contains items which are used atleast once. Based on client usage, 
  item from sentinel_spare list will be removed and 
  put in this list. The items in sentinel_used are 
  maintained in time order.  The first item in the list is the most recent 
  (current) item being used
  */

  uint8 num_of_spare_items;
  /*!< Variable to indicate the total number of free items */

  rf_lock_data_type critical_sect;
  /*!< critical section protection to pop and push SNUM items */
  
} rf_fwrsp_snum_pool_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold the callback data related to a given callback
 
  @details
  This structure holds all callback data related to a given UMID. The
  umid_callback is the function call when first-level dispatching is to be
  done. With this funciton, cb_data is passed as a parameter. The callback data
  will have SequenceNumber(SNUM) based callback information. SNUM based
  callbacks are used as second-level dispatching.
*/
typedef struct rf_fwrsp_umid_info_type_t
{
  uint16 index_num;
  /*!< Variable to keep track the index number of the UMID in the UMID list 
  array */

  msgr_umid_type umid;
  /*!< Message router UMID */

  rf_fwrsp_umid_handler_func_type umid_cb;
  /*!< callback function to be called by dispatcher for the given UMID */

  rf_fwrsp_snum_pool_type snum_pool;
  /*!< 
    @brief
    Callback data containing the SequenceNumber based callback 
    information.
   
    @details
    Each FW response messages may or may not have a sequence number associated
    with this. Sequence number is an unique number generated by RF and passed to
    FW. This sequence number is looped back to RF when FW responds to the
    command sent by RF.
   
    In some cases the response messages can come asynchronously (by design or
    due to bug in FW). An instance of by-design - the case when intelliceiver
    is doing poower mode update (by sending IC_UPDATE message to FW); and
    HDR is enablign diversity at the same time. In both the cases, FW will
    respond to RF by sending ACQ_2_TRK_IND message. Now from RF side, we need
    to make sure that these two messages are handled sequentially and properly.
    Thus RF will assign unique sequence number for each case and data is 
    restored in above structure. Thus when the MSGR response is received, data
    from appropriate row (corresponding to the SNUM) will be fetched and
    dispatched.
   
    The array size indicates that maximum size of concurrent responses 
    supported.
   
    In some cases, there may not be any sequence number (generally old UMIDs),
    in this case, ROW 0 will be used, ROW 0 is dedicated to SNUM-less MSGR
    responses.
   
    Callback functions and data are dynamically intialized and deinitialized
    during runtine. For example, before sending IC_UPDATE command, this table
    will be updated and once the message is received and handled, the entry will
    be invalidated (this applies for all row - including Row 0).
   
    Row 1 to ( RF_FWRSP_MAX_CONCURRENT_RSP - 1) will be used as a circular
    buffer. This means with each new dynamic entry, it will be added added to 
    new row. For example (may not be realistic example; but for
    illustration )-
    1. Say RF_FWRSP_MAX_CONCURRENT_RSP = 6
    2. Row 0 is always fioxed for nonSNUM entry
    3. IC_UPDATE command is to be sent to FW. Thus an entry to row 1
       will be made.
    4. RX_START (diversity) command is to be sent to FW. Thus an entry to row 2
       will be made.
    5. ACQ_2_TRK_IND for IC_UPDATE is received. Thus the response is handled
       and data in Row 1 is invalidated.
    6. IC_UPDATE command is to be sent to FW. Thus an entry to row 3
       will be made.
    7. ACQ_2_TRK_IND for RX_START is received. Thus the response is handled
       and data in Row 2 is invalidated.
    8. ACQ_2_TRK_IND for IC_UPDATE is received. Thus the response is handled
       and data in Row 3 is invalidated.
   
    In order to speed up the process of searching SNUM entry, "Sliding Search
    Window" mechanism is applied.
   
    In this approach, two variables are used to keep track of first index (i1)
    corresponding to undispatched SNUM and last index (i2) corresponding to the
    undispatched SNUM. Since the buffer is circular, these two numbers keep
    on moving with i1 <= i2. Thus the search for the SNUM can be limited within
    [i1,i2].
  */

} rf_fwrsp_umid_info_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold list of UMIDs for the given tech
 
  @details
  This structure is used to hold the list of registered UMIDs for the given
  tech. Tech is defined by the 4 MSB bits of the UMID [Bit 31:28]. Container
  for umid_list is dynamicall allocated (and reallocated) based on the number
  of UMID commands registered for the given tech.
*/
typedef struct
{
  uint8 tech;
  /*!< Technology for which the UMID list is populated. Tech corresponds to 
  4 MSB bits of UMID (Bit 31:28)*/

  uint8 num_of_umids;
  /*!< Number of UMID entries */

  rf_fwrsp_umid_info_type *umid_list;
  /*!< Pointer to the memory which holds the list of UMID data. This will 
  be dyanically allocated when the registration is done. The size of this 
  structure will grow as new UMIDs are being registered. */ 

} rf_fwrsp_tech_umid_info_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure defining the singly linked node containing list of umids for a given
  technology.
 
  @details
  This structure holds the definition of a singly linked list node containing 
  the UMIDs callback information for a given tech.
 
  Method to find the callback data for a give UMID:
  - When the MSGR message is received by RF_FWRSP task, the dispatcher will
    extract the MSGR Tech (bit 31:28) and traverse through the linked list
    to find the node corresponding to the tech.
  - In the tech node, it will then find the entry of the UMID.
  - Dispatcher will then call the callback function for the UMID and will also
    pass the callback_data. This callback data will hold the
    SequenceNumber (SNUM) data. This sequence number data will be handled
    individual UMIDs handler unquiely.
*/
typedef struct rf_fwrsp_dispatch_node
{
  rf_fwrsp_tech_umid_info_type tech_umids;
  /*!< Structure containing the  data payload for the node. This payload 
  contains the list of UMIDs and their corresponding dispatch related data. */

  struct rf_fwrsp_dispatch_node *next;
  /*!< Pointer to Previous Node in the linked list */

} rf_fwrsp_dispatch_node_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold dispatcher information for RF_FWRSP task
*/
typedef struct
{

  rf_fwrsp_dispatch_node_type *head_node;
  /*!< Pointer to the head node of the linked list containing the tech 
  specific UMIDs */

  boolean debug_mode;
  /*!< Flag indicating if the debug mode is to be enabled. This mode will 
  print extra F3 messages to aid debugging */

} rf_fwrsp_dispatcher_type;

/*----------------------------------------------------------------------------*/
/*! Structure holding the RF FW response Task data */
typedef struct
{
  rf_fwrsp_housekeeping_data_type housekeeping;
  /*!< Structure to maintain the RF_FWRSP Task housekeeping data */
  
  rf_fwrsp_cmd_q_struct_type fwrsp_cmd_q;
  /*!< Structure to hold RF FWRSP queue data strucutre */

  rf_fwrsp_task_msgr_type fwrsp_msgr;
  /*!< Sturcture to Hold RF FWRSP task msgr related information */

  rf_fwrsp_dispatch_table_type dispatch_tbl;
  /*!< Command dispatch table for RF_FWRSP */

  rf_fwrsp_dispatcher_type dispatcher;
  /*!< Structure containing RF_FWRSP task dispatcher */

  rf_dispatch_handle_type dispatch_handle;
  /*!< Structure to store the dispatch handle data */

} rf_fwrsp_task_data_type;

/*----------------------------------------------------------------------------*/
boolean
rf_fwrsp_dispatch_snum
(
  rf_fwrsp_cmd_type *req_ptr,
  rf_fwrsp_umid_info_type *umid_info,
  uint32 snum
);

/*----------------------------------------------------------------------------*/
void
rf_fwrsp_signal_to_start_task
(
  void
);

/*----------------------------------------------------------------------------*/
rf_fwrsp_dispatch_status_type
rf_fwrsp_dispatch_register
(
  uint16 umid_tech,
  uint32 umid_module,
  rf_fwrsp_cb_handler_func_type callback_fn
);

/*----------------------------------------------------------------------------*/
void
rf_fwrsp_dispatch_deregister
(
  uint16 umid_tech,
  uint32 umid_module
);

/*----------------------------------------------------------------------------*/
boolean
rf_fwrsp_register_command
(
  msgr_tech_module_type module_self,
  msgr_umid_type umid,
  rf_fwrsp_umid_handler_func_type umid_handler
);


/*----------------------------------------------------------------------------*/
boolean
rf_fwrsp_deregister_command
(
  msgr_tech_module_type module_self,
  msgr_umid_type umid
);

/*----------------------------------------------------------------------------*/
boolean
rf_fwrsp_register_snum_callback
(
  msgr_umid_type umid,
  uint32 snum,
  boolean static_cb_registation,
  rf_fwrsp_snum_handler_func_type snum_handler,
  void *snum_data
);

/*----------------------------------------------------------------------------*/
boolean
rf_fwrsp_deregister_snum_callback
(
  msgr_umid_type umid,
  uint32 snum
);

/*----------------------------------------------------------------------------*/
boolean
rf_fwrsp_is_task_running
(
  void
);

/*----------------------------------------------------------------------------*/
const rf_fwrsp_task_msgr_type*
rf_fwrsp_get_msgr_param
(
  void
);

/*----------------------------------------------------------------------------*/
rf_dispatch_handle_type *
rf_fwrsp_task_get_dispatch_handle
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rf_fwrsp_dispatch
(
  rf_fwrsp_cmd_type *cmd
);

#ifdef __cplusplus
}
#endif

#endif /* RF_FWRSP_TASK_H */


