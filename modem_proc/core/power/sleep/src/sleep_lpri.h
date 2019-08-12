#ifndef SLEEP_LPRI_H
#define SLEEP_LPRI_H
/*==============================================================================
  FILE:         sleep_lpri.h
  
  OVERVIEW:     This file provides the internal functions and data used for
                sleep_lpr functionality.

  DEPENDENCIES: None

                Copyright (c) 2010-2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/sleep_lpri.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "sleep_lpr.h"
#include "npa_resource.h"
#include "CoreCpu.h"
#include "SleepLPR_synth_modes.h"

/* 
===============================================================================
                             MACRO DEFINITIONS
===============================================================================
*/

/**
 * @brief SLEEP_LPR_QUERY_RESOURCES
 *
 * ID for querying the list of registered LPRs.  List will persist in a valid
 * state until returned using SLEEP_LPR_RELEASE_RESOURCE_LIST.
 */
#define SLEEP_LPR_QUERY_RESOURCES (NPA_QUERY_RESERVED_END+1)

/**
 * @brief SLEEP_LPR_REGISTER
 *
 * Internal query.
 */
#define SLEEP_LPR_REGISTER (NPA_QUERY_RESERVED_END+2)

/**
 * SLEEP_ALL_CORES_BKCMP
 *
 * @brief Applicable to all cores on the processor.
 *
 * The use of this macro is just for backwards compatibility. Owners are
 * recommended to use SLEEP_ALL_CORES instead.
 */
#define SLEEP_ALL_CORES_BKCMP 0

/**
 * CORE_CURR_CORE
 *
 * @brief Function call to obtain core id on which this instruction is 
 *        getting executed based on platform.
 *
 * @Note:
 * Eventually will be moved to some commonplace outside sleep in case 
 * other subsystems, if any,  needs core id.
 */
#if SLEEP_NUM_CORES > 1
#define CORE_CURR_CORE Core_GetCurrCoreID()
#else
#define CORE_CURR_CORE 0
#endif



/*==========================================================================
 *                            DATA DECLARATIONS
 *==========================================================================*/

/**
 * gSleepNumOfCores
 *
 * @brief Number of available CPU cores
 */
extern volatile int gSleepNumOfCores;

/**
 * gSleepNumOfFreqs
 *
 * @brief Number of static frequencies associated with the number of mLUTs
 */
extern volatile int gSleepNumOfFreqs;

/* 
===============================================================================
                              TYPE DEFINITIONS
=============================================================================== 
*/

/* 
 * Structure that stores information for a presynthesized mode in case of 
 * offline synthesis version.
 */
typedef struct sleep_lprm_synth
{
  /* 
   * The name of this synthesized low power mode.  This should use the standard
   * naming convention for "lpr.lprm", with a '+' in between
   * modes. For example: "cxo.shutdown + pxo.shutdown".
   */
  const char *mode_name;

  /* 
   * The number of modes that are expected to be registered individually in 
   * order to complete this mode. 
   */
  unsigned int num_component_modes;

  /*
   * Array of pointers to the component modes in this synthesized mode.
   */
  sleep_lprm **m_componentModes;

}sleep_lprm_synth;

/*
 * Structure that represents presynthesized LPRs for offline synthesis version.
 */
typedef struct sleep_lpr_synth
{
  /*
   * The name associated with this resource, that is used to reference the
   * resource by other LPRs. This name can be used to indicate the LPR name
   * and the LPRM group name. The name must be a valid C-language identifier.
   * Further more, if the control of this resource is via an NPA resource, the
   * name of the LPR should be the "file" part of the NPA resource name (e.g.
   * an NPA resource named "/xo/cxo" would have a corresponding LPR named just
   * "cxo"). 
   */
  const char *resource_name;

  /* The number of low power modes this resource has. */
  uint32 mode_count;

  /*
   * The list of low power modes of this resource where preferred modes are
   * listed first. 
   */
  sleep_lprm_synth *modes;
  
  /*
   * Bitmask indicating core on which this synthesized LPR is relevant.
   * Owner can directly set the bit corresponding to the core (i.e. 0th bit 
   * for 0th core, 1st bit for 1st core). However, use of sleep core macros
   * defined in sleep public header file are recommended for this.
   *
   * Note: 
   * All the new synthesized lprs are expected to be relevant on just one
   * core. However, for backwards compatibility it is possible to have this
   * value as zero which means this lpr is shared by all cores.
   */
  uint32 sharing_cores; 
  
}sleep_lpr_synth;


/*
 * @brief
 * Structures that stores the enter/exit/backoff latency and power saving
 * values for a component sleep_lprm so that for a given sleep cycle we
 * don't keep calling those functions.
 *
 ***************************************************************************
 * @Attention 
 * During some sleep cycle, if any of latencies or power cached value is
 * updated, make sure to update all other fields as well otherwise it could
 * result into reading cached values from previous cyle for those fields.
 * With current implementation we are making sure that all latency and power
 * cached values are updated before any of the cached value is used for a
 * given cycle.
 ***************************************************************************
 *
 * @Note
 * Since in multicore environment each core can run sleep task 
 * independently with different duration/latency values, we 
 * need to cache values for each core 
 *
 */
typedef struct sleep_lprm_cached_fcn_data
{
  /* Indicates the cycle counter for the given sleep cycle. It must
   * be changed (increment usually) whenever any sleep parameter
   * like duration (frequency, temparature, etc) changes. It should be
   * noted that same sleep parameters do not result into same
   * cycle_id every time */
  uint32 enter_cycle_id[SLEEP_NUM_CORES][SLEEP_NUM_STATIC_FREQ];
  uint32 exit_cycle_id[SLEEP_NUM_CORES][SLEEP_NUM_STATIC_FREQ];
  uint32 backoff_cycle_id[SLEEP_NUM_CORES][SLEEP_NUM_STATIC_FREQ];
   
  /* Cached value for enter latency */
  uint32 enter_latency[SLEEP_NUM_CORES][SLEEP_NUM_STATIC_FREQ];
  
  /* Cached value for exit latency */
  uint32 exit_latency[SLEEP_NUM_CORES][SLEEP_NUM_STATIC_FREQ];
  
  /* Cached value for backoff latency */
  uint32 backoff[SLEEP_NUM_CORES][SLEEP_NUM_STATIC_FREQ];
}sleep_lprm_cached_fcn_data;

/*
===============================================================================

                        GLOBAL FUNCTION DECLARATIONS

===============================================================================
*/

/**
 * sleep_define_lpr_synth
 *
 * @brief This function will be called once per synthesized LPR at the start up 
 *        time to register a it with the sleep subsystem.
 *
 * Registering presynthesized LPRs requires apriori knowledge of all LPRs and 
 * their dependencies.
 *
 * @param resource: Low power resource we want to register with sleep 
 *                  system.
 */
void sleep_define_lpr_synth(sleep_lpr_synth *resource);

/**
 * sleep_client_request
 *
 * @brief Enables/disables the modes as request by state bitmask.
 * 
 * This function gets called from sleep LPR node's driver function.
 *
 * @param resource: NPA resource node corresponding to sleep LPR node.
 * @param client: Handle to client of sleep LPR node.
 * @param state: Bitmask corresponding to the new state of the client.
 *
 * @return New state of the client.
 */
npa_resource_state sleep_client_request (npa_resource *resource,
                                         npa_client *client,
                                         npa_resource_state state);

/**
 * getCurrCpuId
 *
 * @brief Returns the core/cpu ID in case of multicore environment.
 */
uint32 getCurrCpuId(void);


#ifdef __cplusplus
}
#endif

#endif /* SLEEP_LPRI_H */
