/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              Q M I _ PBM _T A S K I . C

DESCRIPTION
  QMI PBM Task internal API file. Contains APIs and definitions used
  internal to QMI PBM Task processing.

EXTERNALIZED FUNCTIONS
  qmi_pbm_default_sig_handler()
    Implicit handler for signals processed by QMI Task.

  qmi_pbm_default_cmd_handler()
    Implicit handler for commands processed by QMI Task.

  qmi_pbmi_powerup()
    Cold start called before QMI task is started.

  qmi_pbmi_process_cmd()
    Processes command signals in QMI Task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the QMI APIs can be used unless QMI PBM task is created and
  started.

Copyright (c) 2011,2014 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/qmi_pbm_taski.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/25/14    NR     Use v2.0 diag macros
04/01/14    NR     QTF CRM MOB Changes
01/25/11    rk     Created module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "uim_msg.h"
#include "dog.h"
#include "qmi_pbm_taski.h"
#include "qmi_pbm_task_svc.h"
#include "qmi_pbm_task_cmd.h"
#include "amssassert.h"

/*===========================================================================

                  GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

q_type qmi_pbmi_cmd_q;
q_type qmi_pbmi_cmd_free_q;

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers
---------------------------------------------------------------------------*/
qmi_pbmi_sig_handler_info_type qmi_pbmi_sig_handler[QMI_PBM_MAX_SIGNALS];
qmi_pbm_cmd_handler_type qmi_pbmi_cmd_handler[QMI_PBM_MAX_DEFINED_CMD_TYPES];

/*--------------------------------------------------------------------------
  Protocol Stack Task Command links and queues
---------------------------------------------------------------------------*/

qmi_pbmi_cmd_type qmi_pbmi_cmd_buf[QMI_PBMI_CMD_BUF_CNT];

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled signals
---------------------------------------------------------------------------*/
#ifdef FEATURE_REX_SIGS_EXT
rex_sigs_type qmi_pbmi_enabled_sig_mask[QMI_PBM_SIG_ARR_LEN];
#else
rex_sigs_type qmi_pbmi_enabled_sig_mask;
#endif /* FEATURE_REX_SIGS_EXT */

/*--------------------------------------------------------------------------
  QMI's own critical section
---------------------------------------------------------------------------*/
qmi_pbm_crit_sect_type global_qmi_pbm_crit_section;

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION QMI_PBM_DEFAULT_SIG_HANDLER()

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
boolean qmi_pbm_default_sig_handler
(
  qmi_pbm_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  UIM_MSG_ERR_1("qmi_pbm_default_sig_handler(): "
                      "Signal %d, no handler registered", sig);

  /*-------------------------------------------------------------------------
    Return TRUE since we don't want this signal to be enqueued again.
  -------------------------------------------------------------------------*/
  return TRUE;
} /* qmi_pbm_default_sig_handler() */

/*===========================================================================

FUNCTION QMI_PBM_DEFAULT_CMD_HANDLER()

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
void qmi_pbm_default_cmd_handler
(
  void *user_data_ptr            /* Command specific user parameters       */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  UIM_MSG_ERR_0("qmi_pbm_default_cmd_handler(): "
                      "Command %d, no handler registered");
} /* qmi_pbm_default_cmd_handler() */

/*===========================================================================
FUNCTION QMI_PBM_SVC_POWERUP()

DESCRIPTION
  Powerup (coldstart) initialization function for the QMI PBM task.

  This function performs initialization before QMI PBM task is started.
  Note that initializations internal to QMI should be performed before
  initializing other components that run in QMI PBM task context.

DEPENDENCIES
  This function must be called only once at system powerup.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_pbmi_powerup
(
  void
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  UIM_MSG_HIGH_0("qmi_pbmi_powerup(): "
                "QMI PBM Task Powerup");

  /*------------------------------------------------------------------------
    Perform the power up initialization for all other modules that run in
    QMI context
  -------------------------------------------------------------------------*/

  QMI_PBM_INIT_CRIT_SECTION(&global_qmi_pbm_crit_section);

  /*-------------------------------------------------------------------------
    Initialize the command queues and link items onto the command free queue
  -------------------------------------------------------------------------*/
  (void) q_init( &qmi_pbmi_cmd_q );
  (void) q_init( &qmi_pbmi_cmd_free_q );

  for( i = 0; i < QMI_PBMI_CMD_BUF_CNT; i++ )
  {
    q_put( &qmi_pbmi_cmd_free_q, q_link( &qmi_pbmi_cmd_buf[i], &qmi_pbmi_cmd_buf[i].link) );
  }

  /*------------------------------------------------------------------------
    Initialize all the signal and cmd handlers
  -------------------------------------------------------------------------*/
  for( i = 0; i < (int) QMI_PBM_MAX_SIGNALS; i++ )
  {
    qmi_pbmi_sig_handler[i].sig_handler = qmi_pbm_default_sig_handler;
    qmi_pbmi_sig_handler[i].user_data_ptr = NULL;
  }

  for( i = 0; i < (int) QMI_PBM_MAX_DEFINED_CMD_TYPES; i++ )
  {
    qmi_pbmi_cmd_handler[i] = qmi_pbm_default_cmd_handler;
  }

  /*------------------------------------------------------------------------
    Set signal mask for operation of all signals internal to QMI Task
  -------------------------------------------------------------------------*/
