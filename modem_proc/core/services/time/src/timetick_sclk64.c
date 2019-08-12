/*=============================================================================

    T I M E   S E R V I C E   S C L K   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements time-keeping functions using the slow clock.

  Slow Clock Frequency          Granularity     Time Range
    30.000kHz (lower limit)      33.3 us         39.7 hrs
    32.768kHz (nominal freq)     30.5 us         36.4 hrs
    60.000kHz (upper limit)      16.6 us         19.8 hrs


EXTERNALIZED FUNCTIONS
  timetick_sclk64_get
     Read the SLEEP_XTAL_TIMETICK counter. Adds the rollover offset to provide 
     64 bit dynamic range.

  time_sclk_get
     Get the slow-clock based time_sclk

  timetick_sclk64_resync_task
     Task for using a rex timer for timetick_sclk64 resync

  timetick_sclk64_resync
     The 32kHz-based time-of-day is periodically resynced to system time, if
  available.  If it has not been re-synced for 1.5 days, the 32kHz counter
  will 'rollover', and time_sclk would jump backwards 1.5 days.
  To prevent this from occuring, this timer callback is used to resync
  time_sclk before the rollover will occur.

REGIONAL FUNCTIONS
  timetick_sclk64_init
    Initializes the timetick_sclk64 subsystem

INITIALIZATION AND SEQUENCING REQUIREMENTS
 timetick_sclk64 must be called


Copyright (c) 2009 - 2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/timetick_sclk64.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/26/11   din     Fixed compiler warnings.
12/23/09   din     time_sclk_get returns zero if not initialized.
08/13/09   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_conv.h"
#include "DDITimetick.h"
#include "timetick_sclk64.h"
#include "timer.h"
#include "time_svc.h"
#include "osal.h"
#include "assert.h"
#include "ats_v.h"

#if defined(FEATURE_TIME_QDSP6) || defined(FEATURE_QUBE)
#include "qmutex.h"
#endif


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/


/*--------------------------------------------------------------------------
  Linting...
--------------------------------------------------------------------------*/

/*lint -esym(715,unused_*)  parameters prefixed with unused_ are not used */

/*-----------------------------------------------------------------------------
  Usefule Macros
-----------------------------------------------------------------------------*/
#define TIMETICK_REF_COUNT_MAX              0x7FFFFFFF

#define MSB_BIT_NUM                         31

#define MSB_BIT_VALUE                       0x80000000 

/* 10ms error theshold, in 1.25ms units */
#define TIMETICK_SCLK64_DELTA_THRESHOLD     8

/* Error threashold in ms */
#define TIMETICK_SCLK64_DELTA_THRESHOLD_MS  10

/*-----------------------------------------------------------------------------
  Time of day static information
-----------------------------------------------------------------------------*/

typedef struct
{
  /* Time tick corresponding to above timestamp */
  uint64                          base_timetick;
} timetick_sclk64_struct_type;

/* Device handle for timetick calls */
static DalDeviceHandle* hTimetickSclk64Handle = NULL;

static timetick_sclk64_struct_type      timetick_sclk64; 

/*Mapping for intlock*/
#ifndef FEATURE_TIME_QDSP6
 
#define ATS_INTLOCK() INTLOCK()
#define ATS_INTFREE() INTFREE()

#define ATS_INTLOCK_SAV(x) INTLOCK_SAV(x)
#define ATS_INTFREE_SAV(x) INTFREE_SAV(x)      

#else /*else for Q6*/

/* Pointer to the mutex object for protecting critical-section of timer 
   code*/
static qurt_mutex_t timetick_sclk64_mutex;

/*function protect the critical section of timer code*/
void timetick_sclk64_pimutex_init(void);


/*MACRO for locking/unlocking the pimutex*/
#define ATS_INTLOCK()  \
  qurt_pimutex_lock(&timetick_sclk64_mutex); \
  
#define ATS_INTFREE()  \
  qurt_pimutex_unlock(&timetick_sclk64_mutex); \

#define ATS_INTLOCK_SAV(X) \
  qurt_pimutex_lock(&timetick_sclk64_mutex); \
  
