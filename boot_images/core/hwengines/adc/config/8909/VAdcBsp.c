/*============================================================================
  FILE:         VAdcBsp.c

  OVERVIEW:     Board support package for the VADC ADC Physical Device DAL
                on 8916.

  DEPENDENCIES: None

                Copyright (c) 2010-2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/adc/config/8909/VAdcBsp.c#1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------

  2014-01-30  PR   Update USBIN scaling to 1/10 and Amux control reg value for VPH_PWR.
  2014-01-07  PR   updated peripheral Sub_type ID of VADC1_LC_USR.
  2013-11-25  PR   New VADC BSP for 8916. Derived from VAdcBsp.c from 8x10.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DalVAdc.h"
#include "VAdcHal.h"
#include "AdcInputs.h"
#include "AdcScalingUtil.h"
#include "pmic.h"

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

enum
{
   VADC_CONFIG_NORMAL,
   VADC_NUM_CONFIGS
};

/*
 * Main Battery Thermistor Table
 *
 * The first element is Voltage V_t in mV and the second element is the
 * temperature in degrees Celcius.
 *
 *             V_ref ___  1800 mV
 *                      |
 *                      >
 *                 R_1  <  90.9 kOhms
 *                      >
 *                      |
 *                      |
 *                      |- - - V_t
 *                      |
 *                      >
 *                 R_2  <  16.2 kOhms
 *                      >
 *                      |
 *                      |
 *                      >
 *                 R_t  <  68 kOhms (TH05-3J683F)
 *                      >
 *                      |
 *                      |
 *                     Gnd
 *
 */
static const AdcMapPtInt32toInt32Type adcMap_BattTherm[] =
{
   { 1632, -30 },
   { 1534, -20 },
   { 1405, -10 },
   { 1254,   0 },
   { 1094,  10 },
   {  938,  20 },
   {  798,  30 },
   {  681,  40 },
   {  587,  50 },
   {  513,  60 },
   {  457,  70 },
   {  415,  80 }
};

/*
 * System Thermistor Table
 *
 * The first element is Voltage V_t in mV and the second element is the
 * temperature in mDegC.
 *
 *             V_ref ___  1800 mV
 *                      |
 *                      >
 *                 R_p  <  100 kOhms
 *                      >
 *                      |
 *                      |
 *                      |- - - V_t
 *                      |
 *                      >
 *                 R_t  <  100 kOhms (NTCG104EF104FB)
 *                      >
 *                      |
 *                      |
 *                     Gnd
 *
 */
static const AdcMapPtInt32toInt32Type adcMap_NTCG104EF104FB_PU2[] =
{
   { 1759, -40 },
   { 1742, -35 },
   { 1720, -30 },
   { 1691, -25 },
   { 1654, -20 },
   { 1608, -15 },
   { 1551, -10 },
   { 1484, -5 },
   { 1405, 0 },
   { 1316, 5 },
   { 1218, 10 },
   { 1114, 15 },
   { 1007, 20 },
   { 900, 25 },
   { 796, 30 },
   { 697, 35 },
   { 606, 40 },
   { 522, 45 },
   { 449, 50 },
   { 384, 55 },
   { 327, 60 },
   { 278, 65 },
   { 238, 70 },
   { 201, 75 },
   { 173, 80 },
   { 147, 85 },
   { 126, 90 },
   { 108, 95 },
   { 92, 100 },
   { 79, 105 },
   { 69, 110 },
   { 60, 115 },
   { 52, 120 },
   { 45, 125 }
};

/*
 * VADC operating modes and decimation filter setup.
 */
static const VAdcConfigType vAdcConfigs[VADC_NUM_CONFIGS] = {
   /* VADC_CONFIG_NORMAL. Used for standard AMUX Input Channels. */
   {
      /* .eDecimationRatio   */ VADC_DECIMATION_RATIO_1024,
      /* .eClockSelect       */ VADC_CLOCK_SELECT_4P8_MHZ,
      /* .uConversionTime_us */ 213
   },
};

/*
 * VAdc channel configuration.
 */
