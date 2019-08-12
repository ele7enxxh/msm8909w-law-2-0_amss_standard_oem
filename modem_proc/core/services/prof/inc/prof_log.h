#ifndef PROFLOG_H
#define PROFLOG_H
/*===========================================================================

                   L O G  P A C K E T S  F O R  R E X

DESCRIPTION
  This header file contains the definitions of log packets for task
  profiling, memory debug, code coverage, core dump, etc.
 
Copyright (c) 2002-2012      by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/prof/inc/prof_log.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

/* -----------------------------------------------------------------------
** Includes 
** ----------------------------------------------------------------------- */
#include "core_variation.h" 
#include "comdef.h"
#include "log.h"
#include "memheap.h"

#ifdef FEATURE_ERR_SERVICES
/* Include definitions that have moved to services/err */
#include "errlog.h"
#endif /* FEATURE_ERR_SERVICES */

/* ------------------------------------------------------------------------
** Constants
** ------------------------------------------------------------------------ */
#if defined(ADSP_PROF)
  #define PROCESSOR_ID  2
#elif defined(MPSS_PROF)
  #define PROCESSOR_ID  0
#endif

#ifndef TRACER_PROTOCOL_PROF_PM
#define TRACER_PROTOCOL_PROF_PM 1
#endif

#ifndef TRACER_PROTOCOL_PROF_TP
#define TRACER_PROTOCOL_PROF_TP 2
#endif


/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
#endif

