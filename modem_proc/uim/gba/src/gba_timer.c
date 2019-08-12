/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G B A   T I M E R   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the GBA timing functions.

                        COPYRIGHT INFORMATION

Copyright (c) 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/27/15   tl      Add support for GBA cancel
01/26/15   tl      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "rex.h"

#include "gba.h"
#include "gba_bootstrap.h"
#include "gba_cmd.h"
#include "gba_lib.h"
#include "gba_platform.h"
#include "gba_response.h"
#include "gba_state.h"
#include "gba_timer.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

#define GBA_SANITY_TIMER_VALUE             30000 /* 30 seconds */

static rex_timer_type                      gba_sanity_timer;

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION GBA_TIMER_CLEANUP

DESCRIPTION
  Undefines the GBA sanity timer.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_timer_cleanup (
  void
)
{
  (void)rex_undef_timer(&gba_sanity_timer);
} /* gba_timer_cleanup */


/*===========================================================================
FUNCTION GBA_CLEAR_TIMER

DESCRIPTION
  Function clears the GBA sanity timer.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_clear_timer (
  void
)
{
  (void)rex_clr_timer(&gba_sanity_timer);
} /* gba_clear_timer */


/*===========================================================================
FUNCTION GBA_SET_TIMER

DESCRIPTION
  Function sets the GBA sanity timer.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_set_timer (
  void
)
{
  (void)rex_clr_sigs(rex_self(), GBA_SANITY_TIMER_EXPIRE_SIG);

  (void)rex_set_timer(&gba_sanity_timer, GBA_SANITY_TIMER_VALUE);
} /* gba_set_timer */


/*===========================================================================
FUNCTION GBA_HANDLE_SANITY_TIMER_EXPIRE_SIG

DESCRIPTION
  This function, called in gba_task, is called to process sanity timer
  expiration signal.

DEPENDENCIES
  gba_task must be finished initialization.

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_handle_sanity_timer_expire_sig (
  void
)
{
  GBA_MSG_HIGH_0("GBA_SANITY_TIMER_EXPIRE_SIG");

  /* Clear the signal */
  (void)rex_clr_sigs(rex_self(), GBA_SANITY_TIMER_EXPIRE_SIG);

  gba_send_response(GBA_TIMEOUT,
                    NULL);

  gba_reset();

  /* Set the signal if there are one or more commands still in queue
     and GBA is currently in IDLE state. */
  gba_cmd_set_sig();
} /* gba_handle_sanity_timer_expire_sig */


/*===========================================================================
FUNCTION GBA_TIMER_INIT

DESCRIPTION
  Function initializes and defines the GBA sanity timer.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_timer_init (
  void
)
{
  rex_def_timer (&gba_sanity_timer, UIM_GBA_TCB, GBA_SANITY_TIMER_EXPIRE_SIG);
} /* gba_timer_init */

