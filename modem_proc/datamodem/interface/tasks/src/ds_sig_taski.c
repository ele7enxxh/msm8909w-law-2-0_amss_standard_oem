/*===========================================================================
  FILE: ds_sig_taski.c

  OVERVIEW: 
    DataCommon Control Task internal API file. Contains APIs and definitions
    used internal to DS_SIG Task processing. 

  EXTERNALIZED FUNCTIONS
    ds_sigi_default_sig_handler()
      Implicit handler for signals processed by DS_SIG Task.

    ds_sigi_default_cmd_handler()
      Implicit handler for commands processed by DS_SIG Task.

    ds_sigi_powerup()
      Cold start called before DS_SIG task is started.

    ds_sigi_init()
      After task start initialization of the DS_SIG task.

    ds_sigi_process_cmd()
      Processes command signals in DS_SIG Task.

  Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/ds_sig_taski.c#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  01/10/11   ss  Cleaning up of Globals for thread safety in Q6 Free 
                 Floating environment.
  2010-09-27 ss  DOG changes on DS_SIG.  
  2009-08-17 hm  Created module.

===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ds_sig_task.h"
#include "ds_sig_svc.h"
#include "ds_sig_taski.h"
#include "task.h"
#include "ps_crit_sect.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================

                            PUBLIC DATA DEFINITIONS

===========================================================================*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
extern rex_tcb_type   * rex_ds_sig_tcb;
#endif
/*--------------------------------------------------------------------------
  DS_SIG's own critical section
---------------------------------------------------------------------------*/
ps_crit_sect_type ds_sig_crit_sect;

/*--------------------------------------------------------------------------
  Command Buffers. Configuration of DS_SIGI_CMD_BUF_CNT is heurestic.
---------------------------------------------------------------------------*/
#define DS_SIGI_CMD_BUF_CNT   (100)
ds_sigi_cmd_type ds_sigi_cmd_buf[DS_SIGI_CMD_BUF_CNT];

/*--------------------------------------------------------------------------
  Command Queues.
---------------------------------------------------------------------------*/
q_type ds_sigi_cmd_q;
q_type ds_sigi_cmd_free_q;

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers
---------------------------------------------------------------------------*/
ds_sigi_sig_handler_info_type ds_sigi_sig_handler[DS_SIG_SIGNAL_MAX];
ds_sig_cmd_handler_type       ds_sigi_cmd_handler[DS_SIG_CMD_MAX];

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled signals
---------------------------------------------------------------------------*/
rex_sigs_type ds_sigi_enabled_sig_mask;


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/
boolean ds_sigi_default_sig_handler
(
  ds_sig_signal_enum_type sig,             /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_ERROR_1("ds_sigi_default_sig_handler(): "
                  "Signal %d, no handler registered", sig);

  /*-------------------------------------------------------------------------
    Return TRUE since we don't want this signal to be enqueued again.
  -------------------------------------------------------------------------*/
  return TRUE;
} /* ds_sigi_default_sig_handler() */

void ds_sigi_default_cmd_handler
(
  ds_sig_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_ERROR_1("ds_sigi_default_cmd_handler(): "
                  "Command %d, no handler registered", cmd);

} /* ds_sigi_default_cmd_handler() */

void ds_sigi_powerup
(
  void
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_0 ("ds_sigi_powerup(): "
                   "DS_SIG Task Powerup");

  /*------------------------------------------------------------------------
    Init the DS_SIG task critical section.
  -------------------------------------------------------------------------*/
  PS_INIT_CRIT_SECTION (&ds_sig_crit_sect);

  /*-------------------------------------------------------------------------
    Initialize the command queues and link items onto the command free queue
  -------------------------------------------------------------------------*/
  (void) q_init (&ds_sigi_cmd_q);
  (void) q_init (&ds_sigi_cmd_free_q);

  for (i = 0; i < DS_SIGI_CMD_BUF_CNT; i++)
  {
    q_put(&ds_sigi_cmd_free_q, q_link(&ds_sigi_cmd_buf[i], &ds_sigi_cmd_buf[i].link));
  }

  /*------------------------------------------------------------------------
    Initialize all the signal and cmd handlers
  -------------------------------------------------------------------------*/
  for (i = 0; i < DS_SIG_SIGNAL_MAX; i++)
  {
    ds_sigi_sig_handler[i].sig_handler = ds_sigi_default_sig_handler;
    ds_sigi_sig_handler[i].user_data_ptr = NULL;
  }

  for (i = 0; i < DS_SIG_CMD_MAX; i++)
  {
    ds_sigi_cmd_handler[i] = ds_sigi_default_cmd_handler;
  }

  /*------------------------------------------------------------------------
    Set signal mask for operation of all signals internal to DS_SIG Task
  -------------------------------------------------------------------------*/
  ds_sigi_enabled_sig_mask = 
    ((rex_sigs_type) 1 << (rex_sigs_type) DS_SIG_TASK_STOP_SIGNAL)      |
    ((rex_sigs_type) 1 << (rex_sigs_type) DS_SIG_TASK_OFFLINE_SIGNAL)   |
    ((rex_sigs_type) 1 << (rex_sigs_type) DS_SIG_CMD_Q_SIGNAL)          |
    ((rex_sigs_type) 1 << (rex_sigs_type) DS_SIG_DOG_HB_REPORT_SIGNAL)  |
    ((rex_sigs_type) 1 << (rex_sigs_type) DS_SIG_SIG_MASK_CHANGE_SIGNAL);

} /* ds_sigi_powerup() */

