/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    G B A   M A I N   T A S K


GENERAL DESCRIPTION

  This file contains the main GBA task and supporting functions.

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     -----------------------------------------------------------
08/12/15   ar      Added NV cleanup
07/22/15   lm      Add task ready signal and offtarget includes
07/03/15   tkl     Added support for custom fields
03/27/15   tl      Add support for GBA cancel
03/17/15   tl      Include rcinit_rex.h to prevent strict compiler errors
03/11/15   tl      Add caching of the module tcb pointer
01/26/15   tl      Initial Version

=============================================================================*/

/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_msg.h"
#include "rex.h"
#include "rex_types.h"
#include "rcinit.h"
#include "rcinit_rex.h"

#include "gba_bootstrap.h"
#include "gba_cmd.h"
#include "gba_http.h"
#include "gba_lib.h"
#include "gba_mmgsdi.h"
#include "gba_nv.h"
#include "gba_platform.h"
#include "gba_response.h"
#include "gba_state.h"
#include "gba_storage.h"
#include "gba_timer.h"
#include "gba_util.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

/* Flag indicating when GBA task has stopped */
boolean                    gba_task_stopped             = FALSE;

/* Critical section responsible for protecting data in the GBA task */
rex_crit_sect_type         gba_task_state_crit_sect;
rex_crit_sect_type         gba_data_crit_sect;

/* Pointer to the GBA task tcb. Initialized with the task. */
rex_tcb_type             * gba_tcb_ptr                  = NULL;

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*===========================================================================
FUNCTION GBA_WAIT

DESCRIPTION
  Main wait routine for the GBA task.

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals set (the current state
  of all signals). The calling unit can then check for the reason for
  exiting the wait.

SIDE EFFECTS
  None
===========================================================================*/
rex_sigs_type gba_wait (
  rex_sigs_type requested_mask
)
{
  rex_sigs_type          rex_signals_mask = 0x00;  /* Mask of signals returned by REX */

  /* --------------------------------------------------------------------
  Loop until the current signals from REX match with one or more of
  the requested signals.
  -------------------------------------------------------------------- */
  do
  {
    /* ---------------------------------------------------------------
    Perform the real wait to wait for a command...or to force a
    context switch to allow higher priority tasks to run.
    ---------------------------------------------------------------- */
    rex_signals_mask = rex_wait(requested_mask);

    /* do while mask requested does not match current mask */
  } while ((rex_signals_mask & requested_mask) == 0);

  return rex_signals_mask;
} /* gba_wait */


/*===========================================================================
FUNCTION GBA_HANDLE_STOP_SIG

DESCRIPTION
  This function is called to process a stop signal received by the GBA
  task.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void gba_handle_stop_sig (
  void
)
{
  GBA_MSG_HIGH_0("GBA: GOT TASK_STOP_SIG");

   /* Clear signal */
  (void) rex_clr_sigs(rex_self(), GBA_TASK_STOP_SIG);

  GBA_ENTER_TASK_STATE_CRIT_SECT;
  gba_task_stopped = TRUE;
  GBA_LEAVE_TASK_STATE_CRIT_SECT;

  gba_cmd_cleanup();
  gba_bootstrap_cleanup();
  gba_mmgsdi_cleanup();
  gba_http_cleanup();
  gba_nv_cleanup();
  gba_timer_cleanup();
} /* gba_handle_stop_sig */


/*===========================================================================
FUNCTION GBA_TASK_INIT_POST_STARTUP

DESCRIPTION
  Function called to initialize the different GBA modules after RCINIT
  handshake.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void gba_task_init_post_startup (
  void
)
{
  gba_response_reset();
  gba_state_reset();

  gba_storage_init();
  gba_bootstrap_init();
  gba_http_init();
  gba_mmgsdi_init();
  gba_timer_init();
} /* gba_task_init_post_startup */


/* ==========================================================================
FUNCTION GBA_RESET

DESCRIPTION:
  Performs a reset of all modules.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
==========================================================================*/
void gba_reset (
  void
)
{
  gba_response_reset();
  gba_bootstrap_reset();
  gba_state_reset();
  gba_http_cleanup();
  gba_mmgsdi_cleanup();
  gba_nv_cleanup();
} /* gba_reset */


