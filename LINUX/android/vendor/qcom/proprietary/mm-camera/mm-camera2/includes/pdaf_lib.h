/*======================================================================
    Copyright (c) 2015 Qualcomm Technologies, Inc.
    All Rights Reserved.
    Confidential and Proprietary - Qualcomm Technologies, Inc.
=======================================================================*/

#ifndef _PDAF_LIB_H
#define _PDAF_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_PDAF_WINDOW        200  /*max fixed pdaf window number*/
#define MAX_FLOAT_WINDOW       32   /*max floating pdaf window number*/
#define MAX_CAMIF_PDAF_PIXELS  256  /*max number of pd pixels in a block*/
#define MAXWIDTH               25  /* =8K/256 */
#define MAXHEIGHT              19
#define MAXLENGTH2D            MAXWIDTH * MAXHEIGHT
#define MAXLENGTH1D            256

/* function return status */
enum PDAF_RET_t{
    PDAF_LIB_OK,
    PDAF_LIB_CONFIG_NOT_SUPPORT,
    PDAF_LIB_MEM_ERR,
    PDAF_LIB_PARAM_ERR,
    PDAF_LIB_ERR,
    PDAF_LIB_ERR_MAX = 1023,
};
typedef enum PDAF_RET_t PDAF_RET_t;

enum sheild_pd_info_t{
 LEFT_SHEILDED_PDAF_PIXEL,
 RIGHT_SHEILDED_PDAF_PIXEL,
};
typedef enum sheild_pd_info_t sheild_pd_info_t;

/*pdaf window types*/
enum pdaf_sw_window_mode_t {
     FLOAT_WINDOW,
     FIXED_GRID_WINDOW,
     WINDOM_MODE_MAX,
};

typedef enum pdaf_sw_window_mode_t pdaf_sw_window_mode_t;

/*pdaf floating window configure*/
/*each components are defined as a percentage of image size*/
typedef struct{
float pdaf_address_start_hori;  /*(0-1), percentage of image width*/
float pdaf_address_start_ver;   /*(0-1), percentage of image height*/
float pdaf_address_end_hori;    /*(0-1), percentage of image width*/
float pdaf_address_end_ver;     /*(0-1), percentage of image height*/
}PDAF_float_window_t;

typedef struct{
/*number of floating windows*/
  unsigned int         pdaf_number_windows;
PDAF_float_window_t  float_windows[MAX_FLOAT_WINDOW];
}PDAF_float_window_configure_t;

/*fixed pdaf window configuration*/
/*roi width and roi horizontal offset is defined
  as a percentage of image width*/
/*roi height and roi vertical offset is defined
  as a percentage of image height*/
typedef struct{
  float  roi_offset_hori;
  float  roi_offset_ver;
  float  roi_width;
  float  roi_height;
  /*window number along horizontal direction*/
  unsigned int window_number_hori;
  /*window number along vertical direction*/
  unsigned int window_number_ver;
}PDAF_fixed_grid_window_configure_t;

/*window configuration */
typedef struct{
    /*Depending on window mode, either fixed window or
      floating window is configured.*/
    pdaf_sw_window_mode_t               pdaf_sw_window_mode;
    PDAF_float_window_configure_t       float_window_configure;
    PDAF_fixed_grid_window_configure_t  fixed_grid_window_configure;
}PDAF_window_configure_t;

/*pd pixel location in one block*/
typedef struct{
  int x;
  int y;
  sheild_pd_info_t flag;
}camif_pix_t;

/*camif pdaf dump mode*/
enum pdaf_type{
  RDI_STATS,
  CAMIF_SUB_SAMPLE,
  MAX_PDAF_MODES,
};

typedef enum pdaf_type camif_pdaf_parse_t;


/*sensor type*/
enum pdaf_sensor_orientation_t
{
        CANONICAL_ORIENTATION,
        MIRROR_ORIENTATION,
        FLIP_ORIENTATION,
        MIRROR_AND_FLIP_ORIENTATION,
        MAX_ORIENTATION,
};

