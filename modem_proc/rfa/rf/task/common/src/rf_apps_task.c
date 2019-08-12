/*! 
  @file
  rf_apps_task.c
 
  @brief
  This file includes all necessary APIs used for non-time critical RF apps task
 
  RF non-time critical apps task handles all RF algorithms that are not time 
  critical. This task is assigned a priority which is lower than any L1 task.
  
  @addtogroup RF_NON_TIME_CRITICAL_APPS_TASK
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/src/rf_apps_task.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/18/14   px      Hooked up with RF dispatch
03/11/13   aro     Added deinitialization function
04/13/12   hdz     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "rfcommon_msg.h"
#include "task.h"
#include "msgr.h"
#include "msgr_rex.h"
#include "rcinit.h"
#include "rfa_msgr.h"
#include "modem_mem.h"
#include "rf_apps_task.h"

/*----------------------------------------------------------------------------*/
/*! Externalized RF APPS Task control block variable */
extern rex_tcb_type rf_apps_tcb;

/*----------------------------------------------------------------------------*/
/*! Structure holding the RF Apps Task data */
typedef struct
{

  rf_apps_cmd_q_struct_type rf_apps_cmd_q;
  /*!< Structure to hold RF Apps task queue data strucutre */

  rf_apps_task_msgr_type apps_msgr;
  /*!< Sturcture to Hold RF Apps task msgr related information */

  rf_apps_cmd_handler_func_type cmd_dispatch_fn[RF_APPS_MAX_TECH_ID];
  /*!< Command dispatch func pt for RF Apps task */

  boolean is_task_running;
  /*!< TRUE indicates RF APPS task is running; FALSE otherwise */

  rf_dispatch_handle_type dispatch_handle;
  /*!< Structure to store the dispatch handle data */

  rex_tcb_type* rf_apps_task_started_tcb;
  /*!< Pointer to the tcb waiting for rf apps task to start */

} rf_apps_task_data_type;

/*----------------------------------------------------------------------------*/
/*! Variable to store the global data for RF_APPS task */
static rf_apps_task_data_type rf_apps_task_data;

/*============================================================================*/
/*!
  @name REX-MSGR

  @brief
  Functions Pertaining to initialization and de-initialization of REX-MSGR
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to perform RF Apps deinitialization
 
  @details
  This function would be called when RF_APPS task is signalled to be stopped.
  This will free-up the dynamically allocated memory.
*/
static void 
rf_apps_deinit
(
  void
)
{
  msgr_client_delete(&(rf_apps_task_data.apps_msgr.client));
} /* rf_apps_deinit */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF APPS Rex MSGR interface initialization

  @details
  This funciton creates the message router client on the RF APPS TCB.
 
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
rf_apps_rex_msgr_init
(
  rex_tcb_type *p_rf_apps_tcb,
  rex_sigs_type rf_rex_sig
)
{
  boolean ret_val =  TRUE;
  errno_enum_type msgr_status;

  /* Create RF CDMA APPS Task message router client for receiving messages
  from  SW entities */
  msgr_status = msgr_client_create(&(rf_apps_task_data.apps_msgr.client));

  if ( msgr_status != E_SUCCESS )
  {
    RF_MSG( RF_ERROR, "rf_apps_rex_msgr_init : "
            "Unable to create RF message router client ");  
    ret_val = FALSE;
  }

  if (msgr_status == E_SUCCESS)
  {
    msgr_status = msgr_client_add_rex_q(
                                      &(rf_apps_task_data.apps_msgr.client),
                                      p_rf_apps_tcb,
                                      rf_rex_sig, 
                                      &(rf_apps_task_data.rf_apps_cmd_q.cmd_q),
                                      &(rf_apps_task_data.rf_apps_cmd_q.free_q),
                                      sizeof(rf_apps_cmd_rex_hdr_type),
                                      sizeof(q_link_type),
                                      sizeof(rf_apps_cmd_type),
                                      &(rf_apps_task_data.apps_msgr.mb)
                                      );

    if (msgr_status != E_SUCCESS)
    {
      RF_MSG( RF_ERROR,"rf_apps_rex_msgr_init : "
              "Unable to add RF REX MSGR client ");  
      ret_val = FALSE;
    }
  }

  /* Return Status */
  return ret_val;

} /* rf_apps_rex_msgr_init */

