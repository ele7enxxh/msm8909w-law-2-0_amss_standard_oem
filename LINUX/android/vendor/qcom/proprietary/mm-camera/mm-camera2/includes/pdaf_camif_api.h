/*======================================================================
Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=======================================================================*/

#ifndef _PDAF_CAMIF_API_H
#define _PDAF_CAMIF_API_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "pdaf_lib.h"

/*camif lib handle*/
typedef void * PD_CAMIF_HANDLE;

typedef struct{
    /*sensor type*/
    pdaf_sensor_pattern_t  sensor_pattern;
    /*pdaf pattern, location of pdaf pixels*/
    pdaf_info_t                pixel_pattern;
    pdaf_camif_binning_info_t  binning_info;
    pdaf_camif_info_t          camif_info;
} pdaf_camif_init_param_t;




unsigned int pdaf_pix_count;
/*pixel 2D pos, left_pixel,right_pixel*/
camif_pix_t pdaf_pix_coords[MAX_CAMIF_PDAF_PIXELS];

enum pdaf_camif_t3_data_type_t
{
    RAW10_PACKED,
    MAX_DATA_TYPE,
};

typedef enum pdaf_camif_t3_data_type_t pdaf_camif_t3_data_type_t;

typedef struct{
    int32_t x;
    int32_t y;
    int32_t flag;   //0 - left, 1 - right
    int32_t b_x;    //block index
    int32_t b_y;
}camif_pix_epd_t;

typedef struct{

    int32_t sub_pixel_width;    // pixel width in subsampled image
    int32_t sub_pixel_height;
    int32_t offset_x;
    int32_t offset_y;
    camif_pix_epd_t  adj_pdaf_pix_coords[MAX_CAMIF_PDAF_PIXELS];
    unsigned int pdaf_pix_count;

} pdaf_camif_t3_data_buffer_config_t;


unsigned int pdaf_pix_count;
/*pixel 2D pos, left_pixel,right_pixel*/
camif_pix_t pdaf_pix_coords[MAX_CAMIF_PDAF_PIXELS];


typedef struct{
    /*camif input data info*/
    /*for type3 pdaf, pointer to downsampled camif pdaf left vfe image
       compact format,*/
    /*for type2 pdaf, pointer to pdaf stats*/
    /*pointer to the main image in the single vfe case*/
  unsigned char  *p_left;

    /*T3, pointer to downsampled camif pdaf right vfe image, compact format*/
    /*NULL in single vfe and in T2 */
  unsigned char  *p_right;

    /*support fixed window configuration */
    /*and floating window configuration */
    PDAF_window_configure_t  window_configure;
} pdaf_camif_param_t;

typedef struct{
  unsigned int major_version;
  unsigned int minor_version;
}pdaf_camif_lib_version_t;

/*return lib version*/
void PDAF_CAMIF_get_lib_version(pdaf_camif_lib_version_t *p_version);

/*get lib handle, call per camera session*/
PD_CAMIF_HANDLE PDAF_CAMIF_init(pdaf_camif_init_param_t *p_init_param);

PDAF_RET_t  PDAF_CAMIF_sensor_orientation(PD_CAMIF_HANDLE handle, pdaf_sensor_orientation_t sensor_orientation);

/*get pdaf left and right images for the whole frame*/
/*call per frame*/
PDAF_RET_t PDAF_CAMIF_getPDAF( PD_CAMIF_HANDLE handle,
                               pdaf_camif_param_t *p_param,
                               pdaf_camif_output_data_t * p_out);




/*deinit, call per camera session*/
PDAF_RET_t PDAF_CAMIF_deinit(PD_CAMIF_HANDLE handle);

#endif

