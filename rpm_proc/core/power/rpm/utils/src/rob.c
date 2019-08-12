/*===========================================================================
                              rob.c

SERVICES:

DESCRIPTION:

INITIALIZATION AND SEQUENCING REQUIREMENTS:
  Description...

Copyright (c) 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "rob.h"
#include "mpm.h"
#include "time_service.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
typedef struct
{
  uint64_t ts;
  uint8 valid;
} event_data_type;

typedef struct
{
  const rob_event_type start;
  const rob_event_type end;
  const uint64_t max;
} duration_data_type;


//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------
static svs_mode_id recording_speed;
static event_data_type event_data[ROB_NUM_EVENTS];

// Array to store the max durations - 2 dimensional as we store the durations for both RPM
// processor speeds.
static uint64_t duration[SVS_NUM_MODES][ROB_NUM_DURATIONS] = {0};

//Are these max values in sclks?
static duration_data_type duration_data[ROB_NUM_DURATIONS] =
  {
    {
      .start = ROB_EVENT_VDD_MIN_ENTER,
      .end = ROB_EVENT_MPM_PRE_SW_DONE,
      .max = 117200
    },
    {
      .start = ROB_EVENT_MPM_POST_SW_DONE,
      .end = ROB_EVENT_VDD_MIN_EXIT,
      .max = 117200
    },
    {
      .start = ROB_EVENT_XO_SD_ENTER,
      .end = ROB_EVENT_MPM_PRE_SW_DONE,
      .max = 117200
    },
    {
      .start = ROB_EVENT_MPM_POST_SW_DONE,
      .end = ROB_EVENT_XO_SD_EXIT,
      .max = 117200
    },
    {
      .start = ROB_EVENT_RPM_HALT_ENTER,
      .end = ROB_EVENT_RPM_HALT_PRE_CLKOFF,
      .max = 29300
    },
    {
      .start = ROB_EVENT_RPM_HALT_POST_CLKOFF,
      .end = ROB_EVENT_RPM_HALT_EXIT,
      .max = 29300
    },
    {
      .start = ROB_EVENT_VDD_MIN_BIMC_PCED_ENTER,
      .end = ROB_EVENT_MPM_PRE_SW_DONE,
      .max = 117200
    },
    {
      .start = ROB_EVENT_MPM_POST_SW_DONE,
      .end = ROB_EVENT_VDD_MIN_BIMC_PCED_EXIT,
      .max = 117200
    },
    {
      .start = ROB_EVENT_XO_SD_BIMC_PCED_ENTER,
      .end = ROB_EVENT_MPM_PRE_SW_DONE,
      .max = 117200
    },
    {
      .start = ROB_EVENT_MPM_POST_SW_DONE,
      .end = ROB_EVENT_XO_SD_BIMC_PCED_EXIT,
      .max = 117200
    }
  };
                                        
//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------


//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================
/*===========================================================================
FUNCTION: rob_mark_event (rob_event_type e)

DESCRIPTION: Records the timestamp associated with event e.

RETURN VALUE: The timestamp
===========================================================================*/
uint64_t
rob_mark_event (rob_event_type e)
{
  uint64_t tmp = time_service_now();
  event_data[e].ts = tmp;
  event_data[e].valid = 1;
  return tmp;
}

/*===========================================================================
FUNCTION: rob_mark_duration (rob_duration_type d)

DESCRIPTION: Returns the duration between the end and start events. If either
of the event timestamps are not valid, returns 0.

RETURN VALUE: The duration.
===========================================================================*/
uint64_t
rob_mark_duration (rob_duration_type d)
{
  uint64_t ret = 0;
  event_data_type *s = &event_data[duration_data[d].start];
  event_data_type *e = &event_data[duration_data[d].end];
  
  if (s->valid && e->valid)
  {
    ret = e->ts - s->ts;

    if ((ret > duration[recording_speed][d]) && (ret < duration_data[d].max))
    {
      duration[recording_speed][d] = ret;
    }
    else
    {
      ret = duration[recording_speed][d];
    }
  }
  
  s->valid = 0;
  e->valid = 0;
  return ret;
}

/*===========================================================================
FUNCTION: rob_get_duration (rob_duration_type d)

DESCRIPTION: Returns the duration associated 'd'

RETURN VALUE:
===========================================================================*/
uint64_t
rob_get_duration (rob_duration_type d, svs_mode_id speed)
{
  return duration[speed][d];
}

void rob_set_recording_speed(svs_mode_id speed)
{
  recording_speed = speed;
}
