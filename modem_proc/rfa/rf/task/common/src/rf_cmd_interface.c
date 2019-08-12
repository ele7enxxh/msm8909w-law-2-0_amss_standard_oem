/*!
  @file
  RF Command interface Implementation file.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/src/rf_cmd_interface.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/17/13   aro     Cleaned up unused time profiler code
11/21/12   zhw     Compiler warning fix 
10/05/12   av      Expanded rf task architecture to include more tasks. 
08/24/12   aro     Compiler warning fix
04/24/12   jfc     Use TASK_HANDLE instead of rex_self()
03/19/12   jfc     Replace rf_tcb with rex_self()
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
13/01/10   pl      update due to rf_cmd_type structure changes
08/26/09   mkv     Integrating REX MSGR Interworking into RFA2. 
                   Adding the rf_cmd_struct_type back into the .h file
                   Changed rf_cmd_id_type to uint8 and added rf_cmd_id_type to MSGR UMID style.
07/21/09   mkv     Added a function to return address of the command queues (REX MSGR Interworking).     
07/10/09   ans     Initial Release.

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "rfa_variation.h"
#include "comdef.h"
/*! @TODO rf_cap.h is required for MODEM LTE DEV offtarget features. Should be 
  removed once the custrf.h matures for LTE. */
#ifdef FEATURE_LTE
#include "rf_cap.h" 
#endif /*FEATURE_LTE*/



/* Feature RF Task should be used to encompass complete RF task related file.
   This is not meant to be used for other featurization. This is meant just to 
   provide compatibility with top leavel feature used in TMC and Task modules. */
#ifdef FEATURE_RF_TASK
#include "rex.h"
#include "task.h"
#include "msg.h"
#include "err.h"

#include "rf_task.h"
#include "rf_cmd_interface.h"
#include "rfcommon_time_profile.h"

#ifdef T_WINNT
#error code not present
#endif
/*===========================================================================
                        
                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/ 

/*!

  @brief
  Local rf_cmd storage.

  @details
  Local rf_cmd storage. Stores the RF queue data strucutre.
*/
static rf_cmd_struct_type rf_cmd[RF_MAX_TASK];

/*===========================================================================

                              FUNCTION DECLARATIONS

===========================================================================*/

/*!

  @brief
  Initialize RF CMD queue.

  @details
  This function initializes the RF Command Queue Structures.

  @retval void
*/
void rf_cmd_init( rf_task_num_type task_num )
{
  int loop_cnt;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize the Command Queue, and the Command Free Queue */

  (void) q_init( &rf_cmd[task_num].cmd_q );
  (void) q_init( &rf_cmd[task_num].free_q );

  /* Link the Command Queue Items onto the Command Free Queue */

  for ( loop_cnt = 0; loop_cnt < ARR_SIZE(rf_cmd[task_num].q_items); loop_cnt++)
  {
    q_put(  &(rf_cmd[task_num].free_q),
            q_link( &rf_cmd[task_num].q_items[loop_cnt], &(rf_cmd[task_num].q_items[loop_cnt].rex_hdr.link) )
          );
  }

} /* rf_cmd_init( ) */


/*===========================================================================*/
/*!
  @brief
  Initialize RF CMD queue.

  @details
  This function initializes the RF Command Queue Structures.

  @retval void
==========================================================================*/
rf_cmd_struct_type* rf_cmd_get_queue_address( rf_task_num_type task_num )
{


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* return the address of the Command Queue, and the Command Free Queue */
  return &rf_cmd[task_num];


} /* rf_cmd_init( ) */


