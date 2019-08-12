#ifndef SYNTHLPRM_H
#define SYNTHLPRM_H
/*==============================================================================
  FILE:         synthLPRM.h
  
  OVERVIEW:     Provides support functions for the synth LPRMs

  DEPENDENCIES: None

                Copyright (c) 2013-2014 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/synthesizer/synthLPRM.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "DALStdDef.h"
#include "sleepi.h"
#include "synthTypes.h"
#include "sleep_statsi.h"

/**
 * synthLPRM_isCacheable
 * 
 * @brief Function tests Synth LPRM for Cacheability
 *
 * @return Returns true if the mode is Cacheable, false otherwise 
 */
boolean synthLPRM_isCacheable( sleep_synth_lprm  *synthLPRM );

/**
 * synthLPRM_getLPRMLatency
 *
 * @brief Will be used to find out time required to enter into this mode.
 *
 * If the mode is the presynthesized one, its enter latency will be sum of all
 * its component modes' enter latencies.
 *
 * @param mLUTidx: Mode table index to use to for latency value of lprm 
 
 * @return Returns total time required by this mode to enter.
 */
uint32 synthLPRM_getLPRMLatency(sleep_synth_lprm  *synthLPRM,
                                uint32            mLUTidx);
/**
 * synthLPRM_getBackOffTime
 *
 * @brief Calculates the back off time for the given sleep_synth_lprm. If any of the 
 *        component modes has not specified back off time, its exit
 *        latency will be used.
 *
 * @param fLUT: Frequency LUT pointer for current core frequency 
 *
 * @return Backoff time for the calling sleep_synth_lprm.
 */
uint32 synthLPRM_getBackOffTime( sleep_synth_lprm *synthLPRM,
                                 sleep_fLUT_node  *fLUT);

/**
 * synthLPRM_enter
 *
 * @brief Calls the enter function for a mode or in case of presynthesized
 *        mode calls enter functions of component modes for given durations.
 *
 * @param synthLPRM: Selected mode pointer from solver function
 * @param wakeup_tick: Absolute wakeup time in ticks 
 * @param fLUT: Frequency table pointer for the current core frequency
 */
void synthLPRM_enter( sleep_synth_lprm  *synthLPRM, 
                      uint64            wakeupTick, 
                      sleep_fLUT_node   *fLUT);

/**
 * synthLPRM_exit
 *
 * @brief Calls the exit function for a mode or in case of presynthesized
 *        mode, calls exit functions of component modes for given durations.
 *  
 * @param synthLPRM: Selected mode pointer from solver function 
 * @param fLUT: Frequency table pointer for the current core frequency
 *  
 * It should be noted that the order of calling the exit functions in case of
 * presynthesized mode should be reverse of that of enter functions.
 */
void synthLPRM_exit( sleep_synth_lprm *synthLPRM,
                     sleep_fLUT_node       *fLUT);

/**
 * synthLPRM_update
 *
 * @brief Checks the new status of this mode (ENABLED, DISABLED or IMPOSSIBLE)
 *        and if it differs from old status, it updates the corresponding LPR
 */
void synthLPRM_update( sleep_synth_lprm *synthLPRM, 
                       sleep_lprm       *changedSleepLPRM);

/**
 * synthLPRM_initSynth
 *
 * @brief Constructor to create sleep_synth_lprm from input presynthesized mode.
 *
 * @param lprm: Pointer to presynthesized mode.
 */
void synthLPRM_initSynth(sleep_synth_lprm *synthLPRM);

/**
 * synthLPRM_validate
 *
 * @brief Makes sure that only valid modes are defined in sleep system
 *
 * There are many criteria for a mode to be valid a mode like it must 
 * have enter/exit latency functions, enter or exit (init or
 * tear down) function alongwith other. If any of them is not satisfied, this
 * function will not allow it in sleep system and cause Error Fatal.
 *
 * @param lprm: Pointer to the LPRM we want to validate.
 */
void synthLPRM_validate( sleep_lprm *sleepLPRM );

/**
 * synthLPRM_setComponentModeAttributes
 *
 * @brief Sets the attribute field of each component mode based on 
 *        environment (like number of cores and cores sharing this mode).
 *
 * This function is mainly useful for already existing modes. It sets the
 * attribute in a way so that minimum overhead is incurred.
 */
void synthLPRM_setComponentModeAttributes( sleep_lprm *sleepLPRM );

#endif // SYNTHLPRM_H
