#ifndef HW_MODULE_TYPE_H
#define HW_MODULE_TYPE_H

/*! \ file hw_module_type.h
*
 * \brief   <Brief Comment Block/>
 * \details <Detailed Comment Block/>
 * \n &copy; Copyright 2012 - 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===================================================================
			        EDIT HISTORY FOR MODULE
 This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/pmic_devices/common/src/hw_module_type.h#1 $
$Author: pwbldsvc $

when       who     what, where, why
--------	---		------------------------------------------------
11/03/14    pxm     Correct the sub-type of charger peripheral
====================================================================*/


typedef enum
{
    PM_HW_MODULE_Reserved = 0,
    PM_HW_MODULE_PON = 1,       
    PM_HW_MODULE_CHARGER = 2,
    PM_HW_MODULE_HF_BUCK = 3,
    PM_HW_MODULE_LDO = 4,
    PM_HW_MODULE_VS = 5,
    PM_HW_MODULE_CLOCK = 6,
    PM_HW_MODULE_RTC = 7,
    PM_HW_MODULE_ADC = 8,
    PM_HW_MODULE_ALARM = 9,
    PM_HW_MODULE_INTERRUPT = 10,
    PM_HW_MODULE_INTERFACE = 11,
    PM_HW_MODULE_TRIM = 12,
    PM_HW_MODULE_BMS = 13,
    PM_HW_MODULE_MBG = 14,
    PM_HW_MODULE_LED_DRIVER = 15,
    PM_HW_MODULE_GPIO = 16,
    PM_HW_MODULE_MPP = 17,
    PM_HW_MODULE_KEYPAD = 18,
    PM_HW_MODULE_LPG = 19,
    PM_HW_MODULE_MISC = 20,
    PM_HW_MODULE_VIB = 21,
    PM_HW_MODULE_PBS = 22,
    PM_HW_MODULE_WLED = 23,
    PM_HW_MODULE_FLASH = 24,
    PM_HW_MODULE_RGB = 25,
    PM_HW_MODULE_KEYPAD_BL = 26,
    PM_HW_MODULE_BOOST = 27,
    PM_HW_MODULE_FTS = 28,
    PM_HW_MODULE_BUCK_CMN = 29,
    PM_HW_MODULE_BOOST_BYP = 31,
    PM_HW_MODULE_IBB = 32,
    PM_HW_MODULE_ULT_LDO = 33,
    PM_HW_MODULE_ULT_BUCK = 34,
    PM_HW_MODULE_LAB = 36,
    PM_HW_MODULE_INVALID
}pm_hw_module_type;

typedef enum
{
    PM_HW_MODULE_WLED_Reserved = 0,
    PM_HW_MODULE_WLED_MAIN = 1,
    PM_HW_MODULE_WLED_SINK = 8,
    PM_HW_MODULE_WLED_INVALID
}pm_hw_module_wled_subtype;

typedef enum
{
    PM_HW_MODULE_LPG_Reserved = 0,
    PM_HW_MODULE_LPG_LUT  = 1,
    PM_HW_MODULE_LPG_CHAN = 2,
    PM_HW_MODULE_LPG_PWM = 11,
    PM_HW_MODULE_LPG_INVALID,
}pm_hw_module_lpg_subtype;

typedef enum
{
    PM_HW_MODULE_PON_Reserved = 0,
    PM_HW_MODULE_PON_LV_PON = 1,
    PM_HW_MODULE_PON_INVALID
}pm_hw_module_pon_subtype;

typedef enum
{
    PM_HW_MODULE_CHARGER_Reserved = 0,
    PM_HW_MODULE_CHARGER_BSI = 17,
    PM_HW_MODULE_CHARGER_LBC_CHG = 21,
    PM_HW_MODULE_CHARGER_LBC_BIF = 22,
    PM_HW_MODULE_CHARGER_LBC_USB = 23,
    PM_HW_MODULE_CHARGER_LBC_MISC = 24,
    PM_HW_MODULE_CHARGER_COINCELL = 32,
    PM_HW_MODULE_CHARGER_SMBC_CHG = 49,
    PM_HW_MODULE_CHARGER_SMBC_BUCK = 50,
    PM_HW_MODULE_CHARGER_SMBC_BIF = 51,
    PM_HW_MODULE_CHARGER_SMBC_USB = 52,
    PM_HW_MODULE_CHARGER_SMBC_BOOST = 54,
    PM_HW_MODULE_CHARGER_SMBC_MISC = 55,
    PM_HW_MODULE_CHARGER_INVALID
}pm_hw_module_charger_subtype;