static const VAdcChannelConfigType vAdcChannels[] = {
   /* Channel 0: USBIN */
   {
      /* .pName                     */ ADC_INPUT_USB_IN,
      /* .uAdcHardwareChannel       */ 0x00,
      /* .uConfigIdx                */ VADC_CONFIG_NORMAL,
      /* .eSettlingDelay            */ VADC_SETTLING_DELAY_0_US,
      /* .eFastAverageMode          */ VADC_FAST_AVERAGE_NONE,
      /* .bUseSequencer             */ FALSE,
      /* .uSequencerIdx             */ 0,
      /* .scalingFactor {num, den}  */ {1, 10},
      /* .eScalingMethod            */ VADC_SCALE_TO_MILLIVOLTS,
      /* .pInterpolationTable       */ NULL,
      /* .uInterpolationTableLength */ 0,
      /* .eCalMethod                */ VADC_CAL_METHOD_ABSOLUTE,
      /* .eMppConfig                */ VADC_CHANNEL_MPP_CONFIG_NONE,
      /* .eMpp                      */ PM_MPP_INVALID,
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID
   },

   /* Channel 1: VCOIN */
   {
      /* .pName                     */ ADC_INPUT_VCOIN,
      /* .uAdcHardwareChannel       */ 0x05,
      /* .uConfigIdx                */ VADC_CONFIG_NORMAL,
      /* .eSettlingDelay            */ VADC_SETTLING_DELAY_0_US,
      /* .eFastAverageMode          */ VADC_FAST_AVERAGE_NONE,
      /* .bUseSequencer             */ FALSE,
      /* .uSequencerIdx             */ 0,
      /* .scalingFactor {num, den}  */ {1, 3},
      /* .eScalingMethod            */ VADC_SCALE_TO_MILLIVOLTS,
      /* .pInterpolationTable       */ NULL,
      /* .uInterpolationTableLength */ 0,
      /* .eCalMethod                */ VADC_CAL_METHOD_ABSOLUTE,
      /* .eMppConfig                */ VADC_CHANNEL_MPP_CONFIG_NONE,
      /* .eMpp                      */ PM_MPP_INVALID,
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID
   },

   /* Channel 2: VBATT */
   {
      /* .pName                     */ ADC_INPUT_VBATT,
      /* .uAdcHardwareChannel       */ 0x06,
      /* .uConfigIdx                */ VADC_CONFIG_NORMAL,
      /* .eSettlingDelay            */ VADC_SETTLING_DELAY_0_US,
      /* .eFastAverageMode          */ VADC_FAST_AVERAGE_NONE,
      /* .bUseSequencer             */ FALSE,
      /* .uSequencerIdx             */ 0,
      /* .scalingFactor {num, den}  */ {1, 3},
      /* .eScalingMethod            */ VADC_SCALE_TO_MILLIVOLTS,
      /* .pInterpolationTable       */ NULL,
      /* .uInterpolationTableLength */ 0,
      /* .eCalMethod                */ VADC_CAL_METHOD_ABSOLUTE,
      /* .eMppConfig                */ VADC_CHANNEL_MPP_CONFIG_NONE,
      /* .eMpp                      */ PM_MPP_INVALID,
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID
   },

   /* Channel 3: VPH_PWR */
   {
      /* .pName                     */ ADC_INPUT_VPH_PWR,
      /* .uAdcHardwareChannel       */ 0x07,
      /* .uConfigIdx                */ VADC_CONFIG_NORMAL,
      /* .eSettlingDelay            */ VADC_SETTLING_DELAY_0_US,
      /* .eFastAverageMode          */ VADC_FAST_AVERAGE_NONE,
      /* .bUseSequencer             */ FALSE,
      /* .uSequencerIdx             */ 0,
      /* .scalingFactor {num, den}  */ {1, 3},
      /* .eScalingMethod            */ VADC_SCALE_TO_MILLIVOLTS,
      /* .pInterpolationTable       */ NULL,
      /* .uInterpolationTableLength */ 0,
      /* .eCalMethod                */ VADC_CAL_METHOD_ABSOLUTE,
      /* .eMppConfig                */ VADC_CHANNEL_MPP_CONFIG_NONE,
      /* .eMpp                      */ PM_MPP_INVALID,
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID
   },

   /* Channel 4: DIE_TEMP */
   {
      /* .pName                     */ ADC_INPUT_PMIC_THERM,
      /* .uAdcHardwareChannel       */ 0x08,
      /* .uConfigIdx                */ VADC_CONFIG_NORMAL,
      /* .eSettlingDelay            */ VADC_SETTLING_DELAY_0_US,
      /* .eFastAverageMode          */ VADC_FAST_AVERAGE_NONE,
      /* .bUseSequencer             */ FALSE,
      /* .uSequencerIdx             */ 0,
      /* .scalingFactor {num, den}  */ {1, 1},
      /* .eScalingMethod            */ VADC_SCALE_PMIC_SENSOR_TO_MILLIDEGREES,
      /* .pInterpolationTable       */ NULL,
      /* .uInterpolationTableLength */ 0,
      /* .eCalMethod                */ VADC_CAL_METHOD_ABSOLUTE,
      /* .eMppConfig                */ VADC_CHANNEL_MPP_CONFIG_NONE,
      /* .eMpp                      */ PM_MPP_INVALID,
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID
   },

   /* Channel 5: CHG_TEMP */
   {
      /* .pName                     */ ADC_INPUT_CHG_TEMP,
      /* .uAdcHardwareChannel       */ 0x0B,
      /* .uConfigIdx                */ VADC_CONFIG_NORMAL,
      /* .eSettlingDelay            */ VADC_SETTLING_DELAY_0_US,
      /* .eFastAverageMode          */ VADC_FAST_AVERAGE_NONE,
      /* .bUseSequencer             */ FALSE,
      /* .uSequencerIdx             */ 0,
      /* .scalingFactor {num, den}  */ {1, 1},
      /* .eScalingMethod            */ VADC_SCALE_PMIC_SENSOR_TO_MILLIDEGREES,
      /* .pInterpolationTable       */ NULL,
      /* .uInterpolationTableLength */ 0,
      /* .eCalMethod                */ VADC_CAL_METHOD_ABSOLUTE,
      /* .eMppConfig                */ VADC_CHANNEL_MPP_CONFIG_NONE,
      /* .eMpp                      */ PM_MPP_INVALID,
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID
   },

   /* Channel 6: BATT_THERM */
   {
      /* .pName                     */ ADC_INPUT_BATT_THERM,
      /* .uAdcHardwareChannel       */ 0x30,
      /* .uConfigIdx                */ VADC_CONFIG_NORMAL,
      /* .eSettlingDelay            */ VADC_SETTLING_DELAY_100_US,
      /* .eFastAverageMode          */ VADC_FAST_AVERAGE_NONE,
      /* .bUseSequencer             */ FALSE,
      /* .uSequencerIdx             */ 0,
      /* .scalingFactor {num, den}  */ {1, 1},
      /* .eScalingMethod            */ VADC_SCALE_INTERPOLATE_FROM_MILLIVOLTS,
      /* .pInterpolationTable       */ adcMap_BattTherm,
      /* .uInterpolationTableLength */ ARRAY_LENGTH(adcMap_BattTherm),
      /* .eCalMethod                */ VADC_CAL_METHOD_RATIOMETRIC,
      /* .eMppConfig                */ VADC_CHANNEL_MPP_CONFIG_NONE,
      /* .eMpp                      */ PM_MPP_INVALID,
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID
   },

   /* Channel 7: BATT_ID */
   {
      /* .pName                     */ ADC_INPUT_BATT_ID,
      /* .uAdcHardwareChannel       */ 0x31,
      /* .uConfigIdx                */ VADC_CONFIG_NORMAL,
      /* .eSettlingDelay            */ VADC_SETTLING_DELAY_100_US,
      /* .eFastAverageMode          */ VADC_FAST_AVERAGE_NONE,
      /* .bUseSequencer             */ FALSE,
      /* .uSequencerIdx             */ 0,
      /* .scalingFactor {num, den}  */ {1, 1},
      /* .eScalingMethod            */ VADC_SCALE_TO_MILLIVOLTS,
      /* .pInterpolationTable       */ NULL,
      /* .uInterpolationTableLength */ 0,
      /* .eCalMethod                */ VADC_CAL_METHOD_RATIOMETRIC,
      /* .eMppConfig                */ VADC_CHANNEL_MPP_CONFIG_NONE,
      /* .eMpp                      */ PM_MPP_INVALID,
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID
   },

   /* Channel 8: XO_THERM_MV */
   {
      /* .pName                     */ ADC_INPUT_XO_THERM_MV,
      /* .uAdcHardwareChannel       */ 0x3C,
      /* .uConfigIdx                */ VADC_CONFIG_NORMAL,
      /* .eSettlingDelay            */ VADC_SETTLING_DELAY_500_US,
      /* .eFastAverageMode          */ VADC_FAST_AVERAGE_NONE,
      /* .bUseSequencer             */ FALSE,
      /* .uSequencerIdx             */ 0,
      /* .scalingFactor             */ {1, 1},
      /* .eScalingMethod            */ VADC_SCALE_TO_MILLIVOLTS,
      /* .pInterpolationTable       */ NULL,
      /* .uInterpolationTableLength */ 0,
      /* .eCalMethod                */ VADC_CAL_METHOD_RATIOMETRIC,
      /* .eMppConfig                */ VADC_CHANNEL_MPP_CONFIG_NONE,
      /* .eMpp                      */ PM_MPP_INVALID,
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID
   },
   /* Channel 9: PA_THERM */
   {
      /* .pName                     */ ADC_INPUT_PA_THERM,
      /* .uAdcHardwareChannel       */ 0x36,
      /* .uConfigIdx                */ VADC_CONFIG_NORMAL,
      /* .eSettlingDelay            */ VADC_SETTLING_DELAY_100_US,
      /* .eFastAverageMode          */ VADC_FAST_AVERAGE_NONE,
      /* .bUseSequencer             */ FALSE,
      /* .uSequencerIdx             */ 0,
      /* .scalingFactor {num, den}  */ {1, 1},
      /* .eScalingMethod            */ VADC_SCALE_INTERPOLATE_FROM_MILLIVOLTS,
      /* .pInterpolationTable       */ adcMap_NTCG104EF104FB_PU2,
      /* .uInterpolationTableLength */ ARRAY_LENGTH(adcMap_NTCG104EF104FB_PU2),
      /* .eCalMethod                */ VADC_CAL_METHOD_RATIOMETRIC,
      /* .eMppConfig                */ VADC_CHANNEL_MPP_CONFIG_NONE,
      /* .eMpp                      */ PM_MPP_INVALID,
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID
   },

};

