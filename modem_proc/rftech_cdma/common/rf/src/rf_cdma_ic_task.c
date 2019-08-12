/*! 
  @file
  rf_cdma_task.c
 
  @brief
  This file includes functions pertaining to Tasks used for CDMA Operation
 
  @addtogroup RF_CDMA_COMMON_IC_TASK
  @{
*/
 
/*==============================================================================

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rf_cdma_ic_task.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/23/13   sty     compiler warning fixes
11/29/12   aki     Release msgr mailbox and handshake with RCINIT at task exit
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
07/23/12   Saul    COMMON. Renamed MSGR_MODULE_RF_CDMA to MSGR_MODULE_RF_COMMON
03/19/12   sty     Featurized call to tmc_task_stop()
03/28/12   aro     Moved the handling FW response message to FWRSP Task
03/28/12   aro     Added commands to PAUSE, RESUME, and PM OVERRIDE
10/26/11   aro     Implemented mechanism not to start iC task until a signal
                   is set during intelliceiver initialization
10/26/11   aro     Deleted obselete test function
10/26/11   aro     Deleted COMPLETE_POLLING commands
10/25/11   aro     Added housekeeping variable to keep track of 1x and HDR
                   active device to be used during handling of iC Update rsp
10/25/11   aro     Renamed command ID enums and their dispatcher functions
10/25/11   aro     Added complete polling commands
10/25/11   aro     Renamed the names of iC Task functions to maintain
                   consistency
10/25/11   aro     Moved functions to appropriate doxygen block
10/25/11   aro     Implemented unified dispatch table to handle both REX and
                   MSGR based commands.
10/24/11   aro     Cleaned up unused timers
10/24/11   aro     Removed code to wait for signal from TMC during task start
10/24/11   aro     Implemented an interface to put RF iC command to task Q
10/24/11   aro     Added interface to get the free command buffer
10/24/11   aro     Update doxygen documentation
10/24/11   aro     Removed Watchdog implementation for RF iC Task
10/24/11   aro     Added RCINIT handshake to spawn the task
10/21/11   aro     Grouped functions into various doxygen blocks
10/21/11   aro     Implemented Command processing
10/21/11   aro     Added "Forever Loop" to handle rf_ic_task commands
10/21/11   aro     Added function to deinitialize REX-MSGR
10/21/11   aro     Added function to initialize the task queue commands
10/21/11   aro     Added function for REX MSGR initialization
10/21/11   aro     Created WATCHDOG initialization and kick functions
10/21/11   aro     Moved WATCHDOG variable out from the data structure
10/20/11   aro     Added Code to initialize watchdog timer
10/18/11   aro     Added Dispatcher function for intelliceiver enter steady 
                   state operation
10/18/11   aro     Added Dispatcher function for intelliceiver polling 
                   operation for steady mode.
10/18/11   aro     Initial Revision.

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_cdma_ic_task.h"
#include "rf_cdma_data.h"
#include "rfcommon_msg.h"
#include "dog.h"
#include "msgr_rex.h"
#include "cdmafw_msg.h"
#include "hdrfw_msg.h"

/*----------------------------------------------------------------------------*/
/*! Externalized RF iC Task control block variable */
extern rex_tcb_type rf_ic_tcb;

/*----------------------------------------------------------------------------*/
/*! Variable containing the list of CDMA iC commands to be handled */
static rf_cdma_ic_cmd_dispatch_tbl_type rf_cdma_ic_command_tbl[] =
{
  /* Format */
  /*
     REX/MSGR
     Command ID,
     UMID,
     Command handler,
     Payload Size
  */
  {
    RF_CDMA_IC_REX,
    RF_CDMA_IC_ENTER_STEADY_STATE_C,
    0,
    rf_cdma_ic_handle_enter_steady_state,
    sizeof(rf_cdma_ic_enter_steady_state_payload)
  },
  {
    RF_CDMA_IC_REX,
    RF_CDMA_IC_START_POLLING_C, 
    0,
    rf_cdma_ic_handle_start_polling,
    sizeof(rf_cdma_ic_start_polling_payload)
  },
  {
    RF_CDMA_IC_REX,
    RF_CDMA_IC_PAUSE_IC_C, 
    0,
    rf_cdma_ic_handle_pause,
    sizeof(rf_cdma_ic_pause_payload)
  },
  {
    RF_CDMA_IC_REX,
    RF_CDMA_IC_RESUME_IC_C, 
    0,
    rf_cdma_ic_handle_resume,
    sizeof(rf_cdma_ic_resume_payload)
  },
  {
    RF_CDMA_IC_REX,
    RF_CDMA_IC_OVERRIDE_PM_C, 
    0,
    rf_cdma_ic_handle_pm_override,
    sizeof(rf_cdma_ic_override_pm_payload)
  },
  {
    RF_CDMA_IC_REX,
    RF_CDMA_IC_CLEAR_OVERRIDE_PM_C, 
    0,
    rf_cdma_ic_handle_pm_override_clear,
    sizeof(rf_cdma_ic_override_pm_clear_payload)
  }
};