void ds_sigi_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_0 ("ds_sigi_init(): "
                   "DS_SIG After task start init");

} /* ds_sigi_init() */

boolean  ds_sigi_process_cmd
(
  void
)
{
  static byte         ds_sig_cmds_proc_hi_score = 0;
  ds_sigi_cmd_type   *cmd_ptr;
  byte                processed;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    LOG_MSG_ERROR if no command is available.
  -------------------------------------------------------------------------*/
  if (q_cnt(&ds_sigi_cmd_q) == 0)
  {
    DATA_ERR_FATAL("ds_sigi_process_cmd(): No DS_SIG command to process");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Process all commands in the command q.

    Get each command item in sequence (from the head) of the command queue
    and call the correponding handler with appropriate parameters.
  -------------------------------------------------------------------------*/
  processed = 0;
  while ((cmd_ptr = (ds_sigi_cmd_type *) q_get (&ds_sigi_cmd_q)) != NULL)
  {
    if (cmd_ptr->cmd >= DS_SIG_CMD_MAX)
    {
      LOG_MSG_ERROR_1("ds_sigi_process_cmd(): "
                      "Invalid cmd id 0x%x in Q", cmd_ptr->cmd);
      ASSERT (0);
      q_put(&ds_sigi_cmd_free_q, &cmd_ptr->link);
      continue;
    }

    processed++;
    ds_sigi_cmd_handler[cmd_ptr->cmd] (cmd_ptr->cmd, cmd_ptr->user_data_ptr);

    q_put(&ds_sigi_cmd_free_q, &cmd_ptr->link);
  }

  /*-------------------------------------------------------------------------
    Intlock to assure atomic operation below.

    Note this also ensures no other task will be scheduled since
    q_cnt() and rex_clr_sigs() cannot cause task context switch.
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION (&ds_sig_crit_sect);

  /*-------------------------------------------------------------------------
    Clear the cmd queue signal (which may have been set if a command was
    issued after the rex_clr_sigs() in the outer DS_SIG processing loop)
    Check if any commands are left before doing this, since a command may
    have been posted between closing brace and INTLOCK above.
  -------------------------------------------------------------------------*/
  if (q_cnt(&ds_sigi_cmd_q) == 0)
  {
    DS_SIGI_CLR_SIGNAL(DS_SIG_CMD_Q_SIGNAL);
  }

  /*-------------------------------------------------------------------------
    End atomic operation - Restore interrupts.
  -------------------------------------------------------------------------*/
  PS_LEAVE_CRIT_SECTION (&ds_sig_crit_sect);

  /*-------------------------------------------------------------------------
    For debugging, show if we just processed a ton of commands
  -------------------------------------------------------------------------*/
  if (processed > ds_sig_cmds_proc_hi_score)
  {
    ds_sig_cmds_proc_hi_score = processed;
    LOG_MSG_INFO1_1 ("ds_sigi_process_cmd(): "
                     "New high # DS_SIG cmds processed: %d", processed);
  }

  /*-------------------------------------------------------------------------
    Always return true to clear DS_SIG_CMD_SIG from loop_sigs in case command
    was sent after clearing signal above, so that we never get into the
    situation where we process a command due to loop_sigs, emptying the
    cmd queue, and yet the DS_SIG_CMD_SIG is set, leading to LOG_MSG_ERROR above.
  -------------------------------------------------------------------------*/
  return TRUE;

} /* ds_sigi_process_cmd() */


