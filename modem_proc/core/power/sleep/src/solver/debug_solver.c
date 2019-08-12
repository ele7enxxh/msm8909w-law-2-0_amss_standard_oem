/*============================================================================
  FILE:         debug_solver.c

  OVERVIEW:     This file implements the "debug" solver for debugging purposes.
                However, right now it is just an empty implementation. We can
                have solver as we like here.
 
                Copyright (c) 2010 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/*=======================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep/src/solver/debug_solver.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $
========================================================================*/

#include "CoreVerify.h"
#include "npa.h"
#include "sleep_lpri.h"
#include "debug_solver.h"

/*
===============================================================================

                             INTERNAL DECLARATIONS

===============================================================================
*/

/*
 * sleep_debug_select_modes
 */
void sleep_debug_select_modes(const char * const * const mode_list)
{
}

void sleep_debug_solver_fcn
(
  sleep_solver_input  *input,
  sleep_solver_output *output
)
{
}

uint32 sleep_debug_solver_reset(void)
{
  return 0;
}

sleep_solver_type sleep_debug_solver =
{
  sleep_debug_solver_reset, /// on init or deinit we reset the list selected
  sleep_debug_solver_reset, /// mode list in order to reduce confusion
  sleep_debug_solver_fcn
};