/*! @} */

/*============================================================================*/
/*!
  @name Command Queue

  @brief
  Functions Pertaining to Command Queue operations for RF APPS TASK usage
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize RF APPS TASK CMD queue.

  @details
  This function initializes the RF APPS task Command Queue Structures.
*/
static void
rf_apps_create_command_q
(
  void
)
{
  uint32 loop_cnt;

  /* Initialize the Command Queue, and the Command Free Queue */
  (void) q_init( &(rf_apps_task_data.rf_apps_cmd_q.cmd_q) );
  (void) q_init( &(rf_apps_task_data.rf_apps_cmd_q.free_q) );

  /* Link the Command Queue Items onto the Command Free Queue */
  for ( loop_cnt = 0; 
        loop_cnt < ARR_SIZE((rf_apps_task_data.rf_apps_cmd_q.q_items)); 
        loop_cnt++)
  {
    q_put( &(rf_apps_task_data.rf_apps_cmd_q.free_q),
           q_link( &(rf_apps_task_data.rf_apps_cmd_q.q_items[loop_cnt]), 
             &(rf_apps_task_data.rf_apps_cmd_q.q_items[loop_cnt].rex_hdr.link) )
          );
  }

} /* rf_apps_create_command_q */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the number of commands on RF APPS TASK cmd Queue
 
  @return
  Total the number of commands on the cmd Queue
*/
static int
rf_apps_get_command_num
(
  void
)
{

  return q_cnt( &(rf_apps_task_data.rf_apps_cmd_q.cmd_q) );

} /* rf_apps_get_command_num */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Retreive the command from  RF APPS TASK queue.

  @details
  This function will retreive the command from RF APPS Task queue, and then 
  removes the command from Queue.

  @return
  Address of the command retreived from the command queue.
*/
static rf_apps_cmd_type* 
rf_apps_get_command
(
  void
)
{
   rf_apps_cmd_type * item;

   /* Retrieve next commmand from command queue */
   item = (rf_apps_cmd_type *) q_get(&(rf_apps_task_data.rf_apps_cmd_q.cmd_q));

   if ( item != NULL )
   {
     item->status = RF_APPS_CMD_PROCESSING;
   }
   else
   {
     RF_MSG( RF_ERROR, "rf_apps_get_command : Signal for the"
             " queue was set, but no cmd in Q"); 
   }

   return item;

} /* rf_apps_get_command */

/*----------------------------------------------------------------------------*/
/*!

  @brief
  Returns free buffer on RF APPS TASK command queue.

  @details
  This function returns the pointer of free buffer on RF APPS TASKs command 
  Queue. This is used by calling module to send command to RF. If buffer has 
  been requested by a module, it is required that it calls rf_cmd_put function 
  immediately. Calling module should set the command header and payload fields.

  @return
  Pointer to the RF APPS TASK command buffer.

  @retval NULL
  If command queue full, the function will return NULL.

  @retval Queue pointer
  If command has buffer available, it will return pointer to the buffer.

*/
rf_apps_cmd_type* rf_apps_get_buf
( 
  void
)
{
   rf_apps_cmd_type *item;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   /* Retrieve next commmand from command queue */
   item = (rf_apps_cmd_type *) q_get( &(rf_apps_task_data.rf_apps_cmd_q.free_q) );
   
   if ( item != NULL )
   {
      /* If a command was available, return a pointer to the command
        structure */
      item->status = RF_APPS_CMD_BUF_ALLOCATED;
      return item;
   }
   else
   {
      /* No command buffer is available! */
      RF_MSG( RF_ERROR, " rf_apps_get_buf: RF command buffer full. "
              "Cannot accept any new RF commands" );
      return NULL;
   }
} /* rf_apps_get_buf () */

