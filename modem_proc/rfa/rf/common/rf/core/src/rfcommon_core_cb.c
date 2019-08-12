/*!
   @file
   rfcommon_core_cb.c

   @brief
    This file contains the RF callback functions.

   @details

*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/src/rfcommon_core_cb.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/11   sr      merged cmi code from SCMM to remove the clk.h dependency.  
08/13/09   ckl     Modifications for SCMM.
10/17/06   ycl     Modifications for initial MSM7600 build.
01/18/06   dp/bhas Fix bug with handling callback delay parameter.
07/01/02   sd      Initial revision.
===========================================================================*/

#include "rfcom.h"
#include "timer.h"
#include "amssassert.h"
#include "rfcommon_core_cb.h"

/*---------------------------------------------------------------------------
  Maximum number of clock services callbacks that can be active
  simultaneously.
---------------------------------------------------------------------------*/
#define MAX_NUM_CB_SUPPORTED    2

/* function prototypes */
static void rfcommon_core_cb_handler0( int32 time_ms, timer_cb_data_type data );

static void rfcommon_core_cb_handler1( int32 time_ms, timer_cb_data_type data );

/*---------------------------------------------------------------------------
  Array of callback event structures.  Each element of the array handles
  an active clock services callback.  These are required to handle
  cases where multiple callback events may be registered simultaneously
  with the clock services.  Note: This would not be necessary if clock
  services supported passing a user data pointer to the callbacks
  called by clock services.
---------------------------------------------------------------------------*/
static struct handler_struct
{
  /*! @brief Pointer to callback event structure initialized to define events
     to generate. */
  rfcommon_core_cb_struct_type *cb_event_ptr;

  /*! @brief Pointer to array element of events that identifies current event
     to process.  Events are within callback event structure. */
  rfcommon_core_cb_event_struct_type *current_event_ptr;

  /*! @brief Clock services callback structure */
  timer_type clock_cb;

  /*! @brief Call back routine to be called by clock services */
  void (*clk_routine_ptr)( int32 time_ms, timer_cb_data_type data );

} handler[MAX_NUM_CB_SUPPORTED];

/*!
  @brief
  Free queue to hold buffers used to register callback event sequences.
 */
q_type free_que;

/*!
  @brief
  Items to be placed on free queue.
 */
rfcommon_core_cb_struct_type free_que_bufs[MAX_NUM_CB_SUPPORTED];

/*!
  @brief
  Array of pointers to clock routine for initialization of handler
  structure.
 */
void (*clk_routine_ptrs[MAX_NUM_CB_SUPPORTED])( int32, timer_cb_data_type ) =
{
  rfcommon_core_cb_handler0,
  rfcommon_core_cb_handler1
};

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initializes the callback services.

  @details
  It initializes the callback services. This function must be called before
  calling any other function exported by the callback services.

  @param

