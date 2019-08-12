/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Q M I _ M O D E M _ T A S K _ S V C . C

DESCRIPTION
  QMI Modem Task services provided to its modules. Contained herein are the
  functions needed for signal notification to QMI modem task, accessing NV
  items and sending commands to be processed in QMI modem task context


EXTERNALIZED FUNCTIONS
  qmi_enable_sig()
    Enable a specific signal for QMI task to process.

  qmi_disable_sig()
    Disable a specific signal from QMI task processing.
 
  qmi_set_signal()
    Sets a signal for the QMI_MODEM task 

  qmi_clear_signal()
    Clear a signal for the QMI_MODEM task.

  qmi_set_sig_handler()
    Sets signal handler for specific signal processed in QMI Modem task.

  qmi_get_cmd_data_buf()
    Allocs a new command buffer to be used by caller.

  qmi_free_cmd_data_buf()
    Frees the QMI command buffer.

  qmi_send_cmd()
    Post a command to be handled in QMI task.

  qmi_set_cmd_handler()
    Sets command handler for command handled in QMI Modem task.

  qmi_modem_task_wait()
    Make QMI Modem task wait for requested signals.

  qmi_get_nv_item()
    Retrieve a NV item

  qmi_put_nv_item()
    Write an item to the NV

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the QMI services can be used unless MC has created the QMI task
  and qmi_modem_task() has been invoked.

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/qmi_modem_task_svc.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/25/11    rk     Created module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#include "nv.h"
#include "dog_hb_rex.h"
#include "rex.h"
#include "qmi_modem_task_svc.h"
#include "qmi_modem_taski.h"
#include "qmi_modem_task_cmd.h"
#include "modem_mem.h"
#include "amssassert.h"
#include "ds_Utils_DebugMsg.h"  
#include "rcinit_rex.h"

/*===========================================================================

                  GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

nv_item_type         qmi_nv_item;
nv_stat_enum_type    qmi_nv_status;

/*===========================================================================

                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
  Command item to NV.
---------------------------------------------------------------------------*/
static nv_cmd_type qmi_nv_cmd_buf;

/*===========================================================================

                  EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
extern rex_tcb_type  * rex_qmi_modem_tcb;
#endif


/*===========================================================================
FUNCTION QMI_ENABLE_SIG()

DESCRIPTION
  This function enables the specified signal by adding it to the mask
  which the QMI modem task uses for rex_wait().
 
PARAMETERS 
  sig   : Signal to be enabled.
 
DEPENDENCIES
  Signal handler must be set in advance. Otherwise signal handling is 
  defaulted (printing an err msg.)
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_enable_sig
(
  qmi_sig_enum_type sig              /* Signal to be enabled               */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( qmii_sig_handler[sig].sig_handler == qmi_default_sig_handler )
  {
    LOG_MSG_ERROR_1("qmi_enable_sig(): "
                    "Signal %d enabled without a handler", sig );
  }

  QMI_ENTER_CRIT_SECTION(&global_qmi_crit_section);
  qmii_enabled_sig_mask |= ( (rex_sigs_type) 1 << (rex_sigs_type) sig );
  QMI_LEAVE_CRIT_SECTION(&global_qmi_crit_section);

  QMI_SET_SIGNAL( QMI_SIG_MASK_CHANGE_SIGNAL );
} /* qmi_enable_sig() */

/*===========================================================================
FUNCTION QMI_DISABLE_SIG()

DESCRIPTION
  This function disables a signal handling by removing it from the
  currently enabled signal mask.
 
PARAMETERS 
  sig   : Signal to be disabled.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_disable_sig
(
  qmi_sig_enum_type sig                       /* Signal to be disabled     */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QMI_ENTER_CRIT_SECTION(&global_qmi_crit_section);
  qmii_enabled_sig_mask &= ~( (rex_sigs_type) 1 << (rex_sigs_type) sig );
  QMI_LEAVE_CRIT_SECTION(&global_qmi_crit_section);

} /* qmi_disable_sig() */

