/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              Q M I _ M M C P _T A S K I . C

DESCRIPTION
  QMI Mmcp Task internal API file. Contains APIs and definitions used
  internal to QMI Mmcp Task processing. 

EXTERNALIZED FUNCTIONS
  qmi_mmode_default_sig_handler()
    Implicit handler for signals processed by QMI Task.

  qmi_mmode_default_cmd_handler()
    Implicit handler for commands processed by QMI Task.

  qmi_mmodei_powerup()
    Cold start called before QMI task is started.

  qmi_mmodei_process_cmd()
    Processes command signals in QMI Task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the QMI APIs can be used unless QMI Mmcp task is created and 
  started.

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_mmode_taski.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/08/12     sk     Created module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#include "dog.h"
#include "qmi_mmode_taski.h"
#include "qmi_mmode_task_svc.h"
#include "qmi_mmode_task_cmd.h"
#include "amssassert.h"

#include "msgr_types.h"
#include "msgr.h"
#include "msgr_rex.h"
#include "qmi_nas_internal.h"
#include "qmi_voice_msgr_if.h"

/*===========================================================================

                  GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
q_type qmi_mmodei_cmd_q;
q_type qmi_mmodei_cmd_free_q;

q_type qmi_mmodei_msgr_q;

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers
---------------------------------------------------------------------------*/
qmi_mmodei_sig_handler_info_type qmi_mmodei_sig_handler[QMI_MMODE_MAX_SIGNALS];
qmi_mmode_cmd_handler_type qmi_mmodei_cmd_handler[QMI_MMODE_MAX_DEFINED_CMD_TYPES];

/*--------------------------------------------------------------------------
  Protocol Stack Task Command links and queues
---------------------------------------------------------------------------*/
qmi_mmodei_cmd_type  qmi_mmodei_cmd_buf[QMI_MMODEI_CMD_BUF_CNT];


/*--------------------------------------------------------------------------
  Message Router client object
---------------------------------------------------------------------------*/
msgr_client_t  qmi_mmode_msgr_client;

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled signals
---------------------------------------------------------------------------*/
rex_sigs_type qmi_mmodei_enabled_sig_mask;

/*--------------------------------------------------------------------------
  QMI's own critical section
---------------------------------------------------------------------------*/
qmi_mmode_crit_sect_type global_qmi_mmode_crit_section;

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION QMI_MMODE_DEFAULT_SIG_HANDLER()

DESCRIPTION
  This function is registered as the default signal handler for all the
  signals which do not have a registered handler. It merely prints a warning
  message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean qmi_mmode_default_sig_handler
(
  qmi_mmode_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QM_MSG_FATAL_1("qmi_mmode_default_sig_handler():  Signal %d, no handler registered", sig);

  /*-------------------------------------------------------------------------
    Return TRUE since we don't want this signal to be enqueued again.
  -------------------------------------------------------------------------*/
  return TRUE;
} /* qmi_mmode_default_sig_handler() */

/*===========================================================================

FUNCTION QMI_MMODE_DEFAULT_CMD_HANDLER()

DESCRIPTION
  This function is registered as the default command handler for all the
  commands which do not have a registered handler. It merely prints a warning
  message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void qmi_mmode_default_cmd_handler
(
  qmi_mmode_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  QM_MSG_FATAL_1("qmi_mmode_default_cmd_handler():  Command %d, no handler registered", cmd);
} /* qmi_mmode_default_cmd_handler() */

/*===========================================================================
FUNCTION QMI_MMODE_SVC_POWERUP()

DESCRIPTION
  Powerup (coldstart) initialization function for the QMI MMODE task. 
  
  This function performs initialization before QMI MMODE task is started.
  Note that initializations internal to QMI should be performed before
  initializing other components that run in QMI MMODE task context.

DEPENDENCIES
  This function must be called only once at system powerup.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_mmodei_powerup
(
  void
)
{
  int i;

  /*------------------------------------------------------------------------
    Perform the power up initialization for all other modules that run in
    QMI context
  -------------------------------------------------------------------------*/

  QMI_MMODE_INIT_CRIT_SECTION(&global_qmi_mmode_crit_section);

  /*-------------------------------------------------------------------------
    Initialize the command queues and link items onto the command free queue
  -------------------------------------------------------------------------*/
  (void) q_init( &qmi_mmodei_cmd_q );
  (void) q_init( &qmi_mmodei_cmd_free_q );

  for( i = 0; i < QMI_MMODEI_CMD_BUF_CNT; i++ )
  {
    q_put( &qmi_mmodei_cmd_free_q, q_link( &qmi_mmodei_cmd_buf[i], &qmi_mmodei_cmd_buf[i].link) );
  }

  /*------------------------------------------------------------------------
    Initialize all the signal and cmd handlers
  -------------------------------------------------------------------------*/
  for( i = 0; i < (int) QMI_MMODE_MAX_SIGNALS; i++ )
  {
    qmi_mmodei_sig_handler[i].sig_handler = qmi_mmode_default_sig_handler;
    qmi_mmodei_sig_handler[i].user_data_ptr = NULL;
  }

  for( i = 0; i < (int) QMI_MMODE_MAX_DEFINED_CMD_TYPES; i++ )
  {
    qmi_mmodei_cmd_handler[i] = qmi_mmode_default_cmd_handler;
  }

  /*------------------------------------------------------------------------
    Set signal mask for operation of all signals internal to QMI Task
  -------------------------------------------------------------------------*/
  qmi_mmodei_enabled_sig_mask = 