typedef enum
{
    PM_HW_MODULE_HF_BUCK_Reserved = 0,
    PM_HW_MODULE_HF_BUCK_PS1X = 1,
    PM_HW_MODULE_HF_BUCK_PS2X = 2,
    PM_HW_MODULE_HF_BUCK_PS3X = 3,
    PM_HW_MODULE_HF_BUCK_PS6X = 6,
    PM_HW_MODULE_HF_BUCK_GP_CTL = 8,
    PM_HW_MODULE_HF_BUCK_RF_CTL = 9,
    PM_HW_MODULE_HF_BUCK_INVALID
}pm_hw_module_hf_buck_subtype;

typedef enum
{
    PM_HW_MODULE_ULT_BUCK_Reserved = 0,
    PM_HW_MODULE_ULT_BUCK_CTL_LV = 13,
    PM_HW_MODULE_ULT_BUCK_CTL2 = 14,
    PM_HW_MODULE_ULT_BUCK_CTL3 = 15,
    PM_HW_MODULE_ULT_BUCK_CTL_MV = 16,
    PM_HW_MODULE_ULT_BUCK_INVALID
}pm_hw_module_ult_buck_subtype;

typedef enum
{
    PM_HW_MODULE_LDO_Reserved = 0,
    PM_HW_MODULE_LDO_N50 = 1,
    PM_HW_MODULE_LDO_N150 = 2,
    PM_HW_MODULE_LDO_N300 = 3,
    PM_HW_MODULE_LDO_N600 = 4,
    PM_HW_MODULE_LDO_N1200 = 5,
    PM_HW_MODULE_LDO_N600_STEPPER = 6,
    PM_HW_MODULE_LDO_N1200_STEPPER = 7,
    PM_HW_MODULE_LDO_P50 = 8,
    PM_HW_MODULE_LDO_P150 = 9,
    PM_HW_MODULE_LDO_P300 = 10,
    PM_HW_MODULE_LDO_P600 = 11,
    PM_HW_MODULE_LDO_P1200 = 12,
    PM_HW_MODULE_LDO_LN_LDO = 16,
    PM_HW_MODULE_LDO_USB_LDO = 24,
    PM_HW_MODULE_LDO_LV_P50 = 40,
    PM_HW_MODULE_LDO_LV_P150 = 41,
    PM_HW_MODULE_LDO_LV_P300 = 42,
    PM_HW_MODULE_LDO_LV_P600 = 43,
    PM_HW_MODULE_LDO_LV_P1200 = 44,
    PM_HW_MODULE_LDO_INVALID,
}pm_hw_module_ldo_subtype;

typedef enum
{
    PM_HW_MODULE_ULT_LDO_Reserved = 0,
    PM_HW_MODULE_ULT_LDO_N50 = 1,
    PM_HW_MODULE_ULT_LDO_N150 = 2,
    PM_HW_MODULE_ULT_LDO_N300 = 3,
    PM_HW_MODULE_ULT_LDO_N600 = 4,
    PM_HW_MODULE_ULT_LDO_N1200 = 5,
    PM_HW_MODULE_ULT_LDO_N600_STEPPER = 6,
    PM_HW_MODULE_ULT_LDO_N1200_STEPPER = 7,
    PM_HW_MODULE_ULT_LDO_P50 = 8,
    PM_HW_MODULE_ULT_LDO_P150 = 9,
    PM_HW_MODULE_ULT_LDO_P300 = 10,
    PM_HW_MODULE_ULT_LDO_P600 = 11,
    PM_HW_MODULE_ULT_LDO_P1200 = 12,
    PM_HW_MODULE_ULT_LDO_N900 = 19,
    PM_HW_MODULE_ULT_LDO_N900_STEPPER = 20,
    PM_HW_MODULE_ULT_LDO_N300_STEPPER = 21,
    PM_HW_MODULE_ULT_LDO_LV_P50 = 40,
    PM_HW_MODULE_ULT_LDO_LV_P150 = 41,
    PM_HW_MODULE_ULT_LDO_LV_P300 = 42,
    PM_HW_MODULE_ULT_LDO_LV_P600 = 43,
    PM_HW_MODULE_ULT_LDO_LV_P1200 = 44,
    PM_HW_MODULE_ULT_LDO_P450 = 45,
    PM_HW_MODULE_ULT_LDO_INVALID,
}pm_hw_module_ult_ldo_subtype;

