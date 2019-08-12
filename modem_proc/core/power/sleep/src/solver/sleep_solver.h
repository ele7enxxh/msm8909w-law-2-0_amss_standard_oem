#ifndef SLEEP_SOLVER_H
#define SLEEP_SOLVER_H
/*==============================================================================
  FILE:         sleep_solver.h
  
  OVERVIEW:     This file declares the interface used to declare and configure
                "solvers" that implement the decision logic for the sleep task.

  DEPENDENCIES: None

                Copyright (c) 2010-2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/solver/sleep_solver.h#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "CLprm.h"
#include "DALStdDef.h"
#include "sleep_lpr.h"
#include "lookup_table_types.h"

/* ==================================================================
                        GLOBAL TYPE DEFINITIONS
   ================================================================== */

/**
 * @brief sleep_solver_input
 * 
 * This structure contains data that is needed by the sleep solver,
 * in order to make a decision about which low power modes should be
 * entered.
 */
typedef struct solver_input
{
  uint32        latency_budget;       /* Latency restriction in ticks */
  uint64        hard_duration;        /* Hard wake-up duration in ticks */
  uint64        soft_duration;        /* Soft wake-up duration in ticks */
  CLprRegistry  *lpr_registry;
  uint32        num_of_allowed_modes; /* Max num of modes solver can select */
  uint32        cycle_id;             /* Sleep cycle id for caching fcn data */
  sleep_freq_lookup_table *pfreq_LUT; /* Pointer to the freq LUT */
} sleep_solver_input;

/**
 * @brief sleep_solver_output
 *  
 * This structure contains information that is needed by the 
 * caller of the solver, in order to enter the selected modes, 
 * and wake up on time. 
 */
typedef struct solver_output
{
  CLprm  **selected_modes;        /* Solution array as output */
  uint32 num_of_selected_modes;   /* No of selected modes in sleep cycle */
  uint32 backoff_time;            /* Total backoff time of selected modes */
  uint32 enter_time;              /* Total enter time of selected modes */
  uint32 exit_time;               /* Total exit time of selected modes */
} sleep_solver_output;

/**
 * @brief Type definition for the init function of a solver
 *
 * It possible to write multiple solvers as per requirements. Each solver can
 * have different initialization routine. This is a pointer to that routine.
 * Whenver solver is set to be sleep solver, this function wil be called to
 * perform initialization, if provided.
 *
 * @return Returns a nonzero value on error.
 */
typedef uint32 (*sleep_solver_init_fcn)(void);

/**
 * @brief Type definition for the de-init function of a solver
 *
 * As mentioned above, each sleep solver can have different de-init or tear down
 * routine. This is a pointer to that routine. Whenver we change sleep solver,
 * de-init function of the current sleep solver will be called, if provided.
 *
 * @return Returns a nonzero value on error.
 */
typedef uint32 (*sleep_solver_deinit_fcn)(void);

/**
 * @brief Type definition for a sleep solver function which on execution will
 *        select low power modes that should be entered during CPU idle, based
 *        on the current dynamic conditions.
 *
 * @param input:  List of constraints that are used by the solver 
 *                to select modes.
 * @param output: List of output data from the solver -- 
 *                contains the list of modes to enter, as well as
 *                the backoff time that should be used.
 */
typedef void (*sleep_solver_fcn)( sleep_solver_input  *input, 
                                  sleep_solver_output *output );

/**
 * @brief Type definition of a solver with init, de-init (both optional) and 
 *        solving (compulsory) functions.
 */
typedef struct sleep_solver_type
{
  /* Pointer to init function */
  sleep_solver_init_fcn init;

  /* Pointer to de-init function */
  sleep_solver_deinit_fcn deinit;

  /* Pointer to solving function */
  sleep_solver_fcn solver;

  /* String name of the solver */
  const char *solver_name;

} sleep_solver_type;

/* ==================================================================
                          EXTERN DECLARATIONS
   ================================================================== */

/**
 * sleep_solver
 *
 * @brief Selects low power modes which should be entered during CPU idle, based
 *        on the dynamic conditions that exist at this moment.
 *
 * This function is implemented by the appropriate sleep solver on a per-target
 * basis.  The ultimate goal of this function is to allocate the latency
 * budget, selecting the best set of low power modes that will fit into the
 * latency budget while simultaneously minimizing the power consumption of the
 * system. 
 *  
 * The caveat is this: the problem that this function attempts to solve in real
 * time is NP-complete; the problem is essentially a variant of the knapsack
 * problem.  That said, using this abstract "solver" interface we can come up
 * with multiple implementations of this function which trade off computational
 * complexity for exactness of solution as is appropriate for whatever target
 * the code happens to be running on at the time. 
 *  
 * Ultimately though, this function is really only of interest to the sleep
 * module, and anyone who thinks they need to call this external to the sleep
 * module should probably reconsider why they want to perform this (potentially
 * very expensive) calculation. 
 * 
 */
extern sleep_solver_fcn sleep_solver;


/* ==================================================================
                      GLOBAL FUNCTION DECLARATIONS
   ================================================================== */

/**
 * sleep_solver_set
 *
 * @brief Sets the solver passed as argument to be the sleep solver.
 *
 * Calling this function will result in subsequent calls to de-init function of
 * the current sleep solver (if any) and init-function of the new solver 
 * (if any).
 *
 * @param new_solver: Pointer to the solver we want as sleep solver.
 */
void sleep_solver_set(const sleep_solver_type *new_solver);


#endif /* SLEEP_SOLVER_H */