/*----------------------------------------------------------------------------*/
/*! Enumeration defining the number of commands in the dispatch table */
#define RF_CDMA_IC_NUM_OF_CMDS  ARR_SIZE(rf_cdma_ic_command_tbl)

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
  RF iC Rex MSGR interface initialization

  @details
  This funciton creates the message router client on the RF IC TCB, registers 
  for all the MSGR commands that RF IC task wishes to recieve.
 
  Create message queue to receive message router messages from other SW
  entities. RF is a REX Task and it waits on REX signals. Hence a REX message 
  queue has to be added to the message router client.

  @param ic_task_data
  Pointer to data pertaining to IC TASK
 
  @param p_rf_ic_tcb
  Pointer to RF IC TCB structure
 
  @param sig
  REX Signal to wait on.
 
  @return
  Flag indicating, if clients are successfully created or not.
*/
static boolean 
rf_cdma_ic_rex_msgr_init
(
  rf_cdma_ic_task_data_type *ic_task_data,
  rex_tcb_type *p_rf_ic_tcb,
  rex_sigs_type rf_rex_sig
)
{
  boolean ret_val =  TRUE;
  errno_enum_type msgr_status;

  /* Create RF CDMA iC Task message router client for receiving messages from 
  SW entities */

  msgr_status = msgr_client_create(&(ic_task_data->ic_msgr.client));

  if ( msgr_status != E_SUCCESS )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_rex_msgr_interface_init : "
            "Unable to create RF message router client ");  
    ret_val = FALSE;
  }

  if (msgr_status == E_SUCCESS)
  {
    msgr_status = msgr_client_add_rex_q(
                                      &(ic_task_data->ic_msgr.client),
                                      p_rf_ic_tcb,
                                      rf_rex_sig, 
                                      &(ic_task_data->ic_cmd_q.cmd_q),
                                      &(ic_task_data->ic_cmd_q.free_q),
                                      sizeof(rf_cdma_ic_cmd_rex_hdr_type),
                                      sizeof(q_link_type),
                                      sizeof(rf_cdma_ic_cmd_type),
                                      &(ic_task_data->ic_msgr.mb)
                                      );

    if (msgr_status != E_SUCCESS)
    {
      RF_MSG( RF_ERROR,"rf_cdma_ic_rex_msgr_interface_init : "
              "Unable to add RF REX MSGR client ");  
      ret_val = FALSE;
    }
  }

  /* Return Status */
  return ret_val;

} /* rf_cdma_ic_rex_msgr_init */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF IC Message Router Tear down
 
  @param ic_task_data
  Pointer to data pertaining to IC TASK
 
*/
static void 
rf_cdma_ic_rex_msgr_deinit
(
  rf_cdma_ic_task_data_type *ic_task_data
)
{
  if ( NULL != ic_task_data )
  {
    /*Release all mailboxes associated with RF IC client */
    msgr_client_delete(&(ic_task_data->ic_msgr.client));
  }

  return;

} /* rf_cdma_ic_rex_msgr_deinit */


/*! @} */

