/*============================================================================
 Copyright (c) 2016 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc
============================================================================*/
#include <stdio.h>
#include "sensor_lib.h"

#define SENSOR_MODEL_NO_OV2685 "ov2685"
#define OV2685_LOAD_CHROMATIX(n) \
  "libchromatix_"SENSOR_MODEL_NO_OV2685"_"#n".so"

#undef DEBUG_INFO
/* #define OV2685_DEBUG  */
#ifdef OV2685_DEBUG
#include <utils/Log.h>
#define SERR(fmt, args...) \
    ALOGE("%s:%d "fmt"\n", __func__, __LINE__, ##args)
#define DEBUG_INFO(fmt, args...) SERR(fmt, ##args)
#else
#define DEBUG_INFO(fmt, args...) do { } while (0)
#endif

#define MIN_GAIN 1
#define MAX_ANALOG_GAIN (15.5)
#define MAX_DIGITAL_GAIN (1)
#define MAX_GAIN (MAX_ANALOG_GAIN * MAX_DIGITAL_GAIN)
#define OV2685_MAX_DGAIN_DECIMATOR  2048

static sensor_lib_t sensor_lib_ptr;

static struct msm_sensor_power_setting power_setting[] = {
    {
      .seq_type = SENSOR_VREG,
      .seq_val = CAM_VIO,
      .config_val = 0,
      .delay = 5,
    },
    {
      .seq_type = SENSOR_VREG,
      .seq_val = CAM_VANA,
      .config_val = 0,
      .delay = 0,
    },
    {
      .seq_type = SENSOR_VREG,
      .seq_val = CAM_VDIG,
      .config_val = 0,
      .delay = 5,
    },
    {
      .seq_type = SENSOR_GPIO,
      .seq_val = SENSOR_GPIO_RESET,
      .config_val = GPIO_OUT_LOW,
      .delay = 10,
    },
    {
      .seq_type = SENSOR_GPIO,
      .seq_val = SENSOR_GPIO_RESET,
      .config_val = GPIO_OUT_HIGH,
      .delay = 10,
    },
    {
      .seq_type = SENSOR_GPIO,
      .seq_val = SENSOR_GPIO_STANDBY,
      .config_val = GPIO_OUT_LOW,
      .delay = 0,
    },
    {
      .seq_type = SENSOR_GPIO,
      .seq_val = SENSOR_GPIO_STANDBY,
      .config_val = GPIO_OUT_HIGH,
      .delay = 5,
    },
    {
      .seq_type = SENSOR_CLK,
      .seq_val = SENSOR_CAM_MCLK,
      .config_val = 24000000,
      .delay = 10,
    },
    {
      .seq_type = SENSOR_I2C_MUX,
      .seq_val = 0,
      .config_val = 0,
      .delay = 0,
    },
};

static struct msm_camera_sensor_slave_info sensor_slave_info = {
  /* Camera slot where this camera is mounted */
  .camera_id = CAMERA_1,
  /* sensor slave address */
  .slave_addr = 0x78,
  /* sensor i2c frequency*/
  .i2c_freq_mode = I2C_STANDARD_MODE,
  /* sensor address type */
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  /* sensor id info*/
  .sensor_id_info = {
    /* sensor id register address */
    .sensor_id_reg_addr =0x300a ,
    /* sensor id */
    .sensor_id = 0x2685,
  },
  /* power up / down setting */
  .power_setting_array = {
    .power_setting = power_setting,
    .size = ARRAY_SIZE(power_setting),
  },
};

static struct msm_sensor_init_params sensor_init_params = {
  .modes_supported = 0,
  .position = 1,
  .sensor_mount_angle = 270,
};

static sensor_output_t sensor_output = {
  .output_format = SENSOR_YCBCR,
  .connection_mode = SENSOR_MIPI_CSI,
  .raw_output = SENSOR_8_BIT_DIRECT,
};

static struct msm_sensor_output_reg_addr_t output_reg_addr = {
  .x_output = 0x3808,
  .y_output = 0x380a,
  .line_length_pclk = 0x380c,
  .frame_length_lines = 0x380e,
};

static struct msm_sensor_exp_gain_info_t exp_gain_info = {
  .coarse_int_time_addr = 0x3501,
  .global_gain_addr = 0x350a,
  .vert_offset = 4,
};

static sensor_aec_data_t aec_info = {
  .max_gain = 15.5,
  .max_linecount = 0xFFFF,
};

#ifndef VFE_40
static struct csi_lane_params_t csi_lane_params = {
  .csi_lane_assign = 0x0004,
  .csi_lane_mask = 0x18,
  .csi_if = 1,
  .csid_core = {1},
  .csi_phy_sel = 0,
};
#else
static struct csi_lane_params_t csi_lane_params = {
  .csi_lane_assign = 0x4320,
  .csi_lane_mask = 0x3,
  .csi_if = 1,
  .csid_core = {0},
  .csi_phy_sel = 1,
};
#endif

static struct msm_camera_i2c_reg_array start_reg_array[] = {
  {0x0100, 0x01},
};

static  struct msm_camera_i2c_reg_setting start_settings = {
  .reg_setting = start_reg_array,
  .size = ARRAY_SIZE(start_reg_array),
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  .data_type = MSM_CAMERA_I2C_BYTE_DATA,
  .delay = 0,
  //.qup_i2c_batch = MSM_CAMERA_I2C_BATCH_ENABLE,
};

static struct msm_camera_i2c_reg_array stop_reg_array[] = {
  {0x0100, 0x00},
};

static struct msm_camera_i2c_reg_setting stop_settings = {
  .reg_setting = stop_reg_array,
  .size = ARRAY_SIZE(stop_reg_array),
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  .data_type = MSM_CAMERA_I2C_BYTE_DATA,
  .delay = 0,
  //.qup_i2c_batch = MSM_CAMERA_I2C_BATCH_ENABLE,
};

static struct msm_camera_i2c_reg_array groupon_reg_array[] = {
  {0x3208, 0x0},
};

static struct msm_camera_i2c_reg_setting groupon_settings = {
  .reg_setting = groupon_reg_array,
  .size = ARRAY_SIZE(groupon_reg_array),
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  .data_type = MSM_CAMERA_I2C_BYTE_DATA,
  .delay = 0,
  //.qup_i2c_batch = MSM_CAMERA_I2C_BATCH_ENABLE,
};

static struct msm_camera_i2c_reg_array groupoff_reg_array[] = {
  {0x3208, 0x10},
  {0x3208, 0xA0},
};

static struct msm_camera_i2c_reg_setting groupoff_settings = {
  .reg_setting = groupoff_reg_array,
  .size = ARRAY_SIZE(groupoff_reg_array),
  .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
  .data_type = MSM_CAMERA_I2C_BYTE_DATA,
  .delay = 0,
  //.qup_i2c_batch = MSM_CAMERA_I2C_BATCH_ENABLE,
};

static struct msm_camera_csid_vc_cfg ov2685_cid_cfg[] = {
  {0, CSI_YUV422_8, CSI_DECODE_8BIT},
};

static struct msm_camera_csi2_params ov2685_csi_params = {
  .csid_params = {
    .lane_cnt = 1,
    .lut_params = {
      .num_cid = ARRAY_SIZE(ov2685_cid_cfg),
      .vc_cfg = {
         &ov2685_cid_cfg[0],
      },
    },
  },
  .csiphy_params = {
    .lane_cnt = 1,
    .settle_cnt = 0x16,
#ifndef VFE_40
    .combo_mode = 1,
#endif
  },
};

static struct sensor_pix_fmt_info_t ov2685_pix_fmt0_fourcc[] = {
  { V4L2_MBUS_FMT_YUYV8_2X8 },
};

static sensor_stream_info_t ov2685_stream_info[] = {
  {1, &ov2685_cid_cfg[0], ov2685_pix_fmt0_fourcc},
};

static sensor_stream_info_array_t ov2685_stream_info_array = {
  .sensor_stream_info = ov2685_stream_info,
  .size = ARRAY_SIZE(ov2685_stream_info),
};

static struct msm_camera_i2c_reg_array res0_reg_array[] = {
  {0x0103, 0x01},
  {0x3002, 0x00},
  {0x3016, 0x1C},
  {0x3018, 0x84},
  {0x301D, 0xF0},
  {0x3020, 0x00},
  {0x3082, 0x2C},
  {0x3083, 0x03},
  {0x3084, 0x07},
  {0x3085, 0x03},
  {0x3086, 0x00},
  {0x3087, 0x00},
  {0x3501, 0x4E},
  {0x3502, 0xE0},
  {0x3503, 0x03},
  {0x350B, 0x36},
  {0x3600, 0xB4},
  {0x3603, 0x35},
  {0x3604, 0x24},
  {0x3605, 0x00},
  {0x3620, 0x24},
  {0x3621, 0x34},
  {0x3622, 0x03},
  {0x3628, 0x10},
  {0x3705, 0x3C},
  {0x370A, 0x21},
  {0x370C, 0x50},
  {0x370D, 0xC0},
  {0x3717, 0x58},
  {0x3718, 0x80},
  {0x3720, 0x00},
  {0x3721, 0x09},
  {0x3722, 0x06},
  {0x3723, 0x59},
  {0x3738, 0x99},
  {0x3781, 0x80},
  {0x3784, 0x0C},
  {0x3789, 0x60},
  {0x3800, 0x00},
  {0x3801, 0x00},
  {0x3802, 0x00},
  {0x3803, 0x00},
  {0x3804, 0x06},
  {0x3805, 0x4F},
  {0x3806, 0x04},
  {0x3807, 0xBF},
  {0x3808, 0x06},
  {0x3809, 0x40},
  {0x380A, 0x04},
  {0x380B, 0xB0},
  {0x380C, 0x06},
  {0x380D, 0xA4},
  {0x380E, 0x05},
  {0x380F, 0x0E},
  {0x3810, 0x00},
  {0x3811, 0x08},
  {0x3812, 0x00},
  {0x3813, 0x08},
  {0x3814, 0x11},
  {0x3815, 0x11},
  {0x3819, 0x04},
  {0x3820, 0xC0},
  {0x3821, 0x00},
  {0x3A06, 0x01},
  {0x3A07, 0x84},
  {0x3A08, 0x01},
  {0x3A09, 0x43},
  {0x3A0A, 0x24},
  {0x3A0B, 0x60},
  {0x3A0C, 0x28},
  {0x3A0D, 0x60},
  {0x3A0E, 0x04},
  {0x3A0F, 0x8C},
  {0x3A10, 0x05},
  {0x3A11, 0x0C},
  {0x4000, 0x81},
  {0x4001, 0x40},
  {0x4008, 0x02},
  {0x4009, 0x09},
  {0x4300, 0x30},
  {0x430E, 0x00},
  {0x4602, 0x02},
  {0x481B, 0x40},
  {0x481F, 0x40},
  {0x4837, 0x1E},
  {0x5000, 0xFF},
  {0x5001, 0x05},
  {0x5002, 0x32},
  {0x5003, 0x04},
  {0x5004, 0xFF},
  {0x5005, 0x12},
  {0x0100, 0x00},
  {0x5180, 0xF4},
  {0x5181, 0x11},
  {0x5182, 0x41},
  {0x5183, 0x42},
  {0x5184, 0x78},
  {0x5185, 0x58},
  {0x5186, 0xB5},
  {0x5187, 0xB2},
  {0x5188, 0x08},
  {0x5189, 0x0E},
  {0x518A, 0x0C},
  {0x518B, 0x4C},
  {0x518C, 0x38},
  {0x518D, 0xF8},
  {0x518E, 0x04},
  {0x518F, 0x7F},
  {0x5190, 0x40},
  {0x5191, 0x5F},
  {0x5192, 0x40},
  {0x5193, 0xFF},
  {0x5194, 0x40},
  {0x5195, 0x07},
  {0x5196, 0x04},
  {0x5197, 0x04},
  {0x5198, 0x00},
  {0x5199, 0x05},
  {0x519A, 0xD2},
  {0x519B, 0x04},
  {0x5200, 0x09},
  {0x5201, 0x00},
  {0x5202, 0x06},
  {0x5203, 0x20},
  {0x5204, 0x41},
  {0x5205, 0x16},
  {0x5206, 0x00},
  {0x5207, 0x05},
  {0x520B, 0x30},
  {0x520C, 0x75},
  {0x520D, 0x00},
  {0x520E, 0x30},
  {0x520F, 0x75},
  {0x5210, 0x00},
  {0x5280, 0x14},
  {0x5281, 0x02},
  {0x5282, 0x02},
  {0x5283, 0x04},
  {0x5284, 0x06},
  {0x5285, 0x08},
  {0x5286, 0x0C},
  {0x5287, 0x10},
  {0x5300, 0xC5},
  {0x5301, 0xA0},
  {0x5302, 0x06},
  {0x5303, 0x0A},
  {0x5304, 0x30},
  {0x5305, 0x60},
  {0x5306, 0x90},
  {0x5307, 0xC0},
  {0x5308, 0x82},
  {0x5309, 0x00},
  {0x530A, 0x26},
  {0x530B, 0x02},
  {0x530C, 0x02},
  {0x530D, 0x00},
  {0x530E, 0x0C},
  {0x530F, 0x14},
  {0x5310, 0x1A},
  {0x5311, 0x20},
  {0x5312, 0x80},
  {0x5313, 0x4B},
  {0x5380, 0x01},
  {0x5381, 0x52},
  {0x5382, 0x00},
  {0x5383, 0x4A},
  {0x5384, 0x00},
  {0x5385, 0xB6},
  {0x5386, 0x00},
  {0x5387, 0x8D},
  {0x5388, 0x00},
  {0x5389, 0x3A},
  {0x538A, 0x00},
  {0x538B, 0xA6},
  {0x538C, 0x00},
  {0x5400, 0x0D},
  {0x5401, 0x18},
  {0x5402, 0x31},
  {0x5403, 0x5A},
  {0x5404, 0x65},
  {0x5405, 0x6F},
  {0x5406, 0x77},
  {0x5407, 0x80},
  {0x5408, 0x87},
  {0x5409, 0x8F},
  {0x540A, 0xA2},
  {0x540B, 0xB2},
  {0x540C, 0xCC},
  {0x540D, 0xE4},
  {0x540E, 0xF0},
  {0x540F, 0xA0},
  {0x5410, 0x6E},
  {0x5411, 0x06},
  {0x5480, 0x19},
  {0x5481, 0x00},
  {0x5482, 0x09},
  {0x5483, 0x12},
  {0x5484, 0x04},
  {0x5485, 0x06},
  {0x5486, 0x08},
  {0x5487, 0x0C},
  {0x5488, 0x10},
  {0x5489, 0x18},
  {0x5500, 0x02},
  {0x5501, 0x03},
  {0x5502, 0x04},
  {0x5503, 0x05},
  {0x5504, 0x06},
  {0x5505, 0x08},
  {0x5506, 0x00},
  {0x5600, 0x02},
  {0x5603, 0x40},
  {0x5604, 0x28},
  {0x5609, 0x20},
  {0x560A, 0x60},
  {0x5780, 0x3E},
  {0x5781, 0x0F},
  {0x5782, 0x04},
  {0x5783, 0x02},
  {0x5784, 0x01},
  {0x5785, 0x01},
  {0x5786, 0x00},
  {0x5787, 0x04},
  {0x5788, 0x02},
  {0x5789, 0x00},
  {0x578A, 0x01},
  {0x578B, 0x02},
  {0x578C, 0x03},
  {0x578D, 0x03},
  {0x578E, 0x08},
  {0x578F, 0x0C},
  {0x5790, 0x08},
  {0x5791, 0x04},
  {0x5792, 0x00},
  {0x5793, 0x00},
  {0x5794, 0x03},
  {0x5800, 0x03},
  {0x5801, 0x24},
  {0x5802, 0x02},
  {0x5803, 0x40},
  {0x5804, 0x34},
  {0x5805, 0x05},
  {0x5806, 0x12},
  {0x5807, 0x05},
  {0x5808, 0x03},
  {0x5809, 0x3C},
  {0x580A, 0x02},
  {0x580B, 0x40},
  {0x580C, 0x26},
  {0x580D, 0x05},
  {0x580E, 0x52},
  {0x580F, 0x06},
  {0x5810, 0x03},
  {0x5811, 0x28},
  {0x5812, 0x02},
  {0x5813, 0x40},
  {0x5814, 0x24},
  {0x5815, 0x05},
  {0x5816, 0x42},
  {0x5817, 0x06},
  {0x5818, 0x0D},
  {0x5819, 0x40},
  {0x581A, 0x04},
  {0x581B, 0x0C},
  {0x3A03, 0x4C},
  {0x3A04, 0x40},
  {0x3080, 0x02},
  {0x3082, 0x48},
  {0x3018, 0x44},
  {0x3084, 0x0F},
  {0x3085, 0x06},
  {0x380d, 0xc8},
  {0x380f, 0x10},
  {0x4837, 0x12},
  {0x3002, 0x00},
  {0x3823, 0x30},
  {0x3824, 0x00},
  {0x3825, 0x10},
  {0x3826, 0x00},
  {0x3827, 0x08},
};

static struct msm_camera_i2c_reg_setting res_settings[] = {
  {
    .reg_setting = res0_reg_array,
    .size = ARRAY_SIZE(res0_reg_array),
    .addr_type = MSM_CAMERA_I2C_WORD_ADDR,
    .data_type = MSM_CAMERA_I2C_BYTE_DATA,
    .delay = 0,
    //.qup_i2c_batch = MSM_CAMERA_I2C_BATCH_ENABLE,
  },
};

static struct sensor_lib_reg_settings_array res_settings_array = {
  .reg_settings = res_settings,
  .size = ARRAY_SIZE(res_settings),
};

static struct msm_camera_csi2_params *csi_params[] = {
  &ov2685_csi_params, /* RES 0*/
};

static struct sensor_lib_csi_params_array csi_params_array = {
  .csi2_params = &csi_params[0],
  .size = ARRAY_SIZE(csi_params),
};

static struct sensor_crop_parms_t crop_params[] = {
  {0, 0, 0, 0}, /* RES 0 */
};

static struct sensor_lib_crop_params_array crop_params_array = {
  .crop_params = crop_params,
  .size = ARRAY_SIZE(crop_params),
};

static struct sensor_lib_out_info_t sensor_out_info[] = {
  {
    .x_output = 1600 * 2,
    .y_output = 1200,
    .line_length_pclk = 1736 * 2,
    .frame_length_lines = 1296,
    .vt_pixel_clk = 132000000,
    .op_pixel_clk = 132000000,
    .binning_factor = 1,
    .min_fps = 7.5,
    .max_fps = 24.0,
    .mode = SENSOR_DEFAULT_MODE,
  },
};

static struct sensor_lib_out_info_array out_info_array = {
  .out_info = sensor_out_info,
  .size = ARRAY_SIZE(sensor_out_info),
};

static sensor_res_cfg_type_t ov2685_res_cfg[] = {
  SENSOR_SET_STOP_STREAM,
  SENSOR_SET_NEW_RESOLUTION, /* set stream config */
  SENSOR_SET_CSIPHY_CFG,
  SENSOR_SET_CSID_CFG,
//  SENSOR_LOAD_CHROMATIX, /* set chromatix prt */
  SENSOR_SEND_EVENT, /* send event */
  SENSOR_SET_START_STREAM,
};

static struct sensor_res_cfg_table_t ov2685_res_table = {
  .res_cfg_type = ov2685_res_cfg,
  .size = ARRAY_SIZE(ov2685_res_cfg),
};


/*===========================================================================
 * FUNCTION    - ov2685_real_to_register_gain -
 *
 * DESCRIPTION:
 *==========================================================================*/
static uint16_t ov2685_real_to_register_gain(float gain)
{
  unsigned short reg_analog_gain;
  unsigned char  reg_gain_bit_7to0 = 0;
  unsigned int   addl_gain;

  /* Limiter - min / max */
  if (gain <= 1.0) {
    reg_gain_bit_7to0 = 0x10;
  } else if (gain > MAX_ANALOG_GAIN) {
    reg_gain_bit_7to0 = 0xf8;
  } else if (gain > 1.0 && gain < 2.0) {
    addl_gain = (gain - 1.000)/0.0625;
    reg_gain_bit_7to0 = 16 + addl_gain;
  } else if (gain >= 2.0 && gain <= 4.0) {
    addl_gain = (gain - 2.0000)/0.125;
    reg_gain_bit_7to0 = 32 + addl_gain * 2;
  } else if (gain >= 4.0 && gain <= 8.0) {
    addl_gain = (gain - 4.0000)/0.2500;
    reg_gain_bit_7to0 = 64 + addl_gain * 4;
  } else {
    addl_gain = (gain - 8.0)/0.5;
    reg_gain_bit_7to0 = 128 + addl_gain * 8;
  }

  reg_analog_gain = (reg_gain_bit_7to0 & 0xFF);

  return reg_analog_gain;
}

/*===========================================================================
 * FUNCTION    - ov2685_register_to_real_gain -
 *
 * DESCRIPTION:
 *==========================================================================*/
static float ov2685_register_to_real_gain(uint16_t reg_gain)
{
  float real_gain = 0;
  unsigned char reg_gain_bit_7to0 = 0;

  reg_gain_bit_7to0 = reg_gain & 0x00FF;

  if (reg_gain_bit_7to0 > 0xF8) {
    real_gain = 15.5;
  } else if(reg_gain_bit_7to0 < 0x10) {
    real_gain = 1.0;
  } else if (reg_gain_bit_7to0 >= 0x10 && reg_gain_bit_7to0 <= 0x20) {
    real_gain = 1.000 + (reg_gain_bit_7to0 - 0x10) * 0.0625;
  } else if (reg_gain_bit_7to0 > 0x20 && reg_gain_bit_7to0 <= 0x40) {
    real_gain = 2.000 + (reg_gain_bit_7to0 + (reg_gain_bit_7to0 & 1) - 0x20)
      * 0.0625;
  } else if (reg_gain_bit_7to0 > 0x40 && reg_gain_bit_7to0 <= 0x80) {
    real_gain = 4.000 + (reg_gain_bit_7to0 + (4 - (reg_gain_bit_7to0 & 3))
      - 0x40) * 0.0625;
  } else
    real_gain = 8.000 + (reg_gain_bit_7to0 + (8 - (reg_gain_bit_7to0 & 7))
      - 0x80) * 0.0625;

  return real_gain;
}

/*===========================================================================
 * FUNCTION: ov2685_digital_gain_calc
 *
 * DESCRIPTION: Calcuate the sensor digital gain
 *==========================================================================*/
static unsigned int ov2685_digital_gain_calc(float real_gain, float sensor_real_gain)
{
  unsigned int reg_dig_gain = 0;
  float real_dig_gain = 0;

  if(real_gain > MAX_ANALOG_GAIN)
  {
      real_dig_gain = real_gain / sensor_real_gain;
  }
  else
  {
      real_dig_gain = MIN_GAIN;
  }

  if(real_dig_gain > MAX_DIGITAL_GAIN)
  {
      real_dig_gain = MAX_DIGITAL_GAIN;
  }

  reg_dig_gain = (unsigned int)(real_dig_gain * 2048);

  return reg_dig_gain;
}

/*===========================================================================
 * FUNCTION: ov2685_calculate_exposure
 *
 * DESCRIPTION: Calcuate the sensor exposure
 *==========================================================================*/
static int ov2685_calculate_exposure(float real_gain,
  unsigned int line_count, sensor_exposure_info_t *exp_info,
  __attribute__((unused)) float s_real_gain)
{
  float sensor_real_digital_gain;

  if (!exp_info)
  {
    return -1;
  }

  exp_info->reg_gain = ov2685_real_to_register_gain(real_gain);
  exp_info->sensor_real_gain =
    ov2685_register_to_real_gain(exp_info->reg_gain);
  exp_info->sensor_digital_gain =
    ov2685_digital_gain_calc(real_gain, exp_info->sensor_real_gain);
  sensor_real_digital_gain =
    (float)exp_info->sensor_digital_gain / OV2685_MAX_DGAIN_DECIMATOR;
  exp_info->digital_gain =
    real_gain /(exp_info->sensor_real_gain * sensor_real_digital_gain);
  exp_info->line_count = line_count;

  return 0;
}

/*===========================================================================
 * FUNCTION    - ov2685_fill_exposure_array -
 *
 * DESCRIPTION:
 *==========================================================================*/
static int32_t ov2685_fill_exposure_array(uint16_t gain, uint32_t line,
  uint32_t fl_lines, int32_t luma_avg, uint32_t fgain,
  struct msm_camera_i2c_reg_setting* reg_setting)
{
  int32_t rc = 0;
  uint16_t reg_count = 0;
  uint16_t i = 0;

  if (!reg_setting) {
    return -1;
  }
  DEBUG_INFO("%s:gain=%d,line=%d,fl_lines=%d",__func__,gain,line,fl_lines);

  for (i = 0; i < sensor_lib_ptr.groupon_settings->size; i++) {
    reg_setting->reg_setting[reg_count].reg_addr =
      sensor_lib_ptr.groupon_settings->reg_setting[i].reg_addr;
    reg_setting->reg_setting[reg_count].reg_data =
      sensor_lib_ptr.groupon_settings->reg_setting[i].reg_data;
    reg_count = reg_count + 1;
  }

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.output_reg_addr->frame_length_lines;
  reg_setting->reg_setting[reg_count].reg_data = (fl_lines & 0xFF00) >> 8;
  reg_count++;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.output_reg_addr->frame_length_lines + 1;
  reg_setting->reg_setting[reg_count].reg_data = (fl_lines & 0xFF);
  reg_count++;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info->coarse_int_time_addr;
  reg_setting->reg_setting[reg_count].reg_data = line >> 12;
  reg_count++;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info->coarse_int_time_addr + 1;
  reg_setting->reg_setting[reg_count].reg_data = line >> 4;
  reg_count++;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info->coarse_int_time_addr + 2;
  reg_setting->reg_setting[reg_count].reg_data = line << 4;
  reg_count++;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info->global_gain_addr;
  reg_setting->reg_setting[reg_count].reg_data = (gain & 0xFF00) >> 8;
  reg_count++;

  reg_setting->reg_setting[reg_count].reg_addr =
    sensor_lib_ptr.exp_gain_info->global_gain_addr + 1;
  reg_setting->reg_setting[reg_count].reg_data = (gain & 0xFF);
  reg_count++;

  for (i = 0; i < sensor_lib_ptr.groupoff_settings->size; i++) {
    reg_setting->reg_setting[reg_count].reg_addr =
      sensor_lib_ptr.groupoff_settings->reg_setting[i].reg_addr;
    reg_setting->reg_setting[reg_count].reg_data =
      sensor_lib_ptr.groupoff_settings->reg_setting[i].reg_data;
    reg_count = reg_count + 1;
  }

  reg_setting->size = reg_count;
  reg_setting->addr_type = MSM_CAMERA_I2C_WORD_ADDR;
  reg_setting->data_type = MSM_CAMERA_I2C_BYTE_DATA;
  reg_setting->delay = 0;

  return rc;
}

static sensor_exposure_table_t ov2685_expsoure_tbl = {
  .sensor_calculate_exposure = ov2685_calculate_exposure,
  .sensor_fill_exposure_array = ov2685_fill_exposure_array,
};

static sensor_lens_info_t default_lens_info = {
  .focal_length = 3.79,
  .pix_size = 1.75,
  .f_number = 2.4,
  .total_f_dist = 0.478,
  .hor_view_angle = 54.8,
  .ver_view_angle = 42.5,
};


static sensor_lib_t sensor_lib_ptr = {
  /* sensor slave info */
  .sensor_slave_info = &sensor_slave_info,
  /* sensor init params */
  .sensor_init_params = &sensor_init_params,
  /* sensor output settings */
  .sensor_output = &sensor_output,
  /* sensor output register address */
  .output_reg_addr = &output_reg_addr,
  /* sensor exposure gain register address */
  .exp_gain_info = &exp_gain_info,
  /* sensor aec info */
  .aec_info = &aec_info,
  /* sensor snapshot exposure wait frames info */
  .snapshot_exp_wait_frames = 1,
  /* number of frames to skip after start stream */
  .sensor_num_frame_skip = 1,
  /* number of frames to skip after start HDR stream */
  .sensor_num_HDR_frame_skip = 2,
  /* sensor pipeline immediate delay */
  .sensor_max_pipeline_frame_delay = 2,
  /* sensor exposure table size */
  .exposure_table_size = 10,
  /* csi lane params */
  .csi_lane_params = &csi_lane_params,
  /* csi cid params */
  .csi_cid_params = ov2685_cid_cfg,
  /* csi csid params array size */
  .csi_cid_params_size = ARRAY_SIZE(ov2685_cid_cfg),
  /* start settings */
  .start_settings = &start_settings,
  /* stop settings */
  .stop_settings = &stop_settings,
  /* group on settings */
  .groupon_settings = &groupon_settings,
  /* group off settings */
  .groupoff_settings = &groupoff_settings,
  /* resolution cfg table */
  .sensor_res_cfg_table = &ov2685_res_table,
  /* res settings */
  .res_settings_array = &res_settings_array,
  /* out info array */
  .out_info_array = &out_info_array,
  /* crop params array */
  .crop_params_array = &crop_params_array,
  /* csi params array */
  .csi_params_array = &csi_params_array,
  /* sensor port info array */
  .sensor_stream_info_array = &ov2685_stream_info_array,
  /* exposure funtion table */
  .exposure_func_table = &ov2685_expsoure_tbl,
  /* sensor pipeline immediate delay */
  .sensor_max_immediate_frame_delay = 2,
  .default_lens_info = &default_lens_info,
};

/*===========================================================================
 * FUNCTION    - ov2685_open_lib -
 *
 * DESCRIPTION:
 *==========================================================================*/
void *ov2685_open_lib(void)
{
  return &sensor_lib_ptr;
}
