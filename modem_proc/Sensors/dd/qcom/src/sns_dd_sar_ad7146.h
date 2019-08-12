/*============================================================================
 @file sns_dd_sar_ad7146.h

 @brief
 Header file for AD7146 capacitive touch sensor driver.

 DEPENDENCIES: None.

 Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential
 ============================================================================*/

/*============================================================================
 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order. Please
 use ISO format for dates.

 when        who  what, where, why
 ----------  ---  -----------------------------------------------------------
 2014-08-26  MW   Updated low and high CDC limits
 2014-02-14  MW   Added self test, timer for polling mode
 2014-02-10  TC   Added support for dynamic stage configuration through the registry
 2014-02-04  MW   Initial indication fix CR 611348
 2013-09-16  MW   Initial version
 ============================================================================*/

#ifndef __SNS_DD_SAR_AD7146_H
#define __SNS_DD_SAR_AD7146_H

// enable this to see driver debug messages
//#define AD7146_DEBUG

/**
 * Application Notes Regarding SAR sensors within the QCOM 8084 LIQUID
 *
 * Here are some good default values:
 * # SAR Registry
 * #  Item ID's corresponding to group SNS_REG_DRIVER_GROUP_SAR (Group Id: 1090)
 * 4800 3 0x00010001  #SNS_REG_DRIVER_SAR_x_NUM_ELECTRODES (number_of_stages)
 * 4801 0x0700 0x00010001  #SNS_REG_DRIVER_SAR_x_E0_BIAS (STAGE0_AFE_offset)
 * 4802 0x2700 0x00010001  #SNS_REG_DRIVER_SAR_x_E0_THRESHOLD (STAGE0_offset_high)
 * 4803 0x0700 0x00010001  #SNS_REG_DRIVER_SAR_x_E1_BIAS (STAGE1_AFE_offset)
 * 4804 0x2700 0x00010001  #SNS_REG_DRIVER_SAR_x_E1_THRESHOLD (STAGE1_offset_high)
 * 4805 0x0600 0x00010001  #SNS_REG_DRIVER_SAR_x_E2_BIAS (STAGE2_AFE_offset)
 * 4806 0x2700 0x00010001  #SNS_REG_DRIVER_SAR_x_E2_THRESHOLD (STAGE2_offset_high)
 *
 * 4825 0x2700 0x00010001  #STAGE0_offset_low
 * 4826 0x337B 0x00010001  #STAGE0_connection_6_0
 * 4827 0x3CCC 0x00010001  #STAGE0_connection_12_7
 *
 * 4828 0x2700 0x00010001  #STAGE1_offset_low
 * 4829 0x3333 0x00010001  #STAGE1_connection_6_0
 * 4830 0x3DEC 0x00010001  #STAGE1_connection_12_7
 *
 * 4831 0x2700 0x00010001  #STAGE2_offset_low
 * 4832 0x3B33 0x00010001  #STAGE2_connection_6_0
 * 4833 0x3CCD 0x00010001  #STAGE2_connection_12_7
 *
 * 4861 0x3230 0x00010001  #comp_control_0
 * 4862 0x0419 0x00010001  #comp_control_1
 * 4863 0x0832 0x00010001  #comp_control_2
 *
 * # SAR 2 Registry
 * #  Item ID's corresponding to group SNS_REG_DRIVER_GROUP_SAR_2 (Group Id: 1091)
 * 4900 2 0x00010001  #SNS_REG_DRIVER_SAR_x_NUM_ELECTRODES (number_of_stages)
 * 4901 0x3600 0x00010001  #SNS_REG_DRIVER_SAR_x_E0_BIAS (STAGE0_AFE_offset)
 * 4902 0x0000 0x00010001  #SNS_REG_DRIVER_SAR_x_E0_THRESHOLD (STAGE0_offset_high)
 * 4903 0x3E00 0x00010001  #SNS_REG_DRIVER_SAR_x_E1_BIAS (STAGE1_AFE_offset)
 * 4904 0x0040 0x00010001  #SNS_REG_DRIVER_SAR_x_E1_THRESHOLD (STAGE1_offset_high)
 *
 * 4925 0x0000 0x00010001  #STAGE0_offset_low
 * 4926 0x3FFB 0x00010001  #STAGE0_connection_6_0
 * 4927 0x2FCF 0x00010001  #STAGE0_connection_12_7
 *
 * 4928 0x0100 0x00010001  #STAGE1_offset_low
 * 4929 0x3FF3 0x00010001  #STAGE1_connection_6_0
 * 4930 0x2FEF 0x00010001  #STAGE1_connection_12_7
 *
 * 4961 0x3230 0x00010001  #comp_control_0
 * 4962 0x0419 0x00010001  #comp_control_1
 * 4963 0x0832 0x00010001  #comp_control_2
 *
 * #define AFE_STAGE_0_DEV_0_VALUE  0x3600
 * #define AFE_STAGE_1_DEV_0_VALUE 0x3E00
 *
 * #define AFE_STAGE_0_DEV_1_VALUE 0x0700
 * #define AFE_STAGE_1_DEV_1_VALUE 0x0700
 * #define AFE_STAGE_2_DEV_1_VALUE 0x0600
 *
 * enum
 * {
 *   AD7146_I2C_SLAVE_ADDRESS_1      = 0x2C,  // For AD7146 on left bottom corner has ADD0 and ADD1 ground
 *   AD7146_I2C_SLAVE_ADDRESS_2      = 0x2F,  // For AD7146 on right top corner has ADD0 and ADD1 VCC
 * };
 */