/*============================================================================*/
/*!
  @name Command Queue

  @brief
  Functions Pertaining to Command Queue for RF iC Task usage
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize RF CDMA IC TASK CMD queue.

  @details
  This function initializes the RF CDMA iC task Command Queue Structures.

  @param ic_task_data
  Pointer to RF_IC_TASK data
*/
static void
rf_cdma_ic_create_command_q
(
  rf_cdma_ic_task_data_type *ic_task_data
)
{
  uint32 loop_cnt;

  /* Initialize the Command Queue, and the Command Free Queue */
  (void) q_init( &(ic_task_data->ic_cmd_q.cmd_q) );
  (void) q_init( &(ic_task_data->ic_cmd_q.free_q) );

  /* Link the Command Queue Items onto the Command Free Queue */
  for ( loop_cnt = 0; 
        loop_cnt < ARR_SIZE((ic_task_data->ic_cmd_q.q_items)); 
        loop_cnt++)
  {
    q_put( &(ic_task_data->ic_cmd_q.free_q),
           q_link( &(ic_task_data->ic_cmd_q.q_items[loop_cnt]), 
                   &(ic_task_data->ic_cmd_q.q_items[loop_cnt].rex_hdr.link) )
          );
  }

} /* rf_cdma_ic_create_command_q */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the number of commands on RF IC TASK cmd Queue
 
  @param ic_task_data
  Pointer to data pertaining to IC TASK

  @return
  Total the number of commands on RF cmd Queue
*/
static int
rf_cdma_ic_get_command_num
(
  rf_cdma_ic_task_data_type *ic_task_data
)
{
  return q_cnt( &(ic_task_data->ic_cmd_q.cmd_q) );
} /* rf_cdma_ic_get_command_num */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Retreive the command from  RF IC queue.

  @details
  This function will retreive the command from RF IC Task queue, and then 
  removes the command from Queue.

  @param ic_task_data
  Pointer to data pertaining to IC TASK
 
  @return
  Address of the command retreived from the command queue.
*/
static rf_cdma_ic_cmd_type* 
rf_cdma_ic_get_command
(
  rf_cdma_ic_task_data_type *ic_task_data
)
{
   rf_cdma_ic_cmd_type * item;

   /* Retrieve next commmand from command queue */
   item = (rf_cdma_ic_cmd_type *) q_get( &(ic_task_data->ic_cmd_q.cmd_q) );

   if ( item != NULL )
   {
     item->status = RF_CDMA_IC_CMD_PROCESSING;
   }
   else
   {
     ERR_FATAL( "rf_cdma_ic_get_command : Signal for the"
                " queue was set, but no cmd in Q",0 ,0 ,0 ); 
   }

   return item;

} /* rf_cdma_ic_get_command */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Free the command buffer on RF IC Taskqueue.

  @details
  This function removes an already processed command from RF IC Task queue.
  The memory is returned to free queue.
 
  @param ic_task_data
  Pointer to data pertaining to IC TASK
 
  @param cmd
  The address of the command to be freed from RF task queue.
*/
static void
rf_cdma_ic_free_command
(
  rf_cdma_ic_task_data_type *ic_task_data,
  rf_cdma_ic_cmd_type* cmd
)
{

  if ( cmd == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_free_command: Invalid cmd in Q" );
    return;
  }

  memset( cmd, 0, sizeof(rf_cdma_ic_cmd_type) );

  /* Mark item as no longer valid */
  cmd->status = RF_CDMA_IC_CMD_BUF_EMPTY;

 ( void )q_link( cmd, &(cmd->rex_hdr.link) );

  /* Add the item to the free queue */
  q_put( &(ic_task_data->ic_cmd_q.free_q), &(cmd->rex_hdr.link) );

  return;

} /* rf_cdma_ic_free_command */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Adds the command to RF iC queue.

  @details
  This function adds the command to RF iC queue and signals RF iC task. The 
  command is added on the memory allocated by #rf_cdma_get_command_buffer().

  @param cmd
  The address of the command to be added to RF iC task queue.
 
  @return
  Status indicating, if the command was successfully put on the command Q or not
