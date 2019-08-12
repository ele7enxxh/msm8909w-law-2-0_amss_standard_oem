/*! 
  @file
  rf_fwrsp_task.c
 
  @brief
  This file includes functions pertaining to Tasks used for firmware response
  handling.
 
  -# RF_FWRSP is a custom task capable of handling only the RF responses from
  FW.
  -# Only MSGR module can put commands to the command queue of this task. No
  interfaces are provided to do REX command put.
  
  @addtogroup RF_COMMON_TASK_FWRSP_HANDLER
  @image html rf_cdma_fwrsp_sync_dispatcher.jpg
  @{
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/src/rf_fwrsp_task.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/24/14   kb      Fix compiler warning
07/11/13    Saul    Cal Time Reduction. Disabled debug mode.
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
06/24/13   JJ      Move Initialize Dispatcher func
04/05/13   spa     Make rf_fwrsp_dispatch non static temporarily to fix warning
04/05/13   spa     Call RF dispatch to handle callbacks internally
04/01/13   spa     Migrated to RF Dispatch for RF-FW synchronization
03/26/13   spa     Compiler warning fix 
03/11/13   sar     Updated file for APQ, GNSS only target.
01/16/13   aro     Renamed handler to handle
01/16/13   aro     Renamed dispatch_handler to dispatch_handle
01/15/13   aro     Updated registration interface to pass dispatch handle
01/08/13   aro     Rex data query and UMID extraction functions added
12/06/12   aro     Doc Update
12/05/12   aro     Merged old and new FWRSP dispatch
12/04/12   aro     F3 message update
11/28/12   aki     Added rf_fwrsp_deinit and handshake with RCINIT at task exit
09/28/12   aro     Fixed static callback registration
09/10/12   aro     Feature to have static/dynamic callbakc registration
09/10/12   aro     MSGR request pointer added as argument in SNUM dispatch
08/22/12   aro     F3 update
08/22/12   aro     Coverted SNUM linked list with dynamic memory allocation
                   to support packing in UMID list after command de-registration
                   (memory footprint reduction)
08/22/12   aro     Added UMID index number
08/22/12   aro     F3 message update
08/22/12   aro     Added memset after dynamic memory allocation
08/16/12   aro     Generic FW response handler function
08/16/12   aro     Added common function to extract sequence number from
                   MSGR Payload
08/16/12   aro     Updated SNUM callback function prototype to pass SNUM data
08/16/12   aro     F3 message update
08/15/12   aro     Compiler warning fix
08/15/12   aro     Minor Documentation Update
08/15/12   aro     Added deregistration of SNUM callback functions at the end
                   of SNUM dispatching
08/15/12   aro     SNUM push implemented at the end of SNUM dispatching
08/15/12   aro     SNUM dispatcher function
08/15/12   aro     Added new FWRSP dispatcher function
08/15/12   aro     Converted functions to STATIC
08/15/12   aro     Implemented SNUM dispatcher deregistration to be done
                   using SNUMPool
08/15/12   aro     Updated SNUM dispatcher registration to be done using
                   SNUMPool implementation
08/14/12   aro     Interfaces to handle SNUM doubly linked list
08/14/12   aro     Implementation for SNUM disptacher registration
08/14/12   aro     Linked list deletion while doing MSGR dereg
08/14/12   aro     Added MSGR deregistration in UMID dereg function
08/14/12   aro     Added MSGR registration in UMID reg function
08/14/12   aro     Code to delete linked list node
08/14/12   aro     Implementation to perform UMID deregistration
08/14/12   aro     Enhanced UMID registration with error checkin
08/14/12   aro     Implementation to perform UMID registration
08/14/12   aro     Function to traverse the linked list and to add new node
08/08/12   aro     Defined interfaces to command registration
08/08/12   aro     Moved FWRSP data structure to header file
08/07/12   aro     Renamed function pointer type name
04/19/12   hdz     Fixed klocwork errors 
03/21/12   aro     Added function to get constant pointer to MSGR parameter 
                   used by RF_FWRSP_TASK
03/21/12   aro     Enabled spawning of RF_FWRSP_TASK
03/20/12   aro     Interface to return the running state RF_FWRSP task
03/20/12   aro     Implemented the Command dispatch
03/20/12   aro     Added interface to perform UMID based dispatcher 
                   registration 
03/20/12   aro     Added interface to perform UMID based dispatcher 
                   de-registration
03/20/12   aro     Implemented REX-MSGR initialization
03/20/12   aro     Initial implementation of REX-MSGR initialization
03/20/12   aro     Created function to create / free command queues
03/20/12   aro     Added function to get the number of commands in
                   the queue and the command buffer
03/20/12   aro     Support to wait and start the task
03/20/12   aro     Added initial revision of function to spawn RF_FWRSP task
03/20/12   aro     Data structure to hold RF_FWRSP task
03/20/12   aro     Documentation Update
03/20/12   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "rf_fwrsp_task.h"
#include "rfcommon_msg.h"
#include "task.h"
#include "msgr.h"
#include "msgr_rex.h"
#include "rcinit.h"
#include "rfa_msgr.h"
#include "modem_mem.h"
#include "rfm_internal.h"
#include "rf_dispatch.h"
#include "stringl.h" /* for memscpy */


/*----------------------------------------------------------------------------*/
/*! Externalized RF iC Task control block variable */
extern rex_tcb_type rf_fwrsp_tcb;

/*----------------------------------------------------------------------------*/
/*! Variable to store the global data for RF_FWRSP task */
static rf_fwrsp_task_data_type rf_fwrsp_task_data;

/*============================================================================*/
/*!
  @name REX-MSGR

  @brief
  Functions Pertaining to initialization and de-initialization of REX-MSGR
*/
/*! @{ */

