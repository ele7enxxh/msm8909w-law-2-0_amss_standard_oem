/*============================================================================
@file HAL_cpr_config.c

CPR configuration info that is dependent on the version of the CPR HW, but
independent of the target on which it is implemented.

Copyright © 2013 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
QUALCOMM Confidential and Proprietary.

$Header: //components/rel/core.mpss/3.5.c12.3/power/rbcpr/src/hal/v1/HAL_cpr_config.c#1 $
=============================================================================*/
#include "HAL_cpr.h"

/**
 * CPR version specific HW config that is independent of which target we're on.
 */
HalCprConfigType halCprConfiguration =
{
  .numSensors = 64,
  .interrupts =
  {
    // Comment out the HAL_CPR_INTERRUPT_INVALID mask to enable the
    // desired interrupt.
    HAL_CPR_INTERRUPT_DONE     | HAL_CPR_INTERRUPT_INVALID,
    HAL_CPR_INTERRUPT_MIN      | HAL_CPR_INTERRUPT_INVALID,
    HAL_CPR_INTERRUPT_DOWN, // | HAL_CPR_INTERRUPT_INVALID,
    HAL_CPR_INTERRUPT_MID      | HAL_CPR_INTERRUPT_INVALID,
    HAL_CPR_INTERRUPT_UP,   // | HAL_CPR_INTERRUPT_INVALID,
    HAL_CPR_INTERRUPT_MAX      | HAL_CPR_INTERRUPT_INVALID,
  }
};