*/
boolean 
rf_cdma_ic_put_command
(
  rf_cdma_ic_cmd_type* cmd
)
{
  rf_cdma_ic_task_data_type *ic_task_data; /* iC Task Data container */

  /* Get pointer to the intelliceiver Task Data */
  ic_task_data = rf_cdma_get_ic_task_data();

  if ( ic_task_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_put_command: NULL Task Data" );
    return FALSE;
  }

  if ( cmd == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_put_command: Invalid Command" );
    return FALSE;
  }

  /* Iniliatize the link of the pointer */
  (void) q_link(cmd, &(cmd->rex_hdr.link) );

  /* Add command to the queue */
  q_put( &(ic_task_data->ic_cmd_q.cmd_q), &(cmd->rex_hdr.link) );

  cmd->status = RF_CDMA_IC_CMD_QUEUED;

  cmd->alloc_time = rf_time_get_tick();

  /* Signal the task about availability of the command */
  (void) rex_set_sigs(&rf_ic_tcb, RF_CDMA_IC_CMD_Q_SIG);

  return TRUE;

} /* rf_cdma_ic_put_command*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns free buffer on RF command queue.

  @details
  This function returns the pointer of free buffer on RF iC command Queue. This 
  is used by iC module to send command to RF iC Task. If buffer has been 
  requested, it is required that it calls #rf_cdma_ic_put_command function 
  immediately. Calling module should set the command header and payload fields.
 
  @return
  Pointer to command buffer. If command queue full, the function will return
  NULL. If command has buffer available, it will return pointer to the buffer.
*/
rf_cdma_ic_cmd_type*
rf_cdma_get_command_buffer
(
  void
)
{
  rf_cdma_ic_cmd_type *item;
  rf_cdma_ic_task_data_type *ic_task_data; /* iC Task Data container */

  /* Get pointer to the intelliceiver Task Data */
  ic_task_data = rf_cdma_get_ic_task_data();

  if ( ic_task_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_get_command_buffer: NULL Task Data" );
    return NULL;
  }

  /* Retrieve next commmand from command queue */
  item = (rf_cdma_ic_cmd_type *) q_get( &(ic_task_data->ic_cmd_q.free_q) );
  
  if ( item != NULL )
  {
    /* If a command was available, return a pointer to the command structure */
    item->status = RF_CDMA_IC_CMD_BUF_ALLOCATED;
  }
  else
  {
    /* No command buffer is available! */
    RF_MSG( RF_ERROR, "rf_cdma_get_command_buffer: Command Buffer Full" );
    item = NULL;
  }

  return item;
} /* rf_cdma_get_command_buffer */

/*! @} */

/*============================================================================*/
/*!
  @name Dispatcher

  @brief
  Functions Pertaining command dispatcher for RF iC Task
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Initialize Dispatcher for iC Task
 
  @details
  This function will initialize the Dispatch table containing the dispatch
  function for each REX based or MSGR based commands.
 
  This funciton also creates the message router client on the RF IC TCB and
  registers for all the MSGR commands that RF IC task wishes to recieve.
 
  @return
  Flag indicating if the dispatcher is successfully initialized or not.
*/
boolean
rf_cdma_ic_init_dispatcher
(
  void
)
{
  boolean res; /* API results */
  rf_cdma_ic_task_data_type *ic_task_data; /* iC Task Data container */
  uint32 lp_idx;
  boolean rex_msgr_init_status; /* Status for Rex MSGR init */

  /* Get pointer to the intelliceiver Task Data */
  ic_task_data = rf_cdma_get_ic_task_data();

  /* NULL Pointer check */
  if ( ic_task_data == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_init_dipatcher: NULL iC Task Data" );
    return FALSE;
  } /* if ( ic_task_data == NULL ) */

  /* Initialize REX MSGR Interface */
  rex_msgr_init_status = rf_cdma_ic_rex_msgr_init( ic_task_data, 
                                                   &rf_ic_tcb,
                                                   RF_CDMA_IC_CMD_Q_SIG );

  /* Check if Rex-MSGR init is successful */
  if ( rex_msgr_init_status == FALSE )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_init_dipatcher: REX-MSGR Failed" );
    return FALSE;    
  }

  /* Set the Pointer to Dispatch table */
  ic_task_data->ic_dispatch_tbl.dispatch_tbl = rf_cdma_ic_command_tbl;
  res = TRUE; /* start with good value */

  for ( lp_idx = 0; lp_idx < RF_CDMA_IC_NUM_OF_CMDS; lp_idx++ )
  {
    errno_enum_type msgr_ret = E_SUCCESS; /* holds registration result */  

    /* Perform registration with MSGR, if the command belongs to MSGR */
    if ( rf_cdma_ic_command_tbl[lp_idx].cmd_src == RF_CDMA_IC_MSGR )
    {
      msgr_ret = msgr_register( MSGR_RFA_RF_COMMON,
                                &(ic_task_data->ic_msgr.client),
                                ic_task_data->ic_msgr.mb, 
                                rf_cdma_ic_command_tbl[lp_idx].cdma_umid );
    } /* if ( rf_cdma_ic_command_tbl[lp_idx].cmd_src == RF_CDMA_IC_MSGR ) */

    if ( msgr_ret !=  E_SUCCESS)
    {
      res = FALSE;

      RF_MSG_1( RF_ERROR, "rf_cdma_ic_init_dispatcher: Failed call to "
                          "msgr_register: for UMID = 0x%x ",
                          rf_cdma_ic_command_tbl[lp_idx].cdma_umid );
    }


  } /*for ( lp_idx = 0; lp_idx < ARR_SIZE(rf_cdma_ic_command_tbl); lp_idx++ )*/

  ic_task_data->ic_dispatch_tbl.init_done = TRUE;

  return res;

} /* rf_cdma_ic_init_dispatcher */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Process the RF iC Task commands
 
  @details
  This funciton will pop the command from the queue and process it in order
  to call the appropriate dispatch function.
 
  @param ic_task_data
  Pointer to data pertaining to IC TASK
