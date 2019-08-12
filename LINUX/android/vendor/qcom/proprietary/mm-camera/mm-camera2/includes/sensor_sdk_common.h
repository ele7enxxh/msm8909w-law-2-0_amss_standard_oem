/*==========================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================*/
#ifndef __SENSOR_SDK_COMMON_H__
#define __SENSOR_SDK_COMMON_H__

#define MAX_NAME_SIZE           32
#define MAX_I2C_REG_SET         12
#define MAX_POWER_CONFIG        12
#define U_I2C_SEQ_REG_DATA_MAX  256

#define GPIO_OUT_LOW            (0 << 1)
#define GPIO_OUT_HIGH           (1 << 1)
#define MAX_PDAF_WIN        200

enum camera_i2c_reg_addr_type {
  CAMERA_I2C_BYTE_ADDR = 1,
  CAMERA_I2C_WORD_ADDR,
  CAMERA_I2C_3B_ADDR,
  CAMERA_I2C_ADDR_TYPE_MAX,
};

enum camera_i2c_data_type {
  CAMERA_I2C_BYTE_DATA = 1,
  CAMERA_I2C_WORD_DATA,
  CAMERA_I2C_DWORD_DATA,
  CAMERA_I2C_SET_BYTE_MASK,
  CAMERA_I2C_UNSET_BYTE_MASK,
  CAMERA_I2C_SET_WORD_MASK,
  CAMERA_I2C_UNSET_WORD_MASK,
  CAMERA_I2C_SET_BYTE_WRITE_MASK_DATA,
  CAMERA_I2C_DATA_TYPE_MAX,
};

enum camera_power_seq_type {
  CAMERA_POW_SEQ_CLK,
  CAMERA_POW_SEQ_GPIO,
  CAMERA_POW_SEQ_VREG,
  CAMERA_POW_SEQ_I2C_MUX,
  CAMERA_POW_SEQ_I2C,
};

enum camera_gpio_type {
  CAMERA_GPIO_RESET,
  CAMERA_GPIO_STANDBY,
  CAMERA_GPIO_AF_PWDM,
  CAMERA_GPIO_VIO,
  CAMERA_GPIO_VANA,
  CAMERA_GPIO_VDIG,
  CAMERA_GPIO_VAF,
  CAMERA_GPIO_FL_EN,
  CAMERA_GPIO_FL_NOW,
  CAMERA_GPIO_FL_RESET,
  CAMERA_GPIO_CUSTOM1,
  CAMERA_GPIO_CUSTOM2,
  CAMERA_GPIO_MAX,
};

enum camera_clk_type {
  CAMERA_MCLK,
  CAMERA_CLK,
  CAMERA_CLK_MAX,
};

enum camera_vreg_name {
  CAMERA_VDIG,
  CAMERA_VIO,
  CAMERA_VANA,
  CAMERA_VAF,
  CAMERA_V_CUSTOM1,
  CAMERA_V_CUSTOM2,
  CAMERA_VREG_MAX,
};

struct camera_i2c_reg_array {
  unsigned short reg_addr;
  unsigned short reg_data;
  unsigned int delay;
};

struct camera_i2c_reg_setting_array {
  struct camera_i2c_reg_array reg_setting_a[MAX_I2C_REG_SET];
  unsigned short size;
  enum camera_i2c_reg_addr_type addr_type;
  enum camera_i2c_data_type data_type;
  unsigned short delay;
};

struct camera_power_setting {
  enum camera_power_seq_type seq_type;
  unsigned short seq_val;
  long config_val;
  unsigned short delay;
  void *data[10];
};

struct camera_power_setting_array {
  struct camera_power_setting  power_setting_a[MAX_POWER_CONFIG];
  struct camera_power_setting *power_setting;
  unsigned short size;
  struct camera_power_setting  power_down_setting_a[MAX_POWER_CONFIG];
  struct camera_power_setting *power_down_setting;
  unsigned short size_down;
};

struct camera_i2c_seq_reg_array {
  unsigned short reg_addr;
  unsigned char reg_data[U_I2C_SEQ_REG_DATA_MAX];
  unsigned short reg_data_size;
};

#endif /* __SENSOR_SDK_COMMON_H__ */

