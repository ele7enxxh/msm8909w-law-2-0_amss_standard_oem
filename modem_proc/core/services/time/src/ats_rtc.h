#ifndef ATS_RTC_H
#define ATS_RTC_H
/*=============================================================================

       T I M E   S E R V I C E   T I M E   R T C   S U B S Y S T E M

GENERAL DESCRIPTION
  Initializes and maintains RTC time in the  Generic Framework.

REGIONAL FUNCTIONS
  ats_rtc_init
    Initialize the ats_rtc subsystem

  ats_rtc_remote_update
    Update Time_rtcs from the current proc to another proc

  ats_rtc_genoff_cb
    Callback called to remotely update offset

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ats_rtc_init() must be called.


Copyright (c) 2009 - 2013 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/ats_rtc.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/27/10   din     Fixed compiler warnings.
10/07/09   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "time_remote_reg.h"



/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

typedef enum
{
  TS_RTC_INVALID = FALSE,
  TS_RTC_VALID   = TRUE,
  TS_RTC_UNKNOWN = 0xff
} ats_rtc_status_type; 


/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/


/*=============================================================================

FUNCTION ATS_PRINT_RTC_STATUS

DESCRIPTION
Prints the RTC bootup time & RTC time now

DEPENDENCIES

RETURN VALUE
Returns the value in rtc_statuc global var

SIDE EFFECTS

=============================================================================*/

ats_rtc_status_type ats_print_rtc_status( void );

	

/*=============================================================================

FUNCTION ATS_RTC_INIT                                                 REGIONAL

DESCRIPTION
  Initializes the ats_rtc subsystem

DEPENDENCIES
  Must be called when the phone first powers on, from time_init( ).

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void ats_rtc_init( void );


/*=============================================================================

FUNCTION ATS_RTC_SET_TO_PMIC_RTC

DESCRIPTION
  Synchronize the PMIC's Real Time Clock with the current time-of-day

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void ats_rtc_set_to_pmic_rtc( void );

#endif /* ATS_RTC_H */

