/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G B A   S T A T E   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the GBA state machine functions.

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
01/26/15   tl      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"

#include "gba_lib.h"
#include "gba_platform.h"
#include "gba_state.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
static gba_state_enum_type       gba_state = GBA_STATE_IDLE;

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION GBA_STATE_CHANGE

DESCRIPTION
  Function changes the GBA state.

DEPENDENCIES
  None

RETURN VALUE
  gba_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_result_enum_type gba_state_change (
  gba_state_enum_type      current_state,
  gba_state_enum_type      next_state
)
{
  if(current_state != gba_state)
  {
    GBA_MSG_ERR_2("GBA state mismatch, curr state: 0x%x, global state: 0x%x",
                  current_state, gba_state);
    return GBA_GENERIC_ERROR;
  }

  GBA_MSG_MED_2("GBA state change, old state: 0x%x, new state: 0x%x",
                gba_state, next_state);

  gba_state = next_state;
  return GBA_SUCCESS;
} /* gba_state_change */


/*===========================================================================
FUNCTION GBA_STATE_CURRENT

DESCRIPTION
  Function returns the current GBA state.

DEPENDENCIES
  None

RETURN VALUE
  gba_state_enum_type

SIDE EFFECTS
  None
===========================================================================*/
gba_state_enum_type gba_state_current (
  void
)
{
  return gba_state;
} /* gba_state_current */


/*===========================================================================
FUNCTION GBA_STATE_RESET

DESCRIPTION
  Function called to reset the GBA state machine

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
void gba_state_reset (
  void
)
{
  gba_state = GBA_STATE_IDLE;
} /* gba_state_init */

