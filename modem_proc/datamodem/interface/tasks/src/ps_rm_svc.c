/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     P S _ R M _ S V C . C

DESCRIPTION


EXTERNALIZED FUNCTIONS
  ps_rm_enable_sig()
    Adds specific signal for PS_RM task to process.

  ps_rm_disable_sig()
    Removes specific signal from PS_RM task processing.

  ps_rm_set_sig_handler()
    Sets signal handler for specific signal processed in PS_RM task.

  ps_rm_get_cmd_data_buf()
    Allocs a new command buffer to be used by caller.

  ps_rm_free_cmd_data_buf()
    Frees the PS_RM command buffer.

  ps_rm_send_cmd()
    Post a command to be handled in PS_RM task.

  ps_rm_set_cmd_handler()
    Sets command handler for command handled in PS_RM task.


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/tasks/src/ps_rm_svc.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/07/10    am     created module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#include "msg.h"
#include "err.h"
#include "dog.h"
#include "rex.h"

#include "ps_rm_defs.h"
#include "ps_rm_taski.h"
#include "ps_rm_svc.h"
#include "ds_Utils_DebugMsg.h"
#include "rcinit_rex.h"
#include "dog_hb_rex.h"

/*===========================================================================

                  EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/
/*--------------------------------------------------------------------------
  Dog data structures [extern defs] for PS RM task
---------------------------------------------------------------------------*/
extern dog_report_type ps_rm_dog_rpt_id;                /*! dog report ID */

/*===========================================================================

FUNCTION PS_RM_ENABLE_SIG()

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void 
ps_rm_enable_sig
(
  ps_rm_sig_enum_type sig              /* Signal to be enabled               */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ps_rm_sig_handler[sig].sig_handler == ps_rm_default_sig_handler )
  {
    LOG_MSG_ERROR_1("ps_rm_enable_sig(): "
                    "Signal %d enabled without a handler", sig );
  }

  ps_rm_enabled_sig_mask |= ( (rex_sigs_type) 1 << (rex_sigs_type) sig );

  PS_RM_SET_SIGNAL( PS_RM_SIG_MASK_CHANGE_SIGNAL );
} /* ps_rm_enable_sig() */

/*===========================================================================

FUNCTION PS_RM_DISABLE_SIG()

DESCRIPTION
  This function disables a signal handling by removing it from the
  currently enabled signal mask.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void 
ps_rm_disable_sig
(
  ps_rm_sig_enum_type sig                       /* Signal to be disabled     */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ps_rm_enabled_sig_mask &= ~( (rex_sigs_type) 1 << (rex_sigs_type) sig );
} /* ps_rm_disable_sig() */


/*===========================================================================

FUNCTION PS_RM_SET_SIG_HANDLER()

DESCRIPTION
  This function sets a handler for the specified signal.

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered signal handler

SIDE EFFECTS
  None.

===========================================================================*/

ps_rm_sig_handler_type 
ps_rm_set_sig_handler
(
  ps_rm_sig_enum_type sig,             /* Signal assocaited with handler     */
  ps_rm_sig_handler_type sig_handler,  /* Signal handler function pointer    */
  void *user_data_ptr                /* Param to be passed to the handler  */
)
{
  ps_rm_sig_handler_type old_handler = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (sig > 0 && sig < PS_RM_MAX_SIGNALS)
  {
    old_handler = ps_rm_sig_handler[sig].sig_handler;

    ps_rm_sig_handler[sig].sig_handler = ( sig_handler != NULL) ?
                                          sig_handler :
                                          ps_rm_default_sig_handler;

    ps_rm_sig_handler[sig].user_data_ptr = user_data_ptr;
  }

  return old_handler;
} /* ps_rm_set_sig_handler() */