#ifndef FEATURE_MODEM_RCINIT_PHASE2
    ( (rex_sigs_type) 1 << (rex_sigs_type) QMI_MMODE_TASK_STOP_SIGNAL )       |
    ( (rex_sigs_type) 1 << (rex_sigs_type) QMI_MMODE_TASK_OFFLINE_SIGNAL )    |
#endif
    ( (rex_sigs_type) 1 << (rex_sigs_type) QMI_MMODE_CMD_Q_SIGNAL )           |
    ( (rex_sigs_type) 1 << (rex_sigs_type) QMI_MMODE_DOG_RPT_TIMER_SIGNAL )   |
    ( (rex_sigs_type) 1 << (rex_sigs_type) QMI_MMODE_SIG_MASK_CHANGE_SIGNAL ) |
    ( (rex_sigs_type) 1 << (rex_sigs_type) QMI_MMODE_TASK_MSGR_SIGNAL )       |
    ( (rex_sigs_type) 1 << (rex_sigs_type) QMI_MMODE_NV_CMD_SIGNAL);

  /*------------------------------------------------------------------------
    Perform the power up initialization for all other modules that run in
    QMI context
  -------------------------------------------------------------------------*/

} /* qmi_mmodei_powerup() */

/*===========================================================================

FUNCTION QMI_MMODEI_PROCESS_CMD()

DESCRIPTION
  This function is called in response to QMI_MMODE_CMD_Q_SIGNAL.  It retrieves
  the   next command from the QMI command queue and processes the command by
  calling the registered cmd handler.

DEPENDENCIES
    An item must be on the QMI command queue or an ERR_FATAL will occur.

RETURN VALUE
  FALSE if cmd queue contains more commands to process and this function
  should be called again, TRUE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean qmi_mmodei_process_cmd
(
  void
)
{
  static byte    qmi_mmode_cmds_proc_hi_score = 0;
  qmi_mmodei_cmd_type * cmd_ptr;
  byte           processed;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    ERR_FATAL if no command is available.
  -------------------------------------------------------------------------*/
  if( q_cnt( &qmi_mmodei_cmd_q ) == 0)
  {
    QM_MSG_FATAL("qmi_mmodei_process_cmd():  No QMI command to process");
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Process all commands in the command q.

    Get each command item in sequence (from the head) of the command queue
    and call the correponding handler with appropriate parameters.
  -------------------------------------------------------------------------*/
  processed = 0;
  while( (cmd_ptr = ( qmi_mmodei_cmd_type * ) q_get( &qmi_mmodei_cmd_q )) != NULL )
  {
    if( cmd_ptr->cmd >= QMI_MMODE_MAX_DEFINED_CMD_TYPES )
    {
      QM_MSG_ERROR_1("qmi_mmodei_process_cmd():  Invalid cmd id 0x%x in Q", cmd_ptr->cmd);
      q_put( &qmi_mmodei_cmd_free_q, &cmd_ptr->link );
      continue;
    }
    processed++;
    qmi_mmodei_cmd_handler[cmd_ptr->cmd]( cmd_ptr->cmd, cmd_ptr->user_data_ptr );

#if 1 /* TODO: remove when all MHs take care of their cmd bufs */
    if (NULL != cmd_ptr->user_data_ptr)
    {
      /* Since memory for user data is allocated in QMI task itself */
      qmi_mmode_free_cmd_data_buf(cmd_ptr->user_data_ptr);
    }
#endif /* TODO: ... */

    q_put( &qmi_mmodei_cmd_free_q, &cmd_ptr->link );
  }

  /*-------------------------------------------------------------------------
    Intlock to assure atomic operation below.

    Note this also ensures no other task will be scheduled since
    q_cnt() and rex_clr_sigs() cannot cause task context switch.
  -------------------------------------------------------------------------*/
  QMI_MMODE_ENTER_CRIT_SECTION(&global_qmi_mmode_crit_section);

  /*-------------------------------------------------------------------------
    Clear the cmd queue signal (which may have been set if a command was
    issued after the rex_clr_sigs() in the outer QMI processing loop)
    Check if any commands are left before doing this, since a command may
    have been posted between closing brace and INTLOCK above.
  -------------------------------------------------------------------------*/
  if( q_cnt( &qmi_mmodei_cmd_q ) == 0 )
  {
    QMI_MMODE_CLR_SIGNAL(QMI_MMODE_CMD_Q_SIGNAL);
  }

  /*-------------------------------------------------------------------------
    End atomic operation - Restore interrupts.
  -------------------------------------------------------------------------*/
  QMI_MMODE_LEAVE_CRIT_SECTION(&global_qmi_mmode_crit_section);

  /*-------------------------------------------------------------------------
    For debugging, show if we just processed a ton of commands
  -------------------------------------------------------------------------*/
  if( processed > qmi_mmode_cmds_proc_hi_score )
  {
    qmi_mmode_cmds_proc_hi_score = processed;
    QM_MSG_MED_1("qmi_mmodei_process_cmd():  New high # QMI commands processed:  %d", processed);
  }

  /*-------------------------------------------------------------------------
    Always return true to clear QMI_CMD_SIG from loop_sigs in case command
    was sent after clearing signal above, so that we never get into the
    situation where we process a command due to loop_sigs, emptying the
    cmd queue, and yet the QMI_CMD_SIG is set, leading to ERR_FATAL above.
  -------------------------------------------------------------------------*/
  return TRUE;

} /* qmi_mmodei_process_cmd() */

