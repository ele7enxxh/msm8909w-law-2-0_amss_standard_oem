/*!
  @file
  wci2_trace.c

  @brief
  Implementation for WCI2's debug trace logging

*/

/*===========================================================================

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

===========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/wci2/src/wci2_trace.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
           ckk     Initial implementation

==========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "wci2_trace.h"
#include "wci2_utils.h"
#include <timetick.h>
#include <stringl/stringl.h> /* memeset */


/*===========================================================================

                       DATA TYPES AND MACROS

===========================================================================*/
#define WCI2_TRACE_VERSION_NUM 1

/* Instance of WCI2's trace buffer */
STATIC wci2_trace_event_s wci2_trace_buffer;

/* WCI2 counters. Use wci2_counters_e enum as index into the array */
STATIC wci2_counter_type_s wci2_counters[ WCI2_CNT_MAX_COUNTERS ];

/*===========================================================================

  FUNCTION:  wci2_trace_init

===========================================================================*/
/*!
  @brief
    To initialize WCI2's debug trace/event logging interface

  @return
    None
*/
/*=========================================================================*/
void wci2_trace_init (
  void
)
{
  /*-----------------------------------------------------------------------*/

  /* Initialize the trace buffer */
  memset( &wci2_trace_buffer, 0, sizeof( wci2_trace_event_s ) );

  /* Set index to zero (begining of the trace buffer) */
  atomic_set( &(wci2_trace_buffer.seq_num), 0 );
  wci2_trace_buffer.version_number = WCI2_TRACE_VERSION_NUM;

  /* Initialize the counter buffer */
  memset( &wci2_counters, 0, sizeof( wci2_counters ) );  

  /*-----------------------------------------------------------------------*/

  return;
} /* wci2_trace_init */


/*===========================================================================

  FUNCTION:  wci2_trace_event

===========================================================================*/
/*!
  @brief
    To log WCI2 event into the trace buffer

  @return
    None
*/
/*=========================================================================*/
void wci2_trace_event
(
  wci2_trace_event_types_e event,   /*!< WCI2 event type */
  uint8                     a8,    /*!< 1st of 3 opaque payloads (8-bit) */
  uint16                    b16,   /*!< 2nd of 3 opaque payloads (16-bit) */
  uint32                    c32    /*!< 3rd of 3 opaque payloads (32-bit) */
)
{
  wci2_trace_event_log_s *log_p;     /* pointer to entry in table */ 
  atomic_plain_word_t      my_seq;    /* local seq # */
  uint32                   tbl_indx;  /* table index */

  /*--------------------------------------------------------------------------*/

  /* do an atomic increment of the sequence # and get the result */
  my_seq = atomic_inc_return( &(wci2_trace_buffer.seq_num) );

  /* decrement the result since we get the update value -
     this is our reserved sequence # */
  my_seq -= 1;

  /* mod the index to fit in the size of the table - keep it a power of 2! */
  tbl_indx = my_seq % WCI2_TRACE_NUM_OF_EVENTS;

  /* Get a pointer to the event structure, for convinence */
  log_p = &wci2_trace_buffer.events[ tbl_indx ];

  /* Record the event */
  log_p->event = (uint8)event;
  log_p->a8    = a8;
  log_p->b16   = b16;
  log_p->c32   = c32;
  log_p->ts    = timetick_get_safe();

  /*--------------------------------------------------------------------------*/

  return;
} /* wci2_trace_event */

/*===========================================================================

  FUNCTION:  wci2_counter_event

===========================================================================*/
/*!
  @brief
    To increment a WCI2 counter corresponding to some event. Also log the 
    timestamp and value of that event

  @return
    None
*/
/*=========================================================================*/
void wci2_counter_event
(
  wci2_counters_e counter,    /*!< Unique counter ID */
  uint32           last_value  /*!< Specific to the event that the counter 
                                 refers to. This is meant to be the value of
                                 the event we are counting */
)
{

  /*--------------------------------------------------------------------------*/

  /* guard against buffer overflow */
  WCI2_ASSERT( counter < WCI2_CNT_MAX_COUNTERS );

  /* log the counter event passed in + timestamp */
  wci2_counters[counter].count++;
  wci2_counters[counter].last_value = last_value;
  wci2_counters[counter].last_tstamp = timetick_get_safe();

  /*--------------------------------------------------------------------------*/

  return;
}

/*===========================================================================

  FUNCTION:  wci2_counter_reset

===========================================================================*/
/*!
  @brief
    Reset specified counter. If value passed is WCI2_CNT_MAX_COUNTERS, will
    reset ALL counters.

  @return
    None
*/
/*=========================================================================*/
void wci2_counter_reset
(
  wci2_counters_e counter
)
{

  /*--------------------------------------------------------------------------*/

  /* guard against buffer overflow */
  WCI2_ASSERT( counter <= WCI2_CNT_MAX_COUNTERS );

  if( WCI2_CNT_MAX_COUNTERS == counter )
  {
    /* reset all counters */
    memset( &wci2_counters, 0, sizeof( wci2_counters ) );
  }
  else
  {
    /* reset specific counter */
    wci2_counters[counter].count = 0;
  }

  /*--------------------------------------------------------------------------*/

  return;
}

/*===========================================================================

  FUNCTION:  wci2_get_counters

===========================================================================*/
/*!
  @brief
    Gets all counters

  @return
    None
*/
/*=========================================================================*/
void wci2_get_counters
(
  uint32 counters[]
)
{
  uint8 i;

  /*--------------------------------------------------------------------------*/

  /*get WCI2 counters*/
  for (i=0; i<WCI2_CNT_MAX_COUNTERS; i++)
  {
    counters[i] = wci2_counters[i].count;
  }

  /*--------------------------------------------------------------------------*/

  return;
}

