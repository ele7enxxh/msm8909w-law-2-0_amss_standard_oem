/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                           P S I . C

DESCRIPTION

  Protocol Task local header file. All definitions, structures, and functions
  needed internal to protocol task services.

EXTERNALIZED FUNCTIONS

  psi_process_cmd()
    Procsses a PS task cmd and calls appropriate handler function

Copyright (c) 2002-2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/psi.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/06/11    hm     Use ps_task_crit_section for all PS_task operations. 
08/11/10    ss     Rearchitectured PS extension signal mechanism. Any number
                   of signals supported now.
09/03/09    pp     Moved ps_wait APIs to ps_svc. Removed psi_wait_adapter.
06/06/09    ar     Added PSI_WAIT adapter.
11/28/08    pp     psi_process_cmd: Use of PS_ENTER_CRIT_SECTION/UNLOCK macros.
10/24/08    pp     Removed unused variable.
09/07/07    scb    Added REX signal ext code under FEATURE_REX_SIGS_EXT
04/25/06    mct    Removed tick timer code from old pstimer implementation.
05/18/05    ssh    Fixed lint errors.
04/30/04    mct    Fixed some lint errors.
02/18/04    usb    Fixed compiler warning.
10/28/03    jd     Process all commands in PS command queue before returning
                   from cmd sig handler, to ensure command processing occurs
                   on schedule with data flow (e.g. sockopt is processed
                   before data is delivered to corresponding socket).
05/12/03    usb    Fixed bad while() condition in psi_wait()
08/05/02    usb    Added new function psi_wait()
03/14/02    usb    Created Initial Version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#include "rex.h"
#include "queue.h"
#include "msg.h"
#include "err.h"
#include "dog.h"
#include "psi.h"
#include "ps_svc.h"
#include "ps.h"
#include "ps_utils.h"
#include "ps_crit_sect.h"
#ifdef FEATURE_REX_SIGS_EXT
#include "amssassert.h"
#endif /* FEATURE_REX_SIGS_EXT */
#include "ds_Utils_DebugMsg.h"
#include "rcinit_rex.h"



/*===========================================================================

                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers
---------------------------------------------------------------------------*/
psi_sig_handler_info_type psi_sig_handler[PS_MAX_SIGNALS];
ps_cmd_handler_type psi_cmd_handler[PS_MAX_DEFINED_CMD_TYPES];

/*--------------------------------------------------------------------------
  Protocol Stack Task Command links and queues
---------------------------------------------------------------------------*/

psi_cmd_type psi_cmd_buf[ PSI_CMD_BUF_CNT];

q_type psi_cmd_q;
q_type psi_cmd_free_q;

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled signals
---------------------------------------------------------------------------*/
/*lint -e506 */
rex_sigs_type psi_enabled_sig_mask[PS_MAX_SIG_ARR_LEN];

#ifdef FEATURE_MODEM_RCINIT_PHASE2
extern rex_tcb_type * rex_ps_tcb;
#endif

