/*======================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=======================================================================*/
#ifndef _PDAF_API_H
#define _PDAF_API_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "pdaf_lib.h"

/*defocus confidence*/
#define PDAF_LIB_E_OK                 (0)
#define PDAF_LIB_E_NG                 (-1)

/*pd lib handle*/
/*support multi instance of the lib*/
typedef void * PD_HANDLE;

/*calibration method*/
enum pdaf_calibration_version_t {
    CALIBRATION_VERSION_1D, /*support 1D gain map calibration*/
    CALIBRATION_VERSION_2D, /*support 2D gain map calibration*/
    /* supporting new calibration tool 80-NV125-1 H (or above) output */
    CALIBRATION_VERSION_UPDATE, /* */
    CALIBRATION_VERSION_MAX = 1023,
};

typedef enum pdaf_calibration_version_t pdaf_calibration_version_t;

/*init parameter*/
typedef struct
{
  /*sensor type*/
  pdaf_sensor_pattern_t  sensor_pattern;
  /*pdaf pattern, location of pdaf pixels*/
  pdaf_info_t                pixel_pattern;
  pdaf_camif_binning_info_t  binning_info;
  /*orignal image width in pixel*/
  unsigned int               ori_image_width;
  /*orignal image height in pixel*/
  unsigned int               ori_image_height;
  /*sensor black level*/
  unsigned int               black_level;
  /*calibration type*/
  pdaf_calibration_version_t cali_version;
  /* pointer to calibration parameters from sensor*/
  void *p_calibration_para;

  //algorith_version_id


} pdaf_init_param_t;

typedef struct
{
  /*pdaf left and right image, with size info*/
  pdaf_camif_output_data_t  camif_out;
  /*support fixed window configuration */
  /*and floating window configuration */
  PDAF_window_configure_t    window_configure;
  /*image sensor gain*/
  unsigned int             image_analog_gain;
  /*threshold parameter*/
  int                       defocus_confidence_th;
  /*reserved*/
  void * p_tunnable;
} pdaf_param_t;

typedef struct
{
  pdaf_lib_defocus_t defocus[MAX_PDAF_WINDOW];
  /* number of grids, output data arranged in grid row sequence*/
  /* for floating window, output data arranged in defining order*/
  int               window_number;
} pdaf_output_data_t;

/* vendor info*/
enum pdaf_vendor_t {
    QTI,
    SEMCO,
    MAX_VENDOR = 1023,
};

typedef enum pdaf_vendor_t pdaf_vendor_t;

typedef struct
{
  pdaf_vendor_t  vendor_id;
  unsigned int   major_version;
  unsigned int   minor_version;
}pdaf_lib_version_t;

/*return lib version*/
void PDAF_get_lib_version(pdaf_lib_version_t *p_version);

/*get lib handle, call per camera session*/
PD_HANDLE PDAF_PhaseDetection_init(pdaf_init_param_t *p_init_param);

/*processing function, call per frame*/
PDAF_RET_t PDAF_PhaseDetection(PD_HANDLE handle,
                               pdaf_param_t *p_param,
                               pdaf_output_data_t *p_result);

/* set enable_flag to 0 to diable, 1 to enable */
PDAF_RET_t PDAF_PhaseDetection_enable_Accumlate(PD_HANDLE handle,
    uint32_t enable_flag);


/*get accumulated defocus per frame*/
PDAF_RET_t PDAF_PhaseDetection_accumulate(PD_HANDLE handle,
    pdaf_output_data_t *p_result);


PDAF_RET_t PDAF_PhaseDetection_sensor_orientation(PD_HANDLE handle,
    pdaf_sensor_orientation_t sensor_orientation);


/*deinit, call per camera session*/
PDAF_RET_t PDAF_PhaseDetection_deinit(PD_HANDLE handle);

/*new api to get algorithm version*/

#endif
