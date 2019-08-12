#ifndef SLEEP_H
#define SLEEP_H

/*=======================================================================
  FILE:          sleep.h

  DESCRIPTION:   This file contains public APIs related to Sleep task.

                 Copyright (c) 2013 QUALCOMM Technologies Incorporated.
                 All Rights Reserved.
                 Qualcomm Confidential and Proprietary

  $Header: //source/qcom/qct/core/api/power/sleep/main/latest/sleep.h
==========================================================================*/

/*==========================================================================
 *                          GLOBAL FUNCTION DECLARATIONS
 *==========================================================================*/

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

/**
 * sleep_is_processor_in_stm
 *
 * @brief Checks if the processor is in Single threaded mode (also known
 *        as INTLOCK'd or Sleep context) when this function is called.
 *
 * For multicore (HLOS) targets, it should gives the status of core 
 * on which this function was called.
 *
 * @NOTE 
 * This function is more like a query function which returns the value 
 * at any given point. It may have changed immediately after calling it.
 *
 * @return Non-zero if processor in sleep context otherwise zero.
 */
int sleep_is_processor_in_stm(void);

#endif /* SLEEP_H */

