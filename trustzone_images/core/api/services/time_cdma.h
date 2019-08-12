#ifndef TIME_CDMA_H
#define TIME_CDMA_H
/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

                       C D M A   T I M E   K E E P I N G

GENERAL DESCRIPTION
  Implements cdma time-keeping functions using the symbol combiner.


EXTERNALIZED FUNCTIONS
  time_cdma_set_valid
    Indicates whether or not CDMA time is available

  time_cdma_set
    Set the system timestamp value in 80 ms units.

  time_cdma_tick
    Increment the timestamp by a PN roll.

  time_cdma_inc
    Increment the 64-bit system timestamp in 26.6 ms units

  time_cdma_get
    Return the system timestamp value.

  time_cdma_get_slot_time
    Return the 64-bit system timestamp in units of slot time.

  time_cdma_get_20ms_frame_time
    Return the 64-bit system timestamp in units of 20 millisecond frame time.

  time_cdma_get_20ms_frame_offset_time
    Return the 64-bit system timestamp in units of 20 millisecond frame time,
    adjusted for a frame offset.

  time_cdma_set_20ms_frame_offset
    Set the 20ms frame offset


INITIALIZATION AND SEQUENCING REQUIREMENTS
  time_cdma_set() needs to be called when accurate time is available from the
  base station.


Copyright(c) 1990-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/time_cdma.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/23/09   din     Consolidation of time.
02/09/04   ajn     Renamed time.h to time_svc.h to avoid OS conflict
08/08/03   ajn     Changed timestamp type from qword to time_type (a qword)
07/25/03   ajn     Transfer from services/clk/ts.h to TIME VU

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_svc.h"


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/



/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/


/*=============================================================================

FUNCTION TIME_CDMA_SET_VALID

DESCRIPTION
  Permits other code in the phone to tell TIME_CDMA that the symbol combiner
  position is or is not meaningful.  This may cause TIME to fall back on an
  alternate time source, which as a last resort would be the slow-clock based
  "Time Of Day".

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_cdma_set_valid
(
  /* Flag indicating whether CDMA time is valid or not */
  boolean                         valid
);



/*=============================================================================

FUNCTION TIME_CDMA_SET

DESCRIPTION
  This procedure sets the system timestamp to a specified value which is
  the combination of a multiple of 80 milliseconds and a 26.67 ms phase
  of 80 milliseconds (0, 1, 2).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Initializes the system timestamp and marks CDMA time as valid.

=============================================================================*/

void time_cdma_set
(
  /* Number of elapsed 80 ms units since base date. */
  time_type                       set_val,

  /* The current phase of the 80 ms. */
  uint16                          phase80
);



/*=============================================================================

FUNCTION TIME_CDMA_GET

DESCRIPTION
  This procedure is called to read the 64-bit CDMA system timestamp.

DEPENDENCIES
  time_cdma_set() needs to be called when accurate time is available from
  the base station.  This function does not fall back on "Time Of Day".

RETURN VALUE
  TRUE if CDMA system time is available, FALSE otherwise.
  Parameter ts_val is set to the current system time, in timestamp format.

SIDE EFFECTS
  None

=============================================================================*/

boolean time_cdma_get
(
  /* Address of qword in which to return 64-bit system timestamp. */
  time_type                       ts_val
);



/*=============================================================================

FUNCTION TIME_CDMA_INC

DESCRIPTION
  Increment the 64-bit CDMA system timestamp in 26.6 ms units.

DEPENDENCIES
  The CDMA system clock must be set to a reasonable value in order for
  this function to perform a meaningful operation on the timestamp.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_cdma_inc
(
  /* The number of 26.6ms units to add to system time */
  unsigned int /* fast */         increment
);



/*=============================================================================

FUNCTION TIME_CDMA_SLEEP

DESCRIPTION
  Invalidates CDMA system time, in preperation for CDMA slotted sleep.

DEPENDENCIES
  The CDMA system clock must accurate when this function is called.

RETURN VALUE
  None

SIDE EFFECTS
  Updates the slow-clock based "Time of Day" before invalidating CDMA time.

=============================================================================*/

void time_cdma_sleep( void );



/*=============================================================================

FUNCTION TIME_CDMA_WAKE

DESCRIPTION
  Updates and asserts valid CDMA system time, after CDMA slotted sleep.

DEPENDENCIES
  The symbol combiner timing must be correct when this function is called.

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/

void time_cdma_wake( void );



/*===========================================================================

FUNCTION TIME_CDMA_TICK

DESCRIPTION
  This function is called at every PN Roll (26.67 ms).  It increments the
  timestamp by 80 ms each time phase80 is 0.  Phase80 being 0 indicates that
  80 ms have passed.

DEPENDENCIES
  The CDMA system clock must be set to a reasonable value in order for
  this function to perform a meaningful operation on the timestamp.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void time_cdma_tick
(
  /* Specifies the current phase of the 80 ms. */
  uint16                          phase80
);



/*=============================================================================

FUNCTION TIME_CDMA_GET_SLOT_TIME

DESCRIPTION
  This procedure is returns the 64-bit system timestamp in units of
  slot time, 26.6 millisecond units, modulo the maximum slot cycle
  length.

DEPENDENCIES
  The value returned by this function is meaningless unless the demodulator
  is running and contains a valid value.  This function does not fall back on
  "Time Of Day".

RETURN VALUE
  The current timestamp value, as specified above.

SIDE EFFECTS
  None

=============================================================================*/

uint16 time_cdma_get_slot_time( void );



/*===========================================================================

FUNCTION TIME_CDMA_GET_20MS_FRAME_TIME

DESCRIPTION
  This procedure is returns the 64-bit system timestamp in units of
  20 millisecond frame time (traffic / paging / access channel frame
  time).

DEPENDENCIES
  The value returned by this function is meaningless unless the demodulator
  is running and contains a valid value. This function does not fall back on
  "Time Of Day".

RETURN VALUE
  The current timestamp value, as specified above.

SIDE EFFECTS
  None

===========================================================================*/

void time_cdma_get_20ms_frame_time
(
  /* Address of qword in which to return 64-bit frame time. */
  time_type                       ts_val
);



/*===========================================================================

FUNCTION TIME_CDMA_SET_FRAME_OFFSET

DESCRIPTION
   Set the frame offset, so calls to time_cdma_get_20ms_frame_offset_time( )
   are adjusted by the frame offset.

DEPENDENCIES
  The CDMA system clock must be set to a reasonable value in order for
  this function to perform a meaningful operation on the timestamp.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void time_cdma_set_frame_offset
(
  /* New 20ms frame offset */
  uint8                           frame_offset
);



/*===========================================================================

FUNCTION TIME_CDMA_GET_20MS_FRAME_OFFSET_TIME

DESCRIPTION
  This procedure returns the 64-bit system timestamp (in units of
  20 millisecond frame time) adjusted by frame offset.

DEPENDENCIES
  The value returned by this function is meaningless unless the demodulator
  is running and contains a valid value.  This function does not fall back on
  "Time Of Day".

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void time_cdma_get_20ms_frame_offset_time
(
  /* Address of qword in which to return 64-bit frame time. */
  time_type                       ts_offset_val
);


#endif /* TIME_CDMA_H */
