#ifndef TIMETICK_H
#define TIMETICK_H
/*=============================================================================

                   T I M E T I C K   D R I V E R   

GENERAL DESCRIPTION
    Implements time-keeping functions using the slow clock and provides interfaces
   to the msstimers HAL.

  Slow Clock Frequency          Granularity     Timer Range
    30.000kHz (lower limit)      33.3 us         39.7 hrs
    32.768kHz (nominal freq)     30.5 us         36.4 hrs
    60.000kHz (upper limit)      16.6 us         19.8 hrs


EXTERNALIZED FUNCTIONS

  timetick_get
    Returns the current SLEEP_XTAL_TIMETICK counter's count.

  timetick_get_safe
    Returns the current SLEEP_XTAL_TIMETICK counter's count.
    Must be called from an INTLOCK'd context.

  timetick_get_diff
    Determines the time between two timeticks, in slow clocks, milliseconds,
    seconds, etc.

  timetick_get_elapsed
    Determines the time between a past timetick and now, in slow clocks,
    milliseconds, seconds, etc.

  timetick_get_ms
    Returns a monotonically increasing millisecond count, that is not
    related to system time or time-of-day.  Speed, not accuracy, is
    the focus of this function.

  timetick_cvt_to_sclk
    Converts a time value from seconds, milliseconds, etc to slow clocks

  timetick_cvt_from_sclk
    Converts a time value from slow clocks to seconds, milliseconds, etc

  timetick_sclk_to_prec_us
    Converts sclks to microseconds with precision and full range

  timetick_set_next_interrupt
    Programs the SLEEP_XTAL_TIMETICK to generate an interrupt

INITIALIZATION AND SEQUENCING REQUIREMENTS
    None


Copyright (c) 2003 - 2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/api/systemdrivers/timetick.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/23/09   din     Consoldation of time .
12/15/08   tev     Removed regional (called only by time*.c) frunctions from
                   public timetick.h header file to timetick_v.h
08/31/07   jhs     Support for AMSS RESTART
07/11/07   ebb     Added timetick_get_sclk64 API which manages 32-bit rollover
06/02/07   grl     Added sclk conversion function with precision and range.
10/16/06   ddh     Moved timetick_block_until_next_tick to drivers/mao
08/19/06   gb      Changed FEATURE_POWER_COLLAPSE to FEATURE_TIME_POWER_COLLAPSE
02/22/06   ajn     SCLK offset now applies for both modem & apps processors.
07/06/05   ajn     Added timetick_set_modem_app_sclk_offset (7500 APPS)
04/02/05   ddh     Moved conversion functions from sclk to here
04/30/04   ajn     Added fast monotonic millisecond get function
08/08/03   ajn     Updated for coding standards
07/23/03   ajn     Moved _{to,from}_sclk functions to timetick.h
07/16/03   ajn     File created.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

/*-----------------------------------------------------------------------------
  Nominial slow clock freq in hertz.
-----------------------------------------------------------------------------*/
#ifndef TIMETICK_NOMINAL_FREQ_HZ
#define TIMETICK_NOMINAL_FREQ_HZ      32768uL
#endif /* TIMETICK_NOMINAL_FREQ */

/*-----------------------------------------------------------------------------
  Time units for setting timetick diffs, timers, etc
-----------------------------------------------------------------------------*/

typedef enum
{
  T_SCLK,
  T_USEC,
  T_MSEC,
  T_SEC,
  T_MIN,
  T_HOUR,

  /* Identifier to use if no return value from timer_clr/get is needed */
  T_NONE=T_SCLK
}
timer_unit_type;


/*-----------------------------------------------------------------------------
  Time, in QTimer tick counts
-----------------------------------------------------------------------------*/

typedef uint32                    timetick_type;
typedef uint64                    timetick_type64;

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/
/*=============================================================================

FUNCTION TIMETICK_INIT

DESCRIPTION
  Initializes the timetick module.

DEPENDENCIES
None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timetick_init(void);

/*=============================================================================

FUNCTION TIMETICK_GET_SAFE

DESCRIPTION
  Read the SLEEP_XTAL_TIMETICK counter, and return the current count.

DEPENDENCIES
  Must be called from an INTLOCK'd context.
  Use TIMETICK_GET from non-INTLOCK'd context

RETURN VALUE
  Slow clock timetick counter value

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_get_safe( void );


/*=============================================================================

FUNCTION TIMETICK_GET

DESCRIPTION
  Read the SLEEP_XTAL_TIMETICK counter, and return the current count.

DEPENDENCIES
  None.

RETURN VALUE
  Slow clock timetick counter value

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_get( void );


/*=============================================================================

FUNCTION TIMETICK_GET_FREQ

DESCRIPTION
  Returns the sclk frequency in Hz for use during timetick calculations.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/

uint32 timetick_get_freq(void);

/* =========================================================================
FUNCTION TIMETICK_GET64

DESCRIPTION
  Read the QTIMER count registers, and return the 64 bit current count.

DEPENDENCIES
  None.

RETURN VALUE
  current timetick value

SIDE EFFECTS
  None

=========================================================================*/

timetick_type64 timetick_get64 (void);

#endif /* TIMETICK_H */