/*===========================================================================

FUNCTION QMI_MMODEI_PROCESS_MSGR()

DESCRIPTION
  This function is called in response to QMI_MMODE_TASK_MSGR_SIGNAL. It 
  processes the commands from the DCC msgr queue.

DEPENDENCIES
  An item must be on the QMI_MMODE msgr queue.

RETURN VALUE
  none

SIDE EFFECTS
  None.

===========================================================================*/
void qmi_mmodei_process_msgr
(
  void
)
{
  qmi_mmodei_msgr_type  * msg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  while ((msg = q_get(&qmi_mmodei_msgr_q)) != NULL)
  {
    /*-----------------------------------------------------------------------
      Do not manipulate msg contents. Other services may handle same message.
      So do not change the message. And do not free the item back to free q.
      Freeing q should be done at the end of this section. Again, this is due
      to possibility of same message being handled by multiple services.
    -----------------------------------------------------------------------*/
    qmi_nas_msgr_handler( (void*)msg );
    qmi_voice_msgr_handler( (void*)msg );

    msgr_rex_free_msg_buf(&(msg->link));
  }
} /* dcci_process_msgr() */

/*===========================================================================

FUNCTION qmi_mmodei_msgr_init

DESCRIPTION
  Initializes MSGR queues and registers QMI_MMODE with MSG Router

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void qmi_mmodei_msgr_init( void )
{
  errno_enum_type msgr_err;

  /*-------------------------------------------------------------------------
    Initialize the msgr queues and link items onto the command free queue
    Also perform msgr init and registration
  -------------------------------------------------------------------------*/
  (void) q_init( &qmi_mmodei_msgr_q );

  msgr_err = msgr_client_create( &qmi_mmode_msgr_client );
  if(msgr_err != E_SUCCESS)
  {
    QM_MSG_ERROR_1("Fail to create msgr client for QMI_MMODE ret %d", msgr_err);
  }
  ASSERT( msgr_err == E_SUCCESS );
 
  msgr_err = msgr_client_set_rex_q( &qmi_mmode_msgr_client,
                                    qmi_mmode_tcb_ptr,
                                    (rex_sigs_type)(1<<QMI_MMODE_TASK_MSGR_SIGNAL),
                                    &qmi_mmodei_msgr_q,
                                    MSGR_NO_QUEUE,
                                    QMI_MMODE_OFFSETOF( qmi_mmodei_msgr_type, cmd ),
                                    MSGR_NO_OFFSET,
                                    sizeof( qmi_mmodei_msgr_type ) );
  if(msgr_err != E_SUCCESS)
  {
    QM_MSG_ERROR_1("Error msgr_client_set_rex_q ret %d", msgr_err);
  }
  ASSERT( msgr_err == E_SUCCESS );
 
  qmi_nas_msgr_register( &qmi_mmode_msgr_client );
  qmi_voice_msgr_register( &qmi_mmode_msgr_client );
}