/*===========================================================================

FUNCTION QMI_SET_SIGNAL()

DESCRIPTION 
  Sets a signal for the QMI_MODEM task and This function notifies the scheduler
  that there is processing to be performed in QMI_MODEM task. 
 
PARAMETERS 
  sig   : Signal to be set.
 
DEPENDENCIES
  Signal handler must be set in advance. Otherwise signal handling is 
  defaulted (printing an err msg.)
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qmi_set_signal
(
  qmi_sig_enum_type sig              /* Signal to be enabled               */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( qmii_sig_handler[sig].sig_handler == qmi_default_sig_handler )
  {
    LOG_MSG_ERROR_1("qmi_set_signal(): "
                    "Signal %d set without a handler", sig );
  }

  QMI_SET_SIGNAL( sig );
} /* qmi_set_signal() */

/*===========================================================================
FUNCTION QMI_CLEAR_SIGNAL()

DESCRIPTION
  Clear a signal for the QMI_MODEM task.
  
PARAMETERS 
  sig   : Signal to be cleared.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_clear_signal
(
  qmi_sig_enum_type sig                       /* Signal to be disabled     */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QMI_CLR_SIGNAL( sig );
} /* qmi_disable_sig() */

/*===========================================================================
FUNCTION QMI_SET_SIG_HANDLER()

DESCRIPTION
  This function sets a handler for the specified signal.
 
  If the handler is NULL, processing of the signal is defaulted. If the 
  signal handler is already set, then it is updated to use the new signal
  handler.
 
PARAMETERS 
  sig           : Signal for which the handler is to be set.
  sig_handler   : Callback function for handling signal enable.
  user_data_ptr : User data that would be used while the signal
                  handler is called.user data for signal handling 
                  can be set only once while setting the signal handler.
DEPENDENCIES
  None.

RETURN VALUE
  Previously registered signal handler, if any was set. NULL otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
qmi_sig_handler_type qmi_set_sig_handler
(
  qmi_sig_enum_type sig,             /* Signal assocaited with handler     */
  qmi_sig_handler_type sig_handler,  /* Signal handler function pointer    */
  void *user_data_ptr                /* Param to be passed to the handler  */
)
{
  qmi_sig_handler_type old_handler;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  old_handler = qmii_sig_handler[sig].sig_handler;

  qmii_sig_handler[sig].sig_handler = ( sig_handler != NULL) ?
                                        sig_handler :
                                        qmi_default_sig_handler;

  qmii_sig_handler[sig].user_data_ptr = user_data_ptr;

  return old_handler;
} /* qmi_set_sig_handler() */

/*===========================================================================
FUNCTION QMI_GET_CMD_DATA_BUF()

DESCRIPTION
  Allocates a new command buffer to be used by caller to post commands to
  QMI_MODEM task.
 
PARAMETERS 
  None.
    
DEPENDENCIES
  None.

RETURN VALUE
  Ptr to new command data buffer.

SIDE EFFECTS
  None.
===========================================================================*/
qmi_cmd_data_buf_type * qmi_get_cmd_data_buf
(
  void
)
{
  qmi_cmd_data_buf_type *new_cmd_ptr = NULL;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  new_cmd_ptr = (qmi_cmd_data_buf_type *) 
                modem_mem_alloc(sizeof(qmi_cmd_data_buf_type), 
                                MODEM_MEM_CLIENT_QMI_CRIT );

  if(new_cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_0("qmi_get_cmd_data_buf(): "
                    "QMI cmd data mem alloc error!!");
  }

  return new_cmd_ptr;

} /* qmi_get_cmd_data_buf */

