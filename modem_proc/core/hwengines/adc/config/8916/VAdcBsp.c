/*============================================================================
  FILE:         VAdcBsp.c

  OVERVIEW:     Board support package for the VADC ADC Physical Device DAL
                on 8916.

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

  $Header: //components/rel/core.mpss/3.5.c12.3/hwengines/adc/config/8916/VAdcBsp.c#1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-10-10  SA   Increase hw settle delay of BATT_THERM and BATT_ID channels to 2ms (CR 730175).
  2014-10-03  PR   support for calibration before the conversion(8916 WA for MPP2 current sink issue).
  2014-01-31  PR   Update USBIN scaling to 1/10 and Amux control reg value for VPH_PWR.   
  2014-01-07  PR   Updated VADC3_LC_MDM Peripheral Sub-Type ID.
  2013-11-25  PR   Created for 8916, derived from 8x26/8x10/8974.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DalVAdc.h"
#include "VAdcHal.h"
#include "AdcInputs.h"
#include "AdcScalingUtil.h"

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
   VADC_CONFIG_SLOW,
   VADC_CONFIG_FAST,
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
 * XO Thermistor Table
 *
 * This lookup table is used to convert the XO thermistor reading to temperature
 * in degrees C multiplied by a factor of 1024.
 *
 * The first column in the table is the ratio of Rt' divided by R25 (multiplied
 * by a factor of 2^14).
 *
 * The second column is the temperature in degrees Celsius multiplied by a factor
 * of 1024.
 *
 * Rt' is the resistance of the thermistor at temperature t in parallel with the
 * input impedance of the ADC.
 *
 * R25 is the series resistor. The resistor value should be the same as that
 * of the thermistor at 25 degrees C (100 KOhms in the reference design).
 *
 *             V_ref ___
 *                      |
 *                      >
 *                R_25  <   100 kOhms
 *                      >
 *                      |
 *                      |
 *                      |- - - V_t
 *                      |
 *                      >
 *                R_t   <   100 kOhms (NTCG104EF104FB)
 *                      >
 *                      |
 *                      |
 *                     Gnd
 *
 */