#define SAR_AD7146_ODR              167
#define SAR_AD7146_ADC_BIT_LENGTH   12
#define SAR_AD7146_MAX_FREQ         167  // 2 conversion stages and 256 decimation factor = 6 ms Update time
#define SAR_AD7146_ACTIVE_CURRENT   1200 // 1.2 mA
#define SAR_AD7146_LOW_CURRENT      3

#define SAR_AD7146_MAX_NUM_OF_STAGES  12
#define SAR_AD7146_NUM_OF_AXES        3
#define SAR_AD7146_BYTES_PER_REG      2
#define SAR_AD7146_NUM_STATUS_REG     3
#define SAR_AD7146_NUM_BANK1_REG      22

#define NUM_CONVERSION_CYCLE_TO_SKIP  4

#define SAR_AD7146_20_MS              20000

/* Register definitions */

/******************************* NOTE *****************************************/
/*
 1. Register addresses are 10-bit
 2. All register are 16-bit regsiters
 */
/*****************************************************************************/
enum
{
  AD7146_PWR_CONTROL_REG = 0x000,
  AD7146_STAGE_CAL_EN_REG,
  AD7146_AMB_COMP_CTRL0_REG,
  AD7146_AMB_COMP_CTRL1_REG,
  AD7146_AMB_COMP_CTRL2_REG,
  AD7146_STAGE_LOW_INT_ENABLE_REG,
  AD7146_STAGE_HIGH_INT_ENABLE_REG,
  AD7146_STAGE_COMPLETE_INT_ENABLE_REG,
  AD7146_STAGE_LOW_INT_STATUS_REG,
  AD7146_STAGE_HIGH_INT_STATUS_REG,
  AD7146_STAGE_COMPLETE_INT_STATUS_REG
};

enum
{
  AD7146_DEVICE_ID_REG = 0x017
};

#define AD7146_CDC_RESULT_Sx_REG_BASE              0x0B

/* CDC 16-Bit Conversion Data Registers - 0x00B through 0x016 */
enum
{
  AD7146_CDC_RESULT_S0_REG = 0x00B,
  AD7146_CDC_RESULT_S1_REG,
  AD7146_CDC_RESULT_S2_REG,
  AD7146_CDC_RESULT_S3_REG,
  AD7146_CDC_RESULT_S4_REG,
  AD7146_CDC_RESULT_S5_REG,
  AD7146_CDC_RESULT_S6_REG,
  AD7146_CDC_RESULT_S7_REG,
  AD7146_CDC_RESULT_S8_REG,
  AD7146_CDC_RESULT_S9_REG,
  AD7146_CDC_RESULT_S10_REG,
  AD7146_CDC_RESULT_S11_REG
};

#define AD7146_STAGEx_CONNECTION_6_0_REG_BASE      0x80
#define AD7146_STAGEx_CONNECTION_12_7_REG_BASE     0x81
#define AD7146_STAGEx_AFE_OFFSET_REG_BASE          0x82
#define AD7146_STAGEx_SENSITIVITY_REG_BASE         0x83
#define AD7146_STAGEx_OFFSET_LOW_REG_BASE          0x84
#define AD7146_STAGEx_OFFSET_HIGH_REG_BASE         0x85
#define AD7146_STAGEx_OFFSET_HIGH_CLAMP_REG_BASE   0x86
#define AD7146_STAGEx_OFFSET_LOW_CLAMP_REG_BASE    0x87

#define AD7146_STAGE_INCREMENT                     0x08