/*----------------------------------------------------------------------------*/
/*!

  @brief
  Adds the command to RF queue.

  @details
  This function adds the command to RF APPS TASK queue and signals RF APPS task. 
  The command is added on the memory allocated by rf_apps_get_buf().

  @param cmd
  The address of the command to be added to RF APPS task queue.

*/
void rf_apps_cmd_put
(
   rf_apps_cmd_type* cmd 
)
{

   if ( cmd == NULL )
   {
      RF_MSG( RF_ERROR, "rf_apps_cmd_put: Invalid command passed to RF CMD Queue" );
      return;
   }

   /* Iniliatize the link of the pointer */
   (void) q_link(cmd, &(cmd->rex_hdr.link) );

   /* Add command to the queue */
   q_put( &(rf_apps_task_data.rf_apps_cmd_q.cmd_q), &(cmd->rex_hdr.link) );

   cmd->status = RF_APPS_CMD_QUEUED;

   /* Signal the task about availability of the command */
   (void) rex_set_sigs(&rf_apps_tcb, RF_APPS_CMD_Q_SIG);

   return;
} /* rf_cmd_put () */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Free the command buffer on RF APPS Task queue.

  @details
  This function removes an already processed command from RF APPS Task queue.
  The memory is returned to free queue.
  
  @param cmd
  The address of the command to be freed from RF APPS task queue.
*/
static void
rf_apps_free_command
(
  rf_apps_cmd_type* cmd
)
{

  if ( cmd == NULL )
  {
    RF_MSG( RF_ERROR, "rf_apps_free_command: Invalid cmd in Q" );
    return;
  }

  memset( cmd, 0, sizeof(rf_apps_cmd_type) );

  /* Mark item as no longer valid */
  cmd->status = RF_APPS_CMD_BUF_EMPTY;

 ( void )q_link( cmd, &(cmd->rex_hdr.link) );

  /* Add the item to the free queue */
  q_put( &(rf_apps_task_data.rf_apps_cmd_q.free_q), 
         &(cmd->rex_hdr.link) );

  return;

} /* rf_apps_free_command */

/*! @} */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Rex Data
 
  @details
  In order perform REX-MSGR initialization for RF APPS task, REX data is
  required. This interface return the REX data to be used by RF dispatch to
  perform REX-MSGR registration
 
  @param rex_data_ptr
  Pointer to the container where REX data is populated
 
  @return
  Flag indicating the validity of the populated Rex data
*/
static boolean
rf_apps_get_rex_data
(
  rf_dispatch_rex_data_type *rex_data_ptr
)
{
  /* NULL Pointer check */
  if ( rex_data_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rf_apps_get_rex_data: NULL container" );
    return FALSE;
  } /* if ( rex_data_ptr == NULL ) */

  rex_data_ptr->rex_tcb = &rf_apps_tcb;
  rex_data_ptr->rex_sig = RF_APPS_CMD_Q_SIG;
  rex_data_ptr->cmd_q = &(rf_apps_task_data.rf_apps_cmd_q.cmd_q);
  rex_data_ptr->free_q = &(rf_apps_task_data.rf_apps_cmd_q.free_q);
  rex_data_ptr->header_size = sizeof(rf_apps_cmd_rex_hdr_type);
  rex_data_ptr->q_link_size = sizeof(q_link_type);
  rex_data_ptr->command_size = sizeof(rf_apps_cmd_type);

  return TRUE;
} /* rf_apps_get_rex_data */

/*============================================================================*/
/*!
  @name Dispatcher

  @brief
  Functions Pertaining command dispatcher for RF APPS Task
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Initialize Dispatcher for APPS Task
 
  @details
  This function will initialize the Dispatch table containing the dispatch
  function for each REX based or MSGR based commands.
 
  This funciton also creates the message router client on the RF APPS TCB and
  registers for all the MSGR commands that RF APPS task wishes to recieve.
 
  @return
  Flag indicating if the dispatcher is successfully initialized or not.
*/
static boolean
rf_apps_init_dispatcher
(
  void
)
{
  boolean rex_msgr_init_status = FALSE; /* Status for Rex MSGR init */
  boolean ret_val = TRUE;

  /* Deregister all dispatch function pointers*/
  rf_apps_dispatch_deregister();

  /* Initialize REX MSGR Interface */
  rex_msgr_init_status = rf_apps_rex_msgr_init( &rf_apps_tcb,
                                                 RF_APPS_CMD_Q_SIG );

  /* Check if Rex-MSGR init is successful */
  if ( rex_msgr_init_status == FALSE )
  {
    RF_MSG( RF_ERROR, "rf_apps_init_dispatcher: REX-MSGR Failed" );
    ret_val = FALSE;    
  }

  return ret_val;

} /* rf_apps_init_dispatcher */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process the RF APPS Task commands
 
  @details
  This funciton will pop the command from the queue and process it in order
  to call the appropriate dispatch function.
