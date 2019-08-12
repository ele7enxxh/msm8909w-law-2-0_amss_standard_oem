/*===========================================================================
  FILE: ds_sig_svc.c

  OVERVIEW: This file implements the services provided by the DS_SIG task.

  DEPENDENCIES: None

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/ds_sig_svc.c#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2009-08-17 hm  Created module.

===========================================================================*/

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ds_sig_svc.h"
#include "ds_sig_task.h"
#include "ds_sig_taski.h"
#include "ps_crit_sect.h"
#include "ds_Utils_DebugMsg.h"
#include "rcinit_rex.h"

#ifdef FEATURE_MODEM_RCINIT_PHASE2
extern rex_tcb_type   * rex_ds_sig_tcb;
#endif
/*===========================================================================

                          PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

ds_sig_cmd_handler_type ds_sig_set_cmd_handler
(
  ds_sig_cmd_enum_type      cmd,         /* Command name                   */
  ds_sig_cmd_handler_type   cmd_handler  /* Cmd handler function pointer   */
)
{
  ds_sig_cmd_handler_type old_handler;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION (&ds_sig_crit_sect);

  if (DS_SIG_CMD_MIN > cmd || DS_SIG_CMD_MAX <= cmd)
  {
    LOG_MSG_ERROR_1 ("ds_sig_set_cmd_handler(): "
                     "Inv cmd %d", cmd);
    ASSERT (0);
    return NULL;
  }

  old_handler = ds_sigi_cmd_handler[cmd];

  ds_sigi_cmd_handler[cmd] = (cmd_handler != NULL) ?
                              cmd_handler :
                              ds_sigi_default_cmd_handler;

  PS_LEAVE_CRIT_SECTION (&ds_sig_crit_sect);

  return old_handler;
} /* ds_sig_set_cmd_handler() */



void ds_sig_send_cmd
(
  ds_sig_cmd_enum_type   cmd,             /* Command to be processed       */
  void                  *user_data_ptr    /* Command specific user data    */
)
{
  ds_sigi_cmd_type      *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (DS_SIG_CMD_MIN > cmd || DS_SIG_CMD_MAX <= cmd)
  {
    LOG_MSG_ERROR_1 ("ds_sig_send_cmd(): "
                     "Inv cmd %d", cmd);
    ASSERT (0);
    return;
  }

  /*-------------------------------------------------------------------------
    Get a cmd buffer from the free queue, call error routine if no links
    available, else copy buffer to cmd queue and signal DS_SIG task
  -------------------------------------------------------------------------*/
  cmd_ptr = (ds_sigi_cmd_type *) q_get (&ds_sigi_cmd_free_q);

  if (NULL == cmd_ptr)
  {
    DATA_ERR_FATAL("ds_sig_send_cmd(): DS_SIG task cmd buffers unavailable");
  }
  else
  {
    cmd_ptr->cmd = cmd;
    cmd_ptr->user_data_ptr = user_data_ptr;

    /*-----------------------------------------------------------------------
      Making the operation atomic so that race conditions do not occur
    -----------------------------------------------------------------------*/
    PS_ENTER_CRIT_SECTION (&ds_sig_crit_sect);
    q_put (&ds_sigi_cmd_q, &cmd_ptr->link);
    DS_SIGI_SET_SIGNAL (DS_SIG_CMD_Q_SIGNAL);
    PS_LEAVE_CRIT_SECTION (&ds_sig_crit_sect);
  }
} /* ds_sig_send_cmd() */



ds_sig_signal_handler_type  ds_sig_set_signal_handler
(
  ds_sig_signal_enum_type     sig,          /* Signal assoc with handler   */
  ds_sig_signal_handler_type  sig_handler,  /* Signal handler callback     */
  void                       *user_data_ptr /* User data for the handler   */
)
{
  ds_sig_signal_handler_type     old_handler;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (DS_SIG_SIGNAL_MIN > sig || DS_SIG_SIGNAL_MAX <= sig)
  {
    LOG_MSG_ERROR_1 ("ds_sig_set_signal_handler(): "
                     "Inv signal %d", sig);
    ASSERT (0);
    return NULL;
  }

  PS_ENTER_CRIT_SECTION (&ds_sig_crit_sect);

  old_handler = ds_sigi_sig_handler[sig].sig_handler;
  ds_sigi_sig_handler[sig].sig_handler = (sig_handler != NULL) ?
                                          sig_handler :
                                          ds_sigi_default_sig_handler;

  ds_sigi_sig_handler[sig].user_data_ptr = user_data_ptr;

  PS_LEAVE_CRIT_SECTION (&ds_sig_crit_sect);

  return old_handler;
} /* ds_sig_set_sig_handler() */

void  ds_sig_enable_signal
(
  ds_sig_signal_enum_type sig        /* Signal to be enabled               */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (DS_SIG_SIGNAL_MIN > sig || DS_SIG_SIGNAL_MAX <= sig)
  {
    LOG_MSG_ERROR_1 ("ds_sig_enable_signal(): "
                     "Inv signal %d", sig);
    ASSERT (0);
    return;
  }

  PS_ENTER_CRIT_SECTION (&ds_sig_crit_sect);

  if (ds_sigi_sig_handler[sig].sig_handler == 
      ds_sigi_default_sig_handler)
  {
    LOG_MSG_ERROR_1 ("ds_sig_enable_signal(): "
                     "Signal %d enabled without a handler", sig);
  }

  ds_sigi_enabled_sig_mask |= ((rex_sigs_type) 1 << (rex_sigs_type) sig);

  PS_LEAVE_CRIT_SECTION (&ds_sig_crit_sect);

  DS_SIGI_SET_SIGNAL (DS_SIG_SIG_MASK_CHANGE_SIGNAL);
} /* ds_sig_enable_signal() */

void ds_sig_disable_signal
(
  ds_sig_signal_enum_type sig                 /* Signal to be disabled     */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION (&ds_sig_crit_sect);

  ds_sigi_enabled_sig_mask &= ~((rex_sigs_type) 1 << (rex_sigs_type) sig);

  PS_LEAVE_CRIT_SECTION (&ds_sig_crit_sect);

} /* ds_sig_disable_signal() */

boolean ds_sig_is_current_task
(
  void 
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  return (boolean) (rex_self() == rex_ds_sig_tcb);
#else
  return (boolean) (rex_self() == &ds_sig_tcb);
#endif

} /* ds_sig_is_current_task() */

