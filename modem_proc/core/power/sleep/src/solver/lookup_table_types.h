#ifndef LOOKUP_TABLE_TYPES_H
#define LOOKUP_TABLE_TYPES_H
/*==============================================================================
  FILE:         lookup_table_types.h
  
  OVERVIEW:     Data structures for lookup table solver

  DEPENDENCIES: None

                Copyright (c) 2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/solver/lookup_table_types.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "sleep_stats.h"
#include "LprTypes.h"

/* ========================================================================
                              TYPE DECLARATIONS
=========================================================================== */

typedef union
{
  int   mode_idx;
  CLprm *mode_ptr;
} LUT_elem_u;

typedef struct sleep_LUT_elem
{
  /* The minimum duration during which this mode should be chosen for entry. */
  uint64 duration_thresh;

  /* The mode to enter, if the sleep duration is within the given range,
   * provided that the latency and duration restrictions are still met.
   * For a given duration, the solver will traverse this list of modes,
   * and find the first one that meets the constraints. */
  LUT_elem_u mode[SLEEP_LPR_NUM_SYNTHESIZED_MODES];

  /* Statistics for duration hit counts */
  sleepStats_generic element_stats;
} sleep_LUT_elem;

typedef struct sleep_lookup_table
{
  /* The number of table element entries in this table */
  uint32 numElements;

  /* The mode to enter, if the sleep duration is within the given range,
   * provided that the latency and duration restrictions are still met.
   * For a given duration, the solver will traverse this list of modes,
   * and find the first one that meets the constraints. */
  sleep_LUT_elem *element;

} sleep_lookup_table;

typedef struct sleep_freq_lookup_table
{
  /* The frequency (in HZ) at which the modeTime values apply */
  uint32 frequency;

  /* mode LUT index to use for synth mode selection */
  uint32 mLUT_idx;

  /* Index value to use for statistic & latency cache data arrays */
  uint32 data_idx;

  /* Linked list pointer */
  struct sleep_freq_lookup_table *next;

} sleep_freq_lookup_table;

#endif /* LOOKUP_TABLE_TYPES_H */