typedef enum pdaf_sensor_orientation_t pdaf_sensor_orientation_t;




/*sensor type*/
enum pdaf_sensor_pattern_t {
   /*3M2/3L8 */
  SENSOR_PATTERN0,
  /* IMX258 T2 sensor, with 2D calibration */
  SENSOR_PATTERN1,
  SENSOR_PATTERN2,
  /* OV13853, T3*/
  SENSOR_PATTERN3,
  /* 3P3, T3*/
  SENSOR_PATTERN4,
  /* Obsolete, 3M2, T3 sensor, with 1D calibration, old algorithm, flipped */
  SENSOR_PATTERN0_FILP = 0x10000000,
  /* Obsolete, IMX258 T2 sensor, with 2D calibration, flipped */
  SENSOR_PATTERN1_FILP = 0x10000001,
  /* Obsolete, 3M2, T3 sensor, with 1D calibration, old algorithm, mirrored */
  SENSOR_PATTERN0_MIRROR = 0x20000000,
  /* Obsolete, IMX258 T2 sensor, with 2D calibration, mirrored */
  SENSOR_PATTERN1_MIRROR = 0x20000001,
  /* Obsolete, 3M2, T3 sensor, with 1D calibration, old algorithm, flip and mirrored */
  SENSOR_PATTERN0_MF = 0x30000000,
  /* Obsolete, IMX258 T2 sensor, with 2D calibration, flip and mirrored */
  SENSOR_PATTERN1_MF = 0x30000001,
  MAX_SENSOR_PATTERN = 0xFFFFFFFF,
};

typedef enum pdaf_sensor_pattern_t pdaf_sensor_pattern_t;

typedef struct{
  int width;
  int height;
}camif_dim_t;

typedef struct{
  camif_pdaf_parse_t parse_mode;
  /*offset of first pattern, global offset*/
  unsigned int swin_pdaf_offset_x;
  /*offset of first pattern, global offset*/
  unsigned int swin_pdaf_offset_y;
  /*pdaf pixel number inside of a window*/
  unsigned int pdaf_pix_count;
  /*pixel 2D pos, left_pixel,right_pixel*/
  camif_pix_t pdaf_pix_coords[MAX_CAMIF_PDAF_PIXELS];
  /*pattern window width and height*/
  camif_dim_t block_dim;

  pdaf_sensor_orientation_t  sensor_orientation;

}pdaf_info_t;

typedef struct{
  unsigned short horizontal_resolution;
  unsigned short vertical_resolution;
  unsigned short binning_factor;
  unsigned int is_pdaf_support; /* 0 - not support, 1 - support */
} pdaf_camif_binning_info_t;

/* camif lib output*/
typedef struct{
    /* pointer to pdaf left images, 16bit per pixel*/
  unsigned short  *p_pdaf_left;
  unsigned int    pdaf_left_buffer_width;       /* width, in pixel*/
  unsigned int    pdaf_left_buffer_stride;      /* stride, in byte*/

    /* pointer to pdaf left images, 16bit per pixel*/
  unsigned short  *p_pdaf_right;
  unsigned int    pdaf_right_buffer_width;      /* width, in pixel*/
  unsigned int    pdaf_right_buffer_stride;     /* stride, in byte*/
  unsigned int    pdaf_buffer_height;           /* height*/
} pdaf_camif_output_data_t;

typedef struct
{
  int             defocus;       /*in DAC range*/
  signed char     df_confidence; /*PD_LIB_E_OK or PD_LIB_E_NG */
  unsigned int    df_conf_level; /*confidence level*/
  float           phase_diff;    /*phase difference*/
} pdaf_lib_defocus_t;/*output data*/