/* STAGE0 Configuration Registers - 0x080 through 0x087 */
enum
{
  AD7146_STAGE0_CONNECTION_6_0_REG = 0x080,
  AD7146_STAGE0_CONNECTION_12_7_REG,
  AD7146_STAGE0_AFE_OFFSET_REG,
  AD7146_STAGE0_SENSITIVITY_REG,
  AD7146_STAGE0_OFFSET_LOW_REG,
  AD7146_STAGE0_OFFSET_HIGH_REG,
  AD7146_STAGE0_OFFSET_HIGH_CLAMP_REG,
  AD7146_STAGE0_OFFSET_LOW_CLAMP_REG
};

/* STAGE1 Configuration Registers - 0x088 through 0x08F */
enum
{
  AD7146_STAGE1_CONNECTION_6_0_REG = 0x088,
  AD7146_STAGE1_CONNECTION_12_7_REG,
  AD7146_STAGE1_AFE_OFFSET_REG,
  AD7146_STAGE1_SENSITIVITY_REG,
  AD7146_STAGE1_OFFSET_LOW_REG,
  AD7146_STAGE1_OFFSET_HIGH_REG,
  AD7146_STAGE1_OFFSET_HIGH_CLAMP_REG,
  AD7146_STAGE1_OFFSET_LOW_CLAMP_REG
};

/* STAGE2 Configuration Registers - 0x090 through 0x097 */
enum
{
  AD7146_STAGE2_CONNECTION_6_0_REG = 0x090,
  AD7146_STAGE2_CONNECTION_12_7_REG,
  AD7146_STAGE2_AFE_OFFSET_REG,
  AD7146_STAGE2_SENSITIVITY_REG,
  AD7146_STAGE2_OFFSET_LOW_REG,
  AD7146_STAGE2_OFFSET_HIGH_REG,
  AD7146_STAGE2_OFFSET_HIGH_CLAMP_REG,
  AD7146_STAGE2_OFFSET_LOW_CLAMP_REG
};

#define AD7146_MID_SCALE_LOW_COUNT       9000
#define AD7146_MID_SCALE_HIGH_COUNT      13000
#define AD7146_MAX_AFE_VALUE             0x3F00
#define AD7146_AFE_INCREMENT             0x0100

/* AD7146_PWR_CONTROL_REG register masks */
enum
{
  PWR_CONTROL_FULL_POWER = 0x00,
  PWR_CONTROL_FULL_SHUTDOWN = 0x01,
  PWR_CONTROL_LOW_POWER = 0x10,
};

/**
 * Sensitivity does not fit in the registry when supporting up to 12 stages.
 * Since we do not usually alter the sensitivity and we have not had any
 * requests to do so, we simply set the sensitivity to a pre-defined value for
 * all stages.
 */
#define AD7146_STAGEx_SENSITIVITY_VALUE    0x2626

/**
 * Default power control register (AD7146_PWR_CONTROL_REG) value
 * CDC bias + 50%, enable excitation to CINx, interrupt active low,
 * 256 decimation rate, 200ms low power mode conv delay, full shutdown,
 * no specified number of conversion stages.
 *
 * Note: Number of conversion stages [7:4] need to be configured before using
 * this value!!
 */
#define AD7146_PWR_CONTROL_VALUE_DEFAULT   0xC003

// The amount of time it takes for each stage conversion (in microseconds)
#define AD7146_STAGE_CONVERSION_DELAY_US   4000

