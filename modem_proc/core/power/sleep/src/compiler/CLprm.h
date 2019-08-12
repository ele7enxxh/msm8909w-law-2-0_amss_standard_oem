#ifndef CLPRM_H
#define CLPRM_H
/*==============================================================================
  FILE:         CLprm.h
  
  OVERVIEW:     This file defines the class that is a representation of the
                sleep_lprm[_synth] struct for solver. This class is used for
                purposes like validating sleep_lprm, calling its enter/exit  
                functions.

  DEPENDENCIES: None

                Copyright (c) 2010-2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/compiler/CLprm.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "sleepi.h"
#include "CoreTime.h"
#include "LprTypes.h"
#include "CLpr.h"
#include "lookup_table_types.h"

/**
 * createSynth
 *
 * @brief Creates CLprm from presythesized mode.
 *
 * @param lprm: Pointer to presynthesized mode for which we want to create 
 *              CLprm object.
 *
 * @return Pointer to CLpr object representing input presynthesized node.
 */
CLprm* CLprm_createSynth(sleep_lprm_synth *lprm);

/**
 * setParent
 *
 * @brief Set the parent field of the object to input CLpr object.
 *
 * @param parent: The pointer to an LPR we want as a parent of this mode.
 */
void CLprm_setParent(CLprm *hCLprm, CLpr* parent);

/**
 * getParent
 *
 * @brief Returns the parent (or the owner) of the mode.
 *
 * The parent may be synthesized LPR.
 *
 * @return Pointer to parent node.
 */ 
static SLEEP_INLINE CLpr* CLprm_getParent(CLprm *hCLprm)
{ 
  return hCLprm->m_parent; 
}

/**
 * ~CLprm
 *
 * @brief Destructor
 */
void CLprm_deinit(CLprm *hCLprm);

/**
 * getStatus
 * 
 * @brief Function to inquire the status of mode (enabled or disabled)
 *
 * @return Returns SLEEP_ENABLED if the mode is enabled, SLEEP_DISABLED
 *         if it is disabled or SLEEP_IMPOSSIBLE if it can never be enabled.
 */
sleep_status CLprm_getStatus(CLprm *hCLprm);

/**
 * getEnterLatency
 *
 * @brief Will be used to find out time required to enter into this mode.
 *
 * If the mode is the presynthesized one, its enter latency will be sum of all
 * its component modes' enter latencies.
 *
 * @param duration: Time duration in tick for which the mode might enter. 
 * @param pfreq_LUT: Frequency LUT pointer for current core frequency 
 * @param sleep_cycle_id: Indicates a particular sleep cycle for which we 
 *                        want enter latency of this CLprm.
 *
 * @see
 * sleep_lprm_cached_fcn_data explains sleep cycle id.
 *
 * @return Returns total time required by this mode to enter.
 */
uint32 CLprm_getEnterLatency( CLprm                   *hCLprm, 
                              uint64                  duration,
                              sleep_freq_lookup_table *pfreq_LUT,
                              uint32                  sleep_cycle_id );

/**
 * getExitLatency
 *
 * @brief Will be used to find out time required to exit from this mode.
 *
 * If the mode is the presynthesized one, its exit latency will be sum of all
 * its component modes' exit latencies.
 *
 * @param duration: Time duration in tick for which the mode might enter. 
 * @param pfreq_LUT: Frequency LUT pointer for current core frequency 
 * @param sleep_cycle_id: Indicates a particular sleep cycle for which we
 *                        want exit latency of this CLprm.
 *
 * @see
 * sleep_lprm_cached_fcn_data explains sleep cycle id.
 *
 * @return Returns total time required by this mode to exit.
 */
uint32 CLprm_getExitLatency( CLprm                    *hCLprm, 
                             uint64                   duration,
                             sleep_freq_lookup_table  *pfreq_LUT,
                             uint32                   sleep_cycle_id );

/**
 * getBackOffTime
 *
 * @brief Calculates the back off time for the given CLprm. If any of the 
 *        component modes has not specified back off time, its exit
 *        latency will be used.
 *
 * @param duration: Time duration in tick for which the mode might enter. 
 * @param pfreq_LUT: Frequency LUT pointer for current core frequency 
 * @param sleep_cycle_id: Indicates a particular sleep cycle for which we
 *                        want backoff latency of this CLprm.
 *
 * @see
 * sleep_lprm_cached_fcn_data explains sleep cycle id.
 *
 * @return Backoff time for the calling CLprm.
 */
