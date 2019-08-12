/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              Q M I _ M O D E M _T A S K I . C

DESCRIPTION
  QMI Modem Task internal API file. Contains APIs and definitions used
  internal to QMI Modem Task processing. 

EXTERNALIZED FUNCTIONS
  qmi_default_sig_handler()
    Implicit handler for signals processed by QMI Task.

  qmi_default_cmd_handler()
    Implicit handler for commands processed by QMI Task.

  qmii_powerup()
    Cold start called before QMI task is started.

  qmii_process_cmd()
    Processes command signals in QMI Task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the QMI APIs can be used unless QMI task is created and started.

Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/qmi_modem_taski.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/27/13    wc     Support dynamic port configuration
01/25/11    rk     Created module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */


#include "dog_hb_rex.h"
#include "qmi_modem_taski.h"
#include "qmi_modem_task_svc.h"
#include "qmi_modem_task_cmd.h"
#include "qmi_frameworki.h"
#include "ds_qmux_ext.h"
#include "amssassert.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                  GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
extern rex_tcb_type  * rex_qmi_modem_tcb;
#endif
/*--------------------------------------------------------------------------
  Timers needed for the QMI Ctrl Task.
---------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
  Dog report id.
---------------------------------------------------------------------------*/
dog_report_type qmi_dog_rpt_id                   = 0;        /* Dog report ID       */

q_type qmii_cmd_q;
q_type qmii_cmd_free_q;

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers
---------------------------------------------------------------------------*/
qmii_sig_handler_info_type qmii_sig_handler[QMI_MAX_SIGNALS];
qmi_cmd_handler_type qmii_cmd_handler[QMI_MAX_DEFINED_CMD_TYPES];

/*--------------------------------------------------------------------------
  Protocol Stack Task Command links and queues
---------------------------------------------------------------------------*/

qmii_cmd_type qmii_cmd_buf[QMII_CMD_BUF_CNT];

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled signals
---------------------------------------------------------------------------*/
rex_sigs_type qmii_enabled_sig_mask;

/*--------------------------------------------------------------------------
  QMI's own critical section
---------------------------------------------------------------------------*/
qmi_crit_sect_type global_qmi_crit_section;
qmi_crit_sect_type qmi_port_cfg_crit_section;

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION QMI_DEFAULT_SIG_HANDLER()

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
boolean qmi_default_sig_handler
(
  qmi_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_ERROR_1("qmi_default_sig_handler(): "
                  "Signal %d, no handler registered", sig);

  /*-------------------------------------------------------------------------
    Return TRUE since we don't want this signal to be enqueued again.
  -------------------------------------------------------------------------*/
  return TRUE;
} /* qmi_default_sig_handler() */

/*===========================================================================

FUNCTION QMI_DEFAULT_CMD_HANDLER()

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
void qmi_default_cmd_handler
(
  qmi_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_ERROR_1("qmi_default_cmd_handler(): "
                  "Command %d, no handler registered", cmd );
} /* qmi_default_cmd_handler() */

/*===========================================================================
FUNCTION QMII_POWERUP()

DESCRIPTION
  Powerup (coldstart) initialization function for the QMI task. 
  
  This function performs initialization before QMI task is started.
  Note that initializations internal to QMI should be performed before
  initializing other components that run in QMI task context.

DEPENDENCIES
  This function must be called only once at system powerup.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void qmii_powerup
(
  void
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0("qmii_powerup(): "
                  "QMI Task Powerup");

  /*------------------------------------------------------------------------
    Perform the power up initialization for all other modules that run in
    QMI context
  -------------------------------------------------------------------------*/

  QMI_INIT_CRIT_SECTION(&global_qmi_crit_section);
  QMI_INIT_CRIT_SECTION(&qmi_port_cfg_crit_section);

  if (qmux_powerup_init() == FALSE)
  {
    DATA_ERR_FATAL("qmii_powerup(): QMUX Powerup Initialization failed");
    return;
  }

  if (qmi_framework_powerup_init() == FALSE)
  {
    DATA_ERR_FATAL("qmii_powerup(): QMI Framework Powerup Initialization failed");
    return;
  }

  /*-------------------------------------------------------------------------
    Initialize the command queues and link items onto the command free queue
  -------------------------------------------------------------------------*/
  (void) q_init( &qmii_cmd_q );
  (void) q_init( &qmii_cmd_free_q );

  for( i = 0; i < QMII_CMD_BUF_CNT; i++ )
  {
    q_put( &qmii_cmd_free_q, q_link( &qmii_cmd_buf[i], &qmii_cmd_buf[i].link) );
  }

  /*------------------------------------------------------------------------
    Initialize all the signal and cmd handlers
  -------------------------------------------------------------------------*/
  for( i = 0; i < (int) QMI_MAX_SIGNALS; i++ )
  {
    qmii_sig_handler[i].sig_handler = qmi_default_sig_handler;
    qmii_sig_handler[i].user_data_ptr = NULL;
  }

  for( i = 0; i < (int) QMI_MAX_DEFINED_CMD_TYPES; i++ )
  {
    qmii_cmd_handler[i] = qmi_default_cmd_handler;
  }

  /*------------------------------------------------------------------------
    Set signal mask for operation of all signals internal to QMI Task
  -------------------------------------------------------------------------*/
  qmii_enabled_sig_mask = ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) QMI_MODEM_TASK_STOP_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) QMI_MODEM_TASK_OFFLINE_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) QMI_CMD_Q_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) QMI_DOG_HB_REPORT_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) QMI_SIG_MASK_CHANGE_SIGNAL );

} /* qmii_powerup() */