/*===========================================================================
FUNCTION QMI_FREE_CMD_DATA_BUF()

DESCRIPTION
  Frees QMI Modem task command buffer.
 
PARAMETERS 
  None.
 
DEPENDENCIES
  Command buffer should have been allocated by a prior call to
  qmi_get_cmd_data_buf().

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_free_cmd_data_buf
(
  void *cmd_ptr                  /* Command buffer to be freed             */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cmd_ptr == NULL)
  {
    LOG_MSG_ERROR_0("NULL pointer passed to qmi_free_cmd_data_buf");
    return;
  }

  modem_mem_free(cmd_ptr, MODEM_MEM_CLIENT_QMI_CRIT);
  cmd_ptr = NULL;
  return;

} /* qmi_free_cmd_data_buf */

/*===========================================================================
FUNCTION QMI_SEND_CMD()
  Send command to be processed in QMI_MODEM task.
 
DESCRIPTION
  This method is used to send a command to the QMI_MODEM task for processing. 
  The command handler registered with the command being sent is executed
  when the QMI_MODEM task is run the next time. The QMI_MODEM task is serialized 
  through a command Q, so any pending commands in the QMI_MODEM task would be
  executed before the posted command. 

PARAMETERS
  cmd           - Command to send to QMI MODEM task.
  user_data_ptr - User data sent with the command. The 
                  command handler would be called with this user data.
RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void qmi_send_cmd
(
  qmi_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
)
{
  qmii_cmd_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get a cmd buffer from the free queue, call error routine if no links
    available, else copy buffer to cmd queue and signal QMI Modem task
  -------------------------------------------------------------------------*/
  if( ( cmd_ptr = ( qmii_cmd_type * ) q_get( &qmii_cmd_free_q ) ) == NULL )
  {
    ERR_FATAL( "QMI Modem task cmd buffers unavailable", 0, 0, 0 );
  }
  else
  {
    cmd_ptr->cmd = cmd;
    cmd_ptr->user_data_ptr = user_data_ptr;

    /*-----------------------------------------------------------------------
      Making the operation atomic so that race conditions do not occur
    -----------------------------------------------------------------------*/
    QMI_ENTER_CRIT_SECTION(&global_qmi_crit_section);
    q_put( &qmii_cmd_q, &cmd_ptr->link );
    QMI_SET_SIGNAL( QMI_CMD_Q_SIGNAL );
    QMI_LEAVE_CRIT_SECTION(&global_qmi_crit_section);
  }
} /* qmi_send_cmd() */

/*===========================================================================
FUNCTION QMI_SET_CMD_HANDLER() 
 
DESCRIPTION
  This function sets a handler for the specified cmd type.  If a NULL cmd
  handler is passed, then the command handling is defaulted (see 
  qmi_default_cmd_handler). The command handler can be updated on the 
  fly by calling this funciton again.

PARAMETERS 
  cmd         - Command for which we want to set handler.
  cmd_handler - Callback function to be registered as handler.
 
DEPENDENCIES
  None.

RETURN VALUE
  Previously registered cmd handler

SIDE EFFECTS
  None.
===========================================================================*/
qmi_cmd_handler_type qmi_set_cmd_handler
(
  qmi_cmd_enum_type cmd,              /* Command name                      */
  qmi_cmd_handler_type cmd_handler    /* Cmd handler function pointer      */
)
{
  qmi_cmd_handler_type old_handler;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  old_handler = qmii_cmd_handler[cmd];

  qmii_cmd_handler[cmd] = ( cmd_handler != NULL) ?
                          cmd_handler :
                          qmi_default_cmd_handler;

  return old_handler;
} /* qmi_set_cmd_handler() */