static const VAdcChannelConfigType vAdcCalibrationChannels[] = {
   /* Vdd */
   {
      /* .pName                     */ "VDD",
      /* .uAdcHardwareChannel       */ 0x0F,
      /* .uConfigIdx                */ VADC_CONFIG_NORMAL,
      /* .eSettlingDelay            */ VADC_SETTLING_DELAY_0_US,
      /* .eFastAverageMode          */ VADC_FAST_AVERAGE_NONE,
      /* .bUseSequencer             */ FALSE,
      /* .uSequencerIdx             */ 0,
      /* .scalingFactor {num, den}  */ {1, 1},
      /* .eScalingMethod            */ VADC_SCALE_TO_MILLIVOLTS,
      /* .pInterpolationTable       */ NULL,
      /* .uInterpolationTableLength */ 0,
      /* .eCalMethod                */ VADC_CAL_METHOD_RATIOMETRIC,
      /* .eMppConfig                */ VADC_CHANNEL_MPP_CONFIG_NONE,
      /* .eMpp                      */ PM_MPP_INVALID,
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID
   },

   /* Gnd */
   {
      /* .pName                     */ "GND",
      /* .uAdcHardwareChannel       */ 0x0E,
      /* .uConfigIdx                */ VADC_CONFIG_NORMAL,
      /* .eSettlingDelay            */ VADC_SETTLING_DELAY_0_US,
      /* .eFastAverageMode          */ VADC_FAST_AVERAGE_NONE,
      /* .bUseSequencer             */ FALSE,
      /* .uSequencerIdx             */ 0,
      /* .scalingFactor {num, den}  */ {1, 1},
      /* .eScalingMethod            */ VADC_SCALE_TO_MILLIVOLTS,
      /* .pInterpolationTable       */ NULL,
      /* .uInterpolationTableLength */ 0,
      /* .eCalMethod                */ VADC_CAL_METHOD_RATIOMETRIC,
      /* .eMppConfig                */ VADC_CHANNEL_MPP_CONFIG_NONE,
      /* .eMpp                      */ PM_MPP_INVALID,
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID
   },

   /* Vref1 */
   {
      /* .pName                     */ "VREF1",
      /* .uAdcHardwareChannel       */ 0x09,
      /* .uConfigIdx                */ VADC_CONFIG_NORMAL,
      /* .eSettlingDelay            */ VADC_SETTLING_DELAY_0_US,
      /* .eFastAverageMode          */ VADC_FAST_AVERAGE_NONE,
      /* .bUseSequencer             */ FALSE,
      /* .uSequencerIdx             */ 0,
      /* .scalingFactor {num, den}  */ {1, 1},
      /* .eScalingMethod            */ VADC_SCALE_TO_MILLIVOLTS,
      /* .pInterpolationTable       */ NULL,
      /* .uInterpolationTableLength */ 0,
      /* .eCalMethod                */ VADC_CAL_METHOD_ABSOLUTE,
      /* .eMppConfig                */ VADC_CHANNEL_MPP_CONFIG_NONE,
      /* .eMpp                      */ PM_MPP_INVALID,
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID
   },

   /* Vref2 */
   {
      /* .pName                     */ "VREF2",
      /* .uAdcHardwareChannel       */ 0x0A,
      /* .uConfigIdx                */ VADC_CONFIG_NORMAL,
      /* .eSettlingDelay            */ VADC_SETTLING_DELAY_0_US,
      /* .eFastAverageMode          */ VADC_FAST_AVERAGE_NONE,
      /* .bUseSequencer             */ FALSE,
      /* .uSequencerIdx             */ 0,
      /* .scalingFactor {num, den}  */ {1, 1},
      /* .eScalingMethod            */ VADC_SCALE_TO_MILLIVOLTS,
      /* .pInterpolationTable       */ NULL,
      /* .uInterpolationTableLength */ 0,
      /* .eCalMethod                */ VADC_CAL_METHOD_ABSOLUTE,
      /* .eMppConfig                */ VADC_CHANNEL_MPP_CONFIG_NONE,
      /* .eMpp                      */ PM_MPP_INVALID,
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID
   }
};

