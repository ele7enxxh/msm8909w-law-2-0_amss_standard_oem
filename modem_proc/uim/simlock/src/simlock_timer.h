#ifndef SIMLOCK_TIMER_H
#define SIMLOCK_TIMER_H
/*===========================================================================


            S I M L O C K   T I M E R   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_timer.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/15/15   vv      Support for RSU set configuration in simlock
08/29/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "comdef.h"

#ifdef FEATURE_SIMLOCK

#include "simlock_common.h"
#include "simlock_modem.h"


/*===========================================================================
FUNCTION SIMLOCK_TIMER_START

DESCRIPTION
  This function starts a temporary unlock timer for a category

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_timer_start
(
  simlock_slot_enum_type         slot,
  simlock_category_enum_type     category,
  simlock_time_minutes_type      unlock_minutes
);

/*===========================================================================
FUNCTION SIMLOCK_TIMER_STOP

DESCRIPTION
  This function stops an existing temporary unlock timer

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_timer_stop
(
  simlock_slot_enum_type         slot,
  simlock_category_enum_type     category
);

/*===========================================================================
FUNCTION SIMLOCK_TIMER_CHECK_TEMPORARY_UNLOCK_SET

DESCRIPTION
  This function checks if a temporary unlock is currently set for a category

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_timer_check_is_temporary_unlock_set
(
  simlock_slot_enum_type         slot,
  simlock_category_enum_type     category
);

/*===========================================================================
FUNCTION SIMLOCK_TIMER_PROCESS_EXPIRY_REQUEST

DESCRIPTION
  The function processes the temporary unlock timer expiry request
  in the simlock task

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
simlock_result_enum_type simlock_timer_process_expiry_request
(
  const simlock_timer_expiry_msg_req_type  *  timer_expiry_req_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_TIMER_GET_REMAINING_TIME

DESCRIPTION
  The function gets the time remaining in msecs, before a timer expires

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  Number of milliseconds left, before timer expires

SIDE EFFECTS
  None
===========================================================================*/
uint32 simlock_timer_get_remaining_time
(
  simlock_slot_enum_type         slot,
  simlock_category_enum_type     category
);

#endif /* FEATURE_SIMLOCK */

#endif /* SIMLOCK_TIMER_H */

