/*============================================================================
  FILE:         AdcBsp.c

  OVERVIEW:     Board support package for the ADC DAL on 8952.

  DEPENDENCIES: None
 
                Copyright (c) 2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.


  when        who    what, where, why
  ----------  ---    -----------------------------------------------------------
  2014-11-18  SA     Created.

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
/*
 * 8952_PM8950
 */
static const AdcPhysicalDeviceType adcPhysicalDevices_8952_PM8950[] = {
   /* VADC */
   {
      /* .pszDevName */ "DALDEVICEID_VADC_8952_PM8950",
      /* .uQueueSize */ 10
   },
};

const AdcBspType AdcBsp_8952_PM8950[] = {
   {
      /* .paAdcPhysicalDevices */ adcPhysicalDevices_8952_PM8950,
      /* .uNumPhysicalDevices  */ ARRAY_LENGTH(adcPhysicalDevices_8952_PM8950)
   }
};

