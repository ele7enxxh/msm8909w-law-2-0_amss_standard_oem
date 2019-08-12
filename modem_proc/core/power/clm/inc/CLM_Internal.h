/*============================================================================
@file CLM_Internal.h

Internal data structures and type declarations for CLM

Copyright (c) 2014-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.mpss/3.5.c12.3/power/clm/inc/CLM_Internal.h#1 $
============================================================================*/
#ifndef _CLM_INTERNAL_H_
#define _CLM_INTERNAL_H_

#include "DALSys.h"
#include "DALSysTypes.h"
#include "ULogFront.h"
#include "CoreVerify.h"
#include "CoreTime.h"
#include "CoreMutex.h"
#include "CoreHeap.h"

#if defined(__cplusplus)
  #define CLM_INLINE inline
#else
  #define CLM_INLINE __inline
#endif

#define CLM_LOW_THRESHOLD_LEVEL 0
#define CLM_HIGH_THRESHOLD_LEVEL 1
#define CLM_UNKNOWN_THRESHOLD_LEVEL 2 

#define CLM_EXTRA_LOGS 1

#define CLM_TIMER_TOLERANCE 250

#endif /* _CLM_INTERNAL_H_ */
