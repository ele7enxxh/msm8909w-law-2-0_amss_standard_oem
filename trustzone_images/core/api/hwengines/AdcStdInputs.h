#ifndef ADC_STD_INPUTS_H
#define ADC_STD_INPUTS_H
/*============================================================================
  @file AdcInputs.h

  Analog-to-Digital Converter driver standard (HK) ADC input names.

  This header provides macros to provide common names to access ADC input
  properties.

               Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/hwengines/AdcStdInputs.h#1 $ */

/** BATT_ID - nPhysical units are in millivolts        */
#define ADC_INPUT_BATT_ID          "BATT_ID"

/** BATT_THERM   - nPhysical units are in degrees C    */
#define ADC_INPUT_BATT_THERM       "BATT_THERM"

/** CHG_BATT_AMON - nPhysical units are in millivolts  */
#define ADC_INPUT_CHG_BATT_AMON   "CHG_BATT_AMON"

/** CHG_TEMP - nPhysical units are in millivolts   */
#define ADC_INPUT_CHG_TEMP         "CHG_TEMP"

/** CHG_THERM - nPhyiscal units are in degrees C       */
#define ADC_INPUT_CHG_THERM        "CHG_THERM"


/** HARDWARE_ID - nPhysical units are in millivolts    */
#define ADC_INPUT_PMIC_HARDWARE_ID "PMIC_HARDWARE_ID"

/** HEADSET_SWITCH - nPhysical units are in millivolts */
#define ADC_INPUT_HEADSET_SWITCH   "HEADSET_SWITCH"

/** ICHG - nPhysical units are in millivolts           */
#define ADC_INPUT_ICHG             "ICHG"

/** MSM_THERM - nPhysical units are in degrees C       */
#define ADC_INPUT_MSM_THERM        "MSM_THERM"

/** PA_POWER_DETECT - nPhysical units are in millivolts*/
#define ADC_INPUT_PA_POWER_DETECT  "PA_POWER_DETECT"

/** PA_THERM - nPhysical units are in degrees C        */
#define ADC_INPUT_PA_THERM         "PA_THERM"

/** PA_THERM1 - nPhysical units are in degrees C       */
#define ADC_INPUT_PA_THERM1        "PA_THERM1"

/** PLATFORM_ID - nPhysical units are in millivolts    */
#define ADC_INPUT_PLATFORM_ID      "PLATFORM_ID"

/** PMIC_THERM - nPhysical units are in 0.001 degrees C*/
#define ADC_INPUT_PMIC_THERM       "PMIC_THERM"

/** USB_CURR_SNS - nPhysical units are in millivolts   */
#define ADC_INPUT_USB_CURR_SNS     "USB_CURR_SNS"

/** USB_IN - nPhysical units are in millivolts         */
#define ADC_INPUT_USB_IN           "USB_IN"

/** USB_VBUS - nPhysical units are in millivolts       */
#define ADC_INPUT_USB_VBUS         "USB_VBUS"

/** VBATT - nPhysical units are in millivolts          */
#define ADC_INPUT_VBATT            "VBATT"

/** VBATT when GSM PA ON - nPhysical units are in millivolts */
#define ADC_INPUT_VBATT_GSM        "VBATT_GSM"

/** VCHG - nPhysical units are in millivolts           */
#define ADC_INPUT_VCHG             "VCHG"

/** VCOIN - nPhysical units are in millivolts          */
#define ADC_INPUT_VCOIN            "VCOIN"

/** VPH_PWR - nPhysical units are in millivolts        */
#define ADC_INPUT_VPH_PWR          "VPH_PWR"

/** XO_THERM - nPhysical units are in 2^-10 degrees C  */
#define ADC_INPUT_XO_THERM         "XO_THERM"

/** XO_THERM_GPS - nPhysical units are in 0.001 degrees C */
#define ADC_INPUT_XO_THERM_GPS     "XO_THERM_GPS"

/** NOT_SUPPORTED - adc input is not supported.        */
#define ADC_INPUT_NOT_SUPPORTED    "NOT_SUPPORTED"

#endif
