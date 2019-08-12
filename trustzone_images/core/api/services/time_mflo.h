#ifndef TIME_MFLO_H
#define TIME_MFLO_H

/*=============================================================================

                  T I M E   S E R V I C E   S U B S Y S T E M

                   M E D I A F L O  T I M E   K E E P I N G


GENERAL DESCRIPTION

  Implements MediaFLO time-keeping functions using the MediaFLO time data


EXTERNALIZED FUNCTIONS

  time_mflo_set_at
    Sets the MediaFLO system time or invalidates it.

  time_mflo_get
    Returns system time based on the last MediaFLO time update.

  time_mflo_invalidate
    Invalidates the MediaFLO time.


INITIALIZATION AND SEQUENCING REQUIREMENTS

  MediaFLO System Time has a resolution of 1 second and is retrieved from 
  Overhead Information Symbols (OIS) portion of the MediaFLO Superframe. 
  MediaFLO Superframes are transmitted on a fixed interval of 1 second 
  synchronized to GPS time epoch.

  MediaFLO system time is invalidated if/when MediaFLO service is deemed
  unavailable.


Copyright (c) 2005-2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/time_mflo.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/23/09   din     Consolidation of time.
11/18/05   mod/ajn Added MediaFLO time support to mainline
11/08/05   mod     Added time_mflo_invalidate()
11/03/05   mod     Code review comments
10/28/05   mod     Initial implementation.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_svc.h"
#include "timetick.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

typedef uint32 mflo_time_type;


/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/


/*=============================================================================

FUNCTION TIME_MFLO_SET_AT

DESCRIPTION
  This API will be called whenever OIS has been decoded and the 
  mflo_time_val parameter will contains seconds since the GPS epoch.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The MediaFLO system time is set and marked valid.
  Additionally, "Time Of Day" is synchronized to the specified time. 

=============================================================================*/

void time_mflo_set_at
(
  /* Timestamp to set time of day to */
  mflo_time_type                  mflo_time_val,

  /* Corresponding timetick value */
  timetick_type                   ticks
);



/*=============================================================================

FUNCTION TIME_MFLO_GET

DESCRIPTION
  This procedure is called to read system timestamp based on the last 
  MediaFLO time update.

DEPENDENCIES
  MediaFLO time keeping must be active in order to for a good value to be 
  returned.
  This function does not fall back on "Time Of Day".

RETURN VALUE
  TRUE if MediaFLO system time is available, 
  FALSE otherwise.
  
  Parameter ts_val is set to the current system time, in timestamp format.

SIDE EFFECTS
  None

=============================================================================*/

boolean time_mflo_get
(
  /* Address of qword in which to return 64-bit system timestamp. */
  time_type                       ts_val
);



/*=============================================================================

FUNCTION TIME_MFLO_INVALIDATE

DESCRIPTION
  Marks the MediaFLO time as invalid.

  MediaFLO system time is invalidated if/when MediaFLO service is deemed
  unavailable.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  MediaFLO system time is marked as invalid.

=============================================================================*/

void time_mflo_invalidate( void );

#endif /* TIME_MFLO_H */

