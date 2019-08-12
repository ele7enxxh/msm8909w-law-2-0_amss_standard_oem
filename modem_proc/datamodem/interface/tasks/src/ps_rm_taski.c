/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              P S _ R M _ T A S K I . C

DESCRIPTION
  NAT data processing Task internal API file. Contains APIs and definitions
  used internal to PS_RM Task processing. 

EXTERNALIZED FUNCTIONS
  ps_rm_default_sig_handler()
    Implicit handler for signals processed by PS_RM Task.

  ps_rm_default_cmd_handler()
    Implicit handler for commands processed by PS_RM Task.

  ps_rm_powerup()
    Cold start called before PS_RM task is started.

  ps_rm_process_cmd()
    Processes command signals in PS_RM Task.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/ps_rm_taski.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
i10/07/10   am     created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */


#include "dog.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_rm_taski.h"
#include "ps_rm_defs.h"
#include "ps_rm_svc.h"

/*===========================================================================

                  GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*--------------------------------------------------------------------------
  Timers needed for the DataCommon Ctrl Task.
---------------------------------------------------------------------------*/
//rex_timer_type ps_rm_dog_rpt_timer;        /* for watchdog reporting  */

q_type ps_rm_cmd_q;
q_type ps_rm_cmd_free_q;

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers
---------------------------------------------------------------------------*/
ps_rm_sig_handler_info_type ps_rm_sig_handler[PS_RM_MAX_SIGNALS];
ps_rm_cmd_handler_type ps_rm_cmd_handler[PS_RM_MAX_DEFINED_CMD_TYPES];

/*--------------------------------------------------------------------------
  Protocol Stack Task Command links and queues
---------------------------------------------------------------------------*/

ps_rm_cmd_type ps_rm_cmd_buf[PS_RM_CMD_BUF_CNT];

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled signals
---------------------------------------------------------------------------*/
rex_sigs_type ps_rm_enabled_sig_mask;

/*--------------------------------------------------------------------------
  NAT's own critical section
---------------------------------------------------------------------------*/
ps_crit_sect_type global_ps_rm_crit_section;

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION PS_RM_DEFAULT_SIG_HANDLER()

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

boolean 
ps_rm_default_sig_handler
(
  ps_rm_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_ERROR_1("ps_rm_default_sig_handler(): "
                  "Signal %d, no handler registered", sig);

  /*-------------------------------------------------------------------------
    Return TRUE since we don't want this signal to be enqueued again.
  -------------------------------------------------------------------------*/
  return TRUE;
} /* ps_rm_default_sig_handler() */

/*===========================================================================

FUNCTION PS_RM_DEFAULT_CMD_HANDLER()

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

void 
ps_rm_default_cmd_handler
(
  ps_rm_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_ERROR_1("ps_rm_default_cmd_handler(): "
                  "Command %d, no handler registered", cmd );
} /* ps_rm_default_cmd_handler() */

/*===========================================================================
FUNCTION PS_RM_POWERUP()

DESCRIPTION
  Powerup (coldstart) initialization function for the PS_RM task. 
  
  This function performs initialization before PS_RM task is started.

DEPENDENCIES
  This function must be called only once at system powerup.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void 
ps_rm_powerup
(
  void
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0("ps_rm_powerup(): "
                  "Data Common Control Task Powerup");

  /*------------------------------------------------------------------------
    Perform the power up initialization for all other modules that run in
    NAT context
  -------------------------------------------------------------------------*/
  PS_INIT_CRIT_SECTION(&global_ps_rm_crit_section);

  /*-------------------------------------------------------------------------
    Initialize the command queues and link items onto the command free queue
  -------------------------------------------------------------------------*/
  (void) q_init( &ps_rm_cmd_q );
  (void) q_init( &ps_rm_cmd_free_q );

  for( i = 0; i < PS_RM_CMD_BUF_CNT; i++ )
  {
    q_put( &ps_rm_cmd_free_q, q_link( &ps_rm_cmd_buf[i], &ps_rm_cmd_buf[i].link) );
  }

  /*------------------------------------------------------------------------
    Initialize all the signal and cmd handlers
  -------------------------------------------------------------------------*/
  for( i = 0; i < (int) PS_RM_MAX_SIGNALS; i++ )
  {
    ps_rm_sig_handler[i].sig_handler = ps_rm_default_sig_handler;
    ps_rm_sig_handler[i].user_data_ptr = NULL;
  }

  for( i = 0; i < (int) PS_RM_MAX_DEFINED_CMD_TYPES; i++ )
  {
    ps_rm_cmd_handler[i] = ps_rm_default_cmd_handler;
  }

  /*------------------------------------------------------------------------
    Set signal mask for operation of all signals internal to PS_RM Task
  -------------------------------------------------------------------------*/
  ps_rm_enabled_sig_mask = ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) PS_RM_TASK_STOP_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) PS_RM_TASK_OFFLINE_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) PS_RM_CMD_Q_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) PS_RM_DOG_HB_REPORT_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) PS_RM_SIG_MASK_CHANGE_SIGNAL );

} /* ps_rm_powerup() */