/*===========================================================================

FUNCTION PS_RM_SEND_CMD()

DESCRIPTION
  This function posts a cmd for processing in PS_RM task context.  The cmd is
  processed by calling the registered cmd handler, if any.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void 
ps_rm_send_cmd
(
  ps_rm_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
)
{
  ps_rm_cmd_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get a cmd buffer from the free queue, call error routine if no links
    available, else copy buffer to cmd queue and signal PS_RM task
  -------------------------------------------------------------------------*/
  if( ( cmd_ptr = ( ps_rm_cmd_type * ) q_get( &ps_rm_cmd_free_q ) ) == NULL )
  {
    LOG_MSG_ERROR_0("ps_rm_send_cmd(): "
                    "PS_RM task cmd buffers unavailable" );
  }
  else
  {
    cmd_ptr->cmd = cmd;
    cmd_ptr->user_data_ptr = user_data_ptr;

    /*-----------------------------------------------------------------------
      Making the operation atomic so that race conditions do not occur
    -----------------------------------------------------------------------*/
    PS_ENTER_CRIT_SECTION(&global_ps_rm_crit_section);
    q_put( &ps_rm_cmd_q, &cmd_ptr->link );
    PS_RM_SET_SIGNAL( PS_RM_CMD_Q_SIGNAL );
    PS_LEAVE_CRIT_SECTION(&global_ps_rm_crit_section);
  }
} /* ps_rm_send_cmd() */


/*===========================================================================

FUNCTION PS_RM_SET_CMD_HANDLER()

DESCRIPTION
  This function sets a handler for the specified cmd type.  The cmd handling
  can be disabled by setting a NULL handler

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered cmd handler

SIDE EFFECTS
  None.

===========================================================================*/

ps_rm_cmd_handler_type 
ps_rm_set_cmd_handler
(
  ps_rm_cmd_enum_type cmd,              /* Command name                      */
  ps_rm_cmd_handler_type cmd_handler    /* Cmd handler function pointer      */
)
{
  ps_rm_cmd_handler_type old_handler;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  old_handler = ps_rm_cmd_handler[cmd];

  ps_rm_cmd_handler[cmd] = ( cmd_handler != NULL) ?
                          cmd_handler :
                          ps_rm_default_cmd_handler;

  return old_handler;
} /* ps_rm_set_cmd_handler() */


/*===========================================================================

FUNCTION PS_RM_WAIT()

DESCRIPTION
  This function performs the PS_RM Task waiting function. This function will
  not return until at least one signal from the requested mask is set.
  The watchdog is also kicked at least once.

DEPENDENCIES
  None

RETURN VALUE
  The signal mask actually set out of the requested mask which terminated
  the wait.

SIDE EFFECTS
  All the set signals in the requested mask are cleared

===========================================================================*/
rex_sigs_type
ps_rm_wait
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
  set_sigs = rex_get_sigs( rex_ps_rm_tcb );
#else
  set_sigs = rex_get_sigs( &ps_rm_tcb );
#endif

  while ( (set_sigs & requested_mask) == 0 )
  {
    set_sigs = rex_wait( requested_mask |
                         (  1 << (rex_sigs_type)PS_RM_DOG_HB_REPORT_SIGNAL ) );

    /*-----------------------------------------------------------------------
      If dog heart beat received, ack the beat. Do ping<->pong.
    -----------------------------------------------------------------------*/
    if( set_sigs & ( 1 << (rex_sigs_type)PS_RM_DOG_HB_REPORT_SIGNAL ) )
    {
#ifdef FEATURE_MODEM_RCINIT_PHASE2
      (void)rex_clr_sigs( rex_ps_rm_tcb,
                    (  1 << (rex_sigs_type)PS_RM_DOG_HB_REPORT_SIGNAL ) );
#else
      (void)rex_clr_sigs( &ps_rm_tcb,
                    (  1 << (rex_sigs_type)PS_RM_DOG_HB_REPORT_SIGNAL ) );
#endif
      dog_hb_report( ps_rm_dog_rpt_id );
    }
  }

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  (void)rex_clr_sigs( rex_ps_rm_tcb, ( set_sigs & requested_mask ) );
#else
  (void)rex_clr_sigs( &ps_rm_tcb, ( set_sigs & requested_mask ) );
#endif
  return ( set_sigs & requested_mask );
} /* ps_rm_wait() */