*/
static void
rf_cdma_ic_dispatch_command
(
  rf_cdma_ic_task_data_type *ic_task_data
)
{
  rf_cdma_ic_cmd_type *cmd;
  rf_cdma_ic_cmd_id_type cmd_id;
  rf_cdma_ic_cmd_dispatch_tbl_type *cmd_dispatch_ptr = NULL;
  uint32 lp_idx;
  boolean cmd_found = FALSE;

  /* Get Command */
  cmd = rf_cdma_ic_get_command( ic_task_data );
 
  /* Perform NULL pointer check */
  if ( cmd == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_cmd_process: No Command" );
    return;
  }

  /* Check if the dispatch table is initialized */
  if ( ic_task_data->ic_dispatch_tbl.dispatch_tbl == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_cmd_process: No Dispatch Table" );
    return;
  }

  if ( ic_task_data->ic_dispatch_tbl.init_done == FALSE )
  {
    RF_MSG( RF_ERROR, "rf_cdma_ic_cmd_process: Dispatch not initialized" );
    return;
  }

  /* Get command ID */
  cmd_id = cmd->rex_hdr.cmd_hdr.cmd_id;

  /* Loop through Command table to call the respective dispatcher */
  for ( lp_idx = 0; lp_idx < RF_CDMA_IC_NUM_OF_CMDS; lp_idx++ )
  {
    /* Perform registration with MSGR, if the command belongs to MSGR */
    if ( ic_task_data->ic_dispatch_tbl.dispatch_tbl[lp_idx].cmd_id == cmd_id )
    {
      cmd_dispatch_ptr = &(ic_task_data->ic_dispatch_tbl.dispatch_tbl[lp_idx]);

      if ( cmd_dispatch_ptr->cmd_handler != NULL )
      {
        cmd_dispatch_ptr->cmd_handler( cmd_id, cmd_dispatch_ptr->payload_size,
                                       cmd );
  
        /* If required, send the acknowledge signal */
        if( cmd->complete_sig && cmd->task_to_sig )
        {
          (void) rex_set_sigs( cmd->task_to_sig, cmd->complete_sig );
        }
  
        cmd->status = RF_CDMA_IC_CMD_SUCCESS;
        cmd_found = TRUE;
      }
      else
      {
        RF_MSG( RF_ERROR, "rf_cdma_ic_cmd_process: Bad Command Handler" );
      }

      break;

    } /* if ( rf_cdma_ic_command_tbl[lp_idx].cmd_src == RF_CDMA_IC_MSGR ) */

  } /*for ( lp_idx = 0; lp_idx < ARR_SIZE(rf_cdma_ic_command_tbl); lp_idx++ )*/

  if ( cmd_found == FALSE )
  {
    cmd->status = RF_CDMA_IC_CMD_NOT_SUPPORTED;

    RF_MSG_1( RF_ERROR, "rf_cdma_ic_cmd_process: Command %d not supported",
              cmd_id );
  }
 
  /* Processing of command complete. Free Memory */
  rf_cdma_ic_free_command( ic_task_data, cmd );
 
  return;
 
} /* rf_cdma_ic_dispatch_command */

/*! @} */

/*============================================================================*/
/*!
  @name Signalling

  @brief
  Functions Pertaining to RF iC Task signalling
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Wait for signal to start iC task
 
  @details
  This function will infinitely perform Rex wait on RF_CDMA_IC_TASK_START_SIG
  signal. Once this signal is set, iC task will be started. This signal will
  be set by intelliceiver module.
*/
static void
rf_cdma_ic_wait_to_start_task
(
  void
)
{
  rex_wait(RF_CDMA_IC_TASK_START_SIG); /* Wait for iC Module to set the sig */
  rex_clr_sigs(rex_self(), RF_CDMA_IC_TASK_START_SIG); /*Clear task start sig*/
} /* rf_cdma_ic_wait_to_start_task */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Set Signal to start iC task
 
  @details
  This function will be used to set signal to start iC Task. This function
  will be called by iC module during init.
