#ifndef TIME_TYPE_H
#define TIME_TYPE_H
/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION



=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/time_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/12/10   din     Added internal features for better readability.
17/10/09   din     File created. 

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "qw.h"

/**
@mainpage Time Services CDMA
Defines structures and enums that will are common to time keeping functions. 
*/

/**
@defgroup Time Services CDMA  - Public Interface
This section contains definitions for various public interface defined 
by this module.
*/

/** 
@ingroup Structures and enums for time keeping - Public Interface
@file time_types.h
This file contains public interface defined by this module
*/


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

/*-----------------------------------------------------------------------------
  Time, in slow clock counts, from 0 to 0xFFFFFFFF (~1.5 days)
-----------------------------------------------------------------------------*/
/** Time is represented as a 64-bit value, which is currently a qword */
typedef qword time_type;


/**Julian time structure*/
typedef struct
{
  /** Year [1980 through 2100] */
  uint16                          year;

  /** Month of year [1 through 12] */
  uint16                          month;

  /** Day of month [1 through 31] */
  uint16                          day;

  /** Hour of day [0 through 23] */
  uint16                          hour;

  /** Minute of hour [0 through 59] */
  uint16                          minute;

  /** Second of minute [0 through 59] */
  uint16                          second;

  /** Day of the week [0 through 6] [Monday through Sunday] */
  uint16                          day_of_week;
}
time_julian_type;

/* FEATURE_TIME_MULTI_SERVER_PROC - Only defined on modem proc 
when time is remoted. 
   FEATURE_TIME_SINGLE_MULTI_SERVER_PROC – Only defined on proc to 
enable efs_calls, time_ipc etc. ( functions required on modem proc for 
both single and multi processor chips ) */

/*for 9K, both Q6 and ARM9 are modem and hence we need to distinguish*/
#ifdef T_MDM9X00

#if ( defined(FEATURE_MULTIPROCESSOR) && defined(FEATURE_QDSP6) ) 
#define FEATURE_TIME_MULTI_SERVER_PROC
#define FEATURE_TIME_SINGLE_MULTI_SERVER_PROC
#else /* for ARM9 */
#undef FEATURE_TIME_IPC_TASK
#endif

#else /*else for other targets*/

#if ( defined(FEATURE_MULTIPROCESSOR) && defined(IMAGE_MODEM_PROC) ) 
#define FEATURE_TIME_MULTI_SERVER_PROC
#define FEATURE_TIME_SINGLE_MULTI_SERVER_PROC
#endif /* FEATURE_MULTIPROCESSOR && IMAGE_MODEM_PROC */

#endif

#ifndef FEATURE_MULTIPROCESSOR
#define FEATURE_TIME_SINGLE_MULTI_SERVER_PROC
#endif /* !FEATURE_MULTIPROCESSOR */




#endif /* TIME_TYPES_H */





