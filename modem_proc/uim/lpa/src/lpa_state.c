/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    L P A   S T A T E


GENERAL DESCRIPTION

  This file contains functions for switching between states.

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/lpa/src/lpa_state.c#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
04/11/16   av      Initial Version

=============================================================================*/

/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_msg.h"
#include "lpa.h"
#include "lpa_state.h"
#include "lpalib.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

/* Current LPA state */
static lpa_state_enum_type    lpa_state[LPA_NUM_SLOTS]      = {LPA_STATE_IDLE};

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*===========================================================================
FUNCTION LPA_STATE_CHANGE

DESCRIPTION
  Function moves the LPA state

DEPENDENCIES
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_state_change (
  lpa_state_enum_type      current_state,
  lpa_state_enum_type      next_state,
  lpa_slot_id_enum_type    slot_id
)
{
  LPA_MSG_MED_3(
    "LPA state change: actual current state: 0x%x,current_state=0x%x, next_state=0x%x",
    lpa_state, current_state, next_state);

  if(slot_id >= LPA_NUM_SLOTS ||
     current_state != lpa_state[slot_id])
  {
    return LPA_GENERIC_ERROR;
  }

  lpa_state[slot_id] = next_state;
  return LPA_SUCCESS;
} /* lpa_state_change */


/*===========================================================================
FUNCTION LPA_STATE_GET_CURRENT

DESCRIPTION
  Function returns the current LPA state.

DEPENDENCIES
  None

RETURN VALUE
  lpa_state_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_state_enum_type lpa_state_get_current (
  lpa_slot_id_enum_type             slot_id
)
{
  if(slot_id >= LPA_NUM_SLOTS)
  {
    return LPA_STATE_IDLE;
  }
  return lpa_state[slot_id];
} /* lpa_state_get_current */


/*===========================================================================
FUNCTION LPA_STATE_RESET

DESCRIPTION
  Function called to reset the LPA state machine

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void lpa_state_reset (
  lpa_slot_id_enum_type             slot_id
)
{
   if(slot_id < LPA_NUM_SLOTS)
   {
     lpa_state[slot_id] = LPA_STATE_IDLE;
   }
} /* lpa_state_reset */
