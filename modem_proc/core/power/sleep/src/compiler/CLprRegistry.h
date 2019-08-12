#ifndef CLPRREGISTRY_H
#define CLPRREGISTRY_H
/*==============================================================================
  FILE:         CLprRegistry.h
  
  OVERVIEW:     This class provides regitry from where all the LPRs are being
                kept track of. It will enable us to access the (enabled) LPRs
                that are registered with sleep system.

  DEPENDENCIES: None

                Copyright (c) 2010-2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/compiler/CLprRegistry.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "sleepi.h"
#include "sleep_lpr.h"
#include "CoreLock.h"
#include "LprTypes.h"
#include "CLpr.h"
#include "CLprm.h"
#include "lookup_table_types.h"


/**
 * CLprRegistry
 *
 * @brief Default constructor of CLprRegistry class which will create a 
 *        registry without any LPR registered with it.
 */
void CLprRegistry_init( CLprRegistry *hCLprReg );

/**
 * ~CLprRegistry
 *
 * @brief Destructor of the CLprRegistry class.
 */
void CLprRegistry_deinit( CLprRegistry *hCLprReg );

/**
 * readGlobalConfig
 *
 * @brief Reads the global config from the EFS, and fills in 
 *        enable/disable/config data for the LPR passed in.
 *  
 *        Note that this function is called outside of the sleep
 *        node lock, so it must be re-entrant.
 *
 * @param lpr: Presynthesized lpr we want to read the global 
 *             config for with sleep system.
 */
void CLprRegistry_readGlobalConfig( CLprRegistry *hCLprReg, sleep_lpr *lpr );

/**
 * registerLpr
 *
 * @brief Registers a CLpr for the input sleep_lpr struct with this registry. 
 *
 * @param lpr: sleep_lpr object we want to register with sleep system.
 */
void CLprRegistry_registerLpr( CLprRegistry *hCLprReg, sleep_lpr *lpr );

/**
 * registerLpr
 *
 * @brief Registers a CLpr for the input presynthesized lpr struct with this
 *        registry.
 *
 * @param lpr: Presynthesized lpr we want to register with sleep system.
 */
void CLprRegistry_registerLpr_synth( CLprRegistry *hCLprReg, 
                                     sleep_lpr_synth *lpr );

/**
 * getLprsList
 *
 * @brief This function is used to get the list of all the LPRs registered
 *        with sleep registry for a given core.
 *
 * @param core_id: Core id for which we want the list of CLprs registered
 *                 (starts from 0 to SLEEP_NUM_CORE-1). It defaults to -1
 *                 when no parameter is passed in which case it takes the
 *                 id of the core on which instruction is being executed.
 *
 * @return List of all the LPRs registered in this registry (may contain
 *         synthesized LPRs as well)
 */
CLpr* CLprRegistry_getLprsList( CLprRegistry *hCLprReg, int core_id );

/**
 * getNumberEnabledLprs
 *
 * @brief Checks how many enabled CLprs are in the registry for a given core.
 *
 * @param core_id: Core id for which we want the number of enabled CLprs
 *                 (starts from 0 to SLEEP_NUM_CORE-1). It defaults to -1
 *                 when no parameter is passed in which case it takes the
 *                 id of the core on which instruction is being executed.
 *
 * @return Returns the number of enabled CLprs in the registry.
 */
unsigned int CLprRegistry_getNumberEnabledLprs( CLprRegistry *hCLprReg, 
                                                int core_id );

/**
 * lprUpdated
 *
 * @brief Updates the list of enabled CLprs based on the status of the input
 *        CLpr
 *
 * If the input CLpr is enabled, it will be added to list of enabled CLprs in
 * registry and if it is disabled, it will be removed from the enabled CLprs'
 * list, if its present.
 *
 * @param lpr: CLpr with status change.
 */
void CLprRegistry_lprUpdated( CLprRegistry *hCLprReg, CLpr* lpr );

/* Functions for some sleep optimizations */

/**
 * getMinEnabledModeLatency
 *
 * @brief Returns the sum of enter and exit latencies of the minimum enabled 
 *        mode.
 *
 * Here mode is considered as minimum when the sum of its enter and exit 
 * latencies is the least among all other modes.
 *
 * @Note:
 * This function will return minimum latency for the core running it and 
 * hence it is intended to be called in idle context only.
 *
 *************************************************************************
 *
 * @param duration     : Time duration in ticks for which we might enter
 *                       into low power modes. Default value is zero which 
 *                       will prevent any optimizations for non constant 
 *                       latency functions.
 * @param pfreq_LUT: Frequency lookup table pointer for current core frequency  
 * @param cycle_id: Indicates the sleep cycle count
 *
 * @return Latency of the minimum mode as described above.
 */
uint32 CLprRegistry_getMinEnabledModeLatency( CLprRegistry            *hCLprReg,
                                              uint64                  duration,
                                              sleep_freq_lookup_table *pfreq_LUT,
                                              uint32                  cycle_id);

/**
 * updateMinLatencyEnabledMode
 *
 * @brief Goes through all the enabled modes and determines the one with least
 *        enter and exit latencies and updates the corresponding member
 *        fields.
 *
 * @param lpr: Resource that may have caused to update minimum latency.
 */
void CLprRegistry_updateMinLatencyEnabledMode( CLprRegistry *hCLprReg, 
                                               CLpr *lpr, CLprm *lprm);

#endif // CLPRREGISTRY_H
