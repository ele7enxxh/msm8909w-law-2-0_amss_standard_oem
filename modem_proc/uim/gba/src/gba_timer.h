#ifndef GBA_TIMER_H
#define GBA_TIMER_H
/*===========================================================================


            G B A   T I M E R   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/26/15   tl      Initial revision
===========================================================================*/

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
);

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
);

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
);

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
);

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
);

#endif /* GBA_TIMER_H */