/*===========================================================================

FUNCTION PS_RM_PROCESS_CMD()

DESCRIPTION
  This function is called in response to PS_RM_CMD_Q_SIGNAL.  It retrieves the
  next command from the NAT command queue and processes the command by calling 
  the registered cmd handler.

DEPENDENCIES
    An item must be on the NAT command queue or an LOG_MSG_ERROR will occur.

RETURN VALUE
  FALSE if cmd queue contains more commands to process and this function
  should be called again, TRUE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean 
ps_rm_process_cmd
(
  void
)
{
  static byte    ps_rm_cmds_proc_hi_score = 0;
  ps_rm_cmd_type * cmd_ptr;
  byte           processed;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    LOG_MSG_ERROR if no command is available.
  -------------------------------------------------------------------------*/
  if( q_cnt( &ps_rm_cmd_q ) == 0)
  {
    LOG_MSG_ERROR_0("ps_rm_process_cmd(): "
                    "No NAT command to process" );
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Process all commands in the command q.

    Get each command item in sequence (from the head) of the command queue
    and call the correponding handler with appropriate parameters.
  -------------------------------------------------------------------------*/
  processed = 0;
  while( (cmd_ptr = ( ps_rm_cmd_type * ) q_get( &ps_rm_cmd_q )) != NULL )
  {
    if( cmd_ptr->cmd >= PS_RM_MAX_DEFINED_CMD_TYPES )
    {
      LOG_MSG_ERROR_1("ps_rm_process_cmd(): "
                      "Invalid cmd id 0x%x in Q", cmd_ptr->cmd);
      q_put( &ps_rm_cmd_free_q, &cmd_ptr->link );
      continue;
    }
    processed++;
    ps_rm_cmd_handler[cmd_ptr->cmd]( cmd_ptr->cmd, cmd_ptr->user_data_ptr );

    q_put( &ps_rm_cmd_free_q, &cmd_ptr->link );
  }

  /*-------------------------------------------------------------------------
    Intlock to assure atomic operation below.

    Note this also ensures no other task will be scheduled since
    q_cnt() and rex_clr_sigs() cannot cause task context switch.
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&global_ps_rm_crit_section);

  /*-------------------------------------------------------------------------
    Clear the cmd queue signal (which may have been set if a command was
    issued after the rex_clr_sigs() in the outer NAT processing loop)
    Check if any commands are left before doing this, since a command may
    have been posted between closing brace and INTLOCK above.
  -------------------------------------------------------------------------*/
  if( q_cnt( &ps_rm_cmd_q ) == 0 )
  {
    PS_RM_CLR_SIGNAL(PS_RM_CMD_Q_SIGNAL);
  }

  /*-------------------------------------------------------------------------
    End atomic operation - Restore interrupts.
  -------------------------------------------------------------------------*/
  PS_LEAVE_CRIT_SECTION(&global_ps_rm_crit_section);

  /*-------------------------------------------------------------------------
    For debugging, show if we just processed a ton of commands
  -------------------------------------------------------------------------*/
  if( processed > ps_rm_cmds_proc_hi_score )
  {
    ps_rm_cmds_proc_hi_score = processed;
    LOG_MSG_INFO2_1 ("ps_rm_process_cmd(): "
                     "New high # NAT commands processed: %d", processed);
  }

  /*-------------------------------------------------------------------------
    Always return true to clear NAT_CMD_SIG from loop_sigs in case command
    was sent after clearing signal above, so that we never get into the
    situation where we process a command due to loop_sigs, emptying the
    cmd queue, and yet the NAT_CMD_SIG is set, leading to LOG_MSG_ERROR above.
  -------------------------------------------------------------------------*/
  return TRUE;

} /* ps_rm_process_cmd() */


