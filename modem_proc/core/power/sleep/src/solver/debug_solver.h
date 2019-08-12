#ifndef DEBUG_SOLVER_H
#define DEBUG_SOLVER_H

/*=============================================================================
  FILE:         debug_solver.h
  
  OVERVIEW:     This file declares the availabilty of a "debug" solver that
                can be used in conjunction with sleep_solver_set to control
                the decision making flow used by the sleep task.
 
                Copyright (c) 2010 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/solver/debug_solver.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $
=============================================================================*/

#include "sleep_solver.h"

/*
===============================================================================

                              EXTERN DECLARATIONS

===============================================================================
*/

/**
 * @brief sleep_debug_solver
 *
 * The sleep debug solver is an NPA Sleep solver conforming to the interface
 * specified for sleep solvers in sleep_solver.h and a solver function in
 * sleep_lpr.h.
 *
 * In particular, the debug solver has an extra configuration function that can
 * be used to pass an array of LPRM names.  Each time the configuration
 * function is called it searches through the list of available LPR's to find
 * the matching LPRM and uses those LPRM's as the "decision" at sleep time
 * every time the solver function is called.
 *
 * This isn't particularly useful in an production environment but is
 * extraordinarily useful for testing individual LPRM's.  It allows runtime
 * configuration of a specific LPRM to be executed over and over again for
 * validation that the mode works or for profiling of that modes
 * characteristics.  Since the configuration function takes a list of LPRM's,
 * interactions between LPRM's can also be tested.
 *
 * If what you're looking for is the ability of a specific LPR (and potentially
 * a subset of that LPR's modes) to be the only thing considered by on of the
 * other solvers, you can see the "filter" solver which takes an LPR[M] list
 * and runs an actual solver (selectable) over the filtered list of LPR[M]'s.
 *
 */
extern sleep_solver_type sleep_debug_solver;

/*
===============================================================================

                           GLOBAL FUNCTION DECLARATIONS

===============================================================================
*/

/**
 * @brief Choose the modes returned by the debug solver.
 *
 * As described in the general debug solver documentation, this function
 * selects the modes that the debug solver returns at idle time.  It does so by
 * querying the available LPR list and then matching as many of the LPRM's that
 * are listed as possible.  Henceforth the debug solver's solver function will
 * statically return that list of modes.  This function can of course be called
 * subsequently to change the list of modes yet again.
 *
 * Before this function is called, no modes are returned from the debug solver.
 *
 * @param mode_list The array of LPRM names to select for use at sleep time.
 *                  Note that this array should be terminated will a NULL entry
 *                  as in the example.
 *
 * @example
 * @code
 *   const char *modes_under_test[] =
 *   {
 *     "cxo.shutdown",
 *     "pxo.shutdown",
 *     NULL
 *   }
 *   sleep_debug_select_modes(modes_under_test);
 * @endcode
 *
 */
void sleep_debug_select_modes(const char * const * const mode_list);

#endif /* DEBUG_SOLVER_H */
