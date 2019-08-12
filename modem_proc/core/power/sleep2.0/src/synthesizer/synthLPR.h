#ifndef SYNTHLPR_H
#define SYNTHLPR_H
/*==============================================================================
  FILE:            synthLPR.h
  
  OVERVIEW:        Provides the synth mode functions.
  
  DEPENDENCIES: None
 
                Copyright (c) 2013 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/synthesizer/synthLPR.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "sleepi.h"
#include "synthTypes.h"

/*==============================================================================
                             FUNCTION DEFINITION
 =============================================================================*/
/**
 * synthLPR__getStatus
 *
 * @brief Function to see if this LPR enabled.
 *
 * The LPR is enabled if it has any mode which is selected by sleep solver 
 * as an enabled (or entered) mode during idle time.
 *        
 * @return Returns SLEEP_ENABLED if the LPR is enabled else SLEEP_DISABLED.
 */
sleep_status synthLPR_getStatus(sleep_synth_lpr *synthLPR);

/**
 * synthLPR_modeUpdated
 *
 * @brief This function updates the status of this sleep_synth_lpr with the registry
 *        according to input mode.
 *
 * The status of the sleep_synth_lpr is said to be changed when number of enabled modes
 * is reduced to zero from non-zero or vice versa.
 *
 * @param mode: Mode when made enabled/disabled may cause change in the
 *              status of sleep_synth_lpr.
 */
void synthLPR_modeUpdated(sleep_synth_lprm* synthLPRM);

/**
 * synthLPR_init
 *
 * @brief Sets up initial values for the synth mode
 *
 * @param synthLPR: Pointer to the presynthesized LPR
 */
void synthLPR_init(sleep_synth_lpr *synthLPR);

/**
 * synthLPR_define
 *
 * @brief This function will be called once per synthesized LPR at the start up 
 *        time to register a it with the sleep subsystem.
 *
 * Registering presynthesized LPRs requires apriori knowledge of all LPRs and 
 * their dependencies.
 *
 * @param synthLPR: Low power resource we want to register with sleep 
 *                  system.
 */
void synthLPR_define(sleep_synth_lpr *synthLPR);

#endif // SYNTHLPR_H
