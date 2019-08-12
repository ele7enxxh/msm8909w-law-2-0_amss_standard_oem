#ifndef SLEEP_LPRI_H
#define SLEEP_LPRI_H
/*==============================================================================
  FILE:         sleep_lpri.h
  
  OVERVIEW:     This file provides the internal functions and data used for
                sleep_lpr functionality.

  DEPENDENCIES: None

                Copyright (c) 2010-2013 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/sleep_lpri.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "npa_resource.h"
#include "CoreCpu.h"

/*==============================================================================
                             MACRO DEFINITIONS
 =============================================================================*/
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
#define SLEEP_LPR_REGISTER        (NPA_QUERY_RESERVED_END+2)

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

/*==============================================================================
                             DATA DECLARATIONS
 =============================================================================*/
/**
 * gSleepNumOfCores
 *
 * @brief Number of available CPU cores
 */
extern volatile uint32 g_SleepNumOfCores;

/**
 * gSleepNumOfFreqs
 *
 * @brief Number of static frequencies associated with the number of mLUTs
 */
extern volatile uint32 g_SleepNumOfFreqs;


#ifdef __cplusplus
}
#endif

#endif /* SLEEP_LPRI_H */

