#ifndef SLEEP_QR_TYPES_H
#define SLEEP_QR_TYPES_H
/*==============================================================================
  FILE:         sleep_qr_types.h
  
  OVERVIEW:     Internal types to support the quick response feature
 
  DEPENDENCIES: None

                Copyright (c) 2014-2015 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/statistics/sleep_qr_types.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "DALStdDef.h"

/*==============================================================================
                           INTERNAL TYPE DEFINITIONS
 =============================================================================*/
 /**
 * sleepQR_performed_type
 *
 * @brief State information of the synthmode that indicates what type of QR 
 *        event has been performed 
 */ 
typedef enum 
{
  SLEEP_QR_PERFORMED_NONE             = 0,  /* No QR event occured */
  SLEEP_QR_PERFORMED_COMPONENT_SKIP,        /* Synth has components that don't 
                                             * have valid data */ 
  SLEEP_QR_PERFORMED_SYNTH_ADJUST,          /* Synth has been adjusted as a parent */ 
  SLEEP_QR_PERFORMED_TRIGGER_ADJUST,        /* Synth has been adjusted as a trigger */ 
}sleepQR_performed_type;

/**
 * sleepQR_synthMode
 *
 * @brief Quick Response synthmode data structure used to react quickly to
 *        changing latency.
 */ 
typedef struct sleepQR_synthMode_s
{
  boolean component_adjusted; /* Flag that indicates a component backoff has
                               * been updated and the synth backoff needs to
                               * be recomputed */
  int32 last_delta;           /* Last sleep cycle wakeup error */
  int32 last_event_cnt;       /* Last QR event counter which indicates the
                               * number of late events in a row */
  sleepQR_performed_type QR_performed;  /* Value that indicates the type of QR
                                         * adjustment that has occured on this 
                                         * synthmode */
}sleepQR_synthMode;

/**
 * sleepQR_componentMode
 *
 * @brief Quick Response component mode data structure used to record LPRM data.
 */ 
typedef struct sleepQR_componentMode_s
{
  boolean monitor_backoff;    /* Flag to monitor backoff values */
  uint32 max_cycle_backoff;   /* Maximum measured value of last x cycles */
  uint32 min_cycle_backoff;   /* Minimum measured value of last x cycles */
  uint32 cycle_count;         /* Number of times this component mode has run */
  uint64 total_backoff;       /* Combined backoff of last x cycles */
  uint32 last_cycle_backoff;  /* Last measured backoff value */
  uint32 current_backoff;     /* Current component backoff value */
}sleepQR_componentMode;

#endif /* SLEEP_QR_TYPES_H */

