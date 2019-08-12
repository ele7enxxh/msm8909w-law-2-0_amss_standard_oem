/*============================================================================
  FILE:         TsensBootBsp.c

  OVERVIEW:     8916 BSP for Tsens boot.

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

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/tsens/config/8909/TsensBootBsp.c#1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-17-10  SA   Updated default slope to 3.0
  2014-07-09  jjo  Remove turbo mode setting.
  2013-12-05  PR   Initial version.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "TsensBootBsp.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define ARRAY_LENGTH(a) (sizeof(a) / sizeof(a[0]))

#define TSENS_PERIOD                         2   // 312.5 ms
#define TSENS_SENSOR_CONV_TIME_US          150
#define TSENS_NUM_GET_TEMP_RETRIES           5
#define TSENS_GLOBAL_CONFIG         0x0302F16C

#define TSENS_Y1                            30
#define TSENS_Y2                           120

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
static const TsensBootSensorType aSensors[] =
{
   /* Sensor 0 */
   {
      /* .uTsensConfig   */ 0x1C3,
      /* .eCal           */ TSENS_BSP_SENSOR_CAL_NORMAL,
      /* .nX1_default    */ 500,
      /* .nM_default     */ 10923,  /* TSENS_FACTOR * (1 / 3.0) */
      /* .nCriticalMin   */ -35,
      /* .nCriticalMax   */ 120
   },

   /* Sensor 1 */
   {
      /* .uTsensConfig   */ 0x11C3,
      /* .eCal           */ TSENS_BSP_SENSOR_CAL_NORMAL,
      /* .nX1_default    */ 500,
      /* .nM_default     */ 10923,  /* TSENS_FACTOR * (1 / 3.0) */
      /* .nCriticalMin   */ -35,
      /* .nCriticalMax   */ 120
   },

   /* Sensor 2 */
   {
      /* .uTsensConfig   */ 0x11C3,
      /* .eCal           */ TSENS_BSP_SENSOR_CAL_NORMAL,
      /* .nX1_default    */ 500,
      /* .nM_default     */ 10923,  /* TSENS_FACTOR * (1 / 3.0) */
      /* .nCriticalMin   */ -35,
      /* .nCriticalMax   */ 120
   },

   /* Sensor 3 */
   {
      /* .uTsensConfig   */ 0x11C3,
      /* .eCal           */ TSENS_BSP_SENSOR_CAL_NORMAL,
      /* .nX1_default    */ 500,
      /* .nM_default     */ 10923,  /* TSENS_FACTOR * (1 / 3.0) */
      /* .nCriticalMin   */ -35,
      /* .nCriticalMax   */ 120
   },

   /* Sensor 4 */
   {
      /* .uTsensConfig   */ 0x11C3,
      /* .eCal           */ TSENS_BSP_SENSOR_CAL_NORMAL,
      /* .nX1_default    */ 500,
      /* .nM_default     */ 10923,  /* TSENS_FACTOR * (1 / 3.0) */
      /* .nCriticalMin   */ -35,
      /* .nCriticalMax   */ 120
   },
};

const TsensBootBspType TsensBootBsp[] =
{
   {
      /* .paSensors           */ aSensors,
      /* .uNumSensors         */ ARRAY_LENGTH(aSensors),
      /* .uPeriod             */ TSENS_PERIOD,
      /* .uSensorConvTime_us  */ TSENS_SENSOR_CONV_TIME_US,
      /* .uNumGetTempRetries  */ TSENS_NUM_GET_TEMP_RETRIES,
      /* .uGlobalConfig       */ TSENS_GLOBAL_CONFIG,
      /* .nY1                 */ TSENS_Y1,
      /* .nY2                 */ TSENS_Y2
   }
};

