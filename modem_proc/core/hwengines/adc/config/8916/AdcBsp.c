/*============================================================================
  FILE:         AdcBsp.c

  OVERVIEW:     Board support package for the ADC DAL on 8916.

  DEPENDENCIES: None
 
                Copyright (c) 2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/adc/config/8916/AdcBsp.c#1 $

  when        who    what, where, why
  ----------  ---    -----------------------------------------------------------
  2013-10-01  sivaa  Initial version (based on 8926).

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DalAdc.h"

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

const AdcBspType AdcBsp[] = {
   {
      /* .paAdcPhysicalDevices */ adcPhysicalDevices,
      /* .uNumPhysicalDevices  */ ARRAY_LENGTH(adcPhysicalDevices)
   }
};

