#ifndef SIMLOCK_TEMP_UNLOCK_H
#define SIMLOCK_TEMP_UNLOCK_H
/*===========================================================================


            S I M   L O C K   T E M P   U N L O C K   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2015 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_temp_unlock.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/06/16   stv     Remote simlock support
08/12/15   stv     Clear temp unlock data when temp unlock expires
07/06/15   stv     Clear temp unlock data when a permanent unlock/reset simlock req
06/29/15   stv     Start time in temp unlock req as current time
                   Graceful handling of PMIC reset during temp unlock
05/15/15   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "comdef.h"

#ifdef FEATURE_SIMLOCK
#if defined(FEATURE_SIMLOCK_RSU) || defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK)

#include "simlock.h"
#include "simlock_time.h"

/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_TEMP_UNLOCK_RESET_CONFIG_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the SIMLock Temp Unlock Reset types
-------------------------------------------------------------------------------*/
typedef enum
{
  /* If all locks are unlocked either due to a permanent unlock request on the slot or due to a
     RSU reset simlock setting request */
  SIMLOCK_TEMP_UNLOCK_RESET_CONFIG_IF_UNLOCKED       = 0x00,
  /* If the temporary unlock is expired, means no temporary timer running to exempt the card 
     from lock, then clear the temp unlock details */
  SIMLOCK_TEMP_UNLOCK_RESET_CONFIG_IF_UNLOCK_EXPIRED = 0x01
}simlock_temp_unlock_reset_config_enum_type;

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_INIT

DESCRIPTION
  This function is for the temporary unlock module init. It is invoked
  during the simlock initialization.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_temp_unlock_init
(
  void
);

/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_PROCESS_PENDING_TIMER_EXPIRY

DESCRIPTION
  The function processes the temporary unlock pending timer expiry request
  in the simlock task

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_temp_unlock_process_pending_timer_expiry
(
  void
);

/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_PROCESS_REQUEST

DESCRIPTION
  This function processes the temporary unlock request from a starting
  time and duration provided in the request.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_temp_unlock_process_request
(
  simlock_slot_enum_type       slot,
  simlock_category_enum_type   category,
  simlock_time_type            start_time,
  simlock_time_type            duration
);

/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_IS_ACTIVE

DESCRIPTION
  This function returns true if temporary unlock is active on any category.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  boolean

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_temp_unlock_is_active
(
  void
);

/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_RESET_CONFIG

DESCRIPTION
  This function would post an enginee msg, GET_STATUS
  Based on the input parameter clears the temporary unlock
  related information.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_temp_unlock_reset_config(
  simlock_temp_unlock_reset_config_enum_type   reset_config_type
);
#endif /* FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK */

/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_GET_REMAINING_TIME

DESCRIPTION
  This function retrieves the amount of time remaining for the temporary
  unlock

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_temp_unlock_get_remaining_time
(
  simlock_temp_unlock_remaining_time_type   * resp_msg_ptr
);

#endif /* FEATURE_SIMLOCK */

#endif /* SIMLOCK_TEMP_UNLOCK_H */