*/
static void
rf_apps_dispatch_command
(
  void
)
{
  rf_apps_cmd_type *cmd;
  rf_apps_tech_enum_type rf_tech_id;
  boolean use_new_dispatcher = FALSE; // flag to use new dispatcher.
  
  /* Get command from the Q */
  cmd = rf_apps_get_command();

  /* Perform NULL pointer check */
  if ( cmd == NULL )
  {
    RF_MSG( RF_ERROR, "rf_apps_dispatch_command:  NULL cmd pointer" );

    return;
  }

  rf_tech_id = cmd->rex_hdr.cmd_hdr.tech_id;

  if ( (rf_tech_id < 0) || (rf_tech_id >= RF_APPS_MAX_TECH_ID) )
  {
    RF_MSG_1( RF_ERROR, "rf_apps_dispatch_command: invalid tech id = %d ",
              rf_tech_id );
    return;
  }
  
  if ( rf_apps_task_data.cmd_dispatch_fn[rf_tech_id] != NULL )
  {
    rf_apps_task_data.cmd_dispatch_fn[rf_tech_id]( cmd );

    /* If required, send the acknowledge signal */
    if( cmd->complete_sig && cmd->task_to_sig )
    {
      (void) rex_set_sigs( cmd->task_to_sig, cmd->complete_sig );
    } /* if( cmd->complete_sig && cmd->task_to_sig ) */
  } /* if ( tech_tbl_ptr[tech_id].tech_based_cb != NULL ) */
  else
  {
    RF_MSG_1( RF_ERROR, "rf_apps_dispatch_command: dispatch is not registered "
              " for tech = %d yet ", rf_tech_id );

    use_new_dispatcher = TRUE;
  }  

  /* Temporary hack to use new generic dispatcher*/
  if(use_new_dispatcher)
  {
    rf_dispatch_command(rf_apps_task_get_dispatch_handle(),cmd);
  }  

  /* Processing of command complete. Free Memory */
  rf_apps_free_command( cmd );

  return;
 
} /* rf_apps_dispatch_command */

/*! @} */

/*============================================================================*/
/*!
  @name Command Registration

  @brief
  This section includes dispatcher interfaces 

*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Register rf_apps_dispatch function to rf_apps_task
 
  @details
  This function will fill the dispatch function pt with the input function pt.
 
  @param rf_apps_cmd_handler_func_type
  The function pt of rf_apps_dispatcher
 
  @return
  Status indicating, whether the tech registration was successfull or not.
*/
rf_apps_dispatch_status_type
rf_apps_dispatch_register
(
  rf_apps_cmd_handler_func_type callback_fn,
  rf_apps_tech_enum_type rf_tech_id
)
{

  /* Perform Parameter Validation : tech_callback */
  if ( callback_fn == NULL )
  {
    RF_MSG( RF_ERROR, "rf_apps_dispatch_register: NULL pt ");

    return RF_APPS_CMD_REG_FAILURE;
  } /* if ( tech_callback == NULL ) */

  if ( (rf_tech_id < 0) || (rf_tech_id >= RF_APPS_MAX_TECH_ID) )
  {
    RF_MSG_1( RF_ERROR, "rf_apps_dispatch_register: invalid tech id = %d ",
              rf_tech_id );

    return RF_APPS_CMD_REG_FAILURE;
  }

  rf_apps_task_data.cmd_dispatch_fn[rf_tech_id] = callback_fn;
 
  /* return success */
  return RF_APPS_CMD_REG_SUCCESS;

} /* rf_apps_dispatch_register */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deregister the dispatcher from rf_apps_task
 
  @details
  This function will fill the dispatcher function pt defined in rf_apps_data 
  with NULL.
