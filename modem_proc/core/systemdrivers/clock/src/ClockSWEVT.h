#ifndef __CLOCKSWEVT_H__
#define __CLOCKSWEVT_H__
/*
===========================================================================
*/
/**
  @file ClockSWEVT.h

  This header provides a variety of preprocessor definitions, type, data, and
  function declarations for QDSS event logging.

  This file provides an abstraction layer for QDSS tracer features.

*/
/*
  ====================================================================

  Copyright (c) 2011-12 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/clock/src/ClockSWEVT.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  11/02/12   frv     Created.

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/


#ifdef CLOCK_TRACER_SWEVT
#include "tracer.h"
#include "ClockSWEventId.h"
#endif


/*=========================================================================
      Macros
==========================================================================*/


#ifdef CLOCK_TRACER_SWEVT
#define Clock_SWEvent(...)  \
  tracer_event_simple_vargs(__VA_ARGS__);
#else
#define Clock_SWEvent(...)
#endif


#endif // __CLOCKSWEVT_H__

