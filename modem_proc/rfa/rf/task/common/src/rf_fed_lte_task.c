/*! 
  @file
  rf_fed_lte_task.c
 
  @brief
  
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/src/rf_fed_lte_task.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/26/12   kab     Initial Revision

==============================================================================*/

#include "comdef.h"
#include "rf_fed_lte_task.h"
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
extern rex_tcb_type rf_fed_lte_tcb;

/*----------------------------------------------------------------------------*/
/*! Variable to store the global data for RF_FED LTE task */
static rf_fed_lte_task_data_type rf_fed_lte_task_data;

/*============================================================================*/
/*!
  @name REX-MSGR

  @brief
  Functions Pertaining to initialization and de-initialization of REX-MSGR
*/
/*! @{ */

void rf_fed_lte_deinit(void)
{
  msgr_client_delete(&(rf_fed_lte_task_data.rf_fed_lte_msgr.client));
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FED LTE Task Rex MSGR interface initialization

  @details
  This funciton creates the message router client on the FED LTE TCB, registers 
  for all the MSGR commands that FED LTE task wishes to recieve.
 
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
rf_fed_lte_rex_msgr_init
(
  rex_tcb_type *p_rf_fed_lte_tcb,
  rex_sigs_type rf_rex_sig
)
{
  boolean ret_val =  TRUE;
  errno_enum_type msgr_status;

  /* Create FED LTE Task message router client for receiving messages
  from  SW entities */
  msgr_status = msgr_client_create(&(rf_fed_lte_task_data.rf_fed_lte_msgr.client));

  if ( msgr_status != E_SUCCESS )
  {
    RF_MSG( RF_ERROR, "rf_fed_lte_rex_msgr_init : "
            "Unable to create RF message router client ");  
    ret_val = FALSE;
  }

  if (msgr_status == E_SUCCESS)
  {
    msgr_status = msgr_client_add_rex_q(
                                      &(rf_fed_lte_task_data.rf_fed_lte_msgr.client),
                                      p_rf_fed_lte_tcb,
                                      rf_rex_sig, 
                                      &(rf_fed_lte_task_data.rf_fed_lte_cmd_q.cmd_q),
                                      &(rf_fed_lte_task_data.rf_fed_lte_cmd_q.free_q),
                                      sizeof(rf_fed_lte_cmd_rex_hdr_type),
                                      sizeof(q_link_type),
                                      sizeof(rf_fed_lte_cmd_type),
                                      &(rf_fed_lte_task_data.rf_fed_lte_msgr.mb)
                                      );

    if (msgr_status != E_SUCCESS)
    {
      RF_MSG( RF_ERROR,"rf_fed_lte_rex_msgr_init : "
              "Unable to add RF REX MSGR client ");  
      ret_val = FALSE;
    }
  }

  /* Return Status */
  return ret_val;

} /* rf_fed_lte_rex_msgr_init */

/*! @} */

/*============================================================================*/
/*!
  @name Command Queue

  @brief
  Functions Pertaining to Command Queue for FED LTE usage
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize FED LTE TASK CMD queue.

  @details
  This function initializes the FED LTE task Command Queue Structures.
*/
static void
rf_fed_lte_create_command_q
(
  void
)
{
  uint32 loop_cnt;

  /* Initialize the Command Queue, and the Command Free Queue */
  (void) q_init( &(rf_fed_lte_task_data.rf_fed_lte_cmd_q.cmd_q) );
  (void) q_init( &(rf_fed_lte_task_data.rf_fed_lte_cmd_q.free_q) );

  /* Link the Command Queue Items onto the Command Free Queue */
  for ( loop_cnt = 0; 
        loop_cnt < ARR_SIZE((rf_fed_lte_task_data.rf_fed_lte_cmd_q.q_items)); 
        loop_cnt++)
  {
    q_put( &(rf_fed_lte_task_data.rf_fed_lte_cmd_q.free_q),
           q_link( &(rf_fed_lte_task_data.rf_fed_lte_cmd_q.q_items[loop_cnt]), 
             &(rf_fed_lte_task_data.rf_fed_lte_cmd_q.q_items[loop_cnt].rex_hdr.link) )
          );
  }

} /* rf_fed_lte_create_command_q */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the number of commands on FED LTE TASK cmd Queue
 
  @return
  Total the number of commands on the cmd Queue
*/
static int
rf_fed_lte_get_command_num
(
  void
)
{

  return q_cnt( &(rf_fed_lte_task_data.rf_fed_lte_cmd_q.cmd_q) );

} /* rf_fed_lte_get_command_num */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Retreive the command from  FED LTE queue.

  @details
  This function will retreive the command from FED LTE Task queue, and then 
  removes the command from Queue.

  @return
  Address of the command retreived from the command queue.
*/
static rf_fed_lte_cmd_type* 
rf_fed_lte_get_command
(
  void
)
{
   rf_fed_lte_cmd_type * item;

   /* Retrieve next commmand from command queue */
   item = (rf_fed_lte_cmd_type *) q_get(&(rf_fed_lte_task_data.rf_fed_lte_cmd_q.cmd_q));

   if ( item != NULL )
   {
     item->status = RF_FED_LTE_CMD_PROCESSING;
   }
   else
   {
     ERR_FATAL( "rf_fed_lte_get_command : Signal for the"
                " queue was set, but no cmd in Q",0 ,0 ,0 ); 
   }

   return item;

} /* rf_fed_lte_get_command */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Free the command buffer on FED LTE Task queue.

  @details
  This function removes an already processed command from FED LTE Task queue.
  The memory is returned to free queue.
  
  @param cmd
  The address of the command to be freed from FED LTE task queue.
*/
static void
rf_fed_lte_free_command
(
  rf_fed_lte_cmd_type* cmd
)
{

  if ( cmd == NULL )
  {
    RF_MSG( RF_ERROR, "rf_fed_lte_free_command: Invalid cmd in Q" );
    return;
  }

  memset( cmd, 0, sizeof(rf_fed_lte_cmd_type) );

  /* Mark item as no longer valid */
  cmd->status = RF_FED_LTE_CMD_BUF_EMPTY;

 ( void )q_link( cmd, &(cmd->rex_hdr.link) );

  /* Add the item to the free queue */
  q_put( &(rf_fed_lte_task_data.rf_fed_lte_cmd_q.free_q), 
         &(cmd->rex_hdr.link) );

  return;

} /* rf_fed_lte_free_command */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Rex Data
 
  @details
  In order perform REX-MSGR initialization for FED LTE task, REX data is
  required. This interface return the REX data to be used by RF dispatch to
  perform REX-MSGR registration
 
  @param rex_data_ptr
  Pointer to the container where REX data is populated
 
  @return
  Flag indicating the validity of the populated Rex data
*/
static boolean
rf_fed_lte_get_rex_data
(
  rf_dispatch_rex_data_type *rex_data_ptr
)
{
  /* NULL Pointer check */
  if ( rex_data_ptr == NULL )
  {
    RF_MSG( RF_ERROR, "rf_fed_lte_get_rex_data: NULL container" );
    return FALSE;
  } /* if ( rex_data_ptr == NULL ) */

  rex_data_ptr->rex_tcb = &rf_fed_lte_tcb;
  rex_data_ptr->rex_sig = RF_FED_LTE_TASK_CMD_Q_SIG;
  rex_data_ptr->cmd_q = &(rf_fed_lte_task_data.rf_fed_lte_cmd_q.cmd_q);
  rex_data_ptr->free_q = &(rf_fed_lte_task_data.rf_fed_lte_cmd_q.free_q);
  rex_data_ptr->header_size = sizeof(rf_fed_lte_cmd_rex_hdr_type);
  rex_data_ptr->q_link_size = sizeof(q_link_type);
  rex_data_ptr->command_size = sizeof(rf_fed_lte_cmd_type);

  return TRUE;
} /* rf_fed_lte_get_rex_data */

/*! @} */


/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Initialize Dispatcher for FED LTE Task
 
  @details
  This function will initialize the Dispatch table containing the dispatch
  function for each REX based or MSGR based commands.
 
  This funciton also creates the message router client on the FED LTE TCB and
  registers for all the MSGR commands that FED LTE task wishes to recieve.
 
  @return
  Flag indicating if the dispatcher is successfully initialized or not.
*/
static boolean
rf_fed_lte_init_dispatcher
(
  void
)
{
  boolean rex_msgr_init_status = FALSE; /* Status for Rex MSGR init */
  boolean ret_val = TRUE;

  /* Initialize REX MSGR Interface */
  rex_msgr_init_status = rf_fed_lte_rex_msgr_init( &rf_fed_lte_tcb,
                                                 RF_FED_LTE_TASK_CMD_Q_SIG );

  /* Check if Rex-MSGR init is successful */
  if ( rex_msgr_init_status == FALSE )
  {
    RF_MSG( RF_ERROR, "rf_fed_lte_init_dispatcher: REX-MSGR Failed" );
    ret_val = FALSE;    
  }

  return ret_val;

} /* rf_fed_lte_init_dispatcher */

/*! @} */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Set Signal to start FED LTE task
 
  @details
  This function will be used to set signal to start FED LTE SCHEDULE Task. This function
  will be called during RFM init.
*/
void
rf_fed_lte_signal_to_start_task
(
  void
)
{
  /* Signal RF iC Task to start */
  rex_set_sigs( &rf_fed_lte_tcb, RF_FED_LTE_TASK_START_SIG );

} /* rf_fed_lte_signal_to_start_task */

/*! @} */

/*============================================================================*/
/*!
  @name Query

  @brief
  Functions used for FED LTE parameter queries
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query to check if FED LTE Task is running
 
  @details
  This function will check the FED LTE data structure to return the running
  state of the task.
 
  @return
  Flag indicating if the FED LTE task is running or not.
*/
boolean
rf_fed_lte_is_task_running
(
  void
)
{
  return rf_fed_lte_task_data.housekeeping.is_task_running;
} /* rf_fed_lte_is_task_running */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get constant pointer to MSGR parameter used by FED LTE TASK
 
  @details
  In order receive MSGR messages by FED LTE task, these messages need to be
  registered with MSGR module. To indicate that the message is pertaining
  to FED LTE task, the same CLIENT and Mailbox info should be used during
  MSGR registration of commands. Thus this interface returns the pointer to 
  these parameter (Client and MB)
 
  @return
  Pointer to the MSGR data used by FED LTE task
*/
const rf_fed_lte_task_msgr_type*
rf_fed_lte_get_msgr_param
(
  void
)
{
  return &(rf_fed_lte_task_data.rf_fed_lte_msgr);
} /* rf_fed_lte_get_msgr_param */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query the Dispatch Handle
 
  @details
  This function return the dispatch handler to used used by the client for
  command registration and deregistration
 
  @return
  Pointer to FED LTE dispatch handle data
*/
rf_dispatch_handle_type *
rf_fed_lte_task_get_dispatch_handle
(
  void
)
{
  return &(rf_fed_lte_task_data.dispatch_handle);
} /* rf_fed_lte_task_get_dispatch_handle */

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
rf_fed_lte_extract_umid
(
  void *cmd_ptr,
  msgr_umid_type *umid
)
{
  rf_fed_lte_cmd_type *cmd;

  /* NULL Pointer check */
  if (( cmd_ptr == NULL ) || ( umid == NULL ) )
  {
    RF_MSG( RF_ERROR, "rf_fed_lte_extract_umid: NULL Data" );
    return FALSE;
  } /* if (( cmd_ptr == NULL ) || ( umid == NULL ) ) */

  /* Typecast */
  cmd = (rf_fed_lte_cmd_type*)cmd_ptr;

  /* Extract UMID */
  *umid = cmd->msgr_hdr.id;

  return TRUE;
} /* rf_fed_lte_extract_umid */

/*! @} */

/*============================================================================*/
/*!
  @name Task Spawn

  @brief
  Functions Pertaining to FED LTE task spawning
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FED LTE Task control.

  @details
  This is the Radio Frequency (RF) FED LTE task, created from 
  the Main Control task. After processing task start procedure, the task 
  enters an infiniten loop, awaiting commands received on its command queue.  
  Each command received is processed to completion before the next command is 
  processed. 

  @param parm
  Dummy variable which is ignored.
*/
void
rf_fed_lte_task
(
  dword parm
)
{
  rex_sigs_type sigs = 0;             
  rf_dispatch_init_handle_data_type init_data;

  /* Initialize Dispatcher */
  /* It is moved to here to mitigate race condition between
  init and msgs registration */
  rf_fed_lte_init_dispatcher();

  /* Required RCINIT handshake */
  rcinit_handshake_startup();

  /* Indicate that IC_TASK is not running yet */
  rf_fed_lte_task_data.housekeeping.is_task_running = FALSE;

  /* Initialize Dispatch handler */
  init_data.task_name = "RF_FED_LTE";
  init_data.task_uid = 0x85F78163; /* MD5(FED_LTE) */
  init_data.extract_cid_fp = rf_fed_lte_extract_umid;
  rf_fed_lte_get_rex_data( &(init_data.rex_data) );
  init_data.debug_mode = FALSE;
  rf_dispatch_init_handle( &(rf_fed_lte_task_data.dispatch_handle),
                            init_data );

  /* Wait for RF_FED_LTE_TASK_START_SIG signal to be set before starting
  the task. This signal will be set during RFM init. */
  //f();

  /* Spawn the task only if ic_task_data is not NULL */

  /* Initialize command queue for FED LTE */
  rf_fed_lte_create_command_q();

  /* Indicates that RF Task is now running.*/
  rf_fed_lte_task_data.housekeeping.is_task_running = TRUE;

  /* Loop forever handling commands */
  for(;;)
  {
    sigs = rex_wait( TASK_STOP_SIG |        /* Task shutdown   */
                     RF_FED_LTE_TASK_CMD_Q_SIG   /* Command queue signal */
                    );

    /* There is implicit priority given to signals. Only one signal is
    processed at a time. */
    if ( sigs & TASK_STOP_SIG )
    {
      (void) rex_clr_sigs(&rf_fed_lte_tcb, TASK_STOP_SIG);

      rf_fed_lte_deinit();

      rf_fed_lte_task_data.housekeeping.is_task_running = FALSE;
#ifndef FEATURE_MODEM_RCINIT
      tmc_task_stop();
#else
      rcinit_handshake_term();
#endif
      /* Task is stopped.  No need to run this function anymore. */
      break;
    }
    else if ( sigs & RF_FED_LTE_TASK_CMD_Q_SIG )
    {
      /* First, clear the signal */
      (void) rex_clr_sigs( &rf_fed_lte_tcb, RF_FED_LTE_TASK_CMD_Q_SIG );

      /* process commands in a loop to ensure that RF IC task does not "skip" 
      processing of any messages */
      while ( rf_fed_lte_get_command_num() != 0 )
      {
        /* Process RF commands in the queue */
        #ifndef FEATURE_GNSS_ONLY_NO_WWAN

          rf_fed_lte_cmd_type *cmd;

          /* Get command from the Q */
          cmd = rf_fed_lte_get_command();

          rf_dispatch_command( rf_fed_lte_task_get_dispatch_handle(),
                               cmd );

           /* Processing of command complete. Free Memory */
           rf_fed_lte_free_command( cmd );

        #endif /* FEATURE_GNSS_ONLY_NO_WWAN */
      } /* while ( rf_fed_lte_get_command_num() != 0 ) */
    }/*else if ( sigs & RF_FED_LTE_TASK_CMD_Q_SIG )*/
  } /* for(;;) */

  /* Exit iC Task */
  return;
} /* rf_fed_lte_task */

/*! @} */


/*! @} */