typedef struct {
  /* 0 - single vfe, 1 - dual vfe*/
  unsigned char dual_vfe;
  /* orignal image width in pixel*/
  unsigned int  ori_image_width;
  /* orignal image height in pixel*/
  unsigned int  ori_image_height;
  /* pdaf region configuration*/
  /* global horizonatal offset in pixel*/
  unsigned int  global_offset_horizontal;
  /* global vertical offset in pixel*/
  unsigned int  global_offset_vertical;
  /* subsampled image width in pixel*/
  unsigned int  camif_left_buffer_width;
  /* subsampled image stride in byte*/
  unsigned int  camif_left_buffer_stride;
  /* subsampled image width in pixel*/
  unsigned int  camif_right_buffer_width;
  /* subsampled image stride in byte*/
  unsigned int  camif_right_buffer_stride;
  /* subsampled image height in pixel*/
  unsigned int  camif_buffer_height;
  /* overlap in pixel for dual vfe in the original image*/
  /* before subsampling*/
  unsigned int image_overlap;
  /* horizontal offset in pixel for right image in dual vfe */
  unsigned int right_image_offset;
} pdaf_camif_info_t;

typedef struct {
  void               *eeprom_data;
  int                cur_res;
  pdaf_camif_info_t  isp_camif;
} pdaf_init_info_t;



typedef struct {
  unsigned char      *pd_stats;
  unsigned int       x_offset;   /* offset to PD area in full-size image */
  unsigned int       y_offset;
  unsigned int       x_win_num;  /* PD area grid number */
  unsigned int       y_win_num;
  pdaf_lib_defocus_t defocus[MAX_PDAF_WINDOW];
  unsigned int       status;
  void               *eeprom_data;
  int                cur_res;
  float              sensor_real_gain;
} pdaf_params_t;

typedef struct
{
  /* gain map parameters */
  int gain_tbl_left[256]; /* gain table for left image */
  int gain_tbl_right[256]; /* gain table for right image */

  unsigned int gain_map_DSLength; /* gain map downsampled length */
  unsigned int gain_map_DSRatio;  /* gain map downsample ratio */
  unsigned int gain_map_length;   /* actual length */

  /* phase difference to defocus conversion */
  int PD_conversion_coeff[MAX_PDAF_WINDOW];
} pdaf_calibration_param_t;


typedef struct GainMapDataStructtag
{
    int VersionNum;
    unsigned short DSRatio;
    unsigned short ActualLength;
    unsigned short DSLength;
    unsigned short Left_GainMap[MAXLENGTH1D];
    unsigned short Right_GainMap[MAXLENGTH1D];
    int PD_conversion_coeff[MAX_PDAF_WINDOW];
}pdaf_1d_calibration_param_ver1_t;


typedef struct
{
  unsigned short OffsetX;
  unsigned short OffsetY;
  unsigned short RatioX;
  unsigned short RatioY;
  unsigned short MapWidth;
  unsigned short MapHeight;
  unsigned short Left_GainMap[MAXLENGTH2D];
  unsigned short Right_GainMap[MAXLENGTH2D];
  int   PD_conversion_coeff[MAX_PDAF_WINDOW];
} pdaf_2D_calibration_param_t;



typedef struct
{
    int VersionNum;
    unsigned short OffsetX;
    unsigned short OffsetY;
    unsigned short RatioX;
    unsigned short RatioY;
    unsigned short MapWidth;
    unsigned short MapHeight;
    unsigned short Left_GainMap[MAXLENGTH2D];
    unsigned short Right_GainMap[MAXLENGTH2D];
    int   PD_conversion_coeff[MAX_PDAF_WINDOW];
}pdaf_2D_calibration_param_new_ver2_t;



typedef struct{
    int x;
    int y;
    sheild_pd_info_t flag;
    int group_id;
}pdaf_configure_pix_t;


typedef struct{
    int group_num;
    int layer_num;
    int pixel_per_layer;
    pdaf_configure_pix_t configure_pixels[MAX_CAMIF_PDAF_PIXELS];
}pdaf_configure_t;





#endif
