/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G B A   T I M E   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the GBA time related functions.

                        COPYRIGHT INFORMATION

Copyright (c) 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/15   tkl      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include <stdlib.h>
#include "time_genoff.h"
#include "time_types.h"
#include "time_jul.h"

#include "gba_platform.h"

/*=============================================================================

                       FUNCTIONS

=============================================================================*/

/*===========================================================================
FUNCTION GBA_TIME_GET_CURRENT_TIME

DESCRIPTION
  Function retrieves the current system time in julian format.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  time_julian_type

SIDE EFFECTS
  None
===========================================================================*/
static time_julian_type gba_time_get_current_time (
  void
)
{
  time_genoff_args_struct_type  time_genoff_args = {0};
  time_julian_type              julian_time      = {0};

  // Fill in the time genoff struct
  time_genoff_args.base        = ATS_TOD;
  time_genoff_args.base_source = TIME_SCLK;
  time_genoff_args.ts_val      = (void *)(&julian_time);
  time_genoff_args.unit        = TIME_JULIAN;
  time_genoff_args.operation   = T_GET;
  time_genoff_opr(&time_genoff_args);

  return julian_time;
} /* gba_time_get_current_time */


/*===========================================================================
FUNCTION GBA_TIME_IS_TIME_EXPIRED

DESCRIPTION
  Function compares input time and check if it is expired. optional parameter
  remaining_sec to return the remaining time before expiration.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean gba_time_is_time_expired (
  time_julian_type       gba_julian_time,
  uint32               * remaining_sec_ptr
)
{
  time_julian_type             sys_julian_time = gba_time_get_current_time();
  uint32                       gba_julian_sec  = 0;
  uint32                       sys_julian_sec  = 0;
  boolean                      result          = TRUE;

  GBA_MSG_MED_6("gba time: year %d, month %d, day %d, hour %d, min %d, sec %d",
                gba_julian_time.year, gba_julian_time.month, gba_julian_time.day,
                gba_julian_time.hour, gba_julian_time.minute, gba_julian_time.second);
  GBA_MSG_MED_6("system time: year %d, month %d, day %d, hour %d, min %d, sec %d",
                sys_julian_time.year, sys_julian_time.month, sys_julian_time.day,
                sys_julian_time.hour, sys_julian_time.minute, sys_julian_time.second);

  gba_julian_sec = time_jul_to_secs(&gba_julian_time);
  sys_julian_sec = time_jul_to_secs(&sys_julian_time);

  if(gba_julian_sec > sys_julian_sec)
  {
    if(remaining_sec_ptr != NULL)
    {
      *remaining_sec_ptr = gba_julian_sec - sys_julian_sec;
    }
    result = FALSE;
  }

  GBA_MSG_HIGH_1("Time expired: 0x%x", result);
  return result;
} /* gba_time_is_time_expired */