/*===========================================================================

FUNCTION QMII_PROCESS_CMD()

DESCRIPTION
  This function is called in response to QMI_CMD_Q_SIGNAL.  It retrieves the
  next command from the QMI command queue and processes the command by calling 
  the registered cmd handler.

DEPENDENCIES
    An item must be on the QMI command queue or an ERR_FATAL will occur.

RETURN VALUE
  FALSE if cmd queue contains more commands to process and this function
  should be called again, TRUE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean qmii_process_cmd
(
  void
)
{
  static byte    qmi_cmds_proc_hi_score = 0;
  qmii_cmd_type * cmd_ptr;
  byte           processed;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    ERR_FATAL if no command is available.
  -------------------------------------------------------------------------*/
  if( q_cnt( &qmii_cmd_q ) == 0)
  {
    LOG_MSG_ERROR_0("qmii_process_cmd(): "
                    "No QMI command to process" );
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Process all commands in the command q.

    Get each command item in sequence (from the head) of the command queue
    and call the correponding handler with appropriate parameters.
  -------------------------------------------------------------------------*/
  processed = 0;
  while( (cmd_ptr = ( qmii_cmd_type * ) q_get( &qmii_cmd_q )) != NULL )
  {
    if( cmd_ptr->cmd >= QMI_MAX_DEFINED_CMD_TYPES )
    {
      LOG_MSG_ERROR_1("qmii_process_cmd(): "
                      "Invalid cmd id 0x%x in Q", cmd_ptr->cmd);
      q_put( &qmii_cmd_free_q, &cmd_ptr->link );
      continue;
    }
    processed++;
    qmii_cmd_handler[cmd_ptr->cmd]( cmd_ptr->cmd, cmd_ptr->user_data_ptr );

#if 1 /* TODO: remove when all MHs take care of their cmd bufs */
    if (NULL != cmd_ptr->user_data_ptr)
    {
      /* Since memory for user data is allocated in QMI task itself */
      qmi_free_cmd_data_buf(cmd_ptr->user_data_ptr);
    }
#endif /* TODO: ... */

    q_put( &qmii_cmd_free_q, &cmd_ptr->link );
  }

  /*-------------------------------------------------------------------------
    Intlock to assure atomic operation below.

    Note this also ensures no other task will be scheduled since
    q_cnt() and rex_clr_sigs() cannot cause task context switch.
  -------------------------------------------------------------------------*/
  QMI_ENTER_CRIT_SECTION(&global_qmi_crit_section);

  /*-------------------------------------------------------------------------
    Clear the cmd queue signal (which may have been set if a command was
    issued after the rex_clr_sigs() in the outer QMI processing loop)
    Check if any commands are left before doing this, since a command may
    have been posted between closing brace and INTLOCK above.
  -------------------------------------------------------------------------*/
  if( q_cnt( &qmii_cmd_q ) == 0 )
  {
    QMI_CLR_SIGNAL(QMI_CMD_Q_SIGNAL);
  }

  /*-------------------------------------------------------------------------
    End atomic operation - Restore interrupts.
  -------------------------------------------------------------------------*/
  QMI_LEAVE_CRIT_SECTION(&global_qmi_crit_section);

  /*-------------------------------------------------------------------------
    For debugging, show if we just processed a ton of commands
  -------------------------------------------------------------------------*/
  if( processed > qmi_cmds_proc_hi_score )
  {
    qmi_cmds_proc_hi_score = processed;
    LOG_MSG_INFO2_1 ("qmii_process_cmd(): "
                     "New high # QMI commands processed: %d", processed);
  }

  /*-------------------------------------------------------------------------
    Always return true to clear QMI_CMD_SIG from loop_sigs in case command
    was sent after clearing signal above, so that we never get into the
    situation where we process a command due to loop_sigs, emptying the
    cmd queue, and yet the QMI_CMD_SIG is set, leading to ERR_FATAL above.
  -------------------------------------------------------------------------*/
  return TRUE;

} /* qmii_process_cmd() */