*/
void rfcommon_core_cb_init( void )
{
  struct handler_struct *handler_ptr;
  rfcommon_core_cb_struct_type *buf_ptr;
  int i;

  /* Initialize the free queue. */
  (void) q_init( &free_que );

  /* Initialize each element of handler array and free queue. */
  for ( i = 0; i < MAX_NUM_CB_SUPPORTED; i++ )
  {
    /* Initialize link for each item to be placed on free queue,
       initialize pointer to handler element, and place each item on
       free queue. */
    buf_ptr = &free_que_bufs[i];
    (void) q_link( buf_ptr, &buf_ptr->hdr.q_link );
    q_put( &free_que, &buf_ptr->hdr.q_link );
    handler_ptr = &handler[i];
    buf_ptr->hdr.handler_ptr = (void*) handler_ptr;

    /* Initialize the fields of each handler element. */
    handler_ptr->cb_event_ptr = NULL;
    handler_ptr->current_event_ptr = NULL;
    timer_def2( &handler_ptr->clock_cb, NULL );
    handler_ptr->clk_routine_ptr = clk_routine_ptrs[i];
  }

} /* rfcommon_core_cb_init() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Gets an event structure buffer

  @details
  Gets an event structure buffer that must be used to register a sequence
  of callback events.  Buffer is freed by making a call to
  rfcommon_core_cb_register_events() with the buffer pointer as input.

  @param

*/
rfcommon_core_cb_struct_type* rfcommon_core_cb_get_event_buffer( void )
{
  rfcommon_core_cb_struct_type *rtn_ptr;

  /* Get buffer from free queue. */
  rtn_ptr = (rfcommon_core_cb_struct_type*) q_get( &free_que );

  /* Throw assert if queue empty.  This implies the number of call
     backs supported must be increased. */
  ASSERT( rtn_ptr != NULL );

  /* Make sure we stop here even if ASSERTs are not enabled within
     development builds, which seems to be common practice. */
  if ( rtn_ptr == NULL )    /*lint !e774, Boolean within 'if' always evaluates to False */
  {
    ERR_FATAL("Need to increase number callbacks supported.", 0, 0, 0 );
  }

  return rtn_ptr;

} /* rfcommon_core_cb_get_event_buffer() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Register callback events

  @details
  Registers a sequence of callback events based on events defined in
  events structure passed in.  A sequence of callback events consists
  of a series of calls to the same callback function, with each call
  having a different event passed in.  Each call to the callback function
  occurs at the delay defined in the events structure passed in.

  The input buffer pointed to by cb_event_ptr is freed internally by
  this module after the event sequence has been generated.

  @param rfcommon_core_cb_struct_type Pointer to callback event structure.
  This structure must be initialized to define the event sequence to generate.

*/
void rfcommon_core_cb_register_events
(
  rfcommon_core_cb_struct_type *cb_event_ptr
)
{
  struct handler_struct *handler_ptr;

  /* Verify callback event pointer is valid. */
  ASSERT( cb_event_ptr != NULL );

  /* Verify callback handler pointer is valid. */
  ASSERT( cb_event_ptr->cb_handler != NULL );

  /* Point to current handler element. */
  handler_ptr = cb_event_ptr->hdr.handler_ptr;

  /* Verify handler pointer is valid. */
  ASSERT( handler_ptr != NULL );

  /* Verify clock services routine pointer is valid. */
  ASSERT( handler_ptr->clk_routine_ptr != NULL );

  /* Setup handler to point at callback event structure passed in. */
  handler_ptr->cb_event_ptr = cb_event_ptr;

  /* Setup current event pointer to point to first event in array
     of events. */
  handler_ptr->current_event_ptr = &cb_event_ptr->cb_events[0];

  /* Regisiter clock services routine to be called at the next event
     time rounded up to the nearest millisecond and decrement number
     of events to generate. */
  --cb_event_ptr->num_events;

  timer_reg(&handler_ptr->clock_cb, /* Pointer to the call back struct*/
            (timer_t2_cb_type)handler_ptr->clk_routine_ptr, /* Addr of the callback func */
			(timer_cb_data_type)NULL, /* No data */
            (int4)((handler_ptr->current_event_ptr->delay + 999) / 1000), /* Delay till first call back */
            (int4)0); /* Delay between repeat call backs backs? */
} /* rfcommon_core_cb_register_events() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Call callback handler

  @details
  Calls callback handler each time an event expires and registers
  next event with clock services.  Note: This function is called from
  interrupt level.

  @param handler_struct Pointer to handler array element defining events to
  generate.

*/
static void rfcommon_core_cb_generate_events
(
  struct handler_struct *handler_ptr
)
{
  rfcommon_core_cb_struct_type *cb_event_ptr;

  /* Set pointer to callback event structure in handler. */
  cb_event_ptr = handler_ptr->cb_event_ptr;

  /* Call the callback handler for the current event. */
  cb_event_ptr->cb_handler( handler_ptr->current_event_ptr->event,
                            (void*) cb_event_ptr->user_data_ptr );

  /* Determine if there are more events to generate. */
  if ( cb_event_ptr->num_events > 0 )
  {
    /* More events need to be generated so increment pointer to the next
       event, decrement number of events remaining, and register clock
       services callback routine at time next event should occur. */
    --cb_event_ptr->num_events;
    ++handler_ptr->current_event_ptr;

    /* Regisiter clock services routine to be called at the next event
       time rounded up to the nearest millisecond. */
    timer_reg(&handler_ptr->clock_cb,
              (timer_t2_cb_type)handler_ptr->clk_routine_ptr,
              (timer_cb_data_type)NULL,
              (int4)(((int)(handler_ptr->current_event_ptr->event) + 999) / 1000),
              (int4)0);
  }
  else
  {
    /* Free the event buffer to indicate all events have been generated. */
    q_put( &free_que, &cb_event_ptr->hdr.q_link );
  }

} /* rfcommon_core_cb_generate_events() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Callback routine

  @details
  Call back routines called by clock services when time expires.  A
  seperate callback handler is required for each callback event
  sequence currently active.  Note: These functions are called from
  interrupt level.

  @param interval_ms Interval in milliseconds when callback called.

*/

/* ARGSUSED */
static void rfcommon_core_cb_handler0( int32 time_ms, timer_cb_data_type data )
{
  rfcommon_core_cb_generate_events( &handler[0] );
} /* cb_handler0() */

/* ARGSUSED */
static void rfcommon_core_cb_handler1( int32 time_ms, timer_cb_data_type data )
{
  rfcommon_core_cb_generate_events( &handler[1] );
} /* cb_handler1() */