typedef enum
{
    PM_HW_MODULE_VS_Reserved = 0,
    PM_HW_MODULE_VS_LV100 = 1,
    PM_HW_MODULE_VS_LV300 = 2,
    PM_HW_MODULE_VS_MV300 = 8,
    PM_HW_MODULE_VS_MV500 = 9,
    PM_HW_MODULE_VS_HDMI = 16,
    PM_HW_MODULE_VS_OTG = 17,
    PM_HW_MODULE_VS_INVALID,
}pm_hw_module_vs_subtype;

typedef enum
{
    PM_HW_MODULE_CLOCK_Reserved = 0,
    PM_HW_MODULE_CLOCK_XO_CORE = 1,
    PM_HW_MODULE_CLOCK_CLK_DIST = 2,
    PM_HW_MODULE_CLOCK_BB_CLK = 8,
    PM_HW_MODULE_CLOCK_RF_CLK = 9,
    PM_HW_MODULE_CLOCK_DIFF_CLK = 10,
    PM_HW_MODULE_CLOCK_DIV_CLK = 11,
    PM_HW_MODULE_CLOCK_SLP_CLK = 12,
    PM_HW_MODULE_CLOCK_INVALID,
}pm_hw_module_clk_subtype;

typedef enum
{
    PM_HW_MODULE_RTC_Reserved = 0,
    PM_HW_MODULE_RTC_RTC_RW = 1,
    PM_HW_MODULE_RTC_RTC_RO = 2,
    PM_HW_MODULE_RTC_RTC_ALARM = 3,
    PM_HW_MODULE_RTC_TIMER = 8,
    PM_HW_MODULE_RTC_INVALID,
}pm_hw_module_rtc_subtype;

typedef enum
{
    PM_HW_MODULE_ADC_Reserved = 0,
    PM_HW_MODULE_ADC_VADC1 = 1,
    PM_HW_MODULE_ADC_VADC2 = 2,
    PM_HW_MODULE_ADC_CCADC = 3,
    PM_HW_MODULE_ADC_INVALID,
}pm_hw_module_adc_subtype;

typedef enum
{
    PM_HW_MODULE_ALARM_Reserved = 0,
    PM_HW_MODULE_ALARM_BATT_ALARM = 1,
    PM_HW_MODULE_ALARM_TEMP_ALARM = 8,
    PM_HW_MODULE_ALARM_INVALID,
}pm_hw_module_alarm_subtype;

typedef enum
{
    PM_HW_MODULE_GPIO_Reserved = 0,
    PM_HW_MODULE_GPIO_GPIO = 1,
    PM_HW_MODULE_GPIO_GPIOC = 2,
    PM_HW_MODULE_GPIO_INVALID,
}pm_hw_module_gpio_subtype;

typedef enum
{
    PM_HW_MODULE_MISC_Reserved = 0,
    PM_HW_MODULE_MISC_MISC8941 = 1,
    PM_HW_MODULE_MISC_MISC8841 = 2,
    PM_HW_MODULE_MISC_INVALID,
}pm_hw_module_misc_subtype;

typedef enum
{
    PM_HW_MODULE_FTS__Reserved = 0,
    PM_HW_MODULE_FTS__PS1X = 1,
    PM_HW_MODULE_FTS__PS0p5X = 2,
    PM_HW_MODULE_FTS__FTS2p5_PS = 3,
    PM_HW_MODULE_FTS__FTS_CTL = 8,
    PM_HW_MODULE_FTS__FTS2p5_CTL = 9,
    PM_HW_MODULE_FTS__INVALID,
}pm_hw_module_fts_subtype;

typedef enum
{
    PM_HW_MODULE_BUCK_CMN_Reserved = 0,
    PM_HW_MODULE_BUCK_CMN_BUCK_CMN1 = 1,
    PM_HW_MODULE_BUCK_CMN_BUCK_CMN2 = 2,
    PM_HW_MODULE_BUCK_CMN_INVALID,
}pm_hw_module_buck_cmn_subtype;

typedef enum
{
    PM_HW_MODULE_PBS_Reserved = 0,
    PM_HW_MODULE_PBS_CORE = 1,
    PM_HW_MODULE_PBS_CLIENT = 8,
    PM_HW_MODULE_PBS_INVALID,
}pm_hw_module_pbs_subtype;

#endif //HW_MODULE_TYPE_H