*/
void
rf_apps_dispatch_deregister
(
  void
)
{
  uint8 i;

  for ( i = 0; i < RF_APPS_MAX_TECH_ID; i++ )
  {
    rf_apps_task_data.cmd_dispatch_fn[i] = NULL;
  }
  
  return;

} /* rf_apps_dispatch_deregister */

/*! @} */


/*============================================================================*/
/*!
  @name Signalling

  @brief
  Functions Pertaining to RF APPS signalling
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Wait for signal to start RF APPS task
 
  @details
  This function will infinitely perform Rex wait on RF_APPS_TASK_START_SIG
  signal. Once this signal is set, RF_APPS task will be started. This signal 
  will be set during RFM common init.
*/
static void
rf_apps_wait_to_start_task
(
  void
)
{
  rex_wait( RF_APPS_TASK_START_SIG ); 
  /* Wait for RF APPS TASK START sig is set in RFM init */

  rex_clr_sigs( rex_self(), RF_APPS_TASK_START_SIG ); /*Clear task start sig*/

} /* rf_apps_wait_to_start_task */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Set Signal to start RF_APPS task
 
  @details
  This function will be used to set signal to start RF_APPS Task. This function
  will be called during RFM init.
*/
void
rf_apps_signal_to_start_task
(
  void
)
{
  /* Signal RF APPS Task to start */
  rex_set_sigs( &rf_apps_tcb, RF_APPS_TASK_START_SIG );

} /* rf_apps_signal_to_start_task */

/*!

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Wait for signal that indicates RF apps task has started
 
  @details
  This function will infinitely perform Rex wait on RF_APPS_TASK_STARTED_SIG
  signal. Once this signal is set, RF_APPS task has started and can be used. 
  This signal will be set in RF Apps Task control.
*/
void
rf_apps_wait_till_task_started
(
  rex_tcb_type* rex_tcb
)
{
  /* Store the tcb to be used in rf_apps_signal_task_started */
  rf_apps_task_data.rf_apps_task_started_tcb = rex_tcb;

  if( FALSE == rf_apps_task_data.is_task_running )
  {
    /* Wait for RF APPS TASK STARTED sig to be set */
    rex_wait( RF_APPS_TASK_STARTED_SIG ); 

    /* Clear task started sig */
    rex_clr_sigs( rex_tcb, RF_APPS_TASK_STARTED_SIG );
  }

} /* rf_apps_wait_till_task_started */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Sets signal to indicate RF_APPS task has started
 
  @details
  This function will be used to set signal to show RF_APPS Task has started.
*/
void
rf_apps_signal_task_started
(
  void
)
{
  if( NULL != rf_apps_task_data.rf_apps_task_started_tcb )
  {
    /* Signal that rf apps task has started */
    rex_set_sigs( rf_apps_task_data.rf_apps_task_started_tcb, RF_APPS_TASK_STARTED_SIG );
  }
} /* rf_apps_signal_task_started */

/*! @} */

/*============================================================================*/
/*!
  @name Query

  @brief
  Functions used for RF_APPS parameter queries
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query to check if RF_APPS_TASK is running
 
  @details
  This function will check the RF_APPS data structure to return the running
  state of the task.
 
  @return
  Flag indicating if the RF_APPS task is running or not.
*/
boolean
rf_apps_is_task_running
(
  void
)
{
  return rf_apps_task_data.is_task_running;
} /* rf_apps_is_task_running */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get constant pointer to MSGR parameter used by RF_APPS_TASK
 
  @details
  In order receive MSGR messages by APPS task, these messages need to be
  registered with MSGR module. To indicate that the message is pertaining
  to APPS task, the same CLIENT and Mailbox info should be used during
  MSGR registration of commands. Thus this interface returns the pointer to 
  these parameter (Client and MB)
 
  @return
  Pointer to the MSGR data used by APPS task
*/
const rf_apps_task_msgr_type*
rf_apps_get_msgr_param
(
  void
)
{
  return &(rf_apps_task_data.apps_msgr);
} /* rf_apps_get_msgr_param */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query the Dispatch Handle
 
  @details
  This function return the dispatch handler to used used by the client for
  command registration and deregistration
 
  @return
  Pointer to APPS TASK dispatch handle data