/**
 * SAR Registry Translation
 *
 * This comment describes how each field within the SAR registry is translated
 * and used in this device driver.
 *
 * uint32_t number_of_stages;        // SNS_REG_DRIVER_SAR_x_NUM_ELECTRODES
 * uint32_t STAGE0_AFE_offset;       // SNS_REG_DRIVER_SAR_x_E0_BIAS
 * uint32_t STAGE0_offset_high;      // SNS_REG_DRIVER_SAR_x_E0_THRESHOLD
 * uint32_t STAGE1_AFE_offset;       // SNS_REG_DRIVER_SAR_x_E1_BIAS
 * uint32_t STAGE1_offset_high;      // SNS_REG_DRIVER_SAR_x_E1_THRESHOLD
 * uint32_t STAGE2_AFE_offset;       // SNS_REG_DRIVER_SAR_x_E2_BIAS
 * uint32_t STAGE2_offset_high;      // SNS_REG_DRIVER_SAR_x_E2_THRESHOLD
 * uint32_t STAGE3_AFE_offset;       // SNS_REG_DRIVER_SAR_x_E3_BIAS
 * uint32_t STAGE3_offset_high;      // SNS_REG_DRIVER_SAR_x_E3_THRESHOLD
 * uint32_t STAGE4_AFE_offset;       // SNS_REG_DRIVER_SAR_x_E4_BIAS
 * uint32_t STAGE4_offset_high;      // SNS_REG_DRIVER_SAR_x_E4_THRESHOLD
 * uint32_t STAGE5_AFE_offset;       // SNS_REG_DRIVER_SAR_x_E5_BIAS
 * uint32_t STAGE5_offset_high;      // SNS_REG_DRIVER_SAR_x_E5_THRESHOLD
 * uint32_t STAGE6_AFE_offset;       // SNS_REG_DRIVER_SAR_x_E6_BIAS
 * uint32_t STAGE6_offset_high;      // SNS_REG_DRIVER_SAR_x_E6_THRESHOLD
 * uint32_t STAGE7_AFE_offset;       // SNS_REG_DRIVER_SAR_x_E7_BIAS
 * uint32_t STAGE7_offset_high;      // SNS_REG_DRIVER_SAR_x_E7_THRESHOLD
 * uint32_t STAGE8_AFE_offset;       // SNS_REG_DRIVER_SAR_x_E8_BIAS
 * uint32_t STAGE8_offset_high;      // SNS_REG_DRIVER_SAR_x_E8_THRESHOLD
 * uint32_t STAGE9_AFE_offset;       // SNS_REG_DRIVER_SAR_x_E9_BIAS
 * uint32_t STAGE9_offset_high;      // SNS_REG_DRIVER_SAR_x_E9_THRESHOLD
 * uint32_t STAGE10_AFE_offset;      // SNS_REG_DRIVER_SAR_x_E10_BIAS
 * uint32_t STAGE10_offset_high;     // SNS_REG_DRIVER_SAR_x_E10_THRESHOLD
 * uint32_t STAGE11_AFE_offset;      // SNS_REG_DRIVER_SAR_x_E11_BIAS
 * uint32_t STAGE11_offset_high;     // SNS_REG_DRIVER_SAR_x_E11_THRESHOLD
 *
 * uint32_t STAGE0_offset_low;       // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_01
 * uint32_t STAGE0_connection_6_0;   // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_02
 * uint32_t STAGE0_connection_12_7;  // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_03
 *
 * uint32_t STAGE1_offset_low;       // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_04
 * uint32_t STAGE1_connection_6_0;   // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_05
 * uint32_t STAGE1_connection_12_7;  // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_06
 *
 * uint32_t STAGE2_offset_low;       // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_07
 * uint32_t STAGE2_connection_6_0;   // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_08
 * uint32_t STAGE2_connection_12_7;  // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_09
 *
 * uint32_t STAGE3_offset_low;       // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_10
 * uint32_t STAGE3_connection_6_0;   // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_11
 * uint32_t STAGE3_connection_12_7;  // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_12
 *
 * uint32_t STAGE4_offset_low;       // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_13
 * uint32_t STAGE4_connection_6_0;   // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_14
 * uint32_t STAGE4_connection_12_7;  // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_15
 *
 * uint32_t STAGE5_offset_low;       // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_16
 * uint32_t STAGE5_connection_6_0;   // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_17
 * uint32_t STAGE5_connection_12_7;  // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_18
 *
 * uint32_t STAGE6_offset_low;       // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_19
 * uint32_t STAGE6_connection_6_0;   // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_20
 * uint32_t STAGE6_connection_12_7;  // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_21
 *
 * uint32_t STAGE7_offset_low;       // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_22
 * uint32_t STAGE7_connection_6_0;   // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_23
 * uint32_t STAGE7_connection_12_7;  // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_24
 *
 * uint32_t STAGE8_offset_low;       // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_25
 * uint32_t STAGE8_connection_6_0;   // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_26
 * uint32_t STAGE8_connection_12_7;  // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_27
 *
 * uint32_t STAGE9_offset_low;       // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_28
 * uint32_t STAGE9_connection_6_0;   // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_29
 * uint32_t STAGE9_connection_12_7;  // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_30
 *
 * uint32_t STAGE10_offset_low;      // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_31
 * uint32_t STAGE10_connection_6_0;  // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_32
 * uint32_t STAGE10_connection_12_7; // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_33
 *
 * uint32_t STAGE11_offset_low;      // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_34
 * uint32_t STAGE11_connection_6_0;  // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_35
 * uint32_t STAGE11_connection_12_7; // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_36
 *
 * uint32_t comp_control_0;          // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_37 (Recommended Value: 0x3230)
 * uint32_t comp_control_1;          // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_38 (Recommended Value: 0x0419)
 * uint32_t comp_control_2;          // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_39 (Recommended Value: 0x0832)
 */


