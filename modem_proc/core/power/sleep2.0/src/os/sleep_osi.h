#ifndef SLEEP_OSI_H
#define SLEEP_OSI_H
/*==============================================================================
  FILE:           sleep_osi.h

  OVERVIEW:       This file contains sleep internal declarations of functions 
                  that are used in the common main sleep subsystem

  DEPENDENCIES: None

                Copyright (c) 2013-2014 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/os/sleep_osi.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "DALStdDef.h"

/*==============================================================================
                            GLOBAL DEFINITIONS
 =============================================================================*/
/**
 * sleepOS_IdleModeType
 *
 * @brief Enumeration to indicate the Idle Mode Type
 */
typedef enum
{
  SLEEP_OS_IDLE_MODE_DEFAULT   = 0,   /* Configure for single threaded sleep */
  SLEEP_OS_IDLE_MODE_OVERRIDE  = 1,   /* Configure for low overhead sleep    */
  SLEEP_OS_IDLE_MODE_SWFI_ONLY = 
    SLEEP_OS_IDLE_MODE_OVERRIDE   ,   /* Where override is not available     */ 
  SLEEP_OS_IDLE_MODE_USLEEP    = 2,   /* Configure for uImage based sleep    */
  SLEEP_OS_IDLE_MODE_HOLD_OFF  = 3,   /* Configure for busy wait during idle */
  SLEEP_OS_IDLE_MODE_NUM_STATES
} sleepOS_IdleModeType;

/*==============================================================================
                           GLOBAL FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * sleepOS_configIdleMode
 * 
 * @brief This function configures how processor idle is handled within sleep.
 * 
 * @param coreNum: Core on which we want to modify idle time 
 *                 behavior/configuration.
 * @param idleMode: Idle time behavior/configuration for next cycle.
 */
void sleepOS_configIdleMode(uint32 coreNum, sleepOS_IdleModeType idleMode);

/**
 * sleepOS_getFrequency
 *
 * @brief A query function to return cpu clock frequency in KHz.
 *
 * @return CPU frequency in KHz.
 */
uint32 sleepOS_getFrequency(void);

#endif /* SLEEP_OSI_H */
