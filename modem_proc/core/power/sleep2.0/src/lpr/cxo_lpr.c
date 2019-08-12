/*============================================================================
  FILE:         cxo_lpr.c

  OVERVIEW:     This file provides the LPR definition for the CXO clock 
                low-power modes.

  DEPENDENCIES: None

                Copyright (c) 2012-2014 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/lpr/cxo_lpr.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "vmpm.h"

/*==============================================================================
                               GLOBAL FUNCTIONS
 =============================================================================*/
/**
 * @brief CXOShutdownLPR_initialize
 *
 * Performs required initialization for cxo lpr.
 */
void CXOShutdownLPR_initialize(void)
{
  return;
}

/**
 * @brief CXOShutdownLPR_enter
 *
 * Function that gets called when Master is voting for cxo shutdown during
 * a Sleep cycle.
 *
 * @param wakeup_tick: Absolute time tick (in 19.2 Mhz) when Master is 
 *                     expected to wake up.
 */
void CXOShutdownLPR_enter(uint64 wakeup_tick)
{
  return;
}

/**
 * @brief CXOShutdownLPR_exit
 *
 * Exit function which gets called while exiting from Sleep on master side for
 * a Sleep cycle when master voted for cxo shutdown.
 */
void CXOShutdownLPR_exit( void )
{
  /* Triggering any potential mpm interrupts that may have occured during xo 
   * shutdown. */
  vmpm_TriggerInterrupts();
  return;
}

