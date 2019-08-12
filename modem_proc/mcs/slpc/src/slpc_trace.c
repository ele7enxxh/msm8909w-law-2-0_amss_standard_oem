/*=========================================================================
  FILE: slpc_trace.c
 
  OVERVIEW:
 

  DEPENDENCIES:
 
 
  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.
=========================================================================*/

/*==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/slpc/src/slpc_trace.c#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who   what, where, why
----------   ---   ----------------------------------------------------------- 
09/14/2011   cab   Initial version 

============================================================================*/



/*============================================================================

                           INCLUDE FILES

============================================================================*/

#include "qurt_mutex.h"
#include "slpci.h"
#include "timetick.h"

/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typedefs,
and other items needed by this module.

============================================================================*/

/* sleep controller trace log storage */
slpc_trace_type slpc_trace;

/* for non reentrant sections within slpc_trace module */
qurt_mutex_t slpc_trace_lock;
#define INIT_NON_REENTRANT_SECTION() qurt_mutex_init( &slpc_trace_lock )
#define ENTER_NON_REENTRANT_SECTION() qurt_mutex_lock( &slpc_trace_lock )
#define LEAVE_NON_REENTRANT_SECTION() qurt_mutex_unlock( &slpc_trace_lock )


/******************************************************************************
  @brief SLPC_TRACE_EVENT
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none. 
******************************************************************************/
void slpc_trace_event( slpc_id_type id, slpc_trace_event_enum event,
                       uint64 a, uint64 b, uint64 c )
{
  slpc_trace_event_type       *e;

/*--------------------------------------------------------------------------*/

  ENTER_NON_REENTRANT_SECTION();

  /* Update the pointer to the last event ... */
  if ( ++slpc_trace.last == NUM_TRACE_EVENTS )
  {
    /* ... wrapping at buffer limit */
    slpc_trace.last = 0;
  }

  /* Get a pointer to the event structure, for convinence */
  e = &slpc_trace.events[ slpc_trace.last ];

  /* Record the event */
  e->id    = id;
  e->event = event;
  e->a   = a;
  e->b   = b;
  e->c   = c;
  e->ts  = slpc_get_tick();

  LEAVE_NON_REENTRANT_SECTION();

}


/******************************************************************************
  @brief SLPC_TRACE_EVENT_RLE
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none. 
******************************************************************************/
void slpc_trace_event_rle( slpc_id_type id, slpc_trace_event_enum event,
                           uint64 a, uint64 b )
{
  boolean                         rle = FALSE;
  slpc_trace_event_type           *e;
  uint16                          i, ending_index;

/*--------------------------------------------------------------------------*/

  ENTER_NON_REENTRANT_SECTION();

  /* Find last event by this id, if any */
  if ( slpc_trace.last == (NUM_TRACE_EVENTS - 1) )
  {
    ending_index = 0;
  }
  else
  {
    ending_index = slpc_trace.last + 1;
  }

  for(i = slpc_trace.last; i != ending_index; )
  {
    e = &slpc_trace.events[ i ];
    if ( e->id == id )
    {
      if ( e->event == event )
      {
        rle = TRUE;
        break;
      }
      else
      {
        break;
      }
    }
    else if ( i == 0 )
    {
      i = NUM_TRACE_EVENTS - 1;
    }
    else
    {
      i--;
    }
  }

  /* If the new event is the same as the old event */
  if ( rle == TRUE )
  {
    /* Update the record in place */
    e->a   = a;
    e->b   = b;
    e->c++;
    e->ts = slpc_get_tick();
  }
  else
  {
    /* Otherwise, add a new log entry */
    slpc_trace_event( id, event, a, b, 0 );
  }

  LEAVE_NON_REENTRANT_SECTION();
}

/******************************************************************************
  @brief SLPC_TRACE_INIT
 
  Put a detailed description of the function here. This should
  function as a user's manual for your code, providing a
  developer everything that needs to be known in order to use
  your code. 
 
  @param id: identifies the calling client
  @param tstmr_duration : the duration of sleep requested, in
                        tstmr ticks
  @see Func2
  @return none. 
******************************************************************************/
void slpc_trace_init( void )
{
  INIT_NON_REENTRANT_SECTION();
}
