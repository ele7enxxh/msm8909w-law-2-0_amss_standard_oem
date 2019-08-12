/*!
  @file
  rflte_core_task.c

  @brief
  This file is used to control & support CORE Task.

  @{
*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/src/rflte_core_task.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/07/14   cri     Initial check in

==============================================================================*/

#include "comdef.h"
#include "rflte_core_task.h"
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
extern rex_tcb_type rflte_core_tcb;

/*----------------------------------------------------------------------------*/
/*! Variable to store the global data for LTE_CORE task */
static rflte_core_task_data_type rflte_core_task_data;

/*============================================================================*/
/*!
  @name REX-MSGR

  @brief
  Functions Pertaining to initialization and de-initialization of REX-MSGR
*/
/*! @{ */

void rflte_core_task_deinit(void)
{
  msgr_client_delete(&(rflte_core_task_data.rflte_core_msgr.client));
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
rflte_core_task_rex_msgr_init
(
  rex_tcb_type *p_rflte_core_tcb,
  rex_sigs_type rf_rex_sig
)
{
  boolean ret_val =  TRUE;
  errno_enum_type msgr_status;

  /* Create Task message router client for receiving messages
  from  SW entities */
  msgr_status = msgr_client_create(&(rflte_core_task_data.rflte_core_msgr.client));

  if ( msgr_status != E_SUCCESS )
  {
    RF_MSG( RF_ERROR, "rflte_core_rex_msgr_init : "
            "Unable to create RF message router client ");  
    ret_val = FALSE;
  }

  if (msgr_status == E_SUCCESS)
  {
    msgr_status = msgr_client_add_rex_q(
                                      &(rflte_core_task_data.rflte_core_msgr.client),
                                      p_rflte_core_tcb,
                                      rf_rex_sig, 
                                      &(rflte_core_task_data.rflte_core_task_cmd_q.cmd_q),
                                      &(rflte_core_task_data.rflte_core_task_cmd_q.free_q),
                                      sizeof(rflte_core_task_cmd_rex_hdr_type),
                                      sizeof(q_link_type),
                                      sizeof(rflte_core_task_cmd_type),
                                      &(rflte_core_task_data.rflte_core_msgr.mb)
                                      );

    if (msgr_status != E_SUCCESS)
    {
      RF_MSG( RF_ERROR,"rflte_core_rex_msgr_init : "
              "Unable to add RF REX MSGR client ");  
      ret_val = FALSE;
    }
  }

  /* Return Status */
  return ret_val;

} /* rflte_core_rex_msgr_init */

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
rflte_core_task_create_command_q
(
  void
)
{
  uint32 loop_cnt;

  /* Initialize the Command Queue, and the Command Free Queue */
  (void) q_init( &(rflte_core_task_data.rflte_core_task_cmd_q.cmd_q) );
  (void) q_init( &(rflte_core_task_data.rflte_core_task_cmd_q.free_q) );

  /* Link the Command Queue Items onto the Command Free Queue */
  for ( loop_cnt = 0; 
        loop_cnt < ARR_SIZE((rflte_core_task_data.rflte_core_task_cmd_q.q_items)); 
        loop_cnt++)
  {
    q_put( &(rflte_core_task_data.rflte_core_task_cmd_q.free_q),
           q_link( &(rflte_core_task_data.rflte_core_task_cmd_q.q_items[loop_cnt]), 
             &(rflte_core_task_data.rflte_core_task_cmd_q.q_items[loop_cnt].rex_hdr.link) )
          );
  }

} /* rflte_core_create_command_q */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the number of commands on RF FWRSP TASK cmd Queue
 
  @return
  Total the number of commands on the cmd Queue
*/
static int
rflte_core_task_get_command_num
(
  void
)
{

  return q_cnt( &(rflte_core_task_data.rflte_core_task_cmd_q.cmd_q) );

} /* rflte_core_get_command_num */

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
static rflte_core_task_cmd_type* 
rflte_core_task_get_command
(
  void
)
{
   rflte_core_task_cmd_type * item;

   /* Retrieve next commmand from command queue */
   item = (rflte_core_task_cmd_type *) q_get(&(rflte_core_task_data.rflte_core_task_cmd_q.cmd_q));

   if ( item != NULL )
   {
     item->status = RFLTE_CORE_TASK_CMD_PROCESSING;
   }
   else
   {
     ERR_FATAL( "rflte_core_get_command : Signal for the"
                " queue was set, but no cmd in Q",0 ,0 ,0 ); 
   }

   return item;

} /* rflte_core_get_command */

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
rflte_core_task_free_command
(
  rflte_core_task_cmd_type* cmd
)
{

  if ( cmd == NULL )
  {
    RF_MSG( RF_ERROR, "rflte_core_free_command: Invalid cmd in Q" );
    return;
  }

  memset( cmd, 0, sizeof(rflte_core_task_cmd_type) );

  /* Mark item as no longer valid */
  cmd->status = RFLTE_CORE_TASK_CMD_BUF_EMPTY;

 ( void )q_link( cmd, &(cmd->rex_hdr.link) );

  /* Add the item to the free queue */
  q_put( &(rflte_core_task_data.rflte_core_task_cmd_q.free_q), 
         &(cmd->rex_hdr.link) );

  return;

} /* rflte_core_free_command */

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
rflte_core_task_get_rex_data
(
  rf_dispatch_rex_data_type *rex_data_ptr
)
{
  /* NULL Pointer check */
  if ( rex_data_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rflte_core_get_rex_data: NULL container" );
    return FALSE;
  } /* if ( rex_data_ptr == NULL ) */

  rex_data_ptr->rex_tcb = &rflte_core_tcb;
  rex_data_ptr->rex_sig = RFLTE_CORE_TASK_CMD_Q_SIG;
  rex_data_ptr->cmd_q = &(rflte_core_task_data.rflte_core_task_cmd_q.cmd_q);
  rex_data_ptr->free_q = &(rflte_core_task_data.rflte_core_task_cmd_q.free_q);
  rex_data_ptr->header_size = sizeof(rflte_core_task_cmd_rex_hdr_type);
  rex_data_ptr->q_link_size = sizeof(q_link_type);
  rex_data_ptr->command_size = sizeof(rflte_core_task_cmd_type);

  return TRUE;
} /* rflte_core_get_rex_data */

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
rflte_core_task_init_dispatcher
(
  void
)
{
  boolean rex_msgr_init_status = FALSE; /* Status for Rex MSGR init */
  boolean ret_val = TRUE;

  /* Initialize REX MSGR Interface */
  rex_msgr_init_status = rflte_core_task_rex_msgr_init( &rflte_core_tcb,
                                                 RFLTE_CORE_TASK_CMD_Q_SIG );

  /* Check if Rex-MSGR init is successful */
  if ( rex_msgr_init_status == FALSE )
  {
    RF_MSG( RF_ERROR, "rflte_core_init_dispatcher: REX-MSGR Failed" );
    ret_val = FALSE;    
  }

  return ret_val;

} /* rflte_core_init_dispatcher */

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
  Set Signal to start LTE_CORE task
 
  @details
  This function will be used to set signal to start LTE_CORE Task. This function
  will be called during RFM init.
*/
void
rflte_core_task_signal_to_start_task
(
  void
)
{
  /* Signal RF iC Task to start */
  rex_set_sigs( &rflte_core_tcb, RFLTE_CORE_TASK_START_SIG );

} /* rflte_core_signal_to_start_task */

/*! @} */

/*============================================================================*/
/*!
  @name Query

  @brief
  Functions used for LTE_CORE parameter queries
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query to check if LTE_CORE is running
 
  @details
  This function will check the LTE_CORE data structure to return the running
  state of the task.
 
  @return
  Flag indicating if the LTE_CORE task is running or not.
*/
boolean
rflte_core_task_is_task_running
(
  void
)
{
  return rflte_core_task_data.housekeeping.is_task_running;
} /* rflte_core_is_task_running */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get constant pointer to MSGR parameter used by LTE_CORE_TASK
 
  @details
  In order receive MSGR messages by FWRSP task, these messages need to be
  registered with MSGR module. To indicate that the message is pertaining
  to FWRSP task, the same CLIENT and Mailbox info should be used during
  MSGR registration of commands. Thus this interface returns the pointer to 
  these parameter (Client and MB)
 
  @return
  Pointer to the MSGR data used by FWRSP task
*/
const rflte_core_task_msgr_type*
rflte_core_task_get_msgr_param
(
  void
)
{
  return &(rflte_core_task_data.rflte_core_msgr);
} /* rflte_core_get_msgr_param */

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
rflte_core_task_get_dispatch_handle
(
  void
)
{
  return &(rflte_core_task_data.dispatch_handle);
} /* rflte_core_task_get_dispatch_handle */

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
rflte_core_task_extract_umid
(
  void *cmd_ptr,
  msgr_umid_type *umid
)
{
  rflte_core_task_cmd_type *cmd;

  /* NULL Pointer check */
  if (( cmd_ptr == NULL ) || ( umid == NULL ) )
  {
    RF_MSG( RF_ERROR, "rflte_core_extract_umid: NULL Data" );
    return FALSE;
  } /* if (( cmd_ptr == NULL ) || ( umid == NULL ) ) */

  /* Typecast */
  cmd = (rflte_core_task_cmd_type*)cmd_ptr;

  /* Extract UMID */
  *umid = cmd->msgr_hdr.id;

  return TRUE;
} /* rflte_core_extract_umid */

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
rflte_core_task
(
  dword parm
)
{
  rex_sigs_type sigs = 0;             
  rf_dispatch_init_handle_data_type init_data;

  /* Initialize Dispatcher */
  /* It is moved to here to mitigate race condition between
  init and msgs registration */
  rflte_core_task_init_dispatcher();

  /* Required RCINIT handshake */
  rcinit_handshake_startup();

  /* Indicate that IC_TASK is not running yet */
  rflte_core_task_data.housekeeping.is_task_running = FALSE;

  /* Initialize Dispatch handler */
  init_data.task_name = "LTE_CORE_TASK";
  init_data.task_uid = 0x2F9301CE; /* MD5(RFLTE_CORE_TASK) */
  init_data.extract_cid_fp = rflte_core_task_extract_umid;
  rflte_core_task_get_rex_data( &(init_data.rex_data) );
  init_data.debug_mode = FALSE;
  rf_dispatch_init_handle( &(rflte_core_task_data.dispatch_handle),
                            init_data );

  /* Wait for LTE_CORE_TASK_START_SIG signal to be set before starting
  the task. This signal will be set during RFM init. */
  //f();

  /* Spawn the task only if ic_task_data is not NULL */

  /* Initialize command queue for LTE_CORE */
  rflte_core_task_create_command_q();

  /* Indicates that RF Task is now running.*/
  rflte_core_task_data.housekeeping.is_task_running = TRUE;

  /* Loop forever handling commands */
  for(;;)
  {
    sigs = rex_wait( TASK_STOP_SIG |        /* Task shutdown   */
                     RFLTE_CORE_TASK_CMD_Q_SIG   /* Command queue signal */
                    );

    /* There is implicit priority given to signals. Only one signal is
    processed at a time. */
    if ( sigs & TASK_STOP_SIG )
    {
      (void) rex_clr_sigs(&rflte_core_tcb, TASK_STOP_SIG);

      rflte_core_task_deinit();

      rflte_core_task_data.housekeeping.is_task_running = FALSE;
#ifndef FEATURE_MODEM_RCINIT
      tmc_task_stop();
#else
      rcinit_handshake_term();
#endif
      /* Task is stopped.  No need to run this function anymore. */
      break;
    }
    else if ( sigs & RFLTE_CORE_TASK_CMD_Q_SIG )
    {
      /* First, clear the signal */
      (void) rex_clr_sigs( &rflte_core_tcb, RFLTE_CORE_TASK_CMD_Q_SIG );

      /* process commands in a loop to ensure that RF IC task does not "skip" 
      processing of any messages */
      while ( rflte_core_task_get_command_num() != 0 )
      {
        /* Process RF commands in the queue */
        #ifndef FEATURE_GNSS_ONLY_NO_WWAN

          rflte_core_task_cmd_type *cmd;

          /* Get command from the Q */
          cmd = rflte_core_task_get_command();

          rf_dispatch_command( rflte_core_task_get_dispatch_handle(),
                               cmd );

           /* Processing of command complete. Free Memory */
           rflte_core_task_free_command( cmd );

        #endif /* FEATURE_GNSS_ONLY_NO_WWAN */
      } /* while ( rflte_core_get_command_num() != 0 ) */
    }/*else if ( sigs & RFLTE_CORE_TASK_CMD_Q_SIG )*/
  } /* for(;;) */

  /* Exit iC Task */
  return;
} /* rflte_core_task */

/*! @} */


/*! @} */