/*!==========================================================================

  @brief
  Returns free buffer on RF command queue.

  @details
  This function returns the pointer of free buffer on RF command Queue. This 
  is used by calling module to send command to RF. If buffer has been requested 
  by a module, it is required that it calls rf_cmd_put function immediately.
  Calling module should set the command header and payload fields.

  @return
  Pointer to the RF command buffer.

  @retval NULL
  If command queue full, the function will return NULL.

  @retval Queue pointer
  If command has buffer available, it will return pointer to the buffer.

*/
rf_cmd_type* rf_cmd_get_buf( rf_task_num_type task_num )
{
   rf_cmd_type *item;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   /* Retrieve next commmand from command queue */
   item = (rf_cmd_type *) q_get( &rf_cmd[task_num].free_q );
   
   if ( item != NULL )
   {
      /* If a command was available, return a pointer to the command
        structure */
      item->status = RF_CMD_BUF_ALLOCATED;
      return item;
   }
   else
   {
      /* No command buffer is available! */
      MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "RF command buffer full. Cannot accept any new RF commands" );
      return NULL;
   }
} /* rf_cmd_get_buf () */

/*===========================================================================*/

/*!

  @brief
  Adds the command to RF queue.

  @details
  This function adds the command to RF queue and signals RF task. The command is 
  added on the memory allocated by rf_cmd_get_buf().

  @param cmd
  The address of the command to be added to RF task queue.

*/
void rf_cmd_put( rf_task_num_type task_num, rf_cmd_type* cmd )
{
  rf_time_tick_type rf_tick;

  if ( cmd == NULL )
  {
     MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "Invalid command passed to RF CMD Queue" );
     return;
  }

  rf_tick = rf_time_get_tick();

  /* Iniliatize the link of the pointer */
  (void) q_link(cmd, &(cmd->rex_hdr.link) );

  /* Add command to the queue */
  q_put( &(rf_cmd[task_num].cmd_q), &(cmd->rex_hdr.link) );

  cmd->status = RF_CMD_QUEUED;

  cmd->alloc_time = rf_tick;

  /* Signal the task about availability of the command */
  if ( task_num == RF_TASK_MAIN )
  {
     (void) rex_set_sigs(TASK_HANDLE(rf), RF_CMD_Q_SIG);
  }
  if ( task_num == RF_TASK_2 )
  {
     (void) rex_set_sigs(TASK_HANDLE(rf_2), RF_CMD_Q_SIG);
  }

  return;
} /* rf_cmd_put () */

/*===========================================================================*/

/*!

  @brief
  Retreives the command from  RF queue.

  @details
  Retreives the command from  RF queue. Removes the command from RF Queue.

  @return
  Address of the command retreived from the command queue.

  @retval NULL
  No command present on command Queue. Command Queue empty.

*/
rf_cmd_type* rf_cmd_get( rf_task_num_type task_num )
{
   rf_cmd_type * item;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   /* Retrieve next commmand from command queue */
   item = (rf_cmd_type *) q_get( &rf_cmd[task_num].cmd_q );

   if ( item != NULL )
   {
      item->status = RF_CMD_PROCESSING;
   }
   else
   {
      ERR_FATAL("Signal for the queue was set, but the command not present in the queue.",0 ,0 ,0 ); 
   }

   return item;

} /* rf_cmd_get () */

/*===========================================================================*/

/*!

  @brief
  Free the command buffer on RF queue.

  @details
  This function removes an already processed command from RF queue. The memory is returned to 
  free queue.

  @param cmd
  The address of the command to be freed from RF task queue.

  @retval void
*/
void rf_cmd_free (rf_task_num_type task_num , rf_cmd_type* cmd )
{

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   if ( cmd == NULL )
   {
      MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "Invalid command passed to RF CMD Queue" );
      return;
   }


   memset( cmd, 0, sizeof(rf_cmd_type) );

   /* Mark item as no longer valid */
   cmd->status = RF_CMD_BUF_EMPTY;


  ( void )q_link( cmd, &(cmd->rex_hdr.link) );

   /* Add the item to the free queue */
   q_put( &(rf_cmd[task_num].free_q), &(cmd->rex_hdr.link) );

   return;
} /* rf_cmd_free () */


/*===========================================================================*/
/*!

  @brief
  Returns the number of commands on RF cmd Queue

  @return
  Returns the number of commands on RF cmd Queue

*/
int rf_num_of_cmds_in_q( rf_task_num_type task_num )
{
   return q_cnt( &(rf_cmd[task_num].cmd_q) );
}

#endif /* FEATURE_RF_TASK */
/*! @} */