/*===========================================================================

                           FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION PSI_PROCESS_CMD()

DESCRIPTION
  This function is called in response to PS_CMD_Q_SIGNAL.  It retrieves the
  next command from the PS command queue and processes the command by calling
  the registered cmd handler.

DEPENDENCIES
    An item must be on the PS command queue or an LOG_MSG_ERROR will occur.

RETURN VALUE
  FALSE if cmd queue contains more commands to process and this function
  should be called again, TRUE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean psi_process_cmd
(
  void
)
{
  static byte    ps_cmds_processed_hi_score = 0;
  psi_cmd_type * cmd_ptr;
  byte           processed;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    LOG_MSG_ERROR if no command is available.
  -------------------------------------------------------------------------*/
  if( q_cnt( &psi_cmd_q ) == 0)
  {
    LOG_MSG_ERROR_0("psi_process_cmd(): "
                    "No PS command to process" );
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Process all commands in the command q.

    Get each command item in sequence (from the head) of the command queue
    and call the correponding handler with appropriate parameters.
  -------------------------------------------------------------------------*/
  processed = 0;
  while( (cmd_ptr = ( psi_cmd_type * ) q_get( &psi_cmd_q )) != NULL )
  {
    if( cmd_ptr->cmd >= PS_MAX_DEFINED_CMD_TYPES )
    {
      LOG_MSG_ERROR_1("psi_process_cmd(): "
                      "Invalid cmd id 0x%x in Q", cmd_ptr->cmd);
      q_put( &psi_cmd_free_q, &cmd_ptr->link );
      continue;
    }
    processed++;
    psi_cmd_handler[cmd_ptr->cmd]( cmd_ptr->cmd, cmd_ptr->user_data_ptr );
    q_put( &psi_cmd_free_q, &cmd_ptr->link );
  }

  /*-------------------------------------------------------------------------
    Intlock to assure atomic operation below.

    Note this also ensures no other task will be scheduled since
    q_cnt() and rex_clr_sigs() cannot cause task context switch.
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&ps_task_crit_section);

  /*-------------------------------------------------------------------------
    Clear the cmd queue signal (which may have been set if a command was
    issued after the rex_clr_sigs() in the outer ps processing loop)
    Check if any commands are left before doing this, since a command may
    have been posted between closing brace and INTLOCK above.
  -------------------------------------------------------------------------*/
  if( q_cnt( &psi_cmd_q ) == 0 )
  {
#ifdef FEATURE_REX_SIGS_EXT
    PS_CLR_SIGNAL(PS_CMD_Q_SIGNAL);
#else
  #ifdef FEATURE_MODEM_RCINIT_PHASE2
    (void)rex_clr_sigs( rex_ps_tcb, ( (rex_sigs_type) 1 << PS_CMD_Q_SIGNAL ) );
  #else
    (void)rex_clr_sigs( &ps_tcb, ( (rex_sigs_type) 1 << PS_CMD_Q_SIGNAL ) );
  #endif
#endif /* FEATURE_REX_SIGS_EXT */
  }

  /*-------------------------------------------------------------------------
    End atomic operation - Restore interrupts.
  -------------------------------------------------------------------------*/
  PS_LEAVE_CRIT_SECTION(&ps_task_crit_section);

  /*-------------------------------------------------------------------------
    For debugging, show if we just processed a ton of commands
  -------------------------------------------------------------------------*/
  if( processed > ps_cmds_processed_hi_score )
  {
    ps_cmds_processed_hi_score = processed;
    LOG_MSG_INFO2_1 ("psi_process_cmd(): "
                     "New high # PS commands processed: %d", processed);
  }

  /*-------------------------------------------------------------------------
    Always return true to clear ps_cmd_sig from loop_sigs in case command
    was sent after clearing signal above, so that we never get into the
    situation where we process a command due to loop_sigs, emptying the
    cmd queue, and yet the PS_CMD_SIG is set, leading to LOG_MSG_ERROR above.
  -------------------------------------------------------------------------*/
  return TRUE;

} /* psi_process_cmd() */

/*===========================================================================

FUNCTION DEFAULT_SIG_HANDLER()

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

boolean default_sig_handler
(
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_ERROR_1("default_sig_handler(): "
                  "Signal %d, no handler registered", sig);

  /*-------------------------------------------------------------------------
    To avoid lint warning.
  -------------------------------------------------------------------------*/
  (void)user_data_ptr;
  /*-------------------------------------------------------------------------
    Return TRUE since we don't want this signal to be enqueued again.
  -------------------------------------------------------------------------*/
  return TRUE;
} /* default_sig_handler() */


/*===========================================================================

FUNCTION DEFAULT_CMD_HANDLER()

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
void default_cmd_handler
(
  ps_cmd_enum_type cmd,          /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_ERROR_1("default_cmd_handler(): "
                  "Command %d, no handler registered", cmd );
  /*-------------------------------------------------------------------------
    To avoid lint warning.
  -------------------------------------------------------------------------*/
  (void)user_data_ptr;  
} /* default_cmd_handler() */


/*===========================================================================

FUNCTION PS_START_TIMER

DESCRIPTION
  This function will set the specified timer to the specified duration. The
  time units are in milliseconds.

DEPENDENCIES
  The timer should already be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ps_start_timer
(
  rex_timer_type  *ps_timer_ptr,            /* ptr to timer to be started     */
  dword duration                         /* time interval, in msecs        */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT( ps_timer_ptr != NULL );

  /*------------------------------------------------------------------------
    Set the specified timer with the passed duration (in milliseconds)
  -------------------------------------------------------------------------*/
  (void)rex_set_timer( ps_timer_ptr, duration );

} /* ps_start_timer() */


/*===========================================================================

FUNCTION PS_CANCEL_TIMER

DESCRIPTION
  This function will cancel the specified timer and the associated signal
  mask will also be cleared. NOTE that the signals are associated with
  timers during initialization by the REX_DEF_TIMER function.

DEPENDENCIES
  The timer should already be initialized. The signal associated with the
  timer should be a PS task signal.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void ps_cancel_timer
(
  rex_timer_type  *ps_timer_ptr,            /* ptr to timer to be cancelled   */
  ps_sig_enum_type sig                   /* signal associated with a timer */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( ps_timer_ptr != NULL );

  /*------------------------------------------------------------------------
    Cancel the specified timer and clear the associated signal
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO2_1("ps_cancel_timer(): "
                  "Cancelling timer 0x%x", ps_timer_ptr );
  (void)rex_clr_timer( ps_timer_ptr );
  PS_CLR_SIGNAL( sig );

} /* ps_cancel_timer() */