#define ATS_INTFREE_SAV(X)   \
  qurt_pimutex_unlock(&timetick_sclk64_mutex); \

#endif /*FEATURE_TIME_QDSP6*/

#ifdef FEATURE_TIME_QDSP6
/*=============================================================================

FUNCTION TIMETICK_SCLK64_PIMUTEX_INIT

DESCRIPTION
  This function initializes the pimutex that will be used for protecting
  the crit-section portion of time code.

DEPENDENCIES
  This function is applicable only for qurt.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void timetick_sclk64_pimutex_init(void)
{
  qurt_pimutex_init ( &timetick_sclk64_mutex );
}

#endif

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/


/*=============================================================================

FUNCTION TIMETICK_SCLK64_GET                                           

DESCRIPTION
  Read the QTIMER counter. Adds the rollover offset to provide 
  64 bit dynamic range.

DEPENDENCIES
  TIMETICK_INIT_SCLK64 must be called first to initialize the timer.

RETURN VALUE
   64-bit tick count

SIDE EFFECTS
  None

=============================================================================*/

uint64 timetick_sclk64_get(void)
{
  /* Current timetick */
  uint64 current_timetick = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the current timetick value */
  DalTimetick_GetTimetick64(hTimetickSclk64Handle, &current_timetick);

  /* Return the extended timetick value msb + the 32-bit timetick value */
  return current_timetick;
  
} /* timetick_sclk64_get */


/*=============================================================================

FUNCTION TIME_SCLK_GET                                                  

DESCRIPTION
  Get the slow-clock based time_sclk

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_sclk_get
(
  /* Output: Slow-clock based time_sclk in ts_timestamp format */
   time_type                       ts_val,
   /* Input: XO value to calculate time from */
   uint64                          xo_val
)
{
  /* Slow clock counter value to convert to timestamp */
  uint64                          xo_delta = xo_val;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Prevent changes to time_sclk structure while capturing time. */

  /* Copy the base timestamp to the output timestamp value */
  //qw_set( ts_val, 0, 0 );

  /* Get the elapsed number of ticks since recording the timetick base */
  if (xo_delta == (uint64)-1) 
  {
    DalTimetick_GetTimetick64(hTimetickSclk64Handle, &xo_delta);
  }

  /* Startup time is subtracted from ATS_RTC itself so that
     we dont need to do this calculation everytime */
  /* Subtract the startup time */
  //xo_delta -= timetick_sclk64.base_timetick;

  /* Advance the output timestamp by the elapsed slow clocks */
  timetick_sclk64_add_xo( ts_val, xo_delta );

} /* time_sclk_get */


/*=============================================================================

FUNCTION TIME_SCLK64_GET_BASE_TIMETICK                                          REGIONAL

DESCRIPTION
  return timetick_sclk64 base_timetick

=============================================================================*/
uint64 time_sclk64_get_base_timetick(void)
{
   return timetick_sclk64.base_timetick;
} /* time_sclk64_get_base_timetick */

/*=============================================================================

FUNCTION TIME_SCLK64_DEBUG_STATS                                          REGIONAL

DESCRIPTION
  Prints various details useful for debugging.

=============================================================================*/
void time_sclk64_debug_stats(void)
{
   ATS_ERR_2("base_timetick: 0x%.8x %.8x", timetick_sclk64.base_timetick>>32, timetick_sclk64.base_timetick);
}

/*=============================================================================

FUNCTION TIMETICK_SCLK64_INIT                                          REGIONAL

DESCRIPTION
  Initializes the timetick_sclk64 subsystem

DEPENDENCIES
  Must be called when the phone first powers on, from time_init( ).

RETURN VALUE
  None

SIDE EFFECTS
  Installs a timer to update base values before TimeTick rollover

=============================================================================*/

void timetick_sclk64_init( void )
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DalTimetick_Attach("SystemTimer", &hTimetickSclk64Handle);
  ASSERT(hTimetickSclk64Handle != NULL);
  DalTimetick_GetTimetick64(hTimetickSclk64Handle, &timetick_sclk64.base_timetick);

} /* timetick_sclk64_init */