*/
rf_dispatch_handle_type *
rf_apps_task_get_dispatch_handle
(
  void
)
{
  return &(rf_apps_task_data.dispatch_handle);
} /* rf_apps_task_get_dispatch_handle */

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
rf_apps_extract_umid
(
  void *cmd_ptr,
  msgr_umid_type *umid
)
{
  rf_apps_cmd_type *cmd;

  /* NULL Pointer check */
  if (( cmd_ptr == NULL ) || ( umid == NULL ) )
  {
    RF_MSG( RF_ERROR, "rf_apps_extract_umid: NULL Data" );
    return FALSE;
  } /* if (( cmd_ptr == NULL ) || ( umid == NULL ) ) */

  /* Typecast */
  cmd = (rf_apps_cmd_type*)cmd_ptr;

  /* Extract UMID */
  *umid = cmd->msgr_hdr.id;

  return TRUE;
} /* rf_apps_extract_umid */

/*! @} */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF APPS Task control.

  @details
  This is the Radio Frequency (RF) time uncritical task.  After processing task
  start procedure, the task enters an infiniten loop, and sleep until
  RF_APPS_CMD_Q_SIG or TASK_STOP_SIG coming. When TASK_STOP_SIG is received,
  tmc_task_stop() is called.
 
  @param parm
  Dummy variable which is ignored.
*/
void
rf_apps_task
(
  dword parm
)
{
  rex_sigs_type sigs = 0;             
  rf_dispatch_init_handle_data_type init_data;

  /* Required RCINIT handshake */
  rcinit_handshake_startup();

  /*init all necessary RF APPS task data*/
  rf_apps_task_data.rf_apps_task_started_tcb = NULL;

  /* Indicate that RF_APPS TASK is not running yet */
  rf_apps_task_data.is_task_running = FALSE;
  
  /* Initialize Dispatch handler */
  init_data.task_name = "RF_APPS_TASK";
  init_data.task_uid = 0x427f416f; /* MD5(RF_APPS_TASK) */
  init_data.extract_cid_fp = rf_apps_extract_umid;
  rf_apps_get_rex_data( &(init_data.rex_data) );
  init_data.debug_mode = FALSE;
  rf_dispatch_init_handle( &(rf_apps_task_data.dispatch_handle),
                            init_data );

  /* Wait for RF_APPS_START signal to be set before starting
  the task. This signal will be set during RFM init. */
  rf_apps_wait_to_start_task();

  /* Initialize Dispatcher */
  rf_apps_init_dispatcher();

  /* Initialize command queue for RF_APPS task */
  rf_apps_create_command_q();

  /* Indicates that RF APPS Task is now running.*/
  rf_apps_task_data.is_task_running = TRUE;

  /* Set signal to indicates that RF APPS Task is now running.*/
  rf_apps_signal_task_started();

  /* Loop forever handling commands */
  for(;;)
  {
    sigs = rex_wait( TASK_STOP_SIG |        /* Task shutdown   */
                     RF_APPS_CMD_Q_SIG   /* Command queue signal */
                    );

    /* There is implicit priority given to signals. Only one signal is
    processed at a time. */
    if ( sigs & TASK_STOP_SIG )
    {
      (void) rex_clr_sigs(&rf_apps_tcb, TASK_STOP_SIG);

      rf_apps_deinit();

      rf_apps_task_data.is_task_running = FALSE;
#ifndef FEATURE_MODEM_RCINIT
      tmc_task_stop();
#else
      RF_MSG( RF_ERROR, "rf_apps_task: need to call right API to stop task");
#endif
      /* Task is stopped.  No need to run this function anymore. */
      break;
    }
    else if ( sigs & RF_APPS_CMD_Q_SIG )
    {
      /* First, clear the signal */
      (void) rex_clr_sigs( &rf_apps_tcb, RF_APPS_CMD_Q_SIG );

      /* process commands in a loop to ensure that RF APPS task does not "skip" 
      processing of any messages */
      while ( rf_apps_get_command_num() != 0 )
      {
        /* Process RF commands in the queue */
        rf_apps_dispatch_command();
      
      } /* while ( rf_apps_get_command_num() != 0 ) */
    }
  } /* for(;;) */

  /* Exit RF APPS Task */
  return;
} /* rf_apps_task */

/*! @} */


/*! @} */

