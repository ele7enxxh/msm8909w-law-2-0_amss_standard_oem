#ifndef MCSPROFILE_H
#define MCSPROFILE_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

                                MCS PROFILING

GENERAL DESCRIPTION

  This file implements memory-only profiling to help diagnose performance
  problems in which MCS APIs are implicated


EXTERNALIZED FUNCTIONS

  None


REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

  None


  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document
  are confidential and proprietary information of Qualcomm Technologies
  Incorporated and all rights therein are expressly reserved.  By accepting
  this material the recipient agrees that this material and the information
  contained therein are held in confidence and in trust and will not be used,
  copied, reproduced in whole or in part, nor its contents revealed in any
  manner to others without the express written permission of Qualcomm
  Technologies Incorporated.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==



===============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/mcsprofile/inc/mcsprofile.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
01/27/2015   tl      Move MCS profiling into its own header

=============================================================================*/

/*=============================================================================
                           INCLUDE FILES FOR MODULE
=============================================================================*/

#ifndef T_WINNT
#include "rex.h"
#include "qurt.h"
#include "qurt_atomic_ops.h"
#endif /* T_WINNT */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ttl_map.h"

/*=============================================================================
                   DEFINITIONS AND DECLARATIONS FOR MODULE
=============================================================================*/

/*----------------------------------------------------------------------------
  MCS profiling

    The data structures and macros in this section implement memory-only
    logging of the processor time stamp at which certain markers were
    executed. This allows always-on profiling, and debugging of performance
    problems in which MCS APIs are implicated.

----------------------------------------------------------------------------*/

#ifdef T_WINNT
#error code not present
#else

#ifdef MCS_MARKER_USES_TTL

/* In builds with TTL enabled, use TTL. */
#define MCSMARKER TTLMARKER

#else

/* This data structure is statically allocated by every instance of the
 * MCSMARKER() macro to record the last time each individual event was hit. */
typedef struct {
  /* The processor pcycle count at the time of the event. This can be
   * cross-referenced with the kernel trace log. */
  unsigned long long int pcycle;
  /* Pointer to the task that was running the event. */
  rex_tcb_type* task;
} mcs_profile_type;

/* This data structure records the time, task, and marker for each of the last
 * MCS_PROFILE_COUNT markers. */
typedef struct {
  /* The processor pcycle count at the time of the event. This can be
   * cross-referenced with the kernel trace log. */
  unsigned long long int pcycle;
  /* Pointer to the task that was running the event. */
  rex_tcb_type* task;
  /* Pointer to the statically-allocated marker structure. This functions as
   * the event id. */
  const mcs_profile_type *ptr;
} mcs_profile_array_type;

/* The number of elements in mcs_profile_array. This must be a power of 2 for
 * the conversion in MCSMARKER() to work. */
#define MCS_PROFILE_COUNT 256

/* The index of the last entry in mcs_profile_array. This variable is shared
 * between threads and is accessed exclusively using atomic operations. This
 * variable does not wrap; the true array index is mcs_profile_index %
 * MCS_PROFILE_COUNT. */
extern unsigned int mcs_profile_index;

/* Circular buffer holding the MCS memory profiling data. */
extern mcs_profile_array_type mcs_profile_array[MCS_PROFILE_COUNT];

/* Record the current pcycle count and marker id in the mcs_profile_array
 * circular buffer. */
#define MCSMARKER(m) \
  do { \
    static mcs_profile_type mcs_profile_##m; \
    unsigned int idx; \
    mcs_profile_type temp; \
    idx = qurt_atomic_inc_return(&mcs_profile_index); \
    idx &= (MCS_PROFILE_COUNT - 1); \
    temp.pcycle = qurt_get_core_pcycles(); \
    temp.task = rex_self(); \
    mcs_profile_##m = temp; \
    mcs_profile_array[idx].pcycle = temp.pcycle; \
    mcs_profile_array[idx].task = temp.task; \
    mcs_profile_array[idx].ptr = &mcs_profile_##m; \
  } while(0)

#endif /* !MCS_MARKER_USES_TTL */
#endif /* !T_WINNT */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MCSPROFILE_H */
