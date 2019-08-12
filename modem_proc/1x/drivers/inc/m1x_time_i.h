#ifndef M1X_TIME_I_H
#define M1X_TIME_I_H

/*=============================================================================

              1 X   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements cdma time-keeping functions using the symbol combiner.


EXTERNALIZED FUNCTIONS
  m1x_time_set_valid
    Indicates whether or not CDMA time is available

  m1x_time_set
    Set the system timestamp value in 80 ms units.

  m1x_time_tick
    Increment the timestamp by a PN roll.

  m1x_time_inc
    Increment the 64-bit system timestamp in 26.6 ms units

  m1x_time_get_slot_time
    Return the 64-bit system timestamp in units of slot time.

  m1x_time_get_20ms_frame_time
    Return the 64-bit system timestamp in units of 20 millisecond frame time.

  m1x_time_get_20ms_frame_offset_time
    Return the 64-bit system timestamp in units of 20 millisecond frame time,
    adjusted for a frame offset.

  m1x_time_set_20ms_frame_offset
    Set the 20ms frame offset


INITIALIZATION AND SEQUENCING REQUIREMENTS
  m1x_time_set() needs to be called when accurate time is available from the
  base station.

Copyright (c) 2012 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/drivers/inc/m1x_time_i.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/27/13   as      Added m1x_timetick_diff method.
01/23/13   as      Updated sleep profiling to use XO clock and 64 bit time ticks
09/19/12   trc     Added m1x_time_init() API
08/14/12   cjb     Added changes to use SLPC(new sleepctl replacement module).
04/06/12   trc     Initial version, ported from time_cdma.h

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "m1x_time.h"

#include "DDITimetick.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/



/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/
/** @addtogroup m1x_time
@{ */

/**
Initializes the M1X_TIME timekeeping subsystem for subsequent usage.
Should be called at bootup and once only.

@return
None.

@dependencies
None.
*/
void m1x_time_init( void );

/**
Allows callers to tell M1X_TIME whether the symbol combiner position is
meaningful. If it is not meaningful, TIME may fall back on an alternate time
source with the last resort being the slow-clock-based TOD.

@param[in] valid Flag indicating whether CDMA time is valid.

@return
None.

@dependencies
None.
*/
void m1x_time_set_valid
(
  boolean                         valid
);


/**
Sets the system timestamp to a specified value. The value is the combination of the
number of elapsed 80 ms units since base date and the current phase of the 80 ms frame.

@param[in] set_val Number of elapsed 80 ms units since base date.
@param[in] phase80 Current phase of the 80 ms frame: \n
                   0 -- In the 0-26.67ms phase of the 80 ms frame. \n
                   1 -- In the 26.67-53.34ms phase of the 80ms frame. \n
                   2 -- In the 53.34ms-80ms phase of the 80ms frame.

@return
None.

@dependencies
None.

@sideeffects
This function initalizes the system timestamp. \n
CDMA time is marked as valid.
*/
void m1x_time_set
(
  time_type                       set_val,
  uint16                          phase80
);


/**
Invalidates the CDMA system time in preparation for CDMA slotted sleep.

@return
None.

@dependencies
The CDMA system clock must be accurate when this function is called.

@sideeffects
This updates the slow-clock-based TOD before invalidating the CDMA time.
*/
void m1x_time_sleep( void );


/**
Updates and asserts the valid CDMA system time after CDMA slotted sleep.

@return
None.

@dependencies
The symbol combiner timing must be correct when this function is called.
*/
void m1x_time_wake( void );


/**
Increments the timestamp by 80 ms each time phase80 is 0. Phase80 being 0
indicates that 80 ms have passed. This function is called at every PN Roll
(an interrupt that fires every 26.67 ms).

@param[in] phase80 Specifies the current phase of the 80-millisecond time passage.

@return
None.

@dependencies
The CDMA system clock must be set to a reasonable value in order for this
function to perform a meaningful operation on the timestamp.
*/
void m1x_time_tick
(
  uint16                          phase80
);


/**
Gets the 64-bit system timestamp in units of slot time. Slot time is 26.6-millisecond
units.

@return
The current timestamp value in terms of units of slot time modulo the maximum
slot cycle length.

@dependencies
The value returned by this function is meaningless unless the demodulator
is running and contains a valid value. This function does not fall back on
TOD.
*/
uint16 m1x_time_get_slot_time( void );


/**
Returns the 64-bit system timestamp in units of 20-millisecond frame time
(traffic/paging/access channel frame time).

@param[out] ts_val Address of the qword in which to return the 64-bit frame time.

@return
The current timestamp value in terms of 20-millisecond frame time.

@dependencies
The value returned by this function is meaningless unless the demodulator is
running and contains a valid value. This function does not fall back on Time
of Day.
*/
void m1x_time_get_20ms_frame_time
(
  time_type                       ts_val
);


/**
Sets the frame offset so calls to m1x_time_get_20ms_frame_offset_time( ) are
adjusted by the frame offset.

@param[in] frame_offset New 20-millisecond frame offset.

@return
None.

@dependencies
The CDMA system clock must be set to a reasonable value in order for this
function to perform a meaningful operation on the timestamp.
*/
void m1x_time_set_frame_offset
(
  uint8                           frame_offset
);


