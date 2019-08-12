/*============================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/
#ifndef __CHROMATIX_ADRC_H__
#define __CHROMATIX_ADRC_H__

#include "chromatix.h"

#define ISP_LA_TABLE_LENGTH           64
#define ISP_LA_RATIO_GAIN_LUT_TABLE_LENGTH           47
#define DRC_GAIN_LEVELS 3

#define MAX_SENSOR_GAIN_ENTRY_NUM   6
#define MAX_DRC_GAIN_ENTRY_NUM      4

/* Luma Adaptation gain for ADRC */
typedef struct {
    float LUT_Yratio[ISP_LA_TABLE_LENGTH]; /* LUT for LA*/
}la_ratio_gain_table;

/* Chroma Enhancement ADRC params */
typedef struct {
  float la_gain;
  float sat_factor;
}isp_color_conversion_sat_adj_t;

typedef struct {
  int enable;
  isp_color_conversion_sat_adj_t scale[DRC_GAIN_LEVELS];
}isp_color_conversion_sat_adj_type;

/* SCE ADRC params*/
typedef struct {
  float la_gain;
  sce_cr_cb_triangle_set    origin_triangles_TL84;
  sce_cr_cb_triangle_set    destination_triangles_TL84;
  sce_shift_vector          shift_vector_TL84;
  sce_cr_cb_triangle_set    origin_triangles_A;
  sce_cr_cb_triangle_set    destination_triangles_A;
  sce_shift_vector          shift_vector_A;
  sce_cr_cb_triangle_set    origin_triangles_D65;
  sce_cr_cb_triangle_set    destination_triangles_D65;
  sce_shift_vector          shift_vector_D65;
}isp_skin_enhan_sat_adj_t;

typedef struct {
  int enable;
  isp_skin_enhan_sat_adj_t sce[DRC_GAIN_LEVELS];
}isp_skin_enhan_sat_adj_type;

typedef struct {
  /* Tuning parameters */
  int hist_target_adjust_enable;
  float outdoor_max_target_adjust_ratio;
  float outdoor_min_target_adjust_ratio;
  float indoor_max_target_adjust_ratio;
  float indoor_min_target_adjust_ratio;
  float lowlight_max_target_adjust_ratio;
  float lowlight_min_target_adjust_ratio;
  float target_filter_factor;
  float hist_sat_pct;
  float hist_dark_pct;
  float hist_sat_low_ref;
  float hist_sat_high_ref;
  float hist_dark_low_ref;
  float hist_dark_high_ref;
} adrc_hist_metering_parsed_type;

typedef struct {
  int adrc_denoise_gain_adjust_enable;
  int sensor_gain_entry_num;
  int drc_gain_entry_num;
  float sensor_gain_triggers[MAX_SENSOR_GAIN_ENTRY_NUM];
  float drc_gain_triggers[MAX_DRC_GAIN_ENTRY_NUM];
  float denoise_gain_adjust_ratios[MAX_SENSOR_GAIN_ENTRY_NUM][MAX_DRC_GAIN_ENTRY_NUM];
} adrc_denoise_gain_adjust_type;

typedef struct {
  float lux_start;
  float lux_end;
} adrc_trigger_point_type;

typedef struct {
  adrc_trigger_point_type outdoor_trigger;
  adrc_trigger_point_type lowlight_trigger;
  float outdoor_max_gain;         // Outdoor
  float default_max_gain;         // Default
  float lowlight_max_gain;         // LowLight
} adrc_max_gain_type;

typedef struct {
  float red_gain_adj_factor;
  float blue_gain_adj_factor;
} adrc_awb_gain_adjust_factor_type;

typedef struct {
  int adrc_enable;
  la_ratio_gain_table la_ratio_gain_LUT[ISP_LA_RATIO_GAIN_LUT_TABLE_LENGTH];
  isp_color_conversion_sat_adj_type cv;
  isp_skin_enhan_sat_adj_type sce;
  chromatix_gamma_type adrc_gamma;
  adrc_max_gain_type max_gain_trigger;
  float adrc_conv_speed;
  float gtm_percentage;
  float ltm_percentage;
  float la_percentage;
  float gamma_percentage;
  float luma_tolerance;
  adrc_hist_metering_parsed_type hist;
  adrc_denoise_gain_adjust_type denoise_gain_adj;
  adrc_awb_gain_adjust_factor_type gain_adj_factor[AGW_AWB_MAX_LIGHT];
} chromatix_adrc_type;
#endif
