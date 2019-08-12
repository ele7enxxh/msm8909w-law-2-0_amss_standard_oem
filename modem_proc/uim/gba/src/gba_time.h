#ifndef GBA_TIME_H
#define GBA_TIME_H
/*===========================================================================


            G B A   T I M E   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

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
#include "time_jul.h"

/*=============================================================================

                       FUNCTIONS

=============================================================================*/

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
);

#endif /* GBA_TIME_H */
