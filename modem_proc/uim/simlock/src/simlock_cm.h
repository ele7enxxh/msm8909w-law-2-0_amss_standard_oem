#ifndef SIMLOCK_CM_H
#define SIMLOCK_CM_H
/*===========================================================================


            S I M L O C K   C M   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_cm.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
11/10/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "comdef.h"

#ifdef FEATURE_SIMLOCK

#include "simlock_common.h"

/*===========================================================================
FUNCTION SIMLOCK_SET_DEVICE_MODE_SYNC

DESCRIPTION
  This function calls the CM API to set the device in the emergency or
  normal mode. It enters a synchronous wait state, until a response is
  received or the sanity timer expires.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
simlock_result_enum_type simlock_set_device_mode_sync
(
  simlock_device_mode_enum_type   device_mode
);

/*===========================================================================
FUNCTION SIMLOCK_CM_IS_DEVICE_IN_EMERGENCY_MODE

DESCRIPTION
  This function checks if the device in the emergency only mode.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  TRUE  : If the device is in emergency mode
  FALSE : If otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean simlock_cm_is_device_in_emergency_mode
(
  void
);

#endif /* FEATURE_SIMLOCK */

#endif /* SIMLOCK_CM_H */