static const AdcMapPtInt32toInt32Type adcMap_XO_NTCG104EF104FB_PU2[] =
{
   { 696483, -40960 },
   { 649148, -39936 },
   { 605368, -38912 },
   { 564809, -37888 },
   { 527215, -36864 },
   { 492322, -35840 },
   { 460007, -34816 },
   { 429982, -33792 },
   { 402099, -32768 },
   { 376192, -31744 },
   { 352075, -30720 },
   { 329714, -29696 },
   { 308876, -28672 },
   { 289480, -27648 },
   { 271417, -26624 },
   { 254574, -25600 },
   { 238903, -24576 },
   { 224276, -23552 },
   { 210631, -22528 },
   { 197896, -21504 },
   { 186007, -20480 },
   { 174899, -19456 },
   { 164521, -18432 },
   { 154818, -17408 },
   { 145744, -16384 },
   { 137265, -15360 },
   { 129307, -14336 },
   { 121866, -13312 },
   { 114896, -12288 },
   { 108365, -11264 },
   { 102252, -10240 },
   {  96499,  -9216 },
   {  91111,  -8192 },
   {  86055,  -7168 },
   {  81308,  -6144 },
   {  76857,  -5120 },
   {  72660,  -4096 },
   {  68722,  -3072 },
   {  65020,  -2048 },
   {  61538,  -1024 },
   {  58261,      0 },
   {  55177,   1024 },
   {  52274,   2048 },
   {  49538,   3072 },
   {  46962,   4096 },
   {  44531,   5120 },
   {  42243,   6144 },
   {  40083,   7168 },
   {  38045,   8192 },
   {  36122,   9216 },
   {  34308,  10240 },
   {  32592,  11264 },
   {  30972,  12288 },
   {  29442,  13312 },
   {  27995,  14336 },
   {  26624,  15360 },
   {  25333,  16384 },
   {  24109,  17408 },
   {  22951,  18432 },
   {  21854,  19456 },
   {  20807,  20480 },
   {  19831,  21504 },
   {  18899,  22528 },
   {  18016,  23552 },
   {  17178,  24576 },
   {  16384,  25600 },
   {  15631,  26624 },
   {  14916,  27648 },
   {  14237,  28672 },
   {  13593,  29696 },
   {  12976,  30720 },
   {  12400,  31744 },
   {  11848,  32768 },
   {  11324,  33792 },
   {  10825,  34816 },
   {  10354,  35840 },
   {   9900,  36864 },
   {   9471,  37888 },
   {   9062,  38912 },
   {   8674,  39936 },
   {   8306,  40960 },
   {   7951,  41984 },
   {   7616,  43008 },
   {   7296,  44032 },
   {   6991,  45056 },
   {   6701,  46080 },
   {   6424,  47104 },
   {   6160,  48128 },
   {   5908,  49152 },
   {   5667,  50176 },
   {   5439,  51200 },
   {   5219,  52224 },
   {   5010,  53248 },
   {   4810,  54272 },
   {   4619,  55296 },
   {   4440,  56320 },
   {   4263,  57344 },
   {   4097,  58368 },
   {   3938,  59392 },
   {   3785,  60416 },
   {   3637,  61440 },
   {   3501,  62464 },
   {   3368,  63488 },
   {   3240,  64512 },
   {   3118,  65536 },
   {   2998,  66560 },
   {   2889,  67584 },
   {   2782,  68608 },
   {   2680,  69632 },
   {   2581,  70656 },
   {   2490,  71680 },
   {   2397,  72704 },
   {   2310,  73728 },
   {   2227,  74752 },
   {   2147,  75776 },
   {   2064,  76800 },
   {   1998,  77824 },
   {   1927,  78848 },
   {   1860,  79872 },
   {   1795,  80896 },
   {   1736,  81920 },
   {   1673,  82944 },
   {   1615,  83968 },
   {   1560,  84992 },
   {   1507,  86016 },
   {   1456,  87040 },
   {   1407,  88064 },
   {   1360,  89088 },
   {   1314,  90112 },
   {   1271,  91136 },
   {   1228,  92160 },
   {   1189,  93184 },
   {   1150,  94208 },
   {   1112,  95232 },
   {   1076,  96256 },
   {   1042,  97280 },
   {   1008,  98304 },
   {    976,  99328 },
   {    945, 100352 },
   {    915, 101376 },
   {    886, 102400 },
   {    859, 103424 },
   {    832, 104448 },
   {    807, 105472 },
   {    782, 106496 },
   {    756, 107520 },
   {    735, 108544 },
   {    712, 109568 },
   {    691, 110592 },
   {    670, 111616 },
   {    650, 112640 },
   {    631, 113664 },
   {    612, 114688 },
   {    594, 115712 },
   {    577, 116736 },
   {    560, 117760 },
   {    544, 118784 },
   {    528, 119808 },
   {    513, 120832 },
   {    498, 121856 },
   {    483, 122880 },
   {    470, 123904 },
   {    457, 124928 },
   {    444, 125952 },
   {    431, 126976 },
   {    419, 128000 },
   {    408, 129024 },
   {    396, 130048 }
};

/*
 * VADC operating modes and decimation filter setup.
 */