#ifdef FEATURE_REX_SIGS_EXT
  QMI_SET_SIG(qmi_pbmi_enabled_sig_mask, QMI_PBM_TASK_STOP_SIGNAL);
  QMI_SET_SIG(qmi_pbmi_enabled_sig_mask, QMI_PBM_TASK_OFFLINE_SIGNAL);
  QMI_SET_SIG(qmi_pbmi_enabled_sig_mask, QMI_PBM_CMD_Q_SIGNAL);
  QMI_SET_SIG(qmi_pbmi_enabled_sig_mask, QMI_PBM_DOG_RPT_TIMER_SIGNAL);
  QMI_SET_SIG(qmi_pbmi_enabled_sig_mask, QMI_PBM_SIG_MASK_CHANGE_SIGNAL);
  QMI_SET_SIG(qmi_pbmi_enabled_sig_mask, QMI_PBM_NV_CMD_SIGNAL);
#else
  qmi_pbmi_enabled_sig_mask = ( (rex_sigs_type) 1 <<
  #ifndef FEATURE_MODEM_RCINIT_PHASE2
                            (rex_sigs_type) QMI_PBM_TASK_STOP_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) QMI_PBM_TASK_OFFLINE_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
  #endif
#if (defined FEATURE_UIM_TEST_FRAMEWORK && defined FEATURE_MODEM_RCINIT)
                          #error code not present
#endif
                            (rex_sigs_type) QMI_PBM_CMD_Q_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) QMI_PBM_DOG_RPT_TIMER_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) QMI_PBM_SIG_MASK_CHANGE_SIGNAL )|
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) QMI_PBM_NV_CMD_SIGNAL);
#endif /* FEATURE_REX_SIGS_EXT */

  /*------------------------------------------------------------------------
    Perform the power up initialization for all other modules that run in
    QMI context
  -------------------------------------------------------------------------*/
} /* qmi_pbmi_powerup() */

/*===========================================================================

FUNCTION QMI_PBMI_PROCESS_CMD()

DESCRIPTION
  This function is called in response to QMI_PBM_CMD_Q_SIGNAL.  It retrieves
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
boolean qmi_pbmi_process_cmd
(
  void
)
{
  static byte    qmi_pbm_cmds_proc_hi_score = 0;
  qmi_pbmi_cmd_type * cmd_ptr;
  byte           processed;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    ERR_FATAL if no command is available.
  -------------------------------------------------------------------------*/
  if( q_cnt( &qmi_pbmi_cmd_q ) == 0)
  {
    UIM_MSG_ERR_0("qmi_pbmi_process_cmd(): "
                        "No QMI command to process");
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Process all commands in the command q.

    Get each command item in sequence (from the head) of the command queue
    and call the correponding handler with appropriate parameters.
  -------------------------------------------------------------------------*/
  processed = 0;
  while( (cmd_ptr = ( qmi_pbmi_cmd_type * ) q_get( &qmi_pbmi_cmd_q )) != NULL )
  {
    if( cmd_ptr->cmd >= QMI_PBM_MAX_DEFINED_CMD_TYPES )
    {
      UIM_MSG_ERR_1("qmi_pbmi_process_cmd(): "
                    "Invalid cmd id 0x%x in Q", cmd_ptr->cmd);
      q_put( &qmi_pbmi_cmd_free_q, &cmd_ptr->link );
      continue;
    }
    processed++;
    qmi_pbmi_cmd_handler[cmd_ptr->cmd]( cmd_ptr->user_data_ptr );

#if 1 /* TODO: remove when all MHs take care of their cmd bufs */
    if (NULL != cmd_ptr->user_data_ptr)
    {
      /* Since memory for user data is allocated in QMI task itself */
      qmi_pbm_free_cmd_data_buf(cmd_ptr->user_data_ptr);
    }
#endif /* TODO: ... */

    q_put( &qmi_pbmi_cmd_free_q, &cmd_ptr->link );
  }

  /*-------------------------------------------------------------------------
    Intlock to assure atomic operation below.

    Note this also ensures no other task will be scheduled since
    q_cnt() and rex_clr_sigs() cannot cause task context switch.
  -------------------------------------------------------------------------*/
  QMI_PBM_ENTER_CRIT_SECTION(&global_qmi_pbm_crit_section);

  /*-------------------------------------------------------------------------
    Clear the cmd queue signal (which may have been set if a command was
    issued after the rex_clr_sigs() in the outer QMI processing loop)
    Check if any commands are left before doing this, since a command may
    have been posted between closing brace and INTLOCK above.
  -------------------------------------------------------------------------*/
  if( q_cnt( &qmi_pbmi_cmd_q ) == 0 )
  {
    QMI_PBM_CLR_SIGNAL(QMI_PBM_CMD_Q_SIGNAL);
  }

  /*-------------------------------------------------------------------------
    End atomic operation - Restore interrupts.
  -------------------------------------------------------------------------*/
  QMI_PBM_LEAVE_CRIT_SECTION(&global_qmi_pbm_crit_section);

  /*-------------------------------------------------------------------------
    For debugging, show if we just processed a ton of commands
  -------------------------------------------------------------------------*/
  if( processed > qmi_pbm_cmds_proc_hi_score )
  {
    qmi_pbm_cmds_proc_hi_score = processed;
    UIM_MSG_HIGH_1 ("qmi_pbmi_process_cmd(): "
                   "New high # QMI commands processed: %d", processed);
  }

  /*-------------------------------------------------------------------------
    Always return true to clear QMI_CMD_SIG from loop_sigs in case command
    was sent after clearing signal above, so that we never get into the
    situation where we process a command due to loop_sigs, emptying the
    cmd queue, and yet the QMI_CMD_SIG is set, leading to ERR_FATAL above.
  -------------------------------------------------------------------------*/
  return TRUE;

} /* qmi_pbmi_process_cmd() */