/*===========================================================================
FUNCTION GBA_TASK_INIT

DESCRIPTION
  Function called to initialize the GBA task queue and globals

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
  On targets with RCInit enabled, this function is called in RCInit context.
  Therefore, it should not include references to TCBs, NV
  operations, etc.
===========================================================================*/
void gba_task_init (
  void
)
{
  gba_task_stopped = FALSE;

  rex_init_crit_sect(&gba_task_state_crit_sect);
  rex_init_crit_sect(&gba_data_crit_sect);

  gba_tcb_ptr = rcinit_lookup_rextask("gba");

  gba_cmd_init();
} /* gba_task_init */


/*===========================================================================
FUNCTION GBA_MAIN

DESCRIPTION
  Entry point for the GBA task. This function performs task initialization,
  then sits in an infinite loop, waiting on an input queue, and responding
  to messages received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_main (
  dword dummy
)
{
  rex_sigs_type                       sigs         = GBA_CMD_SIG                    |
                                                     GBA_MMGSDI_CMD_SIG             |
                                                     GBA_HTTP_CMD_SIG               |
                                                     GBA_BOOTSTRAP_CMD_SIG          |
                                                     GBA_SANITY_TIMER_EXPIRE_SIG    |
                                                     GBA_TASK_STOP_SIG;
  rex_sigs_type                       sigs_rcvd    = 0;
  RCEVT_SIGEX_SIGREX                  stop_sig;
  RCINIT_GROUP                        gba_group;

  (void)dummy;

  /* Clear all signals */
  (void) rex_clr_sigs(UIM_GBA_TCB, (rex_sigs_type)~0);

  stop_sig.signal   = UIM_GBA_TCB;
  stop_sig.mask     = GBA_TASK_STOP_SIG;
  gba_group         = rcinit_lookup_group_rextask(UIM_GBA_TCB);

  rcinit_register_term_group(gba_group, RCEVT_SIGEX_TYPE_SIGREX, &stop_sig);
  rcinit_handshake_startup();

  (void)rcevt_signal_name("GBA:READY");

  gba_task_init_post_startup();

  /* Main Task Loop */
  for (;;)
  {
    /* Wait on signal */
    sigs_rcvd = gba_wait(sigs);

    /*-----------------------------------------------------------------------------
                          Process Signal - GBA_TASK_STOP_SIG
    -----------------------------------------------------------------------------*/
    if (sigs_rcvd & GBA_TASK_STOP_SIG)
    {
      gba_handle_stop_sig();
      break;
    } /* end if GBA_TASK_STOP_SIG */

    GBA_ENTER_DATA_CRIT_SECT;
    /*-----------------------------------------------------------------------------
                          Process Signal - GBA_SANITY_TIMER_EXPIRE_SIG
    -----------------------------------------------------------------------------*/
    if (sigs_rcvd & GBA_SANITY_TIMER_EXPIRE_SIG)
    {
      gba_handle_sanity_timer_expire_sig();
    } /* end if GBA_SANITY_TIMER_EXPIRE_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - GBA_MMGSDI_CMD_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & GBA_MMGSDI_CMD_SIG)
    {
      gba_mmgsdi_handle_cmd_sig();
    } /* end if GBA_MMGSDI_CMD_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - GBA_HTTP_CMD_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & GBA_HTTP_CMD_SIG)
    {
      gba_http_handle_cmd_sig();
    } /* end if GBA_HTTP_CMD_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - GBA_BOOTSTRAP_CMD_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & GBA_BOOTSTRAP_CMD_SIG)
    {
      gba_bootstrap_handle_cmd_sig();
    } /* end if GBA_BOOTSTRAP_CMD_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - GBA_CMD_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & GBA_CMD_SIG)
    {
      gba_cmd_handle_cmd_sig();
    } /* end if GBA_CMD_SIG */

    else
    {
      /* Reset sigs by masking out defined sigs and clearing out bad sigs */
      (void)rex_clr_sigs(UIM_GBA_TCB, (sigs_rcvd & ~sigs));
    } /* Invalid signal in GBA task */
    GBA_LEAVE_DATA_CRIT_SECT;
  } /* end for(;;) */

  rcinit_unregister_term_group(gba_group, RCEVT_SIGEX_TYPE_SIGREX, &stop_sig);
  rcinit_handshake_term();
} /* gba_main */