static const VAdcConfigType vAdcConfigs[] = {
   /* VADC_CONFIG_NORMAL. Used for standard AMUX Input Channels. */
   {
      /* .eDecimationRatio   */ VADC_DECIMATION_RATIO_1024,
      /* .eClockSelect       */ VADC_CLOCK_SELECT_4P8_MHZ,
      /* .uConversionTime_us */ 213
   },
   /* VADC_CONFIG_SLOW. Slow operating mode. Used for XO THERM GPS for better accuracy. */
   {
      /* .eDecimationRatio   */ VADC_DECIMATION_RATIO_4096,
      /* .eClockSelect       */ VADC_CLOCK_SELECT_4P8_MHZ,
      /* .uConversionTime_us */ 853
   },
   /* VADC_CONFIG_FAST. Faster mode. Used for VBATT GSM SYNC. */
   {
      /* .eDecimationRatio   */ VADC_DECIMATION_RATIO_512,
      /* .eClockSelect       */ VADC_CLOCK_SELECT_4P8_MHZ,
      /* .uConversionTime_us */ 107
   }
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
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID,
      /* .bRecalibrate              */ TRUE,
      /* .uMaxRecalibrations        */ VADC_MAX_RECALIBRATIONS
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
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID,
      /* .bRecalibrate              */ TRUE,
      /* .uMaxRecalibrations        */ VADC_MAX_RECALIBRATIONS
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
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID,
      /* .bRecalibrate              */ TRUE,
      /* .uMaxRecalibrations        */ VADC_MAX_RECALIBRATIONS
   },

   /* Channel 3: VBATT_GSM */
   {
      /* .pName                     */ ADC_INPUT_VBATT_GSM,
      /* .uAdcHardwareChannel       */ 0x06,
      /* .uConfigIdx                */ VADC_CONFIG_FAST,
      /* .eSettlingDelay            */ VADC_SETTLING_DELAY_0_US,
      /* .eFastAverageMode          */ VADC_FAST_AVERAGE_NONE,
      /* .bUseSequencer             */ TRUE,
      /* .uSequencerIdx             */ 0,
      /* .scalingFactor {num, den}  */ {1, 3},
      /* .eScalingMethod            */ VADC_SCALE_TO_MILLIVOLTS,
      /* .pInterpolationTable       */ NULL,
      /* .uInterpolationTableLength */ 0,
      /* .eCalMethod                */ VADC_CAL_METHOD_ABSOLUTE,
      /* .eMppConfig                */ VADC_CHANNEL_MPP_CONFIG_NONE,
      /* .eMpp                      */ PM_MPP_INVALID,
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID,
      /* .bRecalibrate              */ TRUE,
      /* .uMaxRecalibrations        */ VADC_MAX_RECALIBRATIONS
   },

   /* Channel 4: VPH_PWR */
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
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID,
      /* .bRecalibrate              */ TRUE,
      /* .uMaxRecalibrations        */ VADC_MAX_RECALIBRATIONS
   },

   /* Channel 5: DIE_TEMP */
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
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID,
      /* .bRecalibrate              */ TRUE,
      /* .uMaxRecalibrations        */ VADC_MAX_RECALIBRATIONS
   },

   /* Channel 6: CHG_TEMP */
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
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID,
      /* .bRecalibrate              */ TRUE,
      /* .uMaxRecalibrations        */ VADC_MAX_RECALIBRATIONS
   },

   /* Channel 7: BATT_THERM */
   {
      /* .pName                     */ ADC_INPUT_BATT_THERM,
      /* .uAdcHardwareChannel       */ 0x30,
      /* .uConfigIdx                */ VADC_CONFIG_NORMAL,
      /* .eSettlingDelay            */ VADC_SETTLING_DELAY_2_MS,
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
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID,
      /* .bRecalibrate              */ TRUE,
      /* .uMaxRecalibrations        */ VADC_MAX_RECALIBRATIONS
   },

   /* Channel 8: BATT_ID */
   {
      /* .pName                     */ ADC_INPUT_BATT_ID,
      /* .uAdcHardwareChannel       */ 0x31,
      /* .uConfigIdx                */ VADC_CONFIG_NORMAL,
      /* .eSettlingDelay            */ VADC_SETTLING_DELAY_2_MS,
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
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID,
      /* .bRecalibrate              */ TRUE,
      /* .uMaxRecalibrations        */ VADC_MAX_RECALIBRATIONS
   },

   /* Channel 9: XO_THERM */
   {
      /* .pName                     */ ADC_INPUT_XO_THERM,
      /* .uAdcHardwareChannel       */ 0x3C,
      /* .uConfigIdx                */ VADC_CONFIG_NORMAL,
      /* .eSettlingDelay            */ VADC_SETTLING_DELAY_500_US,
      /* .eFastAverageMode          */ VADC_FAST_AVERAGE_NONE,
      /* .bUseSequencer             */ FALSE,
      /* .uSequencerIdx             */ 0,
      /* .scalingFactor             */ {1, 1},
      /* .eScalingMethod            */ VADC_SCALE_INTERPOLATE_FROM_RT_R25,
      /* .pInterpolationTable       */ adcMap_XO_NTCG104EF104FB_PU2,
      /* .uInterpolationTableLength */ ARRAY_LENGTH(adcMap_XO_NTCG104EF104FB_PU2),
      /* .eCalMethod                */ VADC_CAL_METHOD_RATIOMETRIC,
      /* .eMppConfig                */ VADC_CHANNEL_MPP_CONFIG_NONE,
      /* .eMpp                      */ PM_MPP_INVALID,
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID,
      /* .bRecalibrate              */ TRUE,
      /* .uMaxRecalibrations        */ VADC_MAX_RECALIBRATIONS
   },

   /* Channel 10: XO_THERM_GPS */
   {
      /* .pName                     */ ADC_INPUT_XO_THERM_GPS,
      /* .uAdcHardwareChannel       */ 0x3C,
      /* .uConfigIdx                */ VADC_CONFIG_SLOW,
      /* .eSettlingDelay            */ VADC_SETTLING_DELAY_500_US,
      /* .eFastAverageMode          */ VADC_FAST_AVERAGE_NONE,
      /* .bUseSequencer             */ FALSE,
      /* .uSequencerIdx             */ 0,
      /* .scalingFactor             */ {1, 1},
      /* .eScalingMethod            */ VADC_SCALE_INTERPOLATE_FROM_RT_R25,
      /* .pInterpolationTable       */ adcMap_XO_NTCG104EF104FB_PU2,
      /* .uInterpolationTableLength */ ARRAY_LENGTH(adcMap_XO_NTCG104EF104FB_PU2),
      /* .eCalMethod                */ VADC_CAL_METHOD_RATIOMETRIC,
      /* .eMppConfig                */ VADC_CHANNEL_MPP_CONFIG_NONE,
      /* .eMpp                      */ PM_MPP_INVALID,
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID,
      /* .bRecalibrate              */ TRUE,
      /* .uMaxRecalibrations        */ VADC_MAX_RECALIBRATIONS
   },

   /* Channel 11: PA_THERM */
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
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID,
      /* .bRecalibrate              */ TRUE,
      /* .uMaxRecalibrations        */ VADC_MAX_RECALIBRATIONS
   },
   /* Channel 12: Vdd */
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
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID,
      /* .bRecalibrate              */ FALSE,
      /* .uMaxRecalibrations        */ 0
   },

   /* Channel 13: Gnd */
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
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID,
      /* .bRecalibrate              */ FALSE,
      /* .uMaxRecalibrations        */ 0
   },

   /* Channel 14: Vref1 */
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
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID,
      /* .bRecalibrate              */ FALSE,
      /* .uMaxRecalibrations        */ 0
   },

   /* Channel 15: Vref2 */
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
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID,
      /* .bRecalibrate              */ FALSE,
      /* .uMaxRecalibrations        */ 0
   }
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
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID,
      /* .bRecalibrate              */ FALSE,
      /* .uMaxRecalibrations        */ 0
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
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID,
      /* .bRecalibrate              */ FALSE,
      /* .uMaxRecalibrations        */ 0
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
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID,
      /* .bRecalibrate              */ FALSE,
      /* .uMaxRecalibrations        */ 0
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
      /* .eChSelect                 */ PM_MPP__AIN__CH_INVALID,
      /* .bRecalibrate              */ FALSE,
      /* .uMaxRecalibrations        */ 0
   }
};