/*===========================================================================
FUNCTION QMI_MODEM_TASK_WAIT()

DESCRIPTION
  This function performs the QMI Modem Task waiting function. This function 
  will not return until at least one signal from the requested mask is set.
  The watchdog is also kicked at least once.
 
PARAMETERS 
  requested_mask : QMI SIG mask.

DEPENDENCIES
  None.

RETURN VALUE
  The signal mask actually set out of the requested mask which terminated
  the wait.

SIDE EFFECTS
  All the set signals in the requested mask are cleared.
===========================================================================*/
rex_sigs_type qmi_modem_task_wait
(
  rex_sigs_type requested_mask             /* mask of signals to wait for */
)
{
  rex_sigs_type set_sigs;                         /* Currently set signals */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    First check to see if one of the signals in the mask is already set.  If
    yes, then clear those signals and return. Otherwise enter into the wait
    only if no requested signal events have occurred
  -------------------------------------------------------------------------*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  set_sigs = rex_get_sigs( rex_qmi_modem_tcb );
#else
  set_sigs = rex_get_sigs( &qmi_modem_tcb );
#endif

  while ( (set_sigs & requested_mask) == 0 )
  {
    set_sigs = rex_wait( requested_mask |
                         (  1 << (rex_sigs_type)QMI_DOG_HB_REPORT_SIGNAL ) );

    /*-----------------------------------------------------------------------
      If dog heart beat is received, ack the beat..
    -----------------------------------------------------------------------*/
    if( set_sigs & ( 1 << (rex_sigs_type)QMI_DOG_HB_REPORT_SIGNAL ) )
    {
#ifdef FEATURE_MODEM_RCINIT_PHASE2
      (void)rex_clr_sigs( rex_qmi_modem_tcb,
                    (  1 << (rex_sigs_type)QMI_DOG_HB_REPORT_SIGNAL ) );
#else
      (void)rex_clr_sigs( &qmi_modem_tcb,
                    (  1 << (rex_sigs_type)QMI_DOG_HB_REPORT_SIGNAL ) );
#endif
      dog_hb_report( qmi_dog_rpt_id );
    }
  }

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  (void)rex_clr_sigs( rex_qmi_modem_tcb, ( set_sigs & requested_mask ) );
#else
  (void)rex_clr_sigs( &qmi_modem_tcb, ( set_sigs & requested_mask ) );
#endif
  return ( set_sigs & requested_mask );
} /* qmi_modem_task_wait() */

/*===========================================================================
FUNCTION QMI_GET_NV_ITEM

DESCRIPTION
  This function retrieves the specified item from NV.
 
PARAMETERS 
  item_code : Item to get.
  data_ptr  : Pointer where to put the item.

RETURN VALUE
  Status returned from the NV read request.  An ERR is logged if status is
  other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active
 
DEPENDENCIES
  The NV task has been started and is running.
  This functions is Non-reentrant.

SIDE EFFECTS
  While this function is running all other QMI modem task activities are
  suspended except for watchdog kicking, and until the NV item is
  read in.
===========================================================================*/
nv_stat_enum_type qmi_get_nv_item
(
  nv_items_enum_type  item_code,       /* Item to get                      */
  nv_item_type        *data_ptr        /* Pointer where to put the item    */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  ASSERT( rex_self() == rex_qmi_modem_tcb );
#else
  ASSERT( rex_self() == &qmi_modem_tcb );
#endif

  /*-------------------------------------------------------------------------
    Prepare command buffer to get the item from NV.
  -------------------------------------------------------------------------*/
  qmi_nv_cmd_buf.cmd        = NV_READ_F;             /* Read request        */
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  qmi_nv_cmd_buf.tcb_ptr    = rex_qmi_modem_tcb;     /* Notify this task    */
#else
  qmi_nv_cmd_buf.tcb_ptr    = &qmi_modem_tcb;              /* Notify this task    */
#endif
  qmi_nv_cmd_buf.sigs       = 1 << (rex_sigs_type)QMI_NV_CMD_SIGNAL; /* With this signal    */
  qmi_nv_cmd_buf.done_q_ptr = NULL;             /* Do not enqueue when done */
  qmi_nv_cmd_buf.item       = item_code;             /* Item to get         */
  qmi_nv_cmd_buf.data_ptr   = data_ptr;              /* Where to return it  */

  /*-------------------------------------------------------------------------
   Clear signal, issue the command, and wait for the response.
  -------------------------------------------------------------------------*/
  QMI_CLR_SIGNAL( QMI_NV_CMD_SIGNAL );             /* Clear signal for NV  */
  nv_cmd( &qmi_nv_cmd_buf );                       /* Issue the request    */

  (void)qmi_modem_task_wait((rex_sigs_type) 1 <<
                       (rex_sigs_type)QMI_NV_CMD_SIGNAL );/* Wait for completion*/

  if( qmi_nv_cmd_buf.status != NV_DONE_S &&
      qmi_nv_cmd_buf.status != NV_NOTACTIVE_S )
  {
    ERR( "NV Read Failed Item %d Code %d",
         qmi_nv_cmd_buf.item, qmi_nv_cmd_buf.status, 0 );
  }
  return( qmi_nv_cmd_buf.status );
} /* qmi_get_nv_item() */