/**
 * This structure represents the pre-defined portions of the registry. Namely,
 * the BIAS and the THRESHOLD. These two field are packed together by stage.
 */
typedef PACK(struct)
{
  uint32_t afe_offset;       // SNS_REG_DRIVER_SAR_x_Ey_BIAS
  uint32_t offset_high;      // SNS_REG_DRIVER_SAR_x_Ey_THRESHOLD
} sns_dd_ad7146_stage_config_main_s;

/**
 * This structure represents the portions of the "DRIVER_SPECIFIC" registry
 * that are packed together by stage.
 */
typedef PACK(struct)
{
  uint32_t offset_low;       // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_z
  uint32_t connection_6_0;   // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_z
  uint32_t connection_12_7;  // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_z
} sns_dd_ad7146_stage_config_aux_s;

/**
 * This structure is the AD7146's translation of the SAR registry items. Each
 * item is treated as a 4-byte integer and the entire structure is packed. This
 * allows us to treat the entire structure as a 64-element array of 4-byte ints
 */
typedef PACK(struct)
{
  uint32_t number_of_stages;        // SNS_REG_DRIVER_SAR_x_NUM_ELECTRODES

  sns_dd_ad7146_stage_config_main_s stage_config_main_array[12];

  sns_dd_ad7146_stage_config_aux_s  stage_config_aux_array[12];

  uint32_t comp_control_0;          // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_37 (Recommended Value: 0x3230)
  uint32_t comp_control_1;          // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_38 (Recommended Value: 0x0419)
  uint32_t comp_control_2;          // SNS_REG_DRIVER_SAR_x_DRIVER_SPECIFIC_39 (Recommended Value: 0x0832)
} sns_dd_ad7146_config_s;

typedef struct
{

  sns_ddf_handle_t smgr_handle; /* notify_data handle */
  sns_ddf_handle_t port_handle; /* I2C bus handle */
  uint32_t odr;
  uint16_t interrupt_gpio;
  uint8_t first_indication_count;
  sns_ddf_powerstate_e powerstate;
  bool dri_enable;
  bool is_data_requested;
  bool first_indication_sent;
  bool run_calibration;
  sns_ddf_sensor_data_s* s_data;
  sns_dd_ad7146_config_s stage_config; /* contains general electrode config */
  sns_ddf_port_id_s port_info; /* used in log packets to identify
   between multiple SAR devices */
  sns_ddf_timer_s sar_timer;
  sns_ddf_time_t timestamp;
} sns_dd_ad7146_state_t;

static sns_ddf_sensor_e sns_dd_ad7146_sensor_types[] =
{ SNS_DDF_SENSOR_SAR, SNS_DDF_SENSOR__NONE };

#ifdef AD7146_DEBUG
#if defined QDSP6
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#define AD7146_MSG_0(level,msg)          MSG(MSG_SSID_SNS,DBG_##level##_PRIO, "AD7146 - " msg)
#define AD7146_MSG_1(level,msg,p1)       MSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, "AD7146 - " msg,p1)
#define AD7146_MSG_2(level,msg,p1,p2)    MSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, "AD7146 - " msg,p1,p2)
#define AD7146_MSG_3(level,msg,p1,p2,p3) MSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, "AD7146 - " msg,p1,p2,p3)
#else
#define MED MEDIUM
#include "sns_debug_str.h"
#define AD7146_MSG_0(level,msg)          SNS_PRINTF_STRING_ID_##level##_0(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
#define AD7146_MSG_1(level,msg,p1)       SNS_PRINTF_STRING_ID_##level##_1(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,p1)
#define AD7146_MSG_2(level,msg,p1,p2)    SNS_PRINTF_STRING_ID_##level##_2(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,p1,p2)
#define AD7146_MSG_3(level,msg,p1,p2,p3) SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,p1,p2,p3)
#endif
#else /* AD7146_DEBUG */
#define AD7146_MSG_0(level,msg)
#define AD7146_MSG_1(level,msg,p1)
#define AD7146_MSG_2(level,msg,p1,p2)
#define AD7146_MSG_3(level,msg,p1,p2,p3)
#endif /* AD7146_DEBUG */

#endif /* __SNS_DD_SAR_AD7146_H */