uint32 CLprm_getBackOffTime( CLprm                    *hCLprm,
                             uint64                   duration,
                             sleep_freq_lookup_table  *pfreq_LUT,
                             uint32                   sleep_cycle_id );

/**
 * enter
 *
 * @brief Calls the enter function for a mode or in case of presynthesized
 *        mode calls enter functions of component modes for given durations.
 *
 * @param hCLprm: Selected mode pointer from solver function
 * @param wakeup_tick: Absolute wakeup time in ticks 
 * @param data_idx: Returned value from sleep_lookup_table_get_entry that 
 *                  is the data index from the FLUT
 */
void CLprm_enter( CLprm *hCLprm, uint64 wakeup_tick, uint32 data_idx);

/**
 * exit
 *
 * @brief Calls the exit function for a mode or in case of presynthesized
 *        mode, calls exit functions of component modes for given durations.
 *  
 * @param hCLprm: Selected mode pointer from solver function 
 * @param data_idx: Returned value from sleep_lookup_table_get_entry that 
 *                  is the data index from the FLUT 
 *  
 * It should be noted that the order of calling the exit functions in case of
 * presynthesized mode should be reverse of that of enter functions.
 */
void CLprm_exit( CLprm *hCLprm, uint32 data_idx);

/**
 * update
 *
 * @brief Checks the new status of this mode (ENABLED, DISABLED or IMPOSSIBLE)
 *        and if it differs from old status, it updates the corresponding LPR
 */
void CLprm_update( CLprm *hCLprm, sleep_lprm *changed_mode );

/**
 * hasConstantLatencies
 *
 * @brief Checks if all the component modes (and hence this synthesized mode)
 *        has constant enter and exit latencies
 *
 * @return True if it has constant latencies else false.
 */
boolean CLprm_hasConstantLatencies( CLprm *hCLprm );

/** 
 * offsetSleepDuration 
 *  
 * @brief Updates the sleep duration to be passed to the enter 
 *        functions.
 * 
 * @param offset - Number of ticks to offset the sleep duration 
 *                 by.
 */
inline static void offsetSleepDuration( CLprm *hCLprm, uint64 offset )
{
  hCLprm->wakeup_tick += offset;
}

/**
 * CLprm
 *
 * @brief Constructor to create CLprm from input presynthesized mode.
 *
 * @param lprm: Pointer to presynthesized mode.
 */
void CLprm_initSynth( CLprm *hCLprm, sleep_lprm_synth *lprm );

/**
 * getLPRMLatency
 *
 * @brief This function is used to decide how the latency (enter/exit) 
 *        function will be cast (and then called) based on the latency tags.
 *  
 * @param hCLprm:   Selected mode pointer from solver function 
 * @param latData:  Pointer to the (Enter/Exit) latency data. 
 * @param lutIdx:   Lookup table index that corresponds to the static data array 
 *                  indexes
 * @param duration: Sleep duration
 *
 * @return Value returned by enter/exit latency function.
 */
uint32 CLprm_getLPRMLatency( CLprm          *hCLprm,
                             sleep_lat_data *latData,
                             uint32         mLUTIdx,
                             uint64         duration );
   
/**
 * validate
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
void CLprm_validate( sleep_lprm *lprm );


/**
 * setupLatencyCache
 * 
 * @brief Initializes the fields related to latency caching to default values.
 *
 * This function is called when we create CLprm either from lprm or 
 * presynthesized lprm.
 */
void CLprm_setupLatencyCache( CLprm *hCLprm );
  
/**
 * setComponentModeAttributes
 *
 * @brief Sets the attribute field of each component mode based on 
 *        environment (like number of cores and cores sharing this mode).
 *
 * This function is mainly useful for already existing modes. It sets the
 * attribute in a way so that minimum overhead is incurred.
 */
void CLprm_setComponentModeAttributes( sleep_lprm *mode );

/**
 * CLprm_add_data_elements
 *
 * @brief Adds cache & data array elements corresponding to a newly added fLUT
 * 
 * @param registry: Current LPR registry
 * @param data_idx: Data index for newly added fLUT
 */
void CLprm_add_data_elements(CLprRegistry  *registry,
                             uint32        data_idx);

#endif // CLRPM_H