/*===========================================================================
FUNCTION QMI_PUT_NV_ITEM

DESCRIPTION
  Write an item to NV memory.  Wait until write is completed.
 
PARAMETERS 
  item_code : Item to set.
  data_ptr  : Pointer to data for item.

RETURN VALUE
  Status returned from the NV read request.  An ERR is logged if status is
  other than:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active
 
DEPENDENCIES
  This function can only be called from QMI Modem task.  Also it is not
  reentrant. Calls rex_wait() and doesn't exit until processing is finished.

SIDE EFFECTS
  While this function is running all other QMI Modem task activities are
  suspended except for watchdog kicking, and until the NV item is
  wrote down.
===========================================================================*/
nv_stat_enum_type qmi_put_nv_item
(
  nv_items_enum_type item_code,                              /* which item */
  nv_item_type *data_ptr                       /* pointer to data for item */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  ASSERT( rex_self() == rex_qmi_modem_tcb );
#else
  ASSERT( rex_self() == &qmi_modem_tcb );
#endif

  /*-------------------------------------------------------------------------
    Prepare command buffer to write the item to NV.
    TODO:NV team to be notified to change the nv_cmd_buf.sigs to an array
  -------------------------------------------------------------------------*/
  qmi_nv_cmd_buf.cmd        = NV_WRITE_F;           /* Write request       */
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  qmi_nv_cmd_buf.tcb_ptr    = rex_qmi_modem_tcb;    /* Notify this task    */
#else
  qmi_nv_cmd_buf.tcb_ptr    = &qmi_modem_tcb;             /* Notify this task    */
#endif
  qmi_nv_cmd_buf.sigs       = 1 << (rex_sigs_type)QMI_NV_CMD_SIGNAL; /* With this signal    */
  qmi_nv_cmd_buf.done_q_ptr = NULL;            /* Do not enqueue when done */
  qmi_nv_cmd_buf.item       = item_code;            /* Item to put         */
  qmi_nv_cmd_buf.data_ptr   = data_ptr;             /* Data to write       */


  /*-------------------------------------------------------------------------
   Clear signal, issue the command, and wait for the response.
  -------------------------------------------------------------------------*/
  QMI_CLR_SIGNAL( QMI_NV_CMD_SIGNAL );
  nv_cmd( &qmi_nv_cmd_buf );

  (void)qmi_modem_task_wait( (rex_sigs_type) 1 <<
                 (rex_sigs_type)QMI_NV_CMD_SIGNAL ); /* Wait for completion*/

  if( qmi_nv_cmd_buf.status != NV_DONE_S )
  {
    ERR( "NV Write Failed Item %d Code %d",
         qmi_nv_cmd_buf.item, qmi_nv_cmd_buf.status, 0 );
  }

  return( qmi_nv_cmd_buf.status );
} /* qmi_put_nv_item() */

