#ifndef TRACER_THROTTLE_H
#define TRACER_THROTTLE_H
/** =========================================================================
  @file tracer_throttle.h
  QUALCOMM Debug Subsystem (QDSS) Tracer throttling

  API for throttling of tracer_data() messages:
    When the total number of bytes during each periodic time window
    (fixed at 1 millisecond) exceeds the treshold level (configurable),
    the data message is silently discarded. Subsequent messages within
    the time window will also be discarded. Message generation resumes
    after the end of the time window period.

    By default, throttling is disabled. Enable/disable throttling by
    setting the throttle threshold level.

    Throttling is not performed when tracer is unable to initialize
    throttle SW properly.

    Motivation: Other traffic sharing the AXI bus are being delayed.

===========================================================================*/
/*=============================================================================
  Copyright (c) 2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

/*=============================================================================
 $Header: //components/rel/core.mpss/3.5.c12.3/debugtrace/tracer/src/tracer_throttle.h#1 $
=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "comdef.h"

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* Initialization for throttling of tracer_data().
   Initialization must be successful for throttling to occur.
   Returns result in tracer_throttle.initialized field.
*/
void tracer_throttle_initialize(void);

/*-------------------------------------------------------------------------*/
/* Returns the SW trace data throttle threshold level in bytes per
   millisecond. A zero (0) value indicates throttling is disabled.
*/
uint32 tracer_throttle_get_threshold(void);

/*-------------------------------------------------------------------------*/
/* Sets the SW trace data throttle threshold level to the value specified
   in bytes per millisecond. A zero (0) value disables throttling.
*/
void tracer_throttle_set_threshold(uint32 bytes);

/*-------------------------------------------------------------------------*/
/* Determine whether target is throttling SW trace_data() generation or not.
   Returns: 0 for disabled (no throttling).
            1 for enabled (pkt exceeding threshold is silently discarded).
*/
int tracer_throttle_is_enabled(void);

/*-------------------------------------------------------------------------*/
/* Checks whether generation of message of size data_bytes should be
   generated or not.
   Returns: 0 = Throttling not needed; okay to generate the trace pkt.
            1 = Throttling is needed; do not generate the trace pkt.
*/
int tracer_throttle_check(uint32 data_bytes);

#endif /* #ifndef TRACER_THROTTLE_H */