/*
 * The ADC aribiter features a conversion sequencer for the modem processor.
 * The conversion sequencer is used for vbatt measurement with GSM TX
 * (synchronous vbatt measurement).
 */
static const VAdcSequencerParametersType sequencerParameters[] = {
   {
      /* .eHoldoff          */ VADC_SEQUENCER_HOLDOFF_200_US,
      /* .eTimeoutTime      */ VADC_SEQUENCER_TIMEOUT_TIME_5_MS,
      /* .eTriggerCondition */ VADC_TRIGGER_CONDITION_RISING_EDGE,
      /* .eTriggerInput     */ VADC_TRIGGER_INPUT_TRIG_2,
   }
};

const VAdcBspType VAdcBsp[] = {
   {
      /* .eAccessPriority      */ SPMI_BUS_ACCESS_PRIORITY_LOW,
      /* .uSlaveId             */ 0,
      /* .uPmicDevice          */ 0,
      /* .uPeripheralID        */ 0x32,
      /* .uMasterID            */ 0,
      /* .bUsesInterrupts      */ FALSE,
      /* .uMinDigMinor         */ 0,
      /* .uMinDigMajor         */ 0,
      /* .uMinAnaMinor         */ 0,
      /* .uMinAnaMajor         */ 0,
      /* .uPerphType           */ 0x08,
      /* .uPerphSubType        */ 0x0B,
      /* .uVrefP_mv            */ 1800,
      /* .uVrefN_mv            */ 0,
      /* .uVref1_mv            */ 625,
      /* .uVref2_mv            */ 1250,
      /* .uNumPeriodsTimeout   */ 500,
      /* .uNumSequencerConfigs */ ARRAY_LENGTH(sequencerParameters),
      /* .paSequencerParams    */ sequencerParameters,
      /* .uNumConfigs          */ ARRAY_LENGTH(vAdcConfigs),
      /* .paConfigs            */ vAdcConfigs,
      /* .uNumChannels         */ ARRAY_LENGTH(vAdcChannels),
      /* .paChannels           */ vAdcChannels,
      /* .uNumCalChannels      */ ARRAY_LENGTH(vAdcCalibrationChannels),
      /* .paCalChannels        */ vAdcCalibrationChannels
   }
};

