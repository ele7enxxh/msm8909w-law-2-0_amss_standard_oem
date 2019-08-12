#ifndef SLEEP_H
#define SLEEP_H

/*============================================================================
  @file sleep.h

  Sleep task header file

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  $Header: //source/qcom/qct/core/api/power/sleep/main/latest/sleep.h
============================================================================*/

/*
===============================================================================

                           GLOBAL FUNCTION DECLARATIONS

===============================================================================
*/

/**
 * sleep_task
 *
 * @brief Main processing loop for the sleep task.
 *
 * It continually loops, looking for a consensus that the CPU may be
 * put to sleep.  Lacking a consensus, it still halts the processor, but
 * in "idle" mode -- in which any normal interrupt will restart processing
 * with minimal delay. This procedure should be called by rex_def_task. A side-
 * effect of this is that the CPU may be put to sleep by this task.
 *
 * @return NONE.
 */
extern void sleep_task
(
  /* Parameter received from Main Control task - ignored */
  /*lint -esym(715,ignored)
     Have lint not complain about the ignored parameter 'ignored' which is
     specified to make this routine match the template for rex_def_task(). */
  dword ignored
);

/**
 * sleep_init
 *
 * @brief Initialize the sleep subsystem.
 *
 * Initializing sleep subsystem involves initializing its various components 
 * like logging system, NPA nodes, LPR tracking system and target dependent
 * resources.
 */
void sleep_init(void);

#endif /* SLEEP_H */

