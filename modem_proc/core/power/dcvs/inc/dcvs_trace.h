#ifndef DCVS_TRACE_H
#define DCVS_TRACE_H
/*============================================================================
  @file dcvs_trace.h

  This header provides a variety of preprocessor definitions, type, data, and
  function declarations -- all intended for INTERNAL DCVS USE ONLY.
  
  If you are not a part of the DCVS module, you should not be using this file.

  This file provides an abstraction layer for QDSS tracer features.

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/inc/dcvs_trace.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#if !defined(WINSIM) && defined(DCVS_TRACER_SWEVT)
#include "tracer.h"
#include "dcvs_tracer_event_ids.h"
#endif

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#if defined(WINSIM) || !defined(DCVS_TRACER_SWEVT)

  #define DcvsTraceCpuDynamics(kHz, mb_s)
  #define DcvsTraceCpuMonitor(numArgs, ...)

#else // defined(WINSIM) || !defined(DCVS_TRACER_SWEVT)

  #define DcvsTraceCpuDynamics(kHz, mb_s) \
    tracer_event_simple_vargs(DCVS_CPU_FREQUENCY_UPDATE, 2, kHz, mb_s)

  #define DcvsTraceCpuMonitor(numArgs, ...) \
    tracer_event_simple_vargs(DCVS_CPU_UTILIZATION_UPDATE, numArgs, ##__VA_ARGS__);

#endif // defined(WINSIM) || !defined(DCVS_TRACER_SWEVT)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

#endif /* DCVS_TRACE_H */




