#ifndef TIME_JUL_H
#define TIME_JUL_H

/**
@file time_jul.h
@brief
This module contains procedures to convert seconds to and from Julian calendar 
units.
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The ATS_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      ATS_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the time_jul group 
      description in the ATS_mainpage.dox file. 
=============================================================================*/

/*=============================================================================
  Copyright (c) 1994-2012 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*=============================================================================

                  T I M E   S E R V I C E   S U B S Y S T E M

                  J U L I A N   T I M E   C O N V E R S I O N


GENERAL DESCRIPTION
  This module contains procedures to convert seconds to and from Julian
  calendar units.

  IMPORTANT NOTE: The time calculations in this module are only valid for
  dates and times from 6 Jan 1980, 00:00:00 to 28 Feb 2100, 23:59:59. This
  is due to the fact that this module assumes that all years evenly divisible
  by 4 are leap years. Unfortunately, century years (e.g., 1900, 2000, 2100)
  must also be evenly divisible by 400 in order to be leap years, and so the
  year 2100 does not qualify as a leap year.


EXTERNALIZED FUNCTIONS
  time_jul_from_secs
    This function converts a specified number of elapsed seconds since
    the base date to its equivalent Julian date and time.

  time_jul_to_secs
    This function converts a specified Julian date and time to an
    equivalent number of elapsed seconds since the base date.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/time_jul.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/29/10   scd     (Tech Pubs) Edited Doxygen markup and comments.
10/27/10   EBR     Doxygenated File.
12/5/09    din     Moved definition of time_julian_type to time_types.h
10/08/09   din     removed millisecond field from time_julian_type
02/23/09   din     Consolidation to time Sources. Transferred 
                   time_set/get_julian() to time_svc.h
10/22/08   sep     Add time_jul_from_ms function
09/23/08   sep     Moved system_time_to_julian_time to time_jul.c
04/01/08    mk     Remoted time_get/set_julian APIS 
08/08/03   ajn     Coding standard update.
07/24/03   ajn     Xfer'd clkjul.c from MSM#### CLOCK VU's to general TIME VU

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include "qw.h"
#include "time_types.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

/** @addtogroup time_jul
@{ */

/*-----------------------------------------------------------------------------
  Days of the Week constants
-----------------------------------------------------------------------------*/
/**
Enumeration for days of the week.
*/
typedef enum
{
  MONDAY    = 0,
  TUESDAY   = 1,
  WEDNESDAY = 2,
  THURSDAY  = 3,
  FRIDAY    = 4,
  SATURDAY  = 5,
  SUNDAY    = 6
}
time_weekday_type;


/**
Converts a specified number of elapsed seconds since the base date to its 
equivalent Julian date and time.

@param[in] secs    Number of seconds since base date.
@param[out] julian Pointer to a Julian date record.

@return
None.

@dependencies
None.
*/
void time_jul_from_secs
(
  uint32                          secs,
  time_julian_type                *julian
);


/**
Converts a specified number of elapsed milliseconds since the base date to its 
equivalent Julian date and time.

@param[in] time_ms Number of milliseconds since base date.
@param[out] julian Pointer to a Julian date record.

@return
None.

@dependencies
None.
*/
void time_jul_from_ms
(
  time_type        		          time_ms,
  time_julian_type                *julian
);


/**
Converts a specified Julian date and time to an equivalent number of elapsed 
seconds since the base date.

@param[in] julian Pointer to a Julian date record.

@return
Number of elapsed seconds since base date.

@dependencies
None.
*/
uint32 time_jul_to_secs
(
  const time_julian_type          *julian
);

/** @} */ /* end_addtogroup time_jul */
#endif /* TIME_JUL_H */