*/
void
rf_cdma_ic_signal_to_start_task
(
  void
)
{
  /* Signal RF iC Task to start */
  rex_set_sigs(&rf_ic_tcb, RF_CDMA_IC_TASK_START_SIG);

} /* rf_cdma_ic_signal_to_start_task */

/*! @} */

/*============================================================================*/
/*!
  @name Task Spawn

  @brief
  Functions Pertaining to RF iC Task spawning
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF CDMA Task control.

  @details
  This is the Radio Frequency (RF) CDMA task, created from the Main Control 
  task. After processing task start procedure, the RF task enters an infiniten 
  loop, awaiting commands received on its command queue.  Each command received 
  is processed to completion before the next command is processed.  While in the
  main loop the task kicks the watchdog periodically.

  @param parm
  Dummy variable which is ignored.
*/
void
rf_ic_task
(
  dword parm
)
{
  rf_cdma_ic_task_data_type *ic_task_data; /* iC Task Data container */
  rex_sigs_type sigs = 0;             

  /* Get pointer to the intelliceiver Task Data */
  ic_task_data = rf_cdma_get_ic_task_data();

  /* Required RCINIT handshake */
  rcinit_handshake_startup();

  /* Wait for RF_CDMA_IC_TASK_START_SIG signal to be set before starting
  the task. This signal will be set when intelliceiver module is initialized
  during rf_cdma_ic_init(). */
  rf_cdma_ic_wait_to_start_task();

  rf_cdma_ic_init_dispatcher();

  /* Spawn the task only if ic_task_data is not NULL */
  if ( ic_task_data != NULL )
  {
    /* Indicate that IC_TASK is not running yet */
    ic_task_data->housekeeping.is_task_running = FALSE;

    /* Reset other housekeeping variabeles */
    ic_task_data->housekeeping.onex_ic_active_dev = RFM_INVALID_DEVICE;
    ic_task_data->housekeeping.hdr_ic_active_dev = RFM_INVALID_DEVICE;

    /* Initialize command queue for RF_IC_TASK */
    rf_cdma_ic_create_command_q( ic_task_data );

    /* Indicates that RF Task is now running. QTF returns here after 
    tmc_task_start. Do not change the order. Else the QTF test cases will fail 
    to ASSERT.*/
    ic_task_data->housekeeping.is_task_running = TRUE;

    /* Loop forever handling commands */
    for(;;)
    {
      sigs = rex_wait( TASK_STOP_SIG            /* Task shutdown   */
                     | RF_CDMA_IC_CMD_Q_SIG     /* Command queue signal */
                     );

      /* There is implicit priority given to signals. Only one signal is
      processed at a time. */
      if ( sigs & TASK_STOP_SIG )
      {
        (void) rex_clr_sigs(&rf_ic_tcb, TASK_STOP_SIG);

        ic_task_data->housekeeping.is_task_running = FALSE;

        rf_cdma_ic_rex_msgr_deinit(rf_cdma_get_ic_task_data());

#ifndef FEATURE_MODEM_RCINIT
        tmc_task_stop();
#else
        rcinit_handshake_term();
#endif /* FEATURE_MODEM_RCINIT */

        /* Task is stopped.  No need to run this function anymore. */
        break;
      }
      else if ( sigs & RF_CDMA_IC_CMD_Q_SIG )
      {
        /* First, clear the signal */
        (void) rex_clr_sigs( &rf_ic_tcb, RF_CDMA_IC_CMD_Q_SIG );

        /* process commands in a loop to ensure that RF IC task does not "skip" 
        processing of any messages */
        while ( rf_cdma_ic_get_command_num( ic_task_data ) != 0 )
        {
          /* Process RF commands in the queue */
          rf_cdma_ic_dispatch_command( ic_task_data );

        } /* while ( rf_cdma_ic_get_num_of_cmds_in_q() != 0 ) */
      }
    } /* for(;;) */
  } /* if ( ic_task_data != NULL ) */

  /* Exit iC Task */
  return;
} /* rf_ic_task */
#endif /* FEATURE_CDMA1X */
/*! @} */

/*! @} */