void rf_fwrsp_deinit(void)
{
  msgr_client_delete(&(rf_fwrsp_task_data.fwrsp_msgr.client));
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF FWRSP Rex MSGR interface initialization

  @details
  This funciton creates the message router client on the RF FWRSP TCB, registers 
  for all the MSGR commands that RF FWRSP task wishes to recieve.
 
  Create message queue to receive message router messages from other SW
  entities. RF is a REX Task and it waits on REX signals. Hence a REX message 
  queue has to be added to the message router client.

  @param p_rf_ic_tcb
  Pointer to RF IC TCB structure
 
  @param sig
  REX Signal to wait on.
 
  @return
  Flag indicating, if clients are successfully created or not.
*/
static boolean 
rf_fwrsp_rex_msgr_init
(
  rex_tcb_type *p_rf_fwrsp_tcb,
  rex_sigs_type rf_rex_sig
)
{
  boolean ret_val =  TRUE;
  errno_enum_type msgr_status;

  /* Create RF CDMA FWRSP Task message router client for receiving messages
  from  SW entities */
  msgr_status = msgr_client_create(&(rf_fwrsp_task_data.fwrsp_msgr.client));

  if ( msgr_status != E_SUCCESS )
  {
    RF_MSG( RF_ERROR, "rf_fwrsp_rex_msgr_init : "
            "Unable to create RF message router client ");  
    ret_val = FALSE;
  }

  if (msgr_status == E_SUCCESS)
  {
    msgr_status = msgr_client_add_rex_q(
                                      &(rf_fwrsp_task_data.fwrsp_msgr.client),
                                      p_rf_fwrsp_tcb,
                                      rf_rex_sig, 
                                      &(rf_fwrsp_task_data.fwrsp_cmd_q.cmd_q),
                                      &(rf_fwrsp_task_data.fwrsp_cmd_q.free_q),
                                      sizeof(rf_fwrsp_cmd_rex_hdr_type),
                                      sizeof(q_link_type),
                                      sizeof(rf_fwrsp_cmd_type),
                                      &(rf_fwrsp_task_data.fwrsp_msgr.mb)
                                      );

    if (msgr_status != E_SUCCESS)
    {
      RF_MSG( RF_ERROR,"rf_fwrsp_rex_msgr_init : "
              "Unable to add RF REX MSGR client ");  
      ret_val = FALSE;
    }
  }

  /* Return Status */
  return ret_val;

} /* rf_fwrsp_rex_msgr_init */

/*! @} */

/*============================================================================*/
/*!
  @name Command Queue

  @brief
  Functions Pertaining to Command Queue for RF FWRSP usage
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize RF FWRSP TASK CMD queue.

  @details
  This function initializes the RF FWRSP task Command Queue Structures.
*/
static void
rf_fwrsp_create_command_q
(
  void
)
{
  uint32 loop_cnt;

  /* Initialize the Command Queue, and the Command Free Queue */
  (void) q_init( &(rf_fwrsp_task_data.fwrsp_cmd_q.cmd_q) );
  (void) q_init( &(rf_fwrsp_task_data.fwrsp_cmd_q.free_q) );

  /* Link the Command Queue Items onto the Command Free Queue */
  for ( loop_cnt = 0; 
        loop_cnt < ARR_SIZE((rf_fwrsp_task_data.fwrsp_cmd_q.q_items)); 
        loop_cnt++)
  {
    q_put( &(rf_fwrsp_task_data.fwrsp_cmd_q.free_q),
           q_link( &(rf_fwrsp_task_data.fwrsp_cmd_q.q_items[loop_cnt]), 
             &(rf_fwrsp_task_data.fwrsp_cmd_q.q_items[loop_cnt].rex_hdr.link) )
          );
  }

} /* rf_fwrsp_create_command_q */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the number of commands on RF FWRSP TASK cmd Queue
 
  @return
  Total the number of commands on the cmd Queue
*/
static int
rf_fwrsp_get_command_num
(
  void
)
{

  return q_cnt( &(rf_fwrsp_task_data.fwrsp_cmd_q.cmd_q) );

} /* rf_fwrsp_get_command_num */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Retreive the command from  RF FWRSP queue.

  @details
  This function will retreive the command from RF FWRSP Task queue, and then 
  removes the command from Queue.

  @return
  Address of the command retreived from the command queue.
*/
static rf_fwrsp_cmd_type* 
rf_fwrsp_get_command
(
  void
)
{
   rf_fwrsp_cmd_type * item;

   /* Retrieve next commmand from command queue */
   item = (rf_fwrsp_cmd_type *) q_get(&(rf_fwrsp_task_data.fwrsp_cmd_q.cmd_q));

   if ( item != NULL )
   {
     item->status = RF_FWRSP_CMD_PROCESSING;
   }
   else
   {
     ERR_FATAL( "rf_fwrsp_get_command : Signal for the"
                " queue was set, but no cmd in Q",0 ,0 ,0 ); 
   }

   return item;

} /* rf_fwrsp_get_command */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Free the command buffer on RF FWRSP Task queue.

  @details
  This function removes an already processed command from RF FWRSP Task queue.
  The memory is returned to free queue.
  
  @param cmd
  The address of the command to be freed from RF FWRSP task queue.
*/
static void
rf_fwrsp_free_command
(
  rf_fwrsp_cmd_type* cmd
)
{

  if ( cmd == NULL )
  {
    RF_MSG( RF_ERROR, "rf_fwrsp_free_command: Invalid cmd in Q" );
    return;
  }

  memset( cmd, 0, sizeof(rf_fwrsp_cmd_type) );

  /* Mark item as no longer valid */
  cmd->status = RF_FWRSP_CMD_BUF_EMPTY;

 ( void )q_link( cmd, &(cmd->rex_hdr.link) );

  /* Add the item to the free queue */
  q_put( &(rf_fwrsp_task_data.fwrsp_cmd_q.free_q), 
         &(cmd->rex_hdr.link) );

  return;

} /* rf_fwrsp_free_command */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Rex Data
 
  @details
  In order perform REX-MSGR initialization for FWRSP task, REX data is
  required. This interface return the REX data to be used by RF dispatch to
  perform REX-MSGR registration
 
  @param rex_data_ptr
  Pointer to the container where REX data is populated
 
  @return
  Flag indicating the validity of the populated Rex data
*/
static boolean
rf_fwrsp_get_rex_data
(
  rf_dispatch_rex_data_type *rex_data_ptr
)
{
  /* NULL Pointer check */
  if ( rex_data_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rf_fwrsp_get_rex_data: NULL container" );
    return FALSE;
  } /* if ( rex_data_ptr == NULL ) */

  rex_data_ptr->rex_tcb = &rf_fwrsp_tcb;
  rex_data_ptr->rex_sig = RF_FWRSP_CMD_Q_SIG;
  rex_data_ptr->cmd_q = &(rf_fwrsp_task_data.fwrsp_cmd_q.cmd_q);
  rex_data_ptr->free_q = &(rf_fwrsp_task_data.fwrsp_cmd_q.free_q);
  rex_data_ptr->header_size = sizeof(rf_fwrsp_cmd_rex_hdr_type);
  rex_data_ptr->q_link_size = sizeof(q_link_type);
  rex_data_ptr->command_size = sizeof(rf_fwrsp_cmd_type);

  return TRUE;
} /* rf_fwrsp_get_rex_data */

/*! @} */

/*============================================================================*/
/*!
  @name Command Registration

  @brief
  This section includes dispatcher interfaces to perform registration using
  Technology from UMID. There is no command based registration. Dispatcher
  will maintain an static table to accomodate callback handler funciton
  for each tech, where the number of techs is defined by maximum number of
  techs supported by UMID. Tech is specified by bit[31:24] of an UMID. Thus
  the maximum number of Techs supported would be 2^8 = 256.
 
  There are two kinds of dispatching.
 
  <b><i>Tech Based:</i></b> In this kind of dispatching, the RF_FWRSP_TASK will 
  look at the tech specified in the UMID (bit[31:24]) and dispatch by calling 
  the callback handler function pertaining to this tech.
 
  <b><i>Module Based:</i></b> In this kind of dispatching, the RF_FWRSP_TASK
  will look at the module specified in the UMID (bit[23:16]) and dispatch by 
  calling the callback handler function pertaining to this module. Module based 
  callback function will always have the precedence eventhough the tech based 
  callback handler function is registerd.
 
  @image html rf_task_dispatch_table.jpg
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform UMID tech based registration
 
  @details
  This function should be called to perform registration of the callback
  handler funciton for a technology. The technology enumeration corresponds
  to the tech field (31:24) from 32-bit UMID.
 
  This function should be called only once per tech. If the function is called
  back-to-back for same tech without de-registration in the middle, then the
  scenario will be flagged as a failure and the second registration attempt
  will be ignored.
 
  @param umid_tech
  Tech corresponding bit [31:24] from UMID which is to be registered with the
  RF task
 
  @param umid_module
  Module corresponding bit [23:16] from UMID which is to be registered with the
  RF task. This field will be used to identify, if tech-based or module-based
  registration is requested. If the umid_module is less than
  MSGR_MODULE_RF_MAX, then it is considered module-based; if not, it will be
  tech-based.
 
  This indicates that for this tech, module based registration 
  needs to be done. This will be for the UMIDs which need to dispatched 
  based on module such as MSGR_TECH_RFA. MSGR_TECH_RFA is special because 
  this tech has 1x/HDR/WCDMA/LTE/TDSCDMA as modules; and the UMID has to be 
  dispatched  based on the module. In this case, module dispatching always 
  get precedence over the tech based dispatching. This field will be 
  populated during dispatch init only.
 
  @param tech_callback
  Callback handler function which is to be registered for the given tech.
 
  @return
  Status indicating, whether the tech registration was successfull or not.
*/
rf_fwrsp_dispatch_status_type
rf_fwrsp_dispatch_register
(
  uint16 umid_tech,
  uint32 umid_module,
  rf_fwrsp_cb_handler_func_type callback_fn
)
{
  rf_fwrsp_module_table_type *module_tbl_ptr = NULL;
  rf_fwrsp_tech_table_type *tech_tbl_ptr = NULL;
  rf_fwrsp_dispatch_table_type *disp_tlb = NULL;
  
  /* Perform Parameter Validation : umid_tech */
  if ( umid_tech >= RF_FWRSP_NUM_OF_UMID_TECHS )
  {
    RF_MSG_1( RF_ERROR, "rf_fwrsp_dispatch_register: Invalid Tech 0x%x",
              umid_tech );
    return RF_FWRSP_CMD_REG_FAILURE;
  } /* if ( umid_tech >= RF_FWRSP_NUM_OF_UMID_TECHS ) */

  /* Perform Parameter Validation : tech_callback */
  if ( callback_fn == NULL )
  {
    RF_MSG_1( RF_ERROR, "rf_fwrsp_dispatch_register: Invalid CB for "
                        "Tech 0x%x", umid_tech );
    return RF_FWRSP_CMD_REG_FAILURE;
  } /* if ( tech_callback == NULL ) */

  disp_tlb = &(rf_fwrsp_task_data.dispatch_tbl);

  if ( disp_tlb->first_registration == TRUE )
  {
    disp_tlb->tech_data = 
      modem_mem_alloc( sizeof(rf_fwrsp_tech_table_type)*(umid_tech + 1),
                       MODEM_MEM_CLIENT_RFA);
    disp_tlb->max_tech_id = umid_tech;
    disp_tlb->first_registration = FALSE;

    if ( disp_tlb->tech_data != NULL )
    {
      memset( disp_tlb->tech_data, 0x0,
              sizeof(rf_fwrsp_tech_table_type)*(umid_tech + 1) );
    }
  } /* if ( first_registration == TRUE ) */
  else
  {
    uint8 *tbl_start_memory_ptr = NULL;
    uint32 old_tech_data_size = 0;
    uint8 *tbl_added_memory_ptr = NULL;
    uint32 new_tech_data_size = 0;
    uint32 memset_size = 0;

    /* Check if rellocation is required or not */
    if ( umid_tech + 1 > disp_tlb->max_tech_id )
    {
      new_tech_data_size = sizeof(rf_fwrsp_tech_table_type)*(umid_tech + 1);
      disp_tlb->tech_data =
        modem_mem_realloc( disp_tlb->tech_data,
                           new_tech_data_size,
                           MODEM_MEM_CLIENT_RFA);

      tbl_start_memory_ptr = (uint8*)disp_tlb->tech_data;

      if ( disp_tlb->tech_data != NULL )
      {
        old_tech_data_size = 
         sizeof(rf_fwrsp_tech_table_type)*(disp_tlb->max_tech_id+1);
        memset_size = new_tech_data_size - old_tech_data_size;
        tbl_added_memory_ptr = tbl_start_memory_ptr  + old_tech_data_size;
        memset( tbl_added_memory_ptr, 0x0, memset_size );

        RF_MSG_5( RF_HIGH, "rf_fwrsp_dispatch_register: Tech REALLOC : "
                  "[0x%8x] { 0x%8x + 0x%8x = 0x%8x } [0x%8x]",
                  tbl_start_memory_ptr,
                  old_tech_data_size,
                  memset_size,
                  new_tech_data_size,
                  tbl_added_memory_ptr
                  );
      } /* if ( disp_tlb->tech_data != NULL ) */
      else
      {
        RF_MSG( RF_ERROR, "rf_fwrsp_dispatch_register: Tech REALLOC failed");
      } /* if ! ( disp_tlb->tech_data != NULL ) */

      disp_tlb->max_tech_id = umid_tech;

    } /* if ( umid_tech + 1 > disp_tlb->max_tech_id ) */
  } /* if ! ( first_registration == TRUE ) */

  tech_tbl_ptr = disp_tlb->tech_data;

  if ( tech_tbl_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rf_fwrsp_dispatch_register: Tech REALLOC failed");

    return RF_FWRSP_CMD_REG_FAILURE;
  }

  /* Check, if the registraion is module-based or tech-based */
  if ( umid_module < MSGR_MODULE_RF_MAX )
  {
    /*------------------------------------------------------------------------*/
    /* Module Based registration */
    /*------------------------------------------------------------------------*/

    /* Check whether the Module ID is valis or not */
    if ( umid_module >= MSGR_MODULE_RF_MAX )
    {
      RF_MSG_2( RF_ERROR, "rf_fwrsp_dispatch_register: Invalid Module 0x%x "
                          "for Tech 0x%x", umid_module, umid_tech );
      return RF_FWRSP_CMD_REG_FAILURE;
    } /* if ( umid_module >= MSGR_MODULE_RF_MAX ) */

    /* Only one registration is allowed for each tech. If there are back to
    back registration attempts, then this will be error condition. */
    if ( tech_tbl_ptr[umid_tech].tech_based_cb != NULL )
    {
      //RF_MSG_1( RF_FATAL, "rf_fwrsp_dispatch_register: Tech 0x%x already "
      //           "(Tech Based)registered. Aborting MODULE-BASED.",
      //           umid_tech );

      ERR_FATAL( "rf_fwrsp_dispatch_register: Tech 0x%x already "
                 "(Tech Based)registered. Aborting MODULE-BASED.",
                 umid_tech, 0, 0 );
      return RF_FWRSP_CMD_REG_FAILURE;
    } /* if ( rf_tech_table[umid_tech].tech_based_cb != NULL ) */
    else
    {
      if ( tech_tbl_ptr[umid_tech].module_table != NULL )
      {
        /* Get the Module Table pointer */
        module_tbl_ptr = tech_tbl_ptr[umid_tech].module_table;

        if ( module_tbl_ptr[umid_module].module_based_cb != NULL )
        {
          //RF_MSG_2( RF_FATAL, "rf_fwrsp_dispatch_register: Tech 0x%x Module " 
          //          "0x%x already registered. Aborting MODULE-BASED.",
          //           umid_tech, umid_tech );

          ERR_FATAL( "rf_fwrsp_dispatch_register: Tech 0x%x Module 0x%x "
                     "already registered. Aborting MODULE-BASED.",
                     umid_tech, umid_tech, 0 );
          return RF_FWRSP_CMD_REG_FAILURE;
        }
      } /* if ( rf_tech_table[umid_tech].module_table != NULL ) */
    } /* if ! ( rf_tech_table[umid_tech].tech_based_cb != NULL ) */

    /* Check if the memory allocation for the module table is already done
    or not. If the allocation is not done, then  allocate memory */
    if ( tech_tbl_ptr[umid_tech].module_table == NULL )
    {
      tech_tbl_ptr[umid_tech].module_table = 
         modem_mem_alloc( sizeof(rf_fwrsp_module_table_type)*MSGR_MODULE_RF_MAX,
                          MODEM_MEM_CLIENT_RFA);
  
      /* NULL pointer check for allocated memory */
      if ( tech_tbl_ptr[umid_tech].module_table == NULL )
      {
        RF_MSG_2( RF_ERROR, "rf_fwrsp_dispatch_register: Malloc failed for "
                            "Tech 0x%x, Module 0x%x", umid_tech, umid_module );
        return RF_FWRSP_CMD_REG_FAILURE;
      } /* if ( rf_tech_table[umid_tech].module_table == NULL ) */ 
      else
      {
        memset( tech_tbl_ptr[umid_tech].module_table, 0x0,
                sizeof(rf_fwrsp_module_table_type)*MSGR_MODULE_RF_MAX );
      }
    } /* if ( rf_tech_table[umid_tech].module_table == NULL ) */

    tech_tbl_ptr[umid_tech].tech_id = umid_tech;

    /* Get the Module Table pointer */
    module_tbl_ptr = tech_tbl_ptr[umid_tech].module_table;

    /* Go ahead withe Module Registration */

    /* Register Tech callback funtion */
    module_tbl_ptr[umid_module].module_based_cb = callback_fn;

    module_tbl_ptr[umid_module].module_id = umid_module;

    if (umid_module <= 63 )
    {
      tech_tbl_ptr[umid_tech].module_063_000_reg_mask |= ( 1 << umid_module );
    }

    RF_MSG_3( RF_MED, "rf_fwrsp_dispatch_register: "
              "MODULE BASED [ 0x%x / 0x%x ] | Tech Mask 0x%x", 
              umid_tech, 
              umid_module,
              tech_tbl_ptr[umid_tech].module_063_000_reg_mask );
    
  } /* if ( rf_tech_table[umid_tech].module_based_dispatch == TRUE ) */
  else
  {
    /*------------------------------------------------------------------------*/
    /* Tech Based registration */
    /*------------------------------------------------------------------------*/

    /* Only one registration is allowed for each tech. If there are back to
    back registration attempts, then this will be error condition. */
    if ( tech_tbl_ptr[umid_tech].tech_based_cb != NULL )
    {
      //RF_MSG_1( RF_FATAL, "rf_fwrsp_dispatch_register: Tech 0x%x already "
      //           "(Tech Based)registered",umid_tech );

      ERR_FATAL( "rf_fwrsp_dispatch_register: Tech 0x%x already "
                 "(Tech Based)registered",umid_tech, 0, 0 );
      return RF_FWRSP_CMD_REG_FAILURE;
    }
    else if ( tech_tbl_ptr[umid_tech].module_table != NULL )
    {
      //RF_MSG_1( RF_FATAL, "rf_fwrsp_dispatch_register: Tech 0x%x already "
      //           "(Module Based)registered",umid_tech );

      ERR_FATAL( "rf_fwrsp_dispatch_register: Tech 0x%x already "
                "(Module Based)registered",umid_tech, 0, 0 );
      return RF_FWRSP_CMD_REG_FAILURE;
    }

    /* Go ahead with tech Registration */

    /* Register Tech callback funtion */
    tech_tbl_ptr[umid_tech].tech_based_cb = callback_fn;

    tech_tbl_ptr[umid_tech].tech_id = umid_tech;

    if ( umid_tech <= 63 )
    {
      disp_tlb->tech_063_000_reg_mask |= ( 1 << umid_tech );
    }
    else if ( umid_tech >=127 && umid_tech <= 63 )
    {
      disp_tlb->tech_127_064_reg_mask |= ( 1 << umid_tech );
    }
    else if ( umid_tech >=191 && umid_tech <= 128 )
    {
      disp_tlb->tech_191_128_reg_mask |= ( 1 << umid_tech );
    }
    else if ( umid_tech >=255 && umid_tech <= 192 )
    {
      disp_tlb->tech_255_192_reg_mask |= ( 1 << umid_tech );
    }

    RF_MSG_5( RF_MED, "rf_fwrsp_dispatch_register: "
              "TECH BASED [ 0x%x ] | Tech Mask 0x%x%x%x%x",
              umid_tech,
              disp_tlb->tech_255_192_reg_mask,
              disp_tlb->tech_191_128_reg_mask,
              disp_tlb->tech_127_064_reg_mask,
              disp_tlb->tech_063_000_reg_mask );
    
  } /* if ! ( rf_tech_table[umid_tech].module_based_dispatch == TRUE ) */

  /* return success */
  return RF_FWRSP_CMD_REG_SUCCESS;

} /* rf_fwrsp_dispatch_register */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Perform UMID tech based deregistration
 
  @details
  This function should be called to perform deregistration of the callback
  handler funciton for a technology. The technology enumeration corresponds
  to the tech field (31:24) from 32-bit UMID.
 
  @param umid_tech
  Tech corresponding bit [31:24] from UMID which is to be deregistered with the
  RF task
*/
void
rf_fwrsp_dispatch_deregister
(
  uint16 umid_tech,
  uint32 umid_module
)
{
  rf_fwrsp_dispatch_table_type *disp_tlb = NULL;
  rf_fwrsp_tech_table_type *tech_tbl_ptr = NULL;
  uint64 new_module_063_000_reg_mask = 0;
  uint64 new_tech_063_000_reg_mask = 0;
  uint64 new_tech_127_064_reg_mask = 0;
  uint64 new_tech_191_128_reg_mask = 0;
  uint64 new_tech_255_192_reg_mask = 0;

  /* Perform Parameter Validation : umid_tech */
  if ( umid_tech >= RF_FWRSP_NUM_OF_UMID_TECHS )
  {
    RF_MSG_1( RF_ERROR, "rf_fwrsp_dispatch_deregister: Invalid Tech 0x%x",
              umid_tech );
    return;
  } /* if ( umid_tech >= RF_FWRSP_NUM_OF_UMID_TECHS ) */

  disp_tlb = &(rf_fwrsp_task_data.dispatch_tbl);

  tech_tbl_ptr = disp_tlb->tech_data;

  /* Dellocate memory if module based dispatch was done */

  /* Check if the memory allocation for the module table was done
  or not. If the allocation is done, then de-allocate memory */
  if ( tech_tbl_ptr[umid_tech].module_table != NULL )
  {
    /* Perform Parameter Validation : umid_tech */
    if ( umid_module > disp_tlb->max_tech_id )
    {
      RF_MSG_2( RF_ERROR, "rf_fwrsp_dispatch_deregister: Invalid Mode 0x%x "
                          "for tech 0x%x", umid_module, umid_tech );
      return;
    } /* if ( umid_tech >= disp_tlb->max_tech_id ) */

    tech_tbl_ptr[umid_tech].module_table[umid_module].module_id = 0;
    tech_tbl_ptr[umid_tech].module_table[umid_module].module_based_cb = NULL;

    if ( umid_module <= 63 )
    {
      new_module_063_000_reg_mask |= (1 << umid_module );
      new_module_063_000_reg_mask = ~new_module_063_000_reg_mask;

      /* Update the Vote Mask */
      tech_tbl_ptr[umid_tech].module_063_000_reg_mask &= 
                                              new_module_063_000_reg_mask;
    }

    if ( tech_tbl_ptr[umid_tech].module_063_000_reg_mask == 0x0 )
    {
      modem_mem_free( tech_tbl_ptr[umid_tech].module_table,
                      MODEM_MEM_CLIENT_RFA);
      tech_tbl_ptr[umid_tech].module_table = NULL;
      RF_MSG( RF_MED, "rf_fwrsp_dispatch_deregister: Module deallocated" );      
    }

    RF_MSG_3( RF_MED, "rf_fwrsp_dispatch_deregister: "
              "MODULE BASED [ 0x%x / 0x%x ] | Tech Mask 0x%x", 
              umid_tech, 
              umid_module,
              tech_tbl_ptr[umid_tech].module_063_000_reg_mask );

  } /* if ( rf_tech_table[umid_tech].module_table != NULL ) */
  else
  {
    tech_tbl_ptr[umid_tech].tech_id = 0x0;
    tech_tbl_ptr[umid_tech].tech_based_cb = NULL;

    if ( umid_tech <= 63 )
    {
      new_tech_063_000_reg_mask |= (1 << umid_tech );
      new_tech_063_000_reg_mask = ~new_tech_063_000_reg_mask;
      disp_tlb->tech_063_000_reg_mask &= 
                                              new_tech_063_000_reg_mask;
    }
    else if ( umid_tech >=127 && umid_tech <= 63 )
    {
      new_tech_127_064_reg_mask |= (1 << umid_tech );
      new_tech_127_064_reg_mask = ~new_tech_127_064_reg_mask;
      disp_tlb->tech_127_064_reg_mask &= new_tech_127_064_reg_mask;
    }
    else if ( umid_tech >=191 && umid_tech <= 128 )
    {
      new_tech_191_128_reg_mask |= (1 << umid_tech );
      new_tech_191_128_reg_mask = ~new_tech_191_128_reg_mask;
      disp_tlb->tech_191_128_reg_mask &= new_tech_191_128_reg_mask;
    }
    else if ( umid_tech >=255 && umid_tech <= 192 )
    {
      new_tech_255_192_reg_mask |= (1 << umid_tech );
      new_tech_255_192_reg_mask = ~new_tech_255_192_reg_mask;
      disp_tlb->tech_255_192_reg_mask &= new_tech_255_192_reg_mask;
    }

    if ( ( disp_tlb->tech_063_000_reg_mask == 0x0 ) &&
         ( disp_tlb->tech_127_064_reg_mask == 0x0 ) &&
         ( disp_tlb->tech_191_128_reg_mask == 0x0 ) &&
         ( disp_tlb->tech_255_192_reg_mask == 0x0 )
       )
    {
      modem_mem_free( disp_tlb->tech_data, MODEM_MEM_CLIENT_RFA);
      disp_tlb->tech_data = NULL;
      
      RF_MSG( RF_MED, "rf_fwrsp_dispatch_deregister: Tech deallocated" );
    }

    RF_MSG_5( RF_MED, "rf_fwrsp_dispatch_deregister: "
              "TECH BASED [ 0x%x ] | Tech Mask 0x%x%x%x%x",
              umid_tech,
              disp_tlb->tech_255_192_reg_mask,
              disp_tlb->tech_191_128_reg_mask,
              disp_tlb->tech_127_064_reg_mask,
              disp_tlb->tech_063_000_reg_mask );
  }

  /* return */
  return;

} /* rf_fwrsp_dispatch_deregister */

/*! @} */


/*============================================================================*/
/*!
  @name UMID - SequenceNumber Dispatcher Registration

  @brief
  Functions Pertaining to UMID registration on Dispatcher
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Traverse through the linked list
 
  @details
  This function will traverse through the dispatch node linked list to find the
  node corresponding to the given tech. If no node is found, NULL will be
  returned.
 
  @param start_node
  Pointer to the node from when the search needs to be started
 
  @param tech
  The tech comparison token to match the node.
 
  @return
  Pointer to the Node corresponding to the given tech
*/
static rf_fwrsp_dispatch_node_type*
rf_fwrsp_get_tech_node
(
  rf_fwrsp_dispatch_node_type *head_node,
  uint8 tech
)
{
  rf_fwrsp_dispatch_node_type *node_ptr = NULL;
  rf_fwrsp_dispatch_node_type *match_node = NULL;
  
  /* Intialize the current node with head node value */
  node_ptr = head_node;
  
  /* Proceed only if head_node passsed in not NULL */
  if ( node_ptr != NULL )
  {
    do /* ( curr_node != NULL ) */
    {
      if ( node_ptr->tech_umids.tech == tech )
      {
        match_node = node_ptr;
        break;
      } /* if ( curr_node->tech_umids.tech == tech ) */

      /* Traverse to next node */
      node_ptr = node_ptr->next;

    } while ( node_ptr != NULL );
  } /* if ( curr_node != NULL ) */

  /* return the match node */
  return match_node;

} /* rf_fwrsp_get_tech_node */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Create new tech Node
 
  @details
  This function will traverse through the dispatch node linked list to find the
  last node and will create a new node.
 
  @param head_node
  Pointer to the node from when the search needs to be started
 
  @param tech
  The tech for which the node is to be created.
 
  @return
  Pointer to the new Node corresponding to the given tech
*/
static rf_fwrsp_dispatch_node_type*
rf_fwrsp_create_tech_node
(
  rf_fwrsp_dispatch_node_type **head_node,
  uint8 tech
)
{
  rf_fwrsp_dispatch_node_type *curr_node = NULL;
  rf_fwrsp_dispatch_node_type *last_node = NULL;
  rf_fwrsp_dispatch_node_type *ret_val = NULL;

  /* Intialize the current node with head node value */
  curr_node = *head_node;
  last_node = *head_node;

  /* Proceed only if head_node passsed in not NULL */
  if ( *head_node == NULL )
  {
    *head_node = modem_mem_alloc( sizeof(rf_fwrsp_dispatch_node_type),
                                  MODEM_MEM_CLIENT_RFA );
    if ( *head_node == NULL )
    {
      RF_MSG_1( RF_ERROR, "rf_fwrsp_create_tech_node: TECH.0x%x - Head node "
                        "Malloc failed", tech );
      ret_val = NULL;
    } /* if ( *linked_list == NULL ) */
    else
    {
      memset( *head_node, 0x0, sizeof(rf_fwrsp_dispatch_node_type) );
      (*head_node)->next = NULL;
      (*head_node)->tech_umids.tech = tech;
      (*head_node)->tech_umids.num_of_umids = 0;
      (*head_node)->tech_umids.umid_list = NULL;
      ret_val = *head_node;
      RF_MSG_1( RF_MED, "rf_fwrsp_create_tech_node: TECH.0x%x - "
                        "Head Node created", tech );
    } /* if ! ( *linked_list == NULL ) */
  }
  else
  {
    do /* ( curr_node != NULL ) */
    {
      if ( curr_node->next == NULL )
      {
        last_node = curr_node;
        break;
      } /* if ( curr_node->next == NULL ) */
  
      /* Traverse to next node */
      curr_node = curr_node->next;

    } while ( curr_node != NULL );

    last_node->next = modem_mem_alloc( sizeof(rf_fwrsp_dispatch_node_type),
                                       MODEM_MEM_CLIENT_RFA );
    if ( last_node->next == NULL )
    {
      RF_MSG_1( RF_ERROR, "rf_fwrsp_create_tech_node: TECH.0x%x - Node Malloc "
                          "failed", tech );
      ret_val = NULL;
    } /* if ( *linked_list == NULL ) */
    else
    {
      memset( last_node->next, 0x0, sizeof(rf_fwrsp_dispatch_node_type) );
      last_node->next->next = NULL;
      last_node->next->tech_umids.tech = tech;
      last_node->next->tech_umids.num_of_umids = 0;
      last_node->next->tech_umids.umid_list = NULL;
      ret_val = last_node->next;
      RF_MSG_1( RF_MED, "rf_fwrsp_create_tech_node: TECH.0x%x - Node created",
                tech );
    } /* if ! ( *linked_list == NULL ) */
  }

  /* return the Tech node */
  return ret_val;

} /* rf_fwrsp_create_tech_node */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Delete tech Node
 
  @details
  This function will traverse through the dispatch node linked list to find the
  node corresponding to the given tech to delete.
 
  @param head_node
  Pointer to the node from when the search needs to be started
 
  @param tech
  The tech for which the node is to be deleted.
 
  @return
  Flag indicating the status of Tech Node deletion
*/
static boolean
rf_fwrsp_delete_tech_node
(
  rf_fwrsp_dispatch_node_type **head_node,
  uint8 tech
)
{
  rf_fwrsp_dispatch_node_type *curr_node = NULL;
  rf_fwrsp_dispatch_node_type *match_node = NULL;
  rf_fwrsp_dispatch_node_type *match_n_1_node = NULL;
  boolean ret_val = TRUE;

  /* Intialize the current node with head node value */
  curr_node = *head_node;

  /* Proceed only if head_node passsed in not NULL */
  if ( *head_node != NULL )
  {
    do /* ( curr_node != NULL ) */
    {
      if ( curr_node->tech_umids.tech == tech )
      {
        match_node = curr_node;
        break;
      } /* if ( curr_node->tech_umids.tech == tech ) */
  
      /* Store the current node */
      match_n_1_node = curr_node;

      /* Traverse to next node */
      curr_node = curr_node->next;

    } while ( curr_node != NULL );

    /* Check if a  match is found. For the case where match is found, set
    the NEXT pointers in linked list and delete the node */
    if ( match_node != NULL )
    {
      if ( match_node == *head_node )
      {
        /* Case when head node is to be removed */
        *head_node = match_node->next;
      } /* if ( match_node == *head_node ) */
      else 
      {
        /* Case when non-head node is to be removed */
        match_n_1_node->next = match_node->next;
      } /* if ! ( match_node == *head_node ) */
    
      /* Dellocate the memory for the given tech node */
      modem_mem_free( match_node, MODEM_MEM_CLIENT_RFA );
    } /* if ( match_node != NULL ) */
    else
    {
      ret_val = FALSE;
      RF_MSG_1( RF_FATAL, "rf_fwrsp_delete_tech_node: Tech 0x%x node not found",
                tech );
    } /* if ! ( match_node != NULL ) */

  } /* if ( head_node != NULL ) */
  else
  {
    ret_val = FALSE;
    RF_MSG_1( RF_FATAL, "rf_fwrsp_delete_tech_node: Tech 0x%x NULL head",
              tech );
  } /* if ! ( head_node != NULL ) */

  return ret_val;

} /* rf_fwrsp_delete_tech_node */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query UMID dispatcher info
 
  @details
  This function will locate the linked list node corresponding to the tech and
  locate the appropriate UMID entry.
 
  @param umid
  Messsage router UMID for which the entry is to be located
 
  @return
  Pointer to the memory location which stores UMID entry
*/
static rf_fwrsp_umid_info_type*
rf_fwrsp_get_umid_info
(
  msgr_umid_type umid
)
{
  rf_fwrsp_dispatch_node_type *tech_node = NULL;
  rf_fwrsp_dispatcher_type *disp_data = NULL;
  rf_fwrsp_umid_info_type *umid_info = NULL;
  rf_fwrsp_umid_info_type *umid_start_ptr = NULL;
  rf_fwrsp_umid_info_type *umid_ptr = NULL;
  uint16 umid_idx = 0;
  /* Get the dispatch Data */
  disp_data = &(rf_fwrsp_task_data.dispatcher);

  /* Get the Node corresponding to the Tech from the linked list */
  tech_node = rf_fwrsp_get_tech_node( disp_data->head_node,
                                      MSGR_TECH_VAL(umid) );

  /* Perform NULL pointer check. */
  if ( tech_node != NULL )
  {
    /* Set the UMID Pointer */
    umid_start_ptr = (tech_node->tech_umids.umid_list);

    /* Loop through all entries to locate the UMID entry */
    for ( umid_idx = 0; umid_idx < tech_node->tech_umids.num_of_umids;
          umid_idx++ )
    {
      umid_ptr = umid_start_ptr + umid_idx;
      if ( umid_ptr->umid == umid )
      {
        umid_info = umid_ptr;
        if ( disp_data->debug_mode == TRUE )
        {
          RF_MSG_2( RF_HIGH, "rf_fwrsp_get_umid_info: UMID.0x%x - Found @ "
                             "index %d", umid, umid_idx );
        } /* if ( disp_data->debug_mode == TRUE ) */
        break;
      } /* if ( umid_ptr->umid == umid ) */
    } /* for ( umid_idx = 0; umid_idx < tech_node->tech_umids.num_of_umids;
          umid_idx++ ) */
  } /* if ( tech_node != NULL ) */
  else
  {
    umid_info = NULL;
    RF_MSG_1( RF_ERROR, "rf_fwrsp_get_umid_info: UMID.0x%x - No Tech Node",
              umid );
  } /* if ! ( tech_node != NULL ) */

  /* Retrn UMID INFO */
  return umid_info;

} /* rf_fwrsp_get_umid_info */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Insert Item to doubly linked list
  
  @details
  Inserts a rf_fwrsp_snum_dispatch_info_type into a doubly-linked list of 
  rf_fwrsp_snum_dispatch_info_type.  The item is inserted after the item passed
  in as item_before.
 
  This function will form an double linked list as shown in the diagram.
 
  @image html rf_cdma_intelliceiver_doubly_linkedlist.jpg
 
  @param item_before
  SNUM Item to which a new item is to be inserted
 
  @param item_to_insert
  New SNUM Item to be inserted
*/
static void
rf_fwrsp_snum_insert_item
(
  rf_fwrsp_snum_node_type *item_before, 
  rf_fwrsp_snum_node_type *item_to_insert
)
{

  item_to_insert->next = item_before->next;
  (item_before->next)->prev = item_to_insert;
  item_before->next = item_to_insert;
  item_to_insert->prev = item_before;

} /* rf_fwrsp_snum_insert_item */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Remove SNUM Item to doubly linked list
  
  @details
  Removes a rf_fwrsp_snum_dispatch_info_type into a doubly-linked list of 
  rf_fwrsp_snum_dispatch_info_type. 
 
  @param item_to_remove
  SNUM Item to be removed
*/
static void
rf_fwrsp_snum_remove_item
(
  const rf_fwrsp_snum_node_type *item_to_remove
)
{
  (item_to_remove->prev)->next = item_to_remove->next;
  (item_to_remove->next)->prev = item_to_remove->prev;

} /* rf_fwrsp_snum_remove_item */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Find SNUM Item containg given item ID in used list
 
  @details
  This function locates a rf_fwrsp_snum_dispatch_info_type in the "used" list.
  The  SNUM is used as the comparison key. If an item is found matching the 
  input comparison key, a pointer to that item is returned. If a match is not
  found a pointer to the sentinel is returned
 
  @param sem_token
  Semaphore token to be used as a comparision key to match in the semaphore
  item
 
  @return
  If an item is found matching the input comparison key, a pointer to that item 
  is returned. If a match is not found a pointer to the sentinel is returned.
*/
static rf_fwrsp_snum_node_type*
rf_fwrsp_snum_find_item
(
  rf_fwrsp_umid_info_type *umid_info,
  uint32 snum
)
{
  rf_fwrsp_snum_pool_type *pool_data = NULL; /* Pool Data */
  rf_fwrsp_snum_node_type *found = NULL;

  /* Variable to keep track of number of search counts */
  uint8 search_count = 0; 

  /* Get POOL data */
  pool_data = &(umid_info->snum_pool);

  /* NULL Pointer check */
  if ( pool_data->sentinel_used_items != NULL )
  {
    /* Initialize the variable to point to next item pointed by sentinel */
    found = pool_data->sentinel_used_items->next;
  
    /* Loop through the linked list to find the item containing the given snum */
    while ( found != pool_data->sentinel_used_items )
    {
     /* Increment the search count */
      search_count++;
  
      /* Since the maximum number of possible nodes is
      RF_FWRSP_MAX_CONCURRENT_RSP, the number of search should be finite. If
      it is more, then the Linked List is corrupted */
      if ( search_count > RF_FWRSP_MAX_CONCURRENT_RSP )
      {
        ERR_FATAL( "rf_fwrsp_snum_find_item: [# %d] UMID.0x%x SNUM.0x%x - "
                   "LinkedList is corrupted", umid_info->index_num, 
                   umid_info->umid, snum ); 
      } /* if ( search_count > RF_FWRSP_MAX_CONCURRENT_RSP ) */
      else
      {
        if ( found->snum_info.snum == snum )
        {
          /* Match is found. Therefore break */
          break;
        } /* if ( found->snum_info.snum == snum) */
        else
        {
          /* No match, traverse through the linked list */
          found = found->next;
        } /* if ! ( found->snum_info.snum == snum ) */
      } /* if ! ( search_count > RF_FWRSP_MAX_CONCURRENT_RSP ) */
    } /* while ( found != &(pool_data->sentinel_used_items) ) */
  
    /* Check if the an item with the SNUM was found in the used list.
    If not found, then return NULL*/
    if( found == pool_data->sentinel_used_items )
    {
      found = NULL;
    } /* if( found == &(pool_data->sentinel_used_items) ) */
  } /* if ( pool_data->sentinel_used_items != NULL ) */
  else
  {
    RF_MSG_3( RF_ERROR, "rf_fwrsp_snum_find_item: [# %d] UMID.0x%x SNUM.0x%x - "
               "NULL Used Pointer", umid_info->index_num, 
               umid_info->umid, snum ); 
  } /* if ! ( pool_data->sentinel_used_items != NULL ) */

  /* return the item */
  return found;

} /* rf_fwrsp_snum_find_item */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize SNUM pool
  
  @details
  This will initialize SNUM pool linked list.
 
  @param pool_data
  Pointer to SNUM data
 
  @return
  Status of SNUM pool init
*/
static boolean
rf_fwrsp_snum_pool_init
(
  rf_fwrsp_umid_info_type *umid_ptr
)
{
  uint32 idx = 0;
  boolean ret_val = TRUE;
  rf_fwrsp_snum_pool_type *pool_data;

  /* Get Pool Data */
  pool_data = &(umid_ptr->snum_pool);

  /* Init NO-SNUM-INFO structure */
  pool_data->no_snum_info.item_id = 0xFF;
  pool_data->no_snum_info.snum = 0;
  pool_data->no_snum_info.static_cb_registration = FALSE;
  pool_data->no_snum_info.snum_cb = NULL;
  pool_data->no_snum_info.snum_cb_data = NULL;

  /* Proceed only if item_list is empty */
  if ( pool_data->item_list == NULL )
  {
    pool_data->item_list = modem_mem_alloc( 
                 RF_FWRSP_MAX_CONCURRENT_RSP*sizeof(rf_fwrsp_snum_node_type), 
                 MODEM_MEM_CLIENT_RFA );

    pool_data->sentinel_spare_items = modem_mem_alloc( 
                             sizeof(rf_fwrsp_snum_node_type), 
                             MODEM_MEM_CLIENT_RFA );

    pool_data->sentinel_used_items = modem_mem_alloc( 
                             sizeof(rf_fwrsp_snum_node_type), 
                             MODEM_MEM_CLIENT_RFA );

    /* build the items into the doubly-linked list */

    /* Check if the item list is successfully allocated */
    if ( ( pool_data->item_list != NULL ) &&
         ( pool_data->sentinel_spare_items != NULL ) &&
         ( pool_data->sentinel_used_items != NULL ) )
    {
      memset( pool_data->item_list, 0x0, sizeof(rf_fwrsp_snum_node_type) );
      pool_data->sentinel_spare_items->next = 
                        pool_data->sentinel_spare_items;
      pool_data->sentinel_spare_items->prev = 
                        pool_data->sentinel_spare_items;
  
      /* Insert items into Spare list */
      for ( idx = 0; idx < RF_FWRSP_MAX_CONCURRENT_RSP; idx++ )
      {
        pool_data->item_list[idx].snum_info.item_id = 
                                           (rf_fwrsp_snum_item_id_type)idx;
        pool_data->item_list[idx].snum_info.snum = 0;
        pool_data->item_list[idx].snum_info.static_cb_registration = FALSE;
        pool_data->item_list[idx].snum_info.snum_cb = NULL;
        pool_data->item_list[idx].snum_info.snum_cb_data = NULL;
        rf_fwrsp_snum_insert_item( pool_data->sentinel_spare_items,
                                   &pool_data->item_list[idx] );
      } /* for ( idx = 0; idx < RF_FWRSP_MAX_CONCURRENT_RSP; idx++ ) */
  
      pool_data->sentinel_used_items->next = 
                         pool_data->sentinel_used_items;
      pool_data->sentinel_used_items->prev = 
                         pool_data->sentinel_used_items;
  
      pool_data->num_of_spare_items = RF_FWRSP_MAX_CONCURRENT_RSP;
  
      rf_common_init_critical_section( &(pool_data->critical_sect) );
  
      RF_MSG_2( RF_HIGH, "rf_fwrsp_snum_pool_init: [# %d] UMID.0x%x - "
                "SNUM Pool initialized", umid_ptr->index_num, umid_ptr->umid );
    } /* if ( ( pool_data->item_list != NULL ) &&
              ( pool_data->sentinel_spare_items != NULL ) &&
              ( pool_data->sentinel_used_items != NULL ) ) */
    else
    {
      ret_val = FALSE;
      RF_MSG_2( RF_ERROR, "rf_fwrsp_snum_pool_init: [# %d] UMID.0x%x - UMID "
                "alloc failed", umid_ptr->index_num, umid_ptr->umid );
    } /* if ! ( ( pool_data->item_list != NULL ) &&
                ( pool_data->sentinel_spare_items != NULL ) &&
                ( pool_data->sentinel_used_items != NULL ) ) */
  } /* if ( pool_data->item_list == NULL ) */
  else
  {
    ret_val = FALSE;
    RF_MSG_2( RF_ERROR, "rf_fwrsp_snum_pool_init: [# %d] UMID.0x%x - UMID "
              "List is not NULL. ABORT allocation", umid_ptr->index_num, 
              umid_ptr->umid );
  } /* if !  ( pool_data->item_list == NULL ) */

  return ret_val;

} /* rf_fwrsp_snum_pool_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  De-initialize SNUM pool
  
  @details
  This will de-initialize SNUM pool linked list.
 
  @param umid_ptr
  Pointer to UMID Data pointer
 
  @return
  Status of SNUM pool deinit
*/
static boolean
rf_fwrsp_snum_pool_deinit
(
  rf_fwrsp_umid_info_type *umid_ptr
)
{
  boolean ret_val = TRUE;
  rf_fwrsp_snum_pool_type *pool_data;

  /* Get Pool Data */
  pool_data = &(umid_ptr->snum_pool);

  /* Free the Memory allocated for SNUM Pool item list */
  modem_mem_free( pool_data->item_list, MODEM_MEM_CLIENT_RFA );

  /* Free the Memory allocated for Spare Item*/
  modem_mem_free( pool_data->sentinel_spare_items, MODEM_MEM_CLIENT_RFA );

  /* Free the Memory allocated for Spare Item*/
  modem_mem_free( pool_data->sentinel_used_items, MODEM_MEM_CLIENT_RFA );

  /* De-initialize the critical section */
  rf_common_deinit_critical_section( &(pool_data->critical_sect) );

  /* Memset pool data to 0 */
  memset( pool_data, 0x0, sizeof(rf_fwrsp_snum_pool_type) );

  RF_MSG_2( RF_HIGH, "rf_fwrsp_snum_pool_deinit: [# %d] UMID.0x%x - SNUM Pool "
            "de-initialized", umid_ptr->index_num, umid_ptr->umid );

  return ret_val;

} /* rf_fwrsp_snum_pool_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Pop SNUM item from SNUM Pool
 
  @details
  This function will be used pop any available spare SNUM item from
  the SNUMPool. If no spare SNUM is found, it is a unrecoverable situation.
  Thus an ERR_FATAL will be done.
 
  @param pool_data
  Pointer to the structure containing SNUM Pool Data
 
  @return
  Pointer to the SNUM node info which is poped to be used by client
*/
static rf_fwrsp_snum_dispatch_info_type*
rf_fwrsp_snum_pop_item
(
  rf_fwrsp_umid_info_type *umid_info,
  rf_fwrsp_snum_pool_type *pool_data
)
{
  rf_fwrsp_snum_node_type *found = NULL; /* Var to store the item */
  boolean snum_found = TRUE;

  /* Enter Critical Section */
  rf_common_enter_critical_section( &(pool_data->critical_sect) );

  /*--------------------------------------------------------------------------*/
  /* Get a spare item from sentinel_spare_items */
  /*--------------------------------------------------------------------------*/

  /* NULL pointer check for Spare item pointer */
  if ( ( pool_data->sentinel_spare_items != NULL ) &&
       ( pool_data->sentinel_used_items != NULL ) )
  {
    /* Check if the "Spare" list has atleast one item */
    if ( pool_data->sentinel_spare_items->next != 
         pool_data->sentinel_spare_items
       )
    {
      /* There is free item in Spare list. Use that to push to "Used" list */
      found = pool_data->sentinel_spare_items->next;
  
      /* Remove the item from the list */
      rf_fwrsp_snum_remove_item( found );
  
      /* Insert it at the head of the used list */
      rf_fwrsp_snum_insert_item( pool_data->sentinel_used_items, found );
  
      /* Decrease the number of spare items */
      pool_data->num_of_spare_items--;
  
      RF_MSG_4( RF_MED, "rf_fwrsp_snum_pop_item: [# %d] UMID.0x%x - POP [%d] "
                "{Items left %d}", umid_info->index_num, umid_info->umid, 
                found->snum_info.item_id, pool_data->num_of_spare_items );
  
      snum_found = TRUE;
  
  
    } /* if ( pool_data->sentinel_spare_items->next != 
              pool_data->sentinel_spare_items
            ) */
    else
    {
      /* Spare item not found */
      ERR_FATAL( "rf_fwrsp_snum_pop_item: [# %d] UMID.0x%x - no SPARE SNUM", 
                 umid_info->index_num, umid_info->umid, 0 ); 
      snum_found = FALSE;
    } /* if ! ( pool_data->sentinel_spare_items->next != 
                pool_data->sentinel_spare_items
              ) */
  } /* if ( ( pool_data->sentinel_spare_items != NULL ) &&
            ( pool_data->sentinel_used_items != NULL ) ) */
  else
  {
    RF_MSG_2( RF_MED, "rf_fwrsp_snum_pop_item: [# %d] UMID.0x%x - NULL Spare/"
              "Used pointer", umid_info->index_num, umid_info->umid );
  } /* if ( ( pool_data->sentinel_spare_items != NULL ) &&
          ( pool_data->sentinel_used_items != NULL ) ) */

  /* Leave Critical Section */
  rf_common_leave_critical_section( &(pool_data->critical_sect) );

  if ( snum_found == TRUE )
  {
    return &(found->snum_info);
  } /* if ( snum_found == TRUE ) */
  else
  {
    return NULL;
  } /* if ! ( snum_found == TRUE ) */

} /* rf_fwrsp_snum_pop_item */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Push SNUM item from SNUMPool
 
  @details
  This function will be used to push used SNUM item to
  the SNUMPool.
 
  @param pool_data
  Pointer to the structure containing SNUM Pool Data
 
  @param push_node
  SNUM item Node is to be pushed
 
  @return
  Pointer to the SNUM node info which is poped to be used by client
*/
static boolean
rf_fwrsp_snum_push_item
(
  rf_fwrsp_umid_info_type *umid_info,
  rf_fwrsp_snum_pool_type *pool_data,
  rf_fwrsp_snum_node_type *push_node
)
{
  boolean ret_val = TRUE;

  /* Enter Critical Section */
  rf_common_enter_critical_section( &(pool_data->critical_sect) );

  /* Check if the token is found or not */
  if( push_node == NULL )
  {
    /* Spare item not found */
    ERR_FATAL( "rf_fwrsp_snum_push_item: [# %d] UMID.0x%x - PUSH item "
               "not found", umid_info->index_num, umid_info->umid, 0 );
    ret_val = FALSE;
  } /* if( found == NULL ) */
  else
  {
    /* NULL Pointer check */
    if ( pool_data->sentinel_spare_items != NULL )
    {
      /* Invalidate the SNUM data */
      push_node->snum_info.snum = 0;
      push_node->snum_info.static_cb_registration = FALSE;
      push_node->snum_info.snum_cb = NULL;
      push_node->snum_info.snum_cb_data = NULL;
  
      /* Remove the item from the list */
      rf_fwrsp_snum_remove_item( push_node );
      
      /* Insert it at the head of the spare list */
      rf_fwrsp_snum_insert_item( pool_data->sentinel_spare_items, push_node );
  
      /* Increase the number of spare items */
      pool_data->num_of_spare_items++;
  
      RF_MSG_4( RF_MED, "rf_fwrsp_snum_push_item: [# %d] UMID.0x%x - PUSH [%d] "
                "{Items left %d}", umid_info->index_num, umid_info->umid, 
                push_node->snum_info.item_id, pool_data->num_of_spare_items );
  
      ret_val &= TRUE;
    } /*  if ( pool_data->sentinel_spare_items != NULL ) */
    else
    {
      ret_val &= FALSE;

      RF_MSG_2( RF_MED, "rf_fwrsp_snum_push_item: [# %d] UMID.0x%x - NULL "
                "Spare Pointer", umid_info->index_num, umid_info->umid );

    } /*  if ! ( pool_data->sentinel_spare_items != NULL ) */
  } /* if ! ( found == NULL ) */

  /* Leave Critical Section */
  rf_common_leave_critical_section( &(pool_data->critical_sect) );

  return ret_val;

} /* rf_fwrsp_snum_push_item */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Register UMID to the task
 
  @details
  This function shall be called to register a command to the RF_FWRSP task.
  The UMID passed to this function will be first registered with message
  router module. Then the UMID and its corresponding callback function will
  be registered in appropriate linked list node for future dispatching. Along
  with dispatching the SNUM dispatcher handler table will also be created.
 
  @param module_self
  MSGR module corresponding to the client which is attempting to perform
  command registration
 
  @param umid
  MSGR ID which is to be regsitered with the MSGR module and RF_FWRSP task
 
  @param umid_handler
  Function pointer pointing to the function which is to be called when
  dispatching for the given UMID is to be done.
 
  @return
  Flag indicating the status of commond registration
*/
boolean
rf_fwrsp_register_command
(
  msgr_tech_module_type module_self,
  msgr_umid_type umid,
  rf_fwrsp_umid_handler_func_type umid_handler
)
{
  rf_fwrsp_dispatch_node_type *tech_node = NULL;
  rf_fwrsp_dispatcher_type *disp_data;
  rf_fwrsp_task_msgr_type *fwrsp_msgr;
  boolean ret_val = TRUE;
  uint16 umid_list_size = 0;
  rf_fwrsp_umid_info_type *umid_start_ptr = NULL;
  rf_fwrsp_umid_info_type *umid_ptr = NULL;
  uint16 umid_idx = 0;
  boolean duplicate_umid = FALSE;
  errno_enum_type msgr_res = E_SUCCESS; /* holds registration result */  

  /* Get the dispatch Data */
  disp_data = &(rf_fwrsp_task_data.dispatcher);
  fwrsp_msgr = &(rf_fwrsp_task_data.fwrsp_msgr);

  /* Get the Node corresponding to the Tech from the linked list */
  tech_node = rf_fwrsp_get_tech_node( disp_data->head_node,
                                      MSGR_TECH_VAL(umid) );

  /* If NULL, this means that the node is not present, need to create
  new node */
  if ( tech_node == NULL )
  {
    /* Since the tech node does not exist, create a new new one at the
    end of the linked list */
    tech_node = rf_fwrsp_create_tech_node( &(disp_data->head_node),
                                           MSGR_TECH_VAL(umid) );
  } /* if ( tech_node == NULL ) */

  /* Proceed, only if the tech node was successfully created */
  if ( tech_node != NULL )
  {

    /* Perform Duplicate UMID entry check */
    if ( tech_node->tech_umids.umid_list != NULL )
    {
      /* Set the UMID pointer to the point to correct UMID entry location */
      umid_start_ptr = (tech_node->tech_umids.umid_list);
  
      /* Loop through all entries to make sure that there is no duplicate
      UMID entry */
      for ( umid_idx = 0; umid_idx < tech_node->tech_umids.num_of_umids;
            umid_idx++ )
      {
        /* Increment the UMID pointer */
        umid_ptr = umid_start_ptr + umid_idx;
  
        /* Check for duplicate */
        if ( umid_ptr->umid == umid )
        {
          /* Flag indicating that UMID is duplicate */
          duplicate_umid = TRUE;
          break;
        } /* if ( umid_ptr->umid == umid ) */
      } /* for ( umid_idx = 0; umid_idx < tech_node->tech_umids.num_of_umids;
            umid_idx++ ) */
    } /* if ( tech_node->tech_umids.umid_list != NULL ) */

    /* Allocate the memory to store list of UMID. If it is the firs UMID 
    for the given tech, then perform allocation. If not, perform reallocation.*/

    /* Compute the size (in bytes) to store all UMIDs including the latest
    registration */
    umid_list_size = sizeof(rf_fwrsp_umid_info_type) * 
                         (tech_node->tech_umids.num_of_umids + 1);

    /* Perfom allocation or reallocation based if it is first registration. */
    if ( tech_node->tech_umids.umid_list == NULL )
    {
      tech_node->tech_umids.umid_list = 
                modem_mem_alloc( umid_list_size, MODEM_MEM_CLIENT_RFA );
      if ( tech_node->tech_umids.umid_list != NULL )
      {
        memset( tech_node->tech_umids.umid_list, 0x0, umid_list_size );
      } /* if ( tech_node->tech_umids.umid_list != NULL ) */
    } /* if ( tech_node->tech_umids.umid_list == NULL ) */
    else
    {
      /* Perform realloc only if UMID is not duplicate */
      if ( duplicate_umid == FALSE )
      {
        /* Increase the  size of UMID List*/
        tech_node->tech_umids.umid_list = 
                  modem_mem_realloc( tech_node->tech_umids.umid_list,
                                     umid_list_size,
                                     MODEM_MEM_CLIENT_RFA );

        umid_ptr = tech_node->tech_umids.umid_list;
        umid_ptr += tech_node->tech_umids.num_of_umids;
        memset( umid_ptr, 0x0, sizeof(rf_fwrsp_umid_info_type) );
      } /* if ( duplicate_umid == FALSE ) */
    } /* if ! ( tech_node->tech_umids.umid_list == NULL ) */

    /* Perform registration, only if there is no duplicate registration */
    if ( duplicate_umid == FALSE )
    {
      /* NULL pointer check */
      if ( tech_node->tech_umids.umid_list != NULL )
      {
        /* Register with Message router */
        msgr_res = msgr_register( module_self, 
                                  &(fwrsp_msgr->client), 
                                  fwrsp_msgr->mb,
                                  umid );

        /* Check if the message router registration was successful or not */
        if ( msgr_res != E_SUCCESS )
        {
          RF_MSG_3( RF_ERROR, "rf_fwrsp_register_command: "
                    "MOD.0x%x UMID.0x%x FP.0x%x - Failed MSGR registration",
                    module_self, umid, umid_handler );    
          ret_val = FALSE;
        } /* if ( msgr_res != E_SUCCESS ) */
        else
        {
          /* Set the UMID pointer to the point to correct UMID entry location */
          umid_ptr = (tech_node->tech_umids.umid_list);
          umid_ptr += tech_node->tech_umids.num_of_umids;
     
          /* Update the UMID entries */
          umid_ptr->index_num = tech_node->tech_umids.num_of_umids;
          umid_ptr->umid = umid;
          umid_ptr->umid_cb = umid_handler;

          /* Initialize SNUM Pool */
          rf_fwrsp_snum_pool_init( umid_ptr );
     
          /* Increase the number of registered UMIDs.*/
          tech_node->tech_umids.num_of_umids += 1;
     
          RF_MSG_4( RF_HIGH, "rf_fwrsp_register_command: "
                    "MOD.0x%x UMID.0x%x FP.0x%x - [# %d] Registered !", 
                    module_self, umid, umid_handler, 
                    umid_ptr->index_num );
        } /* if ! ( msgr_res != E_SUCCESS ) */
      } /* if ( tech_node->tech_umids.umid_list != NULL ) */
      else
      {
        ret_val = FALSE;
        RF_MSG_3( RF_FATAL, "rf_fwrsp_register_command: "
                  "MOD.0x%x UMID.0x%x FP.0x%x - UMID List allocaiton failed",
                  module_self, umid, umid_handler );
      } /* if ! ( tech_node->tech_umids.umid_list != NULL ) */
    } /* if ( duplicate_umid == FALSE ) */
    else
    {
      ret_val = FALSE;
      RF_MSG_3( RF_FATAL, "rf_fwrsp_register_command: "
                "MOD.0x%x UMID.0x%x FP.0x%x - UMID already registered !",
                module_self, umid, umid_handler );
    } /* if ! ( duplicate_umid == FALSE ) */
  } /* if ( tech_node != NULL ) */
  else
  {
    ret_val = FALSE;
    RF_MSG_3( RF_FATAL, "rf_fwrsp_register_command: "
              "MOD.0x%x UMID.0x%x FP.0x%x - Tech Node creation failed", 
              module_self, umid, umid_handler );
  } /* if ! ( tech_node != NULL ) */

  /* Return Registration status */
  return ret_val;
} /* rf_fwrsp_register_command */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  De-register UMID to the task
 
  @details
  This function shall be called to deregister a command to the RF_FWRSP task.
  The UMID passed to this function will be first deregistered with message
  router module. Then the UMID and its corresponding callback function will
  be deregistered from appropriate linked list node. If all UMIDs are
  deregistered, then the Linked List node will be deallocated.
 
  @param module_self
  MSGR module corresponding to the client which is attempting to perform
  command deregistration
 
  @param umid
  MSGR ID which is to be deregsitered with the MSGR module and RF_FWRSP task
 
  @return
  Flag indicating the status of commond deregistration
*/
boolean
rf_fwrsp_deregister_command
(
  msgr_tech_module_type module_self,
  msgr_umid_type umid
)
{
  rf_fwrsp_dispatch_node_type *tech_node = NULL;
  rf_fwrsp_dispatcher_type *disp_data;
  rf_fwrsp_task_msgr_type *fwrsp_msgr;
  boolean ret_val = TRUE;
  uint16 umid_list_size = 0;
  rf_fwrsp_umid_info_type *umid_start_ptr = NULL;
  rf_fwrsp_umid_info_type *umid_ptr = NULL;
  rf_fwrsp_umid_info_type *insert_umid = NULL;
  uint16 umid_idx = 0;
  boolean dereg_umid_found = FALSE;
  errno_enum_type msgr_res = E_SUCCESS; /* holds registration result */  
  uint16 adjusted_index_num = 0;

  /* Get the dispatch Data */
  disp_data = &(rf_fwrsp_task_data.dispatcher);
  fwrsp_msgr = &(rf_fwrsp_task_data.fwrsp_msgr);

  /* Get the Node corresponding to the Tech from the linked list */
  tech_node = rf_fwrsp_get_tech_node( disp_data->head_node,
                                      MSGR_TECH_VAL(umid) );

  /* Perform NULL pointer check to find out if the tech node is found or not */
  if ( tech_node != NULL )
  {
    /* Set the UMID Pointer */
    umid_start_ptr = (tech_node->tech_umids.umid_list);

    /* Find the entry for the UMID in the tech table */
    for ( umid_idx = 0; umid_idx < tech_node->tech_umids.num_of_umids; 
          umid_idx++ )
    {
      /* Set the UMID pointer to the point to correct UMID entry location */
      umid_ptr = umid_start_ptr + umid_idx;

      /* Check if the UMID entry is found or not. If found, then memset the
      entry. The entry location for matched UMID is saved so that it can
      be used to copy remaining UMID entries after the matched UMID to this
      location. This is done in order to pack all UMID entries. After PACKing
      is done, realloc is done to reduce memory usage.*/
      if ( umid_ptr->umid == umid )
      {
        /* De-register with MSGR */
        msgr_res = msgr_deregister( module_self, 
                                   &(fwrsp_msgr->client), 
                                   umid );

        /* Update adjusted_index_num so reflect the umid_idx corresponding
        to the message to be deregistered */
        adjusted_index_num = umid_idx;

        /* Check if the message router registration was successful or not */
        if ( msgr_res != E_SUCCESS )
        {
          RF_MSG_2( RF_ERROR, "rf_fwrsp_deregister_command: "
                    "MOD.0x%x UMID.0x%x - MSGR Failure", module_self, umid );    
          ret_val = FALSE;
        } /* if ( msgr_res != E_SUCCESS ) */
        else
        {
          /* Save the UMID pointer. This will be used later to copy the
          remaining UMID to this location */
          insert_umid = umid_ptr;

          /* De-initialize the SNUM Pool before memset'ing the SNUM Pool */
          rf_fwrsp_snum_pool_deinit(umid_ptr);

          /* Memset the UMID entry */
          memset(insert_umid, 0x0, sizeof(rf_fwrsp_umid_info_type));

          /* Flag that UMID is found and deregistered. This will be used
          later to pack the UMIS list  */
          dereg_umid_found = TRUE;
        } /* if ! ( msgr_res != E_SUCCESS ) */
      } /* if ( umid_ptr->umid == umid ) */
      else if ( dereg_umid_found == TRUE )
      {
        /* PACK the UMID entries */
        memscpy( insert_umid, sizeof(rf_fwrsp_umid_info_type), umid_ptr, sizeof(rf_fwrsp_umid_info_type) );

        /* Update the index with adjusted index */
        insert_umid->index_num = adjusted_index_num;

        /* Increment adjusted index */
        adjusted_index_num++;

        /* Move to new insert location */
        insert_umid++;
      } /* else if ( rearrange_umid == TRUE ) */
    } /* for ( umid_idx = 0; umid_idx < tech_node->tech_umids.num_of_umids; 
          umid_idx++ ) */

    /* Since UMID is found and list of UMIDs are PACKed, the UMID List
    memory needs to be reduced by 1 entry. If all UMIDs are unregistered,
    then complete deallocation is done. If not, then just UMID list is
    shortened */
    if ( dereg_umid_found == TRUE )
    {
      /* Compute the size (in bytes) to store all UMIDs including the latest
      de-registration */
      umid_list_size = sizeof(rf_fwrsp_umid_info_type) * 
                           (tech_node->tech_umids.num_of_umids - 1);

      if ( umid_list_size > 0 )
      {
        /* Decrese the  size of UMID List */
        tech_node->tech_umids.umid_list = 
                  modem_mem_realloc( tech_node->tech_umids.umid_list,
                                     umid_list_size,
                                     MODEM_MEM_CLIENT_RFA );

        /* NULL Pointer check */
        if ( tech_node->tech_umids.umid_list != NULL )
        {
          tech_node->tech_umids.num_of_umids -= 1;
          RF_MSG_2( RF_MED, "rf_fwrsp_deregister_command: MOD.0x%x UMID.0x%x - "
                    "Deregistered ", module_self, umid );
        } /* if ( tech_node->tech_umids.umid_list != NULL ) */
        else
        {
          RF_MSG_2( RF_FATAL, "rf_fwrsp_deregister_command: "
                    "MOD.0x%x UMID.0x%x - Realloc failed ", module_self, umid );
        } /* if ! ( tech_node->tech_umids.umid_list != NULL ) */

      } /* if ( umid_list_size > 0 ) */
      else
      {
        /* De-allocaye the UMID List */
        modem_mem_free( tech_node->tech_umids.umid_list,
                        MODEM_MEM_CLIENT_RFA );
        tech_node->tech_umids.umid_list = NULL;

        /* Delete the linked list node corresponding to the Tech */
        if ( rf_fwrsp_delete_tech_node( &(disp_data->head_node),
                                     MSGR_TECH_VAL(umid) ) == TRUE )
        {
          RF_MSG_2( RF_MED, "rf_fwrsp_deregister_command: MOD.0x%x UMID.0x%x - "
                            "Deregistered + Deallocated ", module_self, umid );
        } /* if ( rf_fwrsp_delete_tech_node( &(disp_data->head_node),
                                     MSGR_TECH_VAL(umid) ) == TRUE ) */
        else
        {
          ret_val = FALSE;
          RF_MSG_2( RF_FATAL, "rf_fwrsp_deregister_command: "
                    "MOD.0x%x UMID.0x%x - Node Deletion failed ", 
                    module_self, umid );
        } /* if ! ( rf_fwrsp_delete_tech_node( &(disp_data->head_node),
                                     MSGR_TECH_VAL(umid) ) == TRUE ) */
      } /* if ! ( umid_list_size > 0 ) */
    } /* if ( dereg_umid_found == TRUE ) */
    else
    {
      ret_val = FALSE;
      RF_MSG_2( RF_FATAL, "rf_fwrsp_deregister_command: MOD.0x%x UMID.0x%x - "
                "UMID not found", module_self, umid );
    } /* if ! ( dereg_umid_found == TRUE ) */

  } /* if ( tech_node != NULL ) */
  else
  {
    ret_val = FALSE;
    RF_MSG_2( RF_FATAL, "rf_fwrsp_deregister_command: MOD.0x%x UMID.0x%x - "
              "Tech Node Not found", module_self, umid );
  } /* if ! ( tech_node != NULL ) */
  
  /* Return Registration status */
  return ret_val;
} /* rf_fwrsp_deregister_command */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Register callback function for SNUM dispatching
 
  @details
  This function shall be called to dynamically register the callback function
  used in second-level dispatching for snum based dispatching. the callback
  function will be saved in the last free row in SNUM dispatcher handler table
  corresponding to the given UMID. With the registration of callback, the
  size of the sliding search window will be adjusted (increment by 1)
 
  @param umid
  UMID pertaining to the SNUM for which the handler callback function is to
  be registered
 
  @param snum
  SequenceNumber for the registration. If the message does not have valid
  sequence number, the pass 0 to this field
 
  @param snum_handler
  Callback function to be registered for the sequence number
 
  @param snum_data
  Pointer to opaque data to be used by snum_handler function
 
  @return
  Flag indicating the status of callback function registration
*/
boolean
rf_fwrsp_register_snum_callback
(
  msgr_umid_type umid,
  uint32 snum,
  boolean static_cb_registation,
  rf_fwrsp_snum_handler_func_type snum_handler,
  void *snum_data
)
{
  rf_fwrsp_umid_info_type *umid_info = NULL;
  rf_fwrsp_snum_pool_type *pool_data = NULL;
  rf_fwrsp_snum_node_type *dup_snum = NULL;
  rf_fwrsp_snum_dispatch_info_type* snum_info = NULL;
  boolean ret_val= TRUE;

  /* Get UMID info from the linked list*/
  umid_info = rf_fwrsp_get_umid_info(umid);

  /* Perform NULL pointer check before proceeding */
  if ( umid_info != NULL )
  {
    pool_data = &(umid_info->snum_pool);
    /* Based on SNUM number, register the callback info in appropriate entry.
    Invalid SNUM (RF_FWRSP_NO_SNUM) will always be registered at the
    no_snum_info entry. On the other hand, valid SNUM will dynamically
    get entered by poping an SNUM item from SNUMPool */ 
    if ( snum == RF_FWRSP_NO_SNUM )
    {
      /* Invalid SNUM (RF_FWRSP_NO_SNUM) will always be registered at
      no_snum_info */
      pool_data->no_snum_info.snum = RF_FWRSP_NO_SNUM;
      pool_data->no_snum_info.static_cb_registration = static_cb_registation;
      pool_data->no_snum_info.snum_cb = snum_handler;
      pool_data->no_snum_info.snum_cb_data = snum_data;

      RF_MSG_1( RF_HIGH, "rf_fwrsp_register_snum_callback: UMID.0x%x NoSNUM - "
                "Registred", umid );
    } /* if ( snum == RF_FWRSP_NO_SNUM ) */
    else
    {
      /* Find if the SNUM is already registered */
      dup_snum = rf_fwrsp_snum_find_item( umid_info, snum );

      /* NULL means that the item is not found. */
      if ( dup_snum == NULL )
      {
        /* POP a SNUM item */
        snum_info = rf_fwrsp_snum_pop_item( umid_info, pool_data );
  
        /* Check if an item was successfully pop'ed or not */
        if ( snum_info != NULL )
        {
          snum_info->snum = snum;
          snum_info->static_cb_registration = static_cb_registation;
          snum_info->snum_cb = snum_handler;
          snum_info->snum_cb_data = snum_data;
  
          RF_MSG_3( RF_HIGH, "rf_fwrsp_register_snum_callback: "
                    "UMID.0x%x SNUM.0x%x - Registered @ ITEM.%d", umid,
                    snum, snum_info->item_id );
        } /* if ( snum_info != NULL ) */
        else
        {
          /* This is unrecoverable condition. Thus error fatal ! */
          ret_val = FALSE;
          ERR_FATAL( "rf_fwrsp_register_snum_callback: UMID.0x%x SNUM.0x%x - "
                     "No free SNUM Index", umid, snum, 0 );
        } /* if ! ( snum_info != NULL ) */
      } /* if ( dup_snum == NULL ) */
      else
      {
        ret_val = FALSE;
        RF_MSG_3( RF_ERROR, "rf_fwrsp_register_snum_callback: "
                  "UMID.0x%x SNUM.0x%x - Duplicate @ ITEM.%d", umid,
                  snum, dup_snum->snum_info.item_id );
      } /* if ( dup_snum == NULL ) */
    } /* if ! ( snum == RF_FWRSP_NO_SNUM ) */
  } /* if ( umid_info != NULL ) */
  else
  {
    ret_val = FALSE;
    RF_MSG_2( RF_ERROR, "rf_fwrsp_register_snum_callback: UMID.0x%x "
              "SNUM.0x%x - No UMID info", umid, snum );
  } /* if ! ( umid_info != NULL ) */

  return ret_val;

} /* rf_fwrsp_register_snum_callback */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  De-Register callback function used for SNUM dispatching
 
  @details
  This function shall be called to dynamically deregister the callback function
  used in second-level dispatching for snum based dispatching. the callback
  function will be removed from the row corresponding to the SNUM. With the
  deregistration of callback, the size of the sliding search window will be 
  adjusted (deccrement by 1)
 
  @param umid
  UMID pertaining to the SNUM for which the handler callback function is to
  be deregistered
 
  @param snum
  SequenceNumber for the deregistration. If the message does not have valid
  sequence number, the pass 0 to this field
 
  @return
  Flag indicating the status of callback function deregistration
*/
static boolean
rf_fwrsp_deregister_snum
(
  rf_fwrsp_umid_info_type *umid_info,
  rf_fwrsp_snum_node_type *push_node,
  uint32 snum
)
{
  rf_fwrsp_snum_pool_type *pool_data = NULL;
  boolean ret_val= TRUE;
  boolean push_status = TRUE;

  /* Perform NULL pointer check before proceeding */
  if ( umid_info != NULL )
  {
    pool_data = &(umid_info->snum_pool);
    /* Based on SNUM number, deregister the callback info in appropriate entry.
    Invalid SNUM (RF_FWRSP_NO_SNUM) will always be registered at the
    no_snum_info entry. On the other hand, valid SNUM will dynamically
    get entered by poping an SNUM item from SNUMPool */ 
    if ( snum == RF_FWRSP_NO_SNUM )
    {
      /* Invalid SNUM (RF_FWRSP_NO_SNUM) will always be registered at
      no_snum_info */
      pool_data->no_snum_info.snum = 0;
      pool_data->no_snum_info.static_cb_registration = FALSE;
      pool_data->no_snum_info.snum_cb = NULL;
      pool_data->no_snum_info.snum_cb_data = NULL;
    } /* if ( snum == RF_FWRSP_NO_SNUM ) */
    else
    {
      /* NULL Pointer Check */
      if ( push_node != NULL )
      {
        /* PUSH a SNUM item */
        push_status = rf_fwrsp_snum_push_item( umid_info, pool_data,
                                               push_node );
    
        if ( push_status == FALSE )
        {  
          ret_val = FALSE;
          RF_MSG_2( RF_ERROR, "rf_fwrsp_deregister_snum: "
                    "UMID.0x%x SNUM.0x%x - Failed", umid_info->umid, snum );
        } /* if ( dup_snum == NULL ) */
      } /* if ( push_node != NULL ) */
      else
      {
        RF_MSG_2( RF_ERROR, "rf_fwrsp_deregister_snum: UMID.0x%x "
                  "SNUM.0x%x - PUSH item not found", umid_info->umid, snum );
      } /* if ! ( push_node != NULL ) */
    } /* if ! ( snum == RF_FWRSP_NO_SNUM ) */
  } /* if ( umid_info != NULL ) */
  else
  {
    ret_val = FALSE;
    RF_MSG_1( RF_ERROR, "rf_fwrsp_deregister_snum: SNUM.0x%x - "
                        "No UMID info", snum );
  } /* if ! ( umid_info != NULL ) */

  return ret_val;

} /* rf_fwrsp_deregister_snum */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  De-Register callback function used for SNUM dispatching
 
  @details
  This function shall be called to dynamically deregister the callback function
  used in second-level dispatching for snum based dispatching. the callback
  function will be removed from the row corresponding to the SNUM. With the
  deregistration of callback, the size of the sliding search window will be 
  adjusted (deccrement by 1)
 
  @param umid
  UMID pertaining to the SNUM for which the handler callback function is to
  be deregistered
 
  @param snum
  SequenceNumber for the deregistration. If the message does not have valid
  sequence number, the pass 0 to this field
 
  @return
  Flag indicating the status of callback function deregistration
*/
boolean
rf_fwrsp_deregister_snum_callback
(
  msgr_umid_type umid,
  uint32 snum
)
{
  rf_fwrsp_umid_info_type *umid_info;
  rf_fwrsp_snum_node_type *push_node;
  boolean ret_val= TRUE;

  /* Get UMID info from the linked list*/
  umid_info = rf_fwrsp_get_umid_info(umid);

  /* Perform NULL pointer check before proceeding */
  if ( umid_info != NULL )
  {
    /* Find the SNUM entry */
    push_node = rf_fwrsp_snum_find_item( umid_info, snum );

    ret_val = rf_fwrsp_deregister_snum( umid_info, push_node, snum );
  } /* if ( umid_info != NULL ) */
  else
  {
    ret_val = FALSE;
    RF_MSG_1( RF_ERROR, "rf_fwrsp_deregister_snum_callback: SNUM.0x%x - "
                        "No UMID info", snum );
  } /* if ! ( umid_info != NULL ) */

  return ret_val;

} /* rf_fwrsp_deregister_snum_callback */

/*! @} */

/*============================================================================*/
/*!
  @name Dispatcher

  @brief
  Functions Pertaining command dispatcher for RF FWRSP Task
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Initialize Dispatcher for FWRSP Task
 
  @details
  This function will initialize the Dispatch table containing the dispatch
  function for each REX based or MSGR based commands.
 
  This funciton also creates the message router client on the RF FWRSP TCB and
  registers for all the MSGR commands that RF FWRSP task wishes to recieve.
 
  @return
  Flag indicating if the dispatcher is successfully initialized or not.
*/
static boolean
rf_fwrsp_init_dispatcher
(
  void
)
{
  boolean rex_msgr_init_status = FALSE; /* Status for Rex MSGR init */
  boolean ret_val = TRUE;

  /* Initialize REX MSGR Interface */
  rex_msgr_init_status = rf_fwrsp_rex_msgr_init( &rf_fwrsp_tcb,
                                                 RF_FWRSP_CMD_Q_SIG );

  /* Check if Rex-MSGR init is successful */
  if ( rex_msgr_init_status == FALSE )
  {
    RF_MSG( RF_ERROR, "rf_fwrsp_init_dispatcher: REX-MSGR Failed" );
    ret_val = FALSE;    
  }

  return ret_val;

} /* rf_fwrsp_init_dispatcher */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process the RF FWRSP Task commands
 
  @details
  This funciton will pop the command from the queue and process it in order
  to call the appropriate dispatch function.
*/
boolean
rf_fwrsp_dispatch
(
  rf_fwrsp_cmd_type *cmd
)
{
  boolean ret_val = TRUE;
  rf_fwrsp_umid_info_type *umid_info = NULL;

  /* Perform NULL pointer check */
  if ( cmd == NULL )
  {
    RF_MSG( RF_ERROR, "rf_fwrsp_dispatch: No command in Q" );
    return FALSE;
  } /* if ( cmd == NULL ) */

  /* Get UMID info from the linked list*/
  umid_info = rf_fwrsp_get_umid_info(cmd->msgr_hdr.id);

  /* NULL Pointer check */
  if ( umid_info != NULL )
  {
    /* Check if the callback is present */
    if ( umid_info->umid_cb != NULL )
    {
      umid_info->umid_cb( cmd, umid_info );  
    } /* if ( umid_info->umid_cb != NULL ) */
    else
    {
      ret_val = FALSE;
      RF_MSG_1( RF_ERROR, "rf_fwrsp_dispatch: UMID.0x%x - No Handler",
               cmd->msgr_hdr.id );
    } /* if ! ( umid_info->umid_cb != NULL ) */
  } /* if ( umid_info != NULL ) */
  else
  {
    ret_val = FALSE;
    RF_MSG_1( RF_ERROR, "rf_fwrsp_dispatch: UMID.0x%x - No Entry",
             cmd->msgr_hdr.id );
  } /* if ! ( umid_info != NULL ) */

  /* Return from the dispatcher */
  return ret_val;
 
} /* rf_fwrsp_dispatch */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process the RF FWRSP Task commands
 
  @details
  This funciton will pop the command from the queue and process it in order
  to call the appropriate dispatch function.
*/
static void
rf_fwrsp_dispatch_command
(
  void
)
{
 
  rf_fwrsp_cmd_type *cmd;
  rf_fwrsp_module_id_type  module_id = 0;
  rf_fwrsp_tech_id_type tech_id;
  rf_fwrsp_module_table_type *module_tbl_ptr = NULL;
  rf_fwrsp_tech_table_type *tech_tbl_ptr = NULL;
  rf_fwrsp_dispatch_table_type *disp_tlb = NULL;

  /* Get command from the Q */
  cmd = rf_fwrsp_get_command();

  /* Perform NULL pointer check */
  if ( cmd == NULL )
  {
    return;
  }
  
  /* Get pointer to dispatch table */
  disp_tlb = &(rf_fwrsp_task_data.dispatch_tbl);

  /* Extract the Tech ID and Module ID */
  tech_id = cmd->rex_hdr.cmd_hdr.tech_id;
  module_id = cmd->rex_hdr.cmd_hdr.module_id;

  /* Get Tech Table Pointer */
  tech_tbl_ptr = disp_tlb->tech_data;

  /* Make sure that Tech ID does not go beyond Max_tech_ID. Other wise,
  there will be array out of bounce */
  if ( tech_id <= disp_tlb->max_tech_id )
  {
    /* Check if the tech_based_cb is not NULL */
    if ( tech_tbl_ptr[tech_id].tech_based_cb != NULL )
    {
      /* Since this is not NULL, it is a tech based registration. So call
      the callback function for the tech based registration */
      tech_tbl_ptr[tech_id].tech_based_cb( cmd );

      /* If required, send the acknowledge signal */
      if( cmd->complete_sig && cmd->task_to_sig )
      {
        (void) rex_set_sigs( cmd->task_to_sig, cmd->complete_sig );
      } /* if( cmd->complete_sig && cmd->task_to_sig ) */
    } /* if ( tech_tbl_ptr[tech_id].tech_based_cb != NULL ) */
    else if ( tech_tbl_ptr[tech_id].module_table != NULL )
    {
      /* Since tech_cb is NULL, it is a module based registration. So call
      the callback function for the module based registration */

      /* Get the Module Table pointer */
      module_tbl_ptr = tech_tbl_ptr[tech_id].module_table;
  
      /* NULL pointer check */
      if ( module_tbl_ptr[module_id].module_based_cb != NULL)
      {
        module_tbl_ptr[module_id].module_based_cb( cmd );
        
        /* If required, send the acknowledge signal */
        if( cmd->complete_sig && cmd->task_to_sig )
        {
          (void) rex_set_sigs( cmd->task_to_sig, cmd->complete_sig );
        } /* if( cmd->complete_sig && cmd->task_to_sig ) */
      } /* if ( module_tbl_ptr[module_id].module_based_cb != NULL) */
      else
      {
        RF_MSG_2( RF_ERROR, "[OD] rf_fwrsp_dispatch_command: "
                  "Invalid Module 0x%x for Tech 0x%x ", tech_id, module_id );
      } /* if ! ( module_tbl_ptr[module_id].module_based_cb != NULL) */
    } /* else if ( tech_tbl_ptr[tech_id].module_table != NULL ) */
  } /* if ( tech_id <= disp_tlb->max_tech_id ) */
  else
  {
    RF_MSG_1( RF_LOW, "[OD] rf_fwrsp_dispatch_command: Invalid Tech 0x%x not "
                        "registered", tech_id );
  } /* if ! ( tech_id <= disp_tlb->max_tech_id ) */

  /* Processing of command complete. Free Memory */
  rf_fwrsp_free_command( cmd );

  return;
 
} /* rf_fwrsp_dispatch_command */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process the RF FWRSP Task commands
 
  @details
  This funciton will pop the command from the queue and process it in order
  to call the appropriate dispatch function.
*/
boolean
rf_fwrsp_dispatch_snum
(
  rf_fwrsp_cmd_type *req_ptr,
  rf_fwrsp_umid_info_type *umid_info,
  uint32 snum
)
{
  boolean ret_val = TRUE;
  rf_fwrsp_snum_node_type *snum_node = NULL;
  const rf_fwrsp_snum_dispatch_info_type *snum_data = NULL;
  rf_fwrsp_snum_handler_func_type snum_cb;
  boolean static_cb_registration;

  /* Check if the callback is present */
  if ( umid_info != NULL )
  {
    if ( snum == RF_FWRSP_NO_SNUM )
    {
      /* Get the SNUM data and SNUM callback */
      snum_data = &(umid_info->snum_pool.no_snum_info);
      snum_cb = umid_info->snum_pool.no_snum_info.snum_cb;
      static_cb_registration = 
               umid_info->snum_pool.no_snum_info.static_cb_registration;
      } /* if ( snum == RF_FWRSP_NO_SNUM ) */
    else
    {
      /* Find the SNUM entry */
      snum_node = rf_fwrsp_snum_find_item( umid_info, snum );
  
      /* NULL Pointer check */
      if ( snum_node != NULL )
      {
        /* Get the SNUM data and SNUM callback */
        snum_data = &(snum_node->snum_info);
        snum_cb = snum_node->snum_info.snum_cb;
        static_cb_registration = snum_node->snum_info.static_cb_registration;
      } /* if ( snum_node != NULL ) */
      else
      {
        snum_cb = NULL;
        snum_data = NULL;
        RF_MSG_2( RF_ERROR, "rf_fwrsp_dispatch_snum: UMID.0x%x SNUM.0x%x - No "
                            "SNUM Entry", umid_info->umid, snum );
      } /* if ! ( snum_node != NULL ) */
    } /* if ! ( snum == RF_FWRSP_NO_SNUM ) */

    /* NULL Pointer check */
    if ( snum_cb != NULL )
    {
      /* Call the callback function */
      ret_val &= snum_cb( req_ptr, snum_data );

      /* Deregister SNUM Callback */
      if ( static_cb_registration == FALSE )
      {
        ret_val &= rf_fwrsp_deregister_snum( umid_info, snum_node, snum );
      } /* if ( snum_node->snum_info.static_cb_registration == FALSE ) */
    } /* if ( snum_node->snum_info.snum_cb != NULL ) */
    else
    {
      RF_MSG_2( RF_HIGH, "********* rf_fwrsp_dispatch_snum: "
                "UMID.0x%x SNUM.0x%x - No Handler Found *********", 
                umid_info->umid, snum );
    } /* if ! ( snum_node->snum_info.snum_cb != NULL ) */
  } /* if ( umid_info->umid_cb != NULL ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rf_fwrsp_dispatch_snum: SNUM.0x%x - No "
                        "UMID Entry", snum );
  } /* if ! ( umid_info->umid_cb != NULL ) */

  /* Return from the dispatcher */
  return ret_val;
 
} /* rf_fwrsp_dispatch_snum */

/*! @} */

/*============================================================================*/
/*!
  @name Signalling

  @brief
  Functions Pertaining to RF FWRSP signalling
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Wait for signal to start RF_FWRSP task
 
  @details
  This function will infinitely perform Rex wait on RF_FWRSP_TASK_START_SIG
  signal. Once this signal is set, RF_FWRSP task will be started. This signal 
  will be set during RFM common init.
*/
static void
rf_fwrsp_wait_to_start_task
(
  void
)
{

  rex_wait( RF_FWRSP_TASK_START_SIG ); /* Wait for iC Module to set the sig */
  rex_clr_sigs( rex_self(), RF_FWRSP_TASK_START_SIG ); /*Clear task start sig*/

} /* rf_fwrsp_wait_to_start_task */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Set Signal to start RF_FWRSP task
 
  @details
  This function will be used to set signal to start RF_FWRSP Task. This function
  will be called during RFM init.
*/
void
rf_fwrsp_signal_to_start_task
(
  void
)
{
  /* Signal RF iC Task to start */
  rex_set_sigs( &rf_fwrsp_tcb, RF_FWRSP_TASK_START_SIG );

} /* rf_fwrsp_signal_to_start_task */

/*! @} */

/*============================================================================*/
/*!
  @name Query

  @brief
  Functions used for RF_FWRSP parameter queries
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query to check if RF_FWRSP_TASK is running
 
  @details
  This function will check the RF_FWRSP data structure to return the running
  state of the task.
 
  @return
  Flag indicating if the RF_FWRSP task is running or not.
*/
boolean
rf_fwrsp_is_task_running
(
  void
)
{
  return rf_fwrsp_task_data.housekeeping.is_task_running;
} /* rf_fwrsp_is_task_running */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get constant pointer to MSGR parameter used by RF_FWRSP_TASK
 
  @details
  In order receive MSGR messages by FWRSP task, these messages need to be
  registered with MSGR module. To indicate that the message is pertaining
  to FWRSP task, the same CLIENT and Mailbox info should be used during
  MSGR registration of commands. Thus this interface returns the pointer to 
  these parameter (Client and MB)
 
  @return
  Pointer to the MSGR data used by FWRSP task
*/
const rf_fwrsp_task_msgr_type*
rf_fwrsp_get_msgr_param
(
  void
)
{
  return &(rf_fwrsp_task_data.fwrsp_msgr);
} /* rf_fwrsp_get_msgr_param */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query the Dispatch Handle
 
  @details
  This function return the dispatch handler to used used by the client for
  command registration and deregistration
 
  @return
  Pointer to FWRSP dispatch handle data
*/
rf_dispatch_handle_type *
rf_fwrsp_task_get_dispatch_handle
(
  void
)
{
  return &(rf_fwrsp_task_data.dispatch_handle);
} /* rf_fwrsp_task_get_dispatch_handle */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Extract UMID
 
  @details
  This function will extract the UMID for the command payload pointer
 
  @param cmd_ptr
  Pointer to command payload
 
  @param umid
  Contianer to store the extracted UMID
 
  @return
  Flag indicating the validity of the UMID extraction
*/
static boolean
rf_fwrsp_extract_umid
(
  void *cmd_ptr,
  msgr_umid_type *umid
)
{
  rf_fwrsp_cmd_type *cmd;

  /* NULL Pointer check */
  if (( cmd_ptr == NULL ) || ( umid == NULL ) )
  {
    RF_MSG( RF_ERROR, "rf_fwrsp_extract_umid: NULL Data" );
    return FALSE;
  } /* if (( cmd_ptr == NULL ) || ( umid == NULL ) ) */

  /* Typecast */
  cmd = (rf_fwrsp_cmd_type*)cmd_ptr;

  /* Extract UMID */
  *umid = cmd->msgr_hdr.id;

  return TRUE;
} /* rf_fwrsp_extract_umid */

/*! @} */

/*============================================================================*/
/*!
  @name Task Spawn

  @brief
  Functions Pertaining to RF FWRSP task spawning
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF FWRSP Task control.

  @details
  This is the Radio Frequency (RF) firmware response handling task, created from 
  the Main Control task. After processing task start procedure, the task 
  enters an infiniten loop, awaiting commands received on its command queue.  
  Each command received is processed to completion before the next command is 
  processed. 

  @param parm
  Dummy variable which is ignored.
*/
void
rf_fwrsp_task
(
  dword parm
)
{
  rex_sigs_type sigs = 0;             
  rf_dispatch_init_handle_data_type init_data;

  /* Initialize Dispatcher */
  /* It is moved to here to mitigate race condition between
  init and msgs registration */
  rf_fwrsp_init_dispatcher();

  /* Required RCINIT handshake */
  rcinit_handshake_startup();

  /* Indicate that IC_TASK is not running yet */
  rf_fwrsp_task_data.housekeeping.is_task_running = FALSE;

  /* Initialize Dispatch handler */
  init_data.task_name = "RF_FWRSP_TASK";
  init_data.task_uid = 0x3105fc6c; /* MD5(RF_FWRSP_TASK) */
  init_data.extract_cid_fp = rf_fwrsp_extract_umid;
  rf_fwrsp_get_rex_data( &(init_data.rex_data) );
  init_data.debug_mode = FALSE;
  rf_dispatch_init_handle( &(rf_fwrsp_task_data.dispatch_handle),
                            init_data );

  /* Wait for RF_FWRSP_TASK_START_SIG signal to be set before starting
  the task. This signal will be set during RFM init. */
  rf_fwrsp_wait_to_start_task();

  /* Spawn the task only if ic_task_data is not NULL */

  /* Initialize command queue for RF_FWRSP */
  rf_fwrsp_create_command_q();

  /* Indicates that RF Task is now running.*/
  rf_fwrsp_task_data.housekeeping.is_task_running = TRUE;

  /* Loop forever handling commands */
  for(;;)
  {
    sigs = rex_wait( TASK_STOP_SIG |        /* Task shutdown   */
                     RF_FWRSP_CMD_Q_SIG   /* Command queue signal */
                    );

    /* There is implicit priority given to signals. Only one signal is
    processed at a time. */
    if ( sigs & TASK_STOP_SIG )
    {
      (void) rex_clr_sigs(&rf_fwrsp_tcb, TASK_STOP_SIG);

      rf_fwrsp_deinit();

      rf_fwrsp_task_data.housekeeping.is_task_running = FALSE;
#ifndef FEATURE_MODEM_RCINIT
      tmc_task_stop();
#else
      rcinit_handshake_term();
#endif
      /* Task is stopped.  No need to run this function anymore. */
      break;
    }
    else if ( sigs & RF_FWRSP_CMD_Q_SIG )
    {
      /* First, clear the signal */
      (void) rex_clr_sigs( &rf_fwrsp_tcb, RF_FWRSP_CMD_Q_SIG );

      /* process commands in a loop to ensure that RF IC task does not "skip" 
      processing of any messages */
      while ( rf_fwrsp_get_command_num() != 0 )
      {
        /* Process RF commands in the queue */
        #ifndef FEATURE_GNSS_ONLY_NO_WWAN
          /* call the dispatcher, this internally uses
          new RF dispatch implementation*/
          rf_fwrsp_dispatch_command();
        #endif /* FEATURE_GNSS_ONLY_NO_WWAN */
      } /* while ( rf_fwrsp_get_command_num() != 0 ) */
    }/*else if ( sigs & RF_FWRSP_CMD_Q_SIG )*/
  } /* for(;;) */

  /* Exit iC Task */
  return;
} /* rf_fwrsp_task */

/*! @} */


/*! @} */

