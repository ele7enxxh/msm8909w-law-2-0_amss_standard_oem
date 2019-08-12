#ifndef SLEEPACTIVE_H
#define SLEEPACTIVE_H
/*==============================================================================
  FILE:         sleepActive.h

  OVERVIEW:     This file contains public APIs exposed by the Sleep Active-time 
                Solver Thread.

  DEPENDENCIES: None

  Copyright (c) 2014 Qualcomm Technologies, Inc. (QTI).
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/inc/sleepActive.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/

/*==============================================================================
                                GLOBAL TYPE DECLARATIONS
  =============================================================================*/ 
/* List of Signals that cause the Active Time solver 
 * to re-evaluate its selection of low-power modes */
typedef enum
{
  SLEEP_SIGNAL_CONTROL       = ( 1 << 0 ),     /* Enable/Disable Active Time Solver  */
  SLEEP_SIGNAL_LATENCY       = ( 1 << 1 ),     /* Update in Latency Constraint       */
  SLEEP_SIGNAL_SOFT_DURATION = ( 1 << 2 ),     /* Update Idle Soft Duration          */
  SLEEP_SIGNAL_REGISTRY      = ( 1 << 3 ),     /* Update List of Enabled Synth Modes */
  SLEEP_SIGNAL_CPU_FREQUENCY = ( 1 << 4 ),     /* Cpu Frequency Change               */
  SLEEP_SIGNAL_MASK_ALL      = ( 1 << 5 ) - 1  /* Mask for all signals               */
} sleepActive_SignalType;

/*==============================================================================
                              GLOBAL FUNCTION DECLARATIONS
  =============================================================================*/
/**
 * @brief Trigger a Signal of type sig on Active Time solver thread
 *
 *        Signals set on the Active Time solver are used to determine 
 *        which execution conditions that have changed. 
 *        The Active Time solver thread executes the sleep solver to determine
 *        whether these modified inputs cause a change in the selection of 
 *        low-power mode.
 * 
 * @param sig: Signal to be set on Active Time solver
 */
void sleepActive_SetSignal( uint32 sig );

#endif /* SLEEPACTIVE_H */
