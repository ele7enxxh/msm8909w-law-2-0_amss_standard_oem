#ifndef SYNTHTYPES_H
#define SYNTHTYPES_H
/*==============================================================================
  FILE:         synthTypes.h
  
  OVERVIEW:     This file defines the structures for synth LPRs, & LPRMs

  DEPENDENCIES: None

                Copyright (c) 2013-2015 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/synthesizer/synthTypes.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "sleepi.h"
#include "DALStdDef.h"
#include "sleep_lpr.h"
#include "SleepLPR_synth_modes.h"

/*
 * Helper structure for the presynthesized LPR data
 */
typedef struct synth_seed_latency_s
{
  /* Combined enter & exit latency values at each scaled frequency */
  uint32  enter_exit[SLEEP_NUM_STATIC_FREQ];

  /* If the backoff adjustment feature is enabled, this will be a single backoff
   * value at slowest given frequency.
   * If feature is disabled, it will be the scaled values of the given static
   * frequency list
   */ 
#ifdef SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT
  uint32  backoff[1]; 
#else
  uint32  backoff[SLEEP_NUM_STATIC_FREQ];
#endif
}synth_seed_latency;

/* Typedef's here due to circular dependency between the two strcutures */
typedef struct sleep_synth_lpr_s   sleep_synth_lpr;
typedef struct sleep_synth_lprm_s  sleep_synth_lprm;

/* 
 * Structure that stores information for a presynthesized mode
 */
struct sleep_synth_lprm_s
{
  /* The name of this synthesized low power mode.  This should use the standard
   * naming convention for "lpr.lprm", with a '+' in between
   * modes. For example: "cxo.shutdown + pxo.shutdown". */
  const char *name;

  /* Dynamic index number used to access synth LPRM array data in the dynamic
   * statistics array */
  uint32 dynamic_num;

  /* The number of modes that are expected to be registered individually in 
   * order to complete this mode. */
  uint32 num_component_modes;

  /* Array of pointers to the component modes in this synthesized mode. */
  sleep_lprm **component_modes;

  /* Initial latency values as generated via SleepSynth tool. These are the
   * combined LPRM latency values */
  synth_seed_latency *seed_latency;

  /* Pointer to the parent synth LPR*/
  sleep_synth_lpr *synth_LPR_parent;

  /* Bitmask containing various attributes for the synth mode like 
   * cacheability. */
  synth_mode_attr  attr;

  /* Current status of the mode */
  sleep_status mode_status;

  /* Number of component modes that are enabled in this synthesized mode */
  uint32 num_enabled_component_modes;

  /* The wake-up tick that's currently being used to the enter functions. 
   * This duration could be updated by an enter function. */ 
  uint64 wakeup_tick;
};

/*
 * Structure that represents presynthesized LPRs
 */
struct sleep_synth_lpr_s
{
  /* The name associated with this resource, that is used to reference the
   * resource by other LPRs. This name can be used to indicate the LPR name
   * and the LPRM group name. The name must be a valid C-language identifier.
   * Further more, if the control of this resource is via an NPA resource, the
   * name of the LPR should be the "file" part of the NPA resource name (e.g.
   * an NPA resource named "/xo/cxo" would have a corresponding LPR named just
   * "cxo"). 
   */
  const char *resource_name;

  /* Index number used to access synth LPR array data in the dynamic
   * statistics array */
  uint32 core_num;

  /* The number of low power modes this resource has. */
  uint32 mode_count;

  /* The list of low power modes of this resource where preferred modes are
   * listed first. 
   */
  sleep_synth_lprm *modes;

  /* Number of enabled modes in the array */
  uint32 enabled_mode_count;
};

#endif // SYNTHTYPES_H

