#ifndef TIME_CONV_H
#define TIME_CONV_H
/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements conversion of time into different units .


=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/time_conv.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
17/10/09   din     File created. 

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_types.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/


/*-----------------------------------------------------------------------------

                      SYSTEM TIMESTAMP FORMAT


   |<------------ 48 bits --------------->|<----- 16 bits ------->|
   +--------------------------------------+-----------------------+
   |      1.25 ms counter                 |   1/32 chip counter   |
   +--------------------------------------+-----------------------+
         (11K years of dynamic range)          (1.25 ms dynamic
                                                range. Rolls over
                                                at count 49152)

-----------------------------------------------------------------------------*/


/*=============================================================================

FUNCTION TIME_CONV_TO_MS

DESCRIPTION
  Convert a timestamp from System Time to millisecond units

DEPENDENCIES
  None

RETURN VALUE
  The converted value is stored in the 'time' parameter.

SIDE EFFECTS
  None

=============================================================================*/

void time_conv_to_ms
(
  /* In: Time in time-stamp format;  Out: Time in milliseconds */
  time_type                       ts_val
);



/*=============================================================================

FUNCTION TIME_CONV_TO_SEC

DESCRIPTION
  Convert a timestamp from System Time to seconds

DEPENDENCIES
  None

RETURN VALUE
  Seconds, corresponding to the time stamp

SIDE EFFECTS
  Input argument value is destroyed.

=============================================================================*/

uint32 time_conv_to_sec
(
  /* In: Time in time-stamp format;  Out: Garbage */
  time_type                       ts_val
);



/*=============================================================================

FUNCTION TIME_CONV_MS_TO_TIMESTAMP

DESCRIPTION
  Converts from milliseconds to timestamp.  ts_val receives the timestamp after
  the conversion.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ts_val receives the timestamp.

=============================================================================*/

void time_conv_ms_to_timestamp
(
  /* IN : Timestamp in ms, OUT : Timestamp in timestamp format  */
  time_type                       ts_val
);



/*=============================================================================

FUNCTION TIME_CONV_SECS_TO_TIMESTAMP

DESCRIPTION
  Converts from seconds to timestamp.  ts_val receives the timestamp after
  the conversion.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ts_val receives the timestamp.

=============================================================================*/

void time_conv_secs_to_timestamp
(
  /* IN : Timestamp in secs, OUT : Timestamp in timestamp format  */
  time_type                       ts_val
);



/*=============================================================================

FUNCTION TIME_CONV_TO_20MS_FRAME_TIME

DESCRIPTION
  Converts from timestamp to 2oms frame time.ts_val receives the timestamp 
  after the conversion.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ts_val receives the timestamp.

=============================================================================*/

void time_conv_to_20ms_frame_time
(
  /* Timestamp holder to convert to */
  time_type                       ts_val
);



/*=============================================================================

FUNCTION TIME_CONV_ADD_OFFSET_TO_TIMESTAMP

DESCRIPTION
  Adds offset( in ms ) to timestamp

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ts_val receives the added timestamp.

=============================================================================*/

void time_conv_add_offset_to_timestamp
(
  /* Timestamp holder to convert to */
  time_type                       ts_val,

  /* Offset to be added, in ms */
  int64                           offset
);

#endif /* TIME_CONV_H */





