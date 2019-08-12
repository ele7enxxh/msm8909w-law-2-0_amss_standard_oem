#ifndef ADC_INPUTS_H
#define ADC_INPUTS_H
/*============================================================================
  @file AdcInputs.h

  Analog-to-Digital Converter driver standard ADC input names.

  This header provides macros to provide common names to access ADC input
  properties.

               Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/api/hwengines/AdcInputs.h#1 $ */

/** AMBIENT_THERM - nPhysical units are in degrees C   */
#define ADC_INPUT_AMBIENT_THERM    "AMBIENT_THERM"

/** BATT_ID - nPhysical units are in millivolts        */
#define ADC_INPUT_BATT_ID          "BATT_ID"

/** BATT_THERM   - nPhysical units are in degrees C    */
#define ADC_INPUT_BATT_THERM       "BATT_THERM"

/** CASE_THERM - nPhysical units are in degrees C      */
#define ADC_INPUT_CASE_THERM       "CASE_THERM"

/** CHG_BATT_AMON - nPhysical units are in millivolts  */
#define ADC_INPUT_CHG_BATT_AMON    "CHG_BATT_AMON"

/** CHG_TEMP - nPhysical units are in 0.001 degrees C  */
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

/** XO_THERM_GPS - nPhysical units are in 2^-10 degrees C  */
#define ADC_INPUT_XO_THERM_GPS     "XO_THERM_GPS"

/*! DC_IN - nPhysical units are in millivolts          */
#define ADC_INPUT_DC_IN             "DC_IN"

/** SYS_THERM1 - nPhysical units are in degrees C      */
#define ADC_INPUT_SYS_THERM1        "SYS_THERM1"

/** SYS_THERM2 - nPhysical units are in degrees C      */
#define ADC_INPUT_SYS_THERM2        "SYS_THERM2"

/** SYS_THERM3 - nPhysical units are in degrees C      */
#define ADC_INPUT_SYS_THERM3        "SYS_THERM3"

/** SYS_THERM4 - nPhysical units are in degrees C      */
#define ADC_INPUT_SYS_THERM4        "SYS_THERM4"

/** SYS_THERM5 - nPhysical units are in degrees C      */
#define ADC_INPUT_SYS_THERM5        "SYS_THERM5"

/*! USB_DATA - nPhysical units are in millivolts       */
#define ADC_INPUT_USB_DATA          "USB_DATA"

/*! USB_ID - nPhysical units are in ohms               */
#define ADC_INPUT_USB_ID            "USB_ID"

#endif
