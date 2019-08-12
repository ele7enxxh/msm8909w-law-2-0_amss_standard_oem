/*============================================================================
  FILE:         AdcBsp.c

  OVERVIEW:     Board support package for the ADC boot on 8916.

  DEPENDENCIES: None

                Copyright (c) 2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/adc/config/8909/AdcBsp.c#1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2013-11-25  PR   Initial version.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "AdcBootInternal.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define ARRAY_LENGTH(a) (sizeof(a) / sizeof(a[0]))

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
static const AdcPhysicalDeviceType adcPhysicalDevices[] = {
   /* VADC */
   {
      /* .pszDevName */ "DALDEVICEID_VADC",
      /* .uQueueSize */ 10
   },
};

const AdcBspType AdcBootBsp[] = {
   {
      /* .paAdcPhysicalDevices */ adcPhysicalDevices,
      /* .uNumPhysicalDevices  */ ARRAY_LENGTH(adcPhysicalDevices)
   }
};

