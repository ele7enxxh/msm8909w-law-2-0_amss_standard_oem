#ifndef LPR_TYPES_H
#define LPR_TYPES_H
/*==============================================================================
  FILE:         LprTypes.h
  
  OVERVIEW:     This file defines the structures for CLprs, CLprMs, and
                CLprRegistry

  DEPENDENCIES: None

                Copyright (c) 2010-2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/compiler/LprTypes.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "CoreLock.h"
#include "sleep_lpri.h"
#include "sleepi.h"
#include "SleepLPR_synth_modes.h"

struct CLprRegistry_s;
typedef struct CLprRegistry_s CLprRegistry;

struct CLpr_s;
typedef struct CLpr_s CLpr;

struct CLprm_s;
typedef struct CLprm_s CLprm;

struct CLpr_s
{
  /* Array of synthesized modes in this CLpr */ 
  CLprm *m_modes[SLEEP_LPR_NUM_SYNTHESIZED_MODES];

  /* Number of enabled modes in the m_modes array */
  unsigned int enabled_mode_count;

  /* Pointer to a registry with which this CLpr is registered */
  CLprRegistry* m_registry;
  
  /* Core to which this CLpr is associated with */
  unsigned int sharing_cores;

  /* Pointer to synth lpr generated */
  sleep_lpr_synth *synth_lpr;
};

struct CLprm_s
{
  /**
   * cached_enter_latency
   *
   * @brief Cached value for enter latencies of component lprms. This value is 
   *        valid *only when* all the component modes have constant enter 
   *        latency.
   *        The array size is based on the number of static frequences given
   *        in the XML file and will not change. 
   */
  uint32 *cached_enter_latency;

  /**
   * cached_exit_latency
   *
   * @brief Same as cached_enter_latency except for exit latency. 
   *        Note that the number of elements will depend on the number of
   *        runtime frequencies encountered. 
   */
  uint32 *cached_exit_latency;

  /**
   * cached_backoff_latency
   *
   * @brief Same as cached_exit_latency except for backoff latency.
   */
  uint32 *cached_backoff_latency;
  
  /**
   * entered_modes
   *
   * @brief Keeps pointer to all component modes that have been entered.
   */
  sleep_lprm **entered_modes;

  /** 
   * wakeup_tick
   *
   * @brief The wake-up tick that's currently being used to
   * the enter functions. This duration could be updated by an 
   * enter function. 
   */
  uint64 wakeup_tick;

  /* Name of the LPRM */
  const char *lprmName;

  /* Pointer to the parent (or owner) LPR */
  CLpr* m_parent;

  /* Current status of the mode */
  sleep_status m_status;

  /* Array of all the component modes */
  sleep_lprm **m_componentModes;

  /* Number of component modes in case of presynthesized mode */
  uint32 num_expected_component_modes;

  /* Number of component modes that are enabled in this synthesized mode */
  uint32 num_enabled_component_modes;

  /* Number of cache & statistic data elements currently allocated */
  uint32 num_data_elements;

#ifdef SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT
  /* Data collected for profiling enter/exit latency times */
  void *wakeup_stats;
#endif
};


struct CLprRegistry_s
{
  /* List of all the LPRs registered in this registry */
  CLpr *m_lprs[SLEEP_NUM_CORES];

  /* Pointer list to the enabled mode with minimum enter + exit latency */
  CLprm* *m_minLprm[SLEEP_NUM_CORES];
};


#endif // LPR_TYPES_H
