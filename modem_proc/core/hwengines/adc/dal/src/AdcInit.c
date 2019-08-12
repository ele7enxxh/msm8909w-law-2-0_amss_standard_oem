/*============================================================================
  FILE:         AdcInit.c

  OVERVIEW:     Used to initialize ADC code during boot-up.

  DEPENDENCIES: Requires ADC DAL
 
                Copyright (c) 2009-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/adc/dal/src/AdcInit.c#1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-06-13  PR   stubbed out adc_init for virtio platform for intial 8909 MPSS build
  2013-11-25  PR   ported RUMI platform check
  2012-04-20  jdt  Update for 8974.
  2009-03-16  jdt  Initial revision.

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DDIAdc.h"
#include "AdcDiagDal.h"
#include "DDIPlatformInfo.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/*======================================================================

  FUNCTION        adc_init

  DESCRIPTION     This is used to initialize the ADC DAL diagnostics

  DEPENDENCIES    Requires the ADC DAL

  PARAMETERS      None

  RETURN VALUE    None

  SIDE EFFECTS    None

======================================================================*/
void adc_init(void)
{
  if ((DalPlatformInfo_Platform() == DALPLATFORMINFO_TYPE_RUMI)||(DalPlatformInfo_Platform() == DALPLATFORMINFO_TYPE_VIRTIO))
  {
    return;
  }
  DALSYS_InitMod(NULL);
  (void)AdcDiagDAL_Init();
}