/**
Returns the 64-bit system timestamp (in units of the 20-millisecond frame time) adjusted
by the frame offset.

@param[out] ts_offset_val Address of the qword in which to return the 64-bit frame time.

@return
None.

@dependencies
The value returned by this function is meaningless unless the demodulator is
running and contains a valid value. This function does not fall back on Time of
Day.
*/
void m1x_time_get_20ms_frame_offset_time
(
  time_type                       ts_offset_val
);


/**
Returns the 64-bit system timestamp (in units of 1ms)

@param[out] ts_val Address of the qword in which to return the 64-bit 1ms time.

@return
None.

@dependencies
The value returned by this function is meaningless unless the demodulator is
running and contains a valid value. This function does not fall back on Time of
Day.
*/
void m1x_time_get_ms
(
  /* Out: Time in milliseconds since 6 Jan 1980 00:00:00 */
  time_type                       ts_val
);


/**
Returns the 64-bit system timestamp (in units of 1ms)

@return
The current 64-bit time in 1ms units in a uint64 type

@dependencies
The value returned by this function is meaningless unless the demodulator is
running and contains a valid value. This function does not fall back on Time of
Day.
*/
uint64 m1x_time_get_ms_native( void );

/** @} */ /* end_addtogroup m1x_time */


/*===========================================================================

                     TIMETICK ACCESS AND CONVERSION APIs

===========================================================================*/

/* Returns the time difference: [end - start] floor 0
   NOTE: Both time values are assumed to be unsigned */
#define M1X_TIMETICK_DELTA( start, end ) \
  ( ( end > start )? ( end - start ) : 0 )

/*=============================================================================

FUNCTION      M1X_TIMETICK_INIT
DESCRIPTION   One time initialization before using other TIMETICK wrapper APIs

DEPENDENCIES  None
RETURN VALUE  None
SIDE EFFECTS  None

=============================================================================*/
extern void m1x_timetick_init( void );

/*=============================================================================

FUNCTION      M1X_TIMETICK_CONV_FROM_TICKS
DESCRIPTION   Conversion: Ticks to requested "unit"

DEPENDENCIES  The m1x_timetick_init() routine must have been called previously
RETURN VALUE  Time in "unit" requested
SIDE EFFECTS  None

=============================================================================*/
extern uint64 m1x_timetick_conv_from_ticks
(
  DalTimetickTime64Type     t_ticks,    /* Time in sclk64(or ticks) format */
  DalTimetickUnitType       unit        /* Desired unit for return value */
);

/*=============================================================================

FUNCTION      M1X_TIMETICK_CONV_TO_TICKS
DESCRIPTION   Conversion: Input time "unit" to Ticks

DEPENDENCIES  The m1x_timetick_init() routine must have been called previously
RETURN VALUE  Time in "unit" requested
SIDE EFFECTS  None

=============================================================================*/
extern DalTimetickTime64Type m1x_timetick_conv_to_ticks
(
  uint64                    t_unit,     /* Time in "unit" format */
  DalTimetickUnitType       unit        /* Unit of "t_unit" */
);

/*=============================================================================

FUNCTION      M1X_TIMETICK_CONV_TO_SCLK64
DESCRIPTION   Conversion: Input time "unit" to Ticks(alternatively referred to as sclk64)

DEPENDENCIES  The m1x_timetick_init() routine must have been called previously
RETURN VALUE  Time in "unit" requested
SIDE EFFECTS  None

=============================================================================*/
extern uint64 m1x_timetick_conv_to_sclk64
(
  uint64                    t_unit,     /* Time in "unit" format */
  DalTimetickUnitType       unit        /* Unit of "t_unit" */
);

/*=============================================================================

FUNCTION      M1X_TIMETICK_GET_XO_TICK
DESCRIPTION   Gets the current DalTimetick in XO time ticks

DEPENDENCIES  The m1x_timetick_init() routine must have been called previously
RETURN VALUE  None
SIDE EFFECTS  None

=============================================================================*/
extern void m1x_timetick_get_xo_tick( uint64* );

/*=============================================================================

FUNCTION      M1X_TIMETICK_DIFF
DESCRIPTION   Computes the difference between two timetick counts

DEPENDENCIES  The m1x_timetick_init() routine must have been called previously
RETURN VALUE  None
SIDE EFFECTS  None

=============================================================================*/
extern void m1x_timetick_diff
(
  DalTimetickTime64Type  start, /* Timetick value at the start of the interval */
  DalTimetickTime64Type  end,   /* Timetick value at the end of the interval */
  DalTimetickUnitType    unit,  /* Units to return time interval in */
  DalTimetickTime32Type* diff   /* Time difference between the two timetick
                                   counts, in the unit given */
);

/*=============================================================================

FUNCTION      M1X_TIMETICK_GET_XO_FREQUENCY
DESCRIPTION   Gets the DalTimetick frequency in Hz

DEPENDENCIES  The m1x_timetick_init() routine must have been called previously
RETURN VALUE  Frequency in Hz (uint32)
SIDE EFFECTS  None

=============================================================================*/
extern uint32 m1x_timetick_get_xo_frequency( void );

#endif /* M1X_TIME_I_H */

