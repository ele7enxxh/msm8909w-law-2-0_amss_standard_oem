#ifndef GBA_STATE_H
#define GBA_STATE_H
/*===========================================================================


            G B A   S T A T E   H E A D E R


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
07/09/15   tkl     Check EF GBABP for Ks validity
04/06/15   tl      Add support for GBA_U
01/26/15   tl      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gba_lib.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

/* ----------------------------------------------------------------------------
   ENUM:      GBA_STATE_ENUM_TYPE

   DESCRIPTION:
     This enum indicates the current state of the GBA module
-------------------------------------------------------------------------------*/
typedef enum {
  /* Idle state in which no command is processing. */
  GBA_STATE_IDLE,
  /* GBA registers with MMGSDI and requests the IMPI. */
  GBA_STATE_IMPI,
  /* Retrieve BTID and Lifetime data from EF GBABP, applicable to GBA U only */
  GBA_STATE_GBABP,
  /* Construct and send the initial HTTP request to BSF. Wait for unauth response. */
  GBA_STATE_INITIAL_HTTP,
  /* Authenticate with the UICC. */
  GBA_STATE_UICC_AUTHENTICATION,
  /* Send final HTTP request to BSF with result of authentication. */
  GBA_STATE_FINAL_HTTP,
  /* Generate the Ks ext NAF through the UICC. */
  GBA_STATE_GEN_KS_EXT_NAF,
  /* Generate the TMPI through the UICC. */
  GBA_STATE_GEN_TMPI,
  /* Terminal state to indicate command is complete or error occured. */
  GBA_STATE_DONE
} gba_state_enum_type;

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
);

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
);

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
);

#endif /* GBA_STATE_H */

