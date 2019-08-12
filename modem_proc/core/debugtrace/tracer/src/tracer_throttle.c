/*===========================================================================
  FILE: tracer_throttle.c

  OVERVIEW: QUALCOMM Debug Subsystem (QDSS) Tracer throttling

===========================================================================*/
/*=============================================================================
  Copyright (c) 2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
 $Header: //components/rel/core.mpss/3.5.c12.3/debugtrace/tracer/src/tracer_throttle.c#1 $
=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "stringl/stringl.h"
#include "atomic_ops_plat.h"
#include "DDITimetick.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

typedef struct {

   uint32 running_sum; // Cummulative bytes since start time.

   volatile uint32 threshold_bytes;
      // 0 = no throttle; threshold in bytes to limit tracer_data()
      // generation per TRACER_THROTTLE_TIMETICKS_PER_MS period.

   DalTimetickTime64Type start_time; // Time when sum was started.

   DalDeviceHandle *phDalTimetick; // Handle to timetick

   int initialized; // Throttling SW is initialized (1) or
                    //   not (0).

   uint32 window_timeticks; // Time window in timeticks per ms.

} tracer_throttle_info_t;


/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * ------------------------------------------------------------------------*/

/* Globally shared tracer throttle information
 */
static tracer_throttle_info_t tracer_throttle;

//=========================================================================//
// Tracer SW trace data throttle functions
//=========================================================================//

/*-------------------------------------------------------------------------*/
void tracer_throttle_initialize(void)
{
   // Initialize throttle information
   memset(&tracer_throttle, 0, sizeof(tracer_throttle));

   if(DAL_SUCCESS == DalTimetick_Attach("SystemTimer",
                                        &(tracer_throttle.phDalTimetick)))
   {
      if (DAL_SUCCESS == DalTimetick_GetFreq(
                            tracer_throttle.phDalTimetick,
                            &(tracer_throttle.window_timeticks)))
      {
         tracer_throttle.window_timeticks /= 1000; // Ticks per millisecond
         tracer_throttle.initialized = 1; // Throttle is initialized
      }
   }
}

/*-------------------------------------------------------------------------*/
uint32 tracer_throttle_get_threshold(void)
{
   return tracer_throttle.threshold_bytes;
}

/*-------------------------------------------------------------------------*/
void tracer_throttle_set_threshold(uint32 bytes)
{
   tracer_throttle.threshold_bytes = bytes;
   return;
}

/*-------------------------------------------------------------------------*/
int tracer_throttle_is_enabled(void)
{
   return ((0 == tracer_throttle.threshold_bytes) ||
           (0 == tracer_throttle.initialized)) ? 0 : 1;
}

/*-------------------------------------------------------------------------*/
// Return: 0 = do not throttle, 1 = throttle needed.
/* Programmer note:
    Global parameters running_sum and start_time are dynamic values that
    are potentially read/write by several threads. Several corner cases
    are not nicely covered with priority given to speed. For example, the
    time_window could get reset by a higher priority thread then reset
    again as the lower priority thread completes.
*/
int tracer_throttle_check(uint32 data_bytes)
{
   DalTimetickTime64Type time_now;
   volatile DalTimetickTime64Type time_start;
      // Protect against global start time getting
      // updated and becoming greater than time_now.

   if (!tracer_throttle_is_enabled())
   {
      return 0;  // Throttling not on.
   }

   //atomic_barrier();
   time_start = tracer_throttle.start_time;

   // Check if the running_sum exceeded the threshold within the
   // given time window.
   if (atomic_add_return(&(tracer_throttle.running_sum), data_bytes) >
       tracer_throttle.threshold_bytes)
   {
      DalTimetick_GetTimetick64(tracer_throttle.phDalTimetick, &time_now);

      if (tracer_throttle.window_timeticks > (time_now - time_start))
      {
         // Ideally, want to remove data_bytes from the sum here.
         // However, it is not easy to do so in thread-safe manner.
         // As such, subsequent packets that are within the threshold
         // level are also dropped where they would not have been if the
         // previous discarded packet was not added to the sum.
         // atomic_sub(&(tracer_throttle.running_sum), data_bytes);

         return 1;  // Throttling is needed.
      }
      else
      {
         // Start a new time window - initialize
         atomic64_set(&(tracer_throttle.start_time), time_now);
         atomic_set(&(tracer_throttle.running_sum), data_bytes);
      }
   }
   return 0;   // Throttling not needed.
}

