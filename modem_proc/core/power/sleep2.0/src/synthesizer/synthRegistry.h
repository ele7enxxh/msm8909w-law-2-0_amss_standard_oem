#ifndef SYNTHREGISTRY_H
#define SYNTHREGISTRY_H
/*==============================================================================
  FILE:         synthRegistry.h
  
  OVERVIEW:     This class provides regitry from where all the LPRs are being
                kept track of. It will enable us to access the (enabled) LPRs
                that are registered with sleep system.

  DEPENDENCIES: None

                Copyright (c) 2013-2014 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/synthesizer/synthRegistry.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "DALStdDef.h"
#include "sleep_lpr.h"
#include "synthTypes.h"
#include "synthLPR.h"
#include "synthLPRM.h"

typedef struct synthRegistry_s
{
  /* List of all the synth LPRs registered in this registry */
  sleep_synth_lpr *synthLPRs[SLEEP_NUM_CORES];
}synthRegistry;

/**
 * synthRegistry_getRegistry
 *
 * @brief Default constructor of synthRegistry class which will create a 
 *        registry without any LPR registered with it.
 */
synthRegistry *synthRegistry_getRegistry(void);

/**
 * synthRegistry_init
 *
 * @brief Default constructor of synthRegistry class which will create a 
 *        registry without any LPR registered with it.
 */
void synthRegistry_init(void);

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
void synthRegistry_readGlobalConfig(sleep_lpr *sleepLPR);

/**
 * synthRegistry_registerSleepLPR
 *
 * @brief Registers a sleep LPR with the registry 
 *  
 * @param lpr: sleep_lpr object we want to register with sleep system.
 */
void synthRegistry_registerSleepLPR(sleep_lpr *sleepLPR);

/**
 * synthRegistry_registerSynthLPR
 *
 * @brief Registers a synth LPR with the registry
 *
 * @param lpr: Presynthesized lpr we want to register with sleep system.
 */
void synthRegistry_registerSynthLPR(sleep_synth_lpr *synthLPR);

/**
 * synthRegistry_getSynthLPRList
 *
 * @brief This function is used to get the list of all the LPRs registered
 *        with sleep registry for a given core.
 *
 * @param core_id: Core id for which we want the list of synthLPR's registered
 *                 (starts from 0 to SLEEP_NUM_CORE-1). It defaults to -1
 *                 when no parameter is passed in which case it takes the
 *                 id of the core on which instruction is being executed.
 *
 * @return List of all the LPRs registered in this registry (may contain
 *         synthesized LPRs as well)
 */
sleep_synth_lpr* synthRegistry_getSynthLPRList(int32 coreID );

/**
 * synthRegistry_LPRUpdated
 *
 * @brief Updates the list of enabled LPRs based on the status of the input
 *        LPR
 *  
 * @param lpr: sleep_synth_lpr with status change.
 */
void synthRegistry_LPRUpdated(sleep_synth_lpr *synthLPR);

#endif // SYNTHREGISTRY_H
