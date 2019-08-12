#ifndef ROB_H
#define ROB_H
/*===========================================================================
                             rob.h

SERVICES:

DESCRIPTION:
  RPM Observer utility functions.

PUBLIC CLASSES AND FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:
  

Copyright (c) 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "comdef.h"
#include "svs.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif
  
typedef enum
{
  ROB_EVENT_VDD_MIN_ENTER,
  ROB_EVENT_VDD_MIN_EXIT,
  ROB_EVENT_XO_SD_ENTER,
  ROB_EVENT_XO_SD_EXIT,
  ROB_EVENT_MPM_PRE_SW_DONE,
  ROB_EVENT_MPM_POST_SW_DONE,
  ROB_EVENT_RPM_HALT_ENTER,
  ROB_EVENT_RPM_HALT_EXIT,
  ROB_EVENT_RPM_HALT_PRE_CLKOFF,
  ROB_EVENT_RPM_HALT_POST_CLKOFF,
  ROB_EVENT_VDD_MIN_BIMC_PCED_ENTER,
  ROB_EVENT_VDD_MIN_BIMC_PCED_EXIT,
  ROB_EVENT_XO_SD_BIMC_PCED_ENTER,
  ROB_EVENT_XO_SD_BIMC_PCED_EXIT,
  
  ROB_NUM_EVENTS
} rob_event_type;

// The durations that RPM observer can measure. Each duration is tied to a start
// and an end event (listed above)
typedef enum
{
  ROB_DURATION_VDD_MIN_ENTER,
  ROB_DURATION_VDD_MIN_EXIT,
  ROB_DURATION_XO_SD_ENTER,
  ROB_DURATION_XO_SD_EXIT,
  ROB_DURATION_RPM_HALT_ENTER,
  ROB_DURATION_RPM_HALT_EXIT,
  ROB_DURATION_VDD_MIN_BIMC_PCED_ENTER,
  ROB_DURATION_VDD_MIN_BIMC_PCED_EXIT,
  ROB_DURATION_XO_SD_BIMC_PCED_ENTER,
  ROB_DURATION_XO_SD_BIMC_PCED_EXIT,

  ROB_NUM_DURATIONS
} rob_duration_type;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

// Mark the occurance of an event to the observer.
// Returns the timetick that the event was recorded.
uint64_t rob_mark_event (rob_event_type e);

// Mark the completion of a duration aka observation interval to the observer.
// It is expected that the 'start' and 'end' events have been marked already
// via rob_mark_event ().
uint64_t rob_mark_duration (rob_duration_type);

// Get the maximum observation associated with the duration type.
// Currently, the function applied is 'max' - it could be changed to support
// other types of functions if necessary.
// Duration is a function of the svs_mode_id that the RPM was running when the
// observation was made.
uint64_t rob_get_duration (rob_duration_type d, svs_mode_id speed);

// Set the svs_mode_id that an observation run should associate with the timings.
void rob_set_recording_speed(svs_mode_id speed);

//===========================================================================
//                             Class Definitions
//===========================================================================
#ifdef __cplusplus
}
#endif
#endif // ROB_H
