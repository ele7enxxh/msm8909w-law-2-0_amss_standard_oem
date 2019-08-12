/*============================================================================
  FILE:         BootTempCheckBsp.c

  OVERVIEW:     Thresholds for the boot temp check.

  DEPENDENCIES: None

                Copyright (c) 2013-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/tsens/config/8909/BootTempCheckBsp.c#1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-07-09  jjo  Move boot sensor here.
  2013-12-05  PR   Initial version.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "BootTempCheckBsp.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
const BootTempCheckBspType BootTempCheckBsp[] = {
   {
      /* .nUpperThresholdDegC */ 150,
      /* .nLowerThresholdDegC */ -150,
      /* .uBootSensorIdx      */ 0
   }
};

