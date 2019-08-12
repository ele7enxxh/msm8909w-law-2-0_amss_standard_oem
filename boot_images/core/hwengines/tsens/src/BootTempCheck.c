/*============================================================================
  FILE:         BootTempCheck.c

  OVERVIEW:     Implementation of temperature check in the boot

  DEPENDENCIES: None

                Copyright (c) 2012, 2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.


  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/tsens/src/BootTempCheck.c#1 $$DateTime: 2015/09/01 00:30:35 $$Author: pwbldsvc $


  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-04-02  jjo  Move boot sensor index here.
  2012-07-30  jjo  Added lower threshold.
  2012-05-23  jjo  Added DAL properties.
  2012-05-20  jjo  Ported to 8974.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DALSys.h"
#include "BootTempCheck.h"
#include "BootTempCheckBsp.h"
#include "TsensBoot.h"
#include "DALStdDef.h"

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

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  BootTempCheck
**
**  Description:
**    Checks the TSENS boot temperature.
**
**  Parameters:
**    None
**
**  Return:
**    BootTempCheckStatusType
**
**  Dependencies:
**    None
**
** ========================================================================= */
BootTempCheckStatusType BootTempCheck(void)
{
   static BootTempCheckBspType *pBootTempCheckBsp;
   static DALBOOL bInitialized = FALSE;
   DALSYS_PROPERTY_HANDLE_DECLARE(hBootTempCheckProperties);
   DALSYSPropertyVar propertyVar;
   DALResult status;
   TsensResultType tsensResult;
   int32 nDegC;

   /* Get the boot thresholds */
   if (bInitialized == FALSE)
   {
      status = DALSYS_GetDALPropertyHandleStr("QBootTempCheck", hBootTempCheckProperties);
      if(status != DAL_SUCCESS)
      {
         return BOOT_TEMP_CHECK_TSENS_ERROR;
      }

      status = DALSYS_GetPropertyValue(hBootTempCheckProperties,
                                       "BOOT_TEMP_CHECK_BSP",
                                       0,
                                       &propertyVar);
      if (status != DAL_SUCCESS)
      {
         return BOOT_TEMP_CHECK_TSENS_ERROR;
      }

      pBootTempCheckBsp = (BootTempCheckBspType *)propertyVar.Val.pStruct;

      bInitialized = TRUE;
   }

   /* Get the temperature */
   tsensResult = Tsens_GetTemp(pBootTempCheckBsp->uBootSensorIdx, &nDegC);

   if (tsensResult == TSENS_SUCCESS)
   {
      if (nDegC > pBootTempCheckBsp->nUpperThresholdDegC)
      {
         return BOOT_TEMP_CHECK_TEMP_ABOVE_THRESHOLD;
      }
      else if (nDegC < pBootTempCheckBsp->nLowerThresholdDegC)
      {
         return BOOT_TEMP_CHECK_TEMP_BELOW_THRESHOLD;
      }
      else
      {
         return BOOT_TEMP_CHECK_TEMP_OK;
      }
   }
   else if (tsensResult == TSENS_ERROR_INVALID_CALIBRATION)
   {
      return BOOT_TEMP_CHECK_INVALID_TSENS_CAL;
   }
   else if (tsensResult == TSENS_ERROR_TIMEOUT)
   {
      return BOOT_TEMP_CHECK_TSENS_TIMEOUT;
   }
   else
   {
      return BOOT_TEMP_CHECK_TSENS_ERROR;
   }
}

