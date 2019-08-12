#ifndef __PMICSWEVT_H__
#define __PMICSWEVT_H__
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

  Copyright (c) 2011-13 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/utils/src/pm_sw_event.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  02/08/13   vk     Created.

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/


#ifdef PMIC_NPA_LOGGING
#include "tracer.h"
#include "pm_sw_event_id.h"
#endif


/*=========================================================================
      Macros
==========================================================================*/


#ifdef PMIC_NPA_LOGGING
#define pm_sw_event(...)  \
  tracer_event_simple_vargs(__VA_ARGS__);
#else
#define pm_sw_event(...)
#endif


#endif // __PMICSWEVT_H__

