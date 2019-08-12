/*==============================================================================
  FILE:         sleep_solver.c
  
  OVERVIEW:     This file implements the sleep module interface for configuring
                the "solver" that is invoked at sleep time to decide which low
                power resources should be put into what modes.

  DEPENDENCIES: None

                Copyright (c) 2010-2013 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/solver/sleep_solver.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "sleep_log.h"
#include "sleep_solver.h"

/*==============================================================================
                               EXTERNAL FUNCTIONS
 =============================================================================*/
/**
 * sleepSolver_defaultFunction
 *
 * @brief Default sleep solver if not set (Right now this solver just clears
 *        the input solution list).
 *
 * @param input:  List of constraints that are used by the solver 
 *                to select modes.
 * @param output: List of output data from the solver -- 
 *                contains the list of modes to enter, as well as
 *                the backoff time that should be used.
 */
void sleepSolver_defaultFunction
(
  sleep_solver_input  *input,
  sleep_solver_output *output
);

/**
 * @brief current_solver
 *
 * Pointer to the existing sleep solver.
 */
const static sleep_solver_type *current_solver = NULL;

/**
 * @brief sleepSolver_solveMode
 *
 * Pointer to existing sleep solver function.
 */
sleepSolver_solver_fcn sleepSolver_solveMode = sleepSolver_defaultFunction;

/*
 * sleepSolver_setSolverFunction
 */
void sleepSolver_setSolverFunction(const sleep_solver_type *new_solver)
{
  const char *name;

  /* Clean up the old solver */
  if(NULL != current_solver && current_solver->deinit)
    current_solver->deinit();

  current_solver = new_solver;

  /* Set up the new solver (if any) */
  if(NULL != new_solver && new_solver->init)
    new_solver->init();

  if(NULL != new_solver && new_solver->solver)
  {
    sleepSolver_solveMode = new_solver->solver;

    /* Require a solver name on the new solver */
    CORE_VERIFY(new_solver->solver_name);
    name = new_solver->solver_name;
  }
  else
  {
    sleepSolver_solveMode = sleepSolver_defaultFunction;
    name = "Default";
  }

  sleepLog_printf(SLEEP_LOG_LEVEL_DEBUG, 1, 
                  "Sleep solver set (name: %s)",
                  name);
}

/*
 * sleepSolver_defaultFunction
 */
void sleepSolver_defaultFunction
(
  sleep_solver_input  *input,
  sleep_solver_output *output
)
{
  return;
}