const VAdcBspType VAdcBsp[] = {
   {
      /* .eAccessPriority      */ SPMI_BUS_ACCESS_PRIORITY_LOW,
      /* .uSlaveId             */ 0,
      /* .uPmicDevice          */ 0,
      /* .uPeripheralID        */ 0x31,
      /* .uMasterID            */ 0,
      /* .bUsesInterrupts      */ FALSE,
      /* .uMinDigMinor         */ 0,
      /* .uMinDigMajor         */ 0,
      /* .uMinAnaMinor         */ 0,
      /* .uMinAnaMajor         */ 0,
      /* .uPerphType           */ 0x08,
      /* .uPerphSubType        */ 0x09,
      /* .uVrefP_mv            */ 1800,
      /* .uVrefN_mv            */ 0,
      /* .uVref1_mv            */ 625,
      /* .uVref2_mv            */ 1250,
      /* .uNumPeriodsTimeout   */ 10,
      /* .uNumSequencerConfigs */ 0,
      /* .paSequencerParams    */ NULL,
      /* .uNumConfigs          */ ARRAY_LENGTH(vAdcConfigs),
      /* .paConfigs            */ vAdcConfigs,
      /* .uNumChannels         */ ARRAY_LENGTH(vAdcChannels),
      /* .paChannels           */ vAdcChannels,
      /* .uNumCalChannels      */ ARRAY_LENGTH(vAdcCalibrationChannels),
      /* .paCalChannels        */ vAdcCalibrationChannels
   }
};

