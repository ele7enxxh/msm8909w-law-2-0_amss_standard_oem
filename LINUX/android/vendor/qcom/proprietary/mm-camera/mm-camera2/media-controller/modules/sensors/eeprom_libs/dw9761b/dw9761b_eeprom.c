/*============================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

============================================================================*/
#include <stdio.h>
#include "chromatix.h"
#include "eeprom.h"
#include "sensor_common.h"

/*Enable this for supporting pdaf flip & mirror*/
//#define FLIP_MIRROR
#define LEFT_SHEILDED_PDAF_PIXEL 0
#define RIGHT_SHEILDED_PDAF_PIXEL 1

#define AWB_FLAG_OFFSET    0
#define MODULE_INFO_OFFSET 1
#define AWB_OFFSET         9

#define LSC_FLAG_OFFSET    22
#define LSC_OFFSET         LSC_FLAG_OFFSET + 1
#define LSC_GRID_SIZE      221 // 17 x 13

#define AF_FLAG_OFFSET     LSC_OFFSET + LSC_GRID_SIZE * 2 * 4 + 1
#define AF_OFFSET          AF_FLAG_OFFSET + 1

#define VALID_FLAG         0x01
#define QVALUE             1024.0
#define INVALID_DATA       0xFFFF

#define PDGAIN             AF_OFFSET + 7
#define LEFT_GAIN_OFFSET   PDGAIN + 7
#define PDGAIN_MAP_SIZE    131
#define RIGHT_GAIN_OFFSET  LEFT_GAIN_OFFSET + PDGAIN_MAP_SIZE * 2
#define PD_CC              RIGHT_GAIN_OFFSET + PDGAIN_MAP_SIZE * 2 + 1

/* margin is defined corresponding to the margin of golden */
#define INFINITY_MARGIN    0.25
#define MACRO_MARGIN       0.3

typedef struct {
  uint8_t id;
  uint8_t year;
  uint8_t month;
  uint8_t day;
} module_info_t;

typedef struct {
  uint8_t r_over_gr_h;
  uint8_t r_over_gr_l;
  uint8_t b_over_gb_h;
  uint8_t b_over_gb_l;
  uint8_t gr_over_gb_h;
  uint8_t gr_over_gb_l;
} awb_data_t;

typedef struct {
  uint8_t macro_h;
  uint8_t macro_l;
  uint8_t infinity_h;
  uint8_t infinity_l;
} af_data_t;

static unsigned char awb_present = 0;
static unsigned char af_present = 0;
static unsigned char lsc_present = 0;

/** dw9761b_eeprom_get_calibration_items:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Get calibration capabilities and mode items.
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
void dw9761b_eeprom_get_calibration_items(void *e_ctrl)
{
  SLOW("Enter");
  sensor_eeprom_data_t *ectrl = (sensor_eeprom_data_t *)e_ctrl;
  eeprom_calib_items_t *e_items = &(ectrl->eeprom_data.items);

  e_items->is_wbc = awb_present ? TRUE : FALSE;
  e_items->is_afc = af_present ? TRUE : FALSE;
  e_items->is_lsc = lsc_present ? TRUE : FALSE;
  e_items->is_dpc = FALSE;
  e_items->is_insensor = FALSE;
  e_items->is_pdaf3_sensor = TRUE;
  SLOW("Exit");
}

/** dw9761b_eeprom_format_wbdata:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format the data structure of white balance calibration
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
static void dw9761b_eeprom_format_wbdata(sensor_eeprom_data_t *e_ctrl)
{
  uint8_t        flag;
  module_info_t *module_info;
  awb_data_t    *wb;
  float          r_over_gr, b_over_gb, gr_over_gb;
  uint32_t       i;
  SLOW("Enter");
  /* Check validity */
  flag = e_ctrl->eeprom_params.buffer[AWB_FLAG_OFFSET];
  if (flag != VALID_FLAG) {
    awb_present = FALSE;
    SERR("AWB : empty or invalid data");
    return;
  }
  awb_present = TRUE;
  /* Print module info */
  module_info = (module_info_t *)
    (e_ctrl->eeprom_params.buffer + MODULE_INFO_OFFSET);

  SLOW("Module ID : 0x%x", module_info->id);
  SLOW("Y/M/D : %d/%d/%d",
    module_info->year, module_info->month, module_info->day);

  /* Get AWB data */
  wb = (awb_data_t *)(e_ctrl->eeprom_params.buffer + AWB_OFFSET);

  r_over_gr = ((float)((wb->r_over_gr_h << 8) | wb->r_over_gr_l)) / QVALUE;
  b_over_gb = ((float)((wb->b_over_gb_h << 8) | wb->b_over_gb_l)) / QVALUE;
  gr_over_gb = ((float)((wb->gr_over_gb_h << 8) | wb->gr_over_gb_l)) / QVALUE;

  SLOW("AWB : r/gr = %f", r_over_gr);
  SLOW("AWB : b/gb = %f", b_over_gb);
  SLOW("AWB : gr/gb = %f", gr_over_gb);

  for (i = 0; i < AGW_AWB_MAX_LIGHT; i++) {
    e_ctrl->eeprom_data.wbc.r_over_g[i] = r_over_gr;
    e_ctrl->eeprom_data.wbc.b_over_g[i] = b_over_gb;
  }
  e_ctrl->eeprom_data.wbc.gr_over_gb = gr_over_gb;
  SLOW("Exit");
}

/** eeprom_print_matrix:
 *    @paramlist: address of pointer to
 *                   chromatix struct
 *
 * Prints out debug logs
 *
 * This function executes in module sensor context
 *
 * Return:
 * void
 **/
void eeprom_print_matrix(float* paramlist)
{
  int j =0;

  for(j=0; j < MESH_ROLLOFF_SIZE; j = j+17) {
    SLOW("%.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, \
%.1f, %.1f, %.1f, %.1f, %.1f, %.1f",
      paramlist[j], paramlist[j+1], paramlist[j+2], paramlist[j+3],
      paramlist[j+4], paramlist[j+5], paramlist[j+6],  paramlist[j+7],
      paramlist[j+8], paramlist[j+9], paramlist[j+10], paramlist[j+11],
      paramlist[j+12], paramlist[j+13], paramlist[j+14], paramlist[j+15],
      paramlist[j+16]);
  }
}
/** dw9761b_eeprom_format_lensshading:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format the data structure of lens shading correction calibration
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
void dw9761b_eeprom_format_lensshading(sensor_eeprom_data_t *e_ctrl)
{
  uint8_t  flag;
  uint16_t i, light, grid_size;
  uint8_t *lsc_r, *lsc_b, *lsc_gr, *lsc_gb;
  float    gain;
  SLOW("Enter");
  /* Check validity */
  flag = e_ctrl->eeprom_params.buffer[LSC_FLAG_OFFSET];
  if (flag != VALID_FLAG) {
   lsc_present = FALSE;
   SERR("LSC : empty or invalid data");
   return;
  }
  lsc_present = TRUE;
  /* Get LSC data */
  grid_size = LSC_GRID_SIZE * 2;

  lsc_r = e_ctrl->eeprom_params.buffer + LSC_OFFSET;
  lsc_b = lsc_r + grid_size;
  lsc_gr = lsc_b + grid_size;
  lsc_gb = lsc_gr + grid_size;
  
  for (light = 0; light < ROLLOFF_MAX_LIGHT; light++) {
    e_ctrl->eeprom_data.lsc.lsc_calib[light].mesh_rolloff_table_size =
      LSC_GRID_SIZE;
  }

  /* (1) r gain */
  for (i = 0; i < grid_size; i += 2) {
    gain = lsc_r[i + 1] << 8 | lsc_r[i];

    for (light = 0; light < ROLLOFF_MAX_LIGHT; light++) {
      e_ctrl->eeprom_data.lsc.lsc_calib[light].r_gain[i/2] = gain;
    }
  }
  SLOW("r_gain after parser");
  eeprom_print_matrix(e_ctrl->eeprom_data.lsc.lsc_calib[0].r_gain);

  /* (2) b gain */
  for (i = 0; i < grid_size; i += 2) {
    gain = lsc_b[i + 1] << 8 | lsc_b[i];

    for (light = 0; light < ROLLOFF_MAX_LIGHT; light++) {
      e_ctrl->eeprom_data.lsc.lsc_calib[light].b_gain[i/2] = gain;
    }
  }
  SLOW("b_gain after parser");
  eeprom_print_matrix(e_ctrl->eeprom_data.lsc.lsc_calib[0].b_gain);

  /* (3) gr gain */
  for (i = 0; i < grid_size; i += 2) {
    gain = lsc_gr[i + 1] << 8 | lsc_gr[i];

    for (light = 0; light < ROLLOFF_MAX_LIGHT; light++) {
      e_ctrl->eeprom_data.lsc.lsc_calib[light].gr_gain[i/2] = gain;
    }
  }
  SLOW("gr_gain after parser");
  eeprom_print_matrix(e_ctrl->eeprom_data.lsc.lsc_calib[0].gr_gain);

  /* (4) gb gain */
  for (i = 0; i < grid_size; i += 2) {
    gain = lsc_gb[i + 1] << 8 | lsc_gb[i];

    for (light = 0; light < ROLLOFF_MAX_LIGHT; light++) {
      e_ctrl->eeprom_data.lsc.lsc_calib[light].gb_gain[i/2] = gain;
    }
  }
  SLOW("gb_gain after parser");
  eeprom_print_matrix(e_ctrl->eeprom_data.lsc.lsc_calib[0].gb_gain);
  SLOW("Exit");
}

/** dw9761b_eeprom_format_afdata:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format the data structure of white balance calibration
 *
 * This function executes in eeprom module context
 *
 * Return: void.
 **/
static void dw9761b_eeprom_format_afdata(sensor_eeprom_data_t *e_ctrl)
{
  uint8_t    flag;
  af_data_t *af;
  SLOW("Enter");
  /* Check validity */
  flag = e_ctrl->eeprom_params.buffer[AF_FLAG_OFFSET];
  if (flag != VALID_FLAG) {
   /* need to call autofocus caliberation to boost up code_per_step
      invalid AF EEPROM data will not be consumed by af parameters
   */
   SERR("AF : empty or invalid data");
  }
  af_present = TRUE;
  /* Get AF data */
  af = (af_data_t *)(e_ctrl->eeprom_params.buffer + AF_OFFSET);

  e_ctrl->eeprom_data.afc.macro_dac = ((af->macro_h << 8) | af->macro_l);
  e_ctrl->eeprom_data.afc.infinity_dac =
    ((af->infinity_h << 8) | af->infinity_l);
  e_ctrl->eeprom_data.afc.starting_dac = e_ctrl->eeprom_data.afc.infinity_dac;

  SLOW("AF : macro %d infinity %d (no starting DAC set to infinity)",
    e_ctrl->eeprom_data.afc.macro_dac, e_ctrl->eeprom_data.afc.infinity_dac);
  SLOW("Exit");
}

static void  dw9761b_eeprom_format_pdafinfo(sensor_eeprom_data_t *ectrl){
  SLOW("Enter");
  uint8_t i;
   /* pdaf info */
  pdaf_info_t pdafInfo =
   {
   .swin_pdaf_offset_x = 24,
   .swin_pdaf_offset_y = 24,
  .parse_mode = ISP_CAMIF_SUB_SAMPLE,
   .pdaf_pix_count = 32,
   .block_dim =
   {
    .width= 64,
    .height = 64,
   },
   .pdaf_pix_coords =
   {
    /* x,   Y,           flag         */
#ifndef FLIP_MIRROR
    {28, 31, LEFT_SHEILDED_PDAF_PIXEL,},
    {28, 35, RIGHT_SHEILDED_PDAF_PIXEL,},
    {44, 35, LEFT_SHEILDED_PDAF_PIXEL,},
    {44, 39, RIGHT_SHEILDED_PDAF_PIXEL,},
    {64, 35, LEFT_SHEILDED_PDAF_PIXEL,},
    {64, 39, RIGHT_SHEILDED_PDAF_PIXEL,},
    {80, 31, LEFT_SHEILDED_PDAF_PIXEL,},
    {80, 35, RIGHT_SHEILDED_PDAF_PIXEL,},
    {32, 51, LEFT_SHEILDED_PDAF_PIXEL,},
    {32, 47, RIGHT_SHEILDED_PDAF_PIXEL,},
    {48, 55, LEFT_SHEILDED_PDAF_PIXEL,},
    {48, 51, RIGHT_SHEILDED_PDAF_PIXEL,},
    {60, 55, LEFT_SHEILDED_PDAF_PIXEL,},
    {60, 51, RIGHT_SHEILDED_PDAF_PIXEL,},
    {76, 51, LEFT_SHEILDED_PDAF_PIXEL,},
    {76, 47, RIGHT_SHEILDED_PDAF_PIXEL,},
    {32, 67, LEFT_SHEILDED_PDAF_PIXEL,},
    {32, 71, RIGHT_SHEILDED_PDAF_PIXEL,},
    {48, 63, LEFT_SHEILDED_PDAF_PIXEL,},
    {48, 67, RIGHT_SHEILDED_PDAF_PIXEL,},
    {60, 63, LEFT_SHEILDED_PDAF_PIXEL,},
    {60, 67, RIGHT_SHEILDED_PDAF_PIXEL,},
    {76, 67, LEFT_SHEILDED_PDAF_PIXEL,},
    {76, 71, RIGHT_SHEILDED_PDAF_PIXEL,},
    {28, 87, LEFT_SHEILDED_PDAF_PIXEL,},
    {28, 83, RIGHT_SHEILDED_PDAF_PIXEL,},
    {44, 83, LEFT_SHEILDED_PDAF_PIXEL,},
    {44, 79, RIGHT_SHEILDED_PDAF_PIXEL,},
    {64, 83, LEFT_SHEILDED_PDAF_PIXEL,},
    {64, 79, RIGHT_SHEILDED_PDAF_PIXEL,},
    {80, 87, LEFT_SHEILDED_PDAF_PIXEL,},
    {80, 83, RIGHT_SHEILDED_PDAF_PIXEL,},
#else /*if Flip & Mirror is enabled*/
    {83, 80, LEFT_SHEILDED_PDAF_PIXEL,},
    {83, 76, RIGHT_SHEILDED_PDAF_PIXEL,},
    {67, 76, LEFT_SHEILDED_PDAF_PIXEL,},
    {67, 72, RIGHT_SHEILDED_PDAF_PIXEL,},
    {47, 76, LEFT_SHEILDED_PDAF_PIXEL,},
    {47, 72, RIGHT_SHEILDED_PDAF_PIXEL,},
    {31, 80, LEFT_SHEILDED_PDAF_PIXEL,},
    {31, 76, RIGHT_SHEILDED_PDAF_PIXEL,},
    {79, 60, LEFT_SHEILDED_PDAF_PIXEL,},
    {79, 64, RIGHT_SHEILDED_PDAF_PIXEL,},
    {63, 56, LEFT_SHEILDED_PDAF_PIXEL,},
    {63, 60, RIGHT_SHEILDED_PDAF_PIXEL,},
    {51, 56, LEFT_SHEILDED_PDAF_PIXEL,},
    {51, 60, RIGHT_SHEILDED_PDAF_PIXEL,},
    {35, 60, LEFT_SHEILDED_PDAF_PIXEL,},
    {35, 64, RIGHT_SHEILDED_PDAF_PIXEL,},
    {79, 44, LEFT_SHEILDED_PDAF_PIXEL,},
    {79, 40, RIGHT_SHEILDED_PDAF_PIXEL,},
    {63, 48, LEFT_SHEILDED_PDAF_PIXEL,},
    {63, 44, RIGHT_SHEILDED_PDAF_PIXEL,},
    {51, 48, LEFT_SHEILDED_PDAF_PIXEL,},
    {51, 44, RIGHT_SHEILDED_PDAF_PIXEL,},
    {35, 44, LEFT_SHEILDED_PDAF_PIXEL,},
    {35, 40, RIGHT_SHEILDED_PDAF_PIXEL,},
    {83, 24, LEFT_SHEILDED_PDAF_PIXEL,},
    {83, 28, RIGHT_SHEILDED_PDAF_PIXEL,},
    {67, 28, LEFT_SHEILDED_PDAF_PIXEL,},
    {67, 32, RIGHT_SHEILDED_PDAF_PIXEL,},
    {47, 28, LEFT_SHEILDED_PDAF_PIXEL,},
    {47, 32, RIGHT_SHEILDED_PDAF_PIXEL,},
    {31, 24, LEFT_SHEILDED_PDAF_PIXEL,},
    {31, 28, RIGHT_SHEILDED_PDAF_PIXEL,},
#endif
   },
   };
  memcpy(&ectrl->eeprom_data.pdafInfo, &pdafInfo, sizeof(pdaf_info_t));
   for (i = 0; i < ectrl->eeprom_data.pdafInfo.pdaf_pix_count; i++) {
   SLOW("pix[%d]:[%d,%d] Left/Right %d",
         i,
        ectrl->eeprom_data.pdafInfo.pdaf_pix_coords[i].x,
        ectrl->eeprom_data.pdafInfo.pdaf_pix_coords[i].y,
        ectrl->eeprom_data.pdafInfo.pdaf_pix_coords[i].flag
         );
   }
  SLOW("Exit");
}
static void dw9761b_eeprom_format_pdafgain(sensor_eeprom_data_t *e_ctrl)
{

  pdaf_calibration_param_t     pdaf3_gain;
  unsigned char                i, j, flag, flag2;
  unsigned char                *data, *data2;

  SLOW("Enter");
  /* Check validity */
  flag = e_ctrl->eeprom_params.buffer[PDGAIN];
  flag2 = e_ctrl->eeprom_params.buffer[PD_CC];

  memset(e_ctrl->eeprom_data.pdafc_1d.gain_tbl_left,
         0, sizeof(int) * MAXLENGTH1D);
  memset(e_ctrl->eeprom_data.pdafc_1d.gain_tbl_right,
         0, sizeof(int) * MAXLENGTH1D);

  if ( (flag == VALID_FLAG) && (flag2 == VALID_FLAG))
  {
    SLOW("PD: prase table inside of eeprom");
    data = e_ctrl->eeprom_params.buffer + PDGAIN + 1;
    data2 = e_ctrl->eeprom_params.buffer + PD_CC + 1;
    e_ctrl->eeprom_data.pdafc_1d.gain_map_DSRatio = (data[0] << 8) | data[1];
    e_ctrl->eeprom_data.pdafc_1d.gain_map_length = (data[2] << 8) | data[3];
    e_ctrl->eeprom_data.pdafc_1d.gain_map_DSLength = (data[4] << 8) | data[5];
    for ( i = 0; i < PDGAIN_MAP_SIZE; i++)
    {
      j = 2 * i;
      e_ctrl->eeprom_data.pdafc_1d.gain_tbl_left[i] =
       (data[6 + j] << 8) | data[j + 7];
      e_ctrl->eeprom_data.pdafc_1d.gain_tbl_right[i] =
       (data[6 + PDGAIN_MAP_SIZE * 2 + j] << 8)
       | data[PDGAIN_MAP_SIZE *2 + j + 7];
    }
    e_ctrl->eeprom_data.pdafc_1d.PD_conversion_coeff[0] =
     (data2[0] << 8)| data2[1];
  } else /* use default */
  {
    SLOW("PD: no valid gain map, use default table");
    int  left_gain[MAXLENGTH1D] = {164,164,165,163,164,162,161,160,159,
    159,159,157,157,155,156,155,153,155,153,153,154,154,152,152,153,152,
    151,150,150,149,150,150,150,150,149,150,148,148,149,150,149,149,149,
    150,149,149,150,149,150,150,150,149,150,150,150,152,150,149,151,148,
    150,150,151,152,151,151,153,153,155,156,154,155,155,158,158,156,160,
    159,160,162,161,163,162,163,163,165,165,165,166,167,168,169,171,170,
    171,174,175,177,176,176,181,178,180,181,179,184,183,184,185,186,184,
    188,187,190,188,189,191,189,191,192,191,191,193,192,195,195,192,199,
    203,208,207};
   int right_gain[MAXLENGTH1D] = {199,201,203,201,201,202,203,203,203,
    203,204,203,203,203,206,207,206,207,207,205,204,205,207,203,204,204,
    203,203,204,201,201,202,201,203,201,199,201,199,198,196,197,197,196,
    196,194,192,192,193,191,190,191,189,186,186,186,187,184,186,184,182,
    181,180,181,182,181,179,179,180,179,178,178,177,176,176,175,176,173,
    173,176,174,174,174,171,171,171,172,170,171,171,170,170,170,171,168,
    170,168,167,168,166,165,167,166,167,167,165,167,166,166,165,164,164,
    160,163,163,164,164,163,163,164,165,164,162,164,165,164,164,164,160,
    159,158,158};
   e_ctrl->eeprom_data.pdafc_1d.gain_map_DSRatio = 2;
   e_ctrl->eeprom_data.pdafc_1d.gain_map_DSLength = 131;
   e_ctrl->eeprom_data.pdafc_1d.gain_map_length = 260;
   e_ctrl->eeprom_data.pdafc_1d.PD_conversion_coeff[0] = 23240;
   memcpy(e_ctrl->eeprom_data.pdafc_1d.gain_tbl_left, &left_gain,
     sizeof(int) * MAXLENGTH1D);
   memcpy(e_ctrl->eeprom_data.pdafc_1d.gain_tbl_right, &right_gain,
     sizeof(int) * MAXLENGTH1D);
  }
  SLOW("Exit");
}


/** dw9761b_eeprom_format_calibration_data:
 *    @e_ctrl: point to sensor_eeprom_data_t of the eeprom device
 *
 * Format all the data structure of calibration
 *
 * This function executes in eeprom module context and generate
 *   all the calibration registers setting of the sensor.
 *
 * Return: void.
 **/
void dw9761b_eeprom_format_calibration_data(void *e_ctrl) {
  SLOW("Enter");
  sensor_eeprom_data_t *ectrl = (sensor_eeprom_data_t *)e_ctrl;
  if (!ectrl->eeprom_params.buffer || !ectrl->eeprom_params.num_bytes) {
    SERR("Buff pointer %p buffer size %d", ectrl->eeprom_params.buffer,
      ectrl->eeprom_params.num_bytes);
    return;
  }

  dw9761b_eeprom_format_wbdata(ectrl);
  dw9761b_eeprom_format_lensshading(ectrl);
  dw9761b_eeprom_format_afdata(ectrl);
  dw9761b_eeprom_format_pdafinfo(ectrl);
  dw9761b_eeprom_format_pdafgain(e_ctrl);
  SLOW("Exit");
}

static boolean dw9761b_autofocus_calibration(void *e_ctrl) {
  sensor_eeprom_data_t *ectrl = (sensor_eeprom_data_t *) e_ctrl;
  int32_t                 i = 0;
  actuator_tuned_params_t *af_driver_tune = NULL;
  actuator_params_t       *af_params = NULL;
  uint32_t                total_steps = 0;
  int16_t                 macro_dac, infinity_dac;
  int16_t                 new_step_bound, otp_step_bound;
  uint32_t                qvalue = 0;

  /* Validate params */
  RETURN_ON_NULL(e_ctrl);
  RETURN_ON_NULL(ectrl->eeprom_afchroma.af_driver_ptr);

  af_driver_tune =
    &(ectrl->eeprom_afchroma.af_driver_ptr->actuator_tuned_params);
  af_params = &(ectrl->eeprom_afchroma.af_driver_ptr->actuator_params);

  /* Get the total steps */
  total_steps = af_driver_tune->region_params[af_driver_tune->region_size - 1].
    step_bound[0] - af_driver_tune->region_params[0].step_bound[1];

  if (!total_steps) {
    SERR("Invalid total_steps count = 0");
    return FALSE;
  }

  qvalue = af_driver_tune->region_params[0].qvalue;
  if(qvalue < 1 && qvalue > 4096){
    SERR("Invalid qvalue %d", qvalue);
    return FALSE;
  }
  if (ectrl->eeprom_data.afc.macro_dac < INVALID_DATA)
  {
   macro_dac = ectrl->eeprom_data.afc.macro_dac; 
   infinity_dac = ectrl->eeprom_data.afc.infinity_dac;
   otp_step_bound = macro_dac - infinity_dac;
  /* adjust af_driver_ptr */
   af_driver_tune->initial_code = infinity_dac - otp_step_bound * INFINITY_MARGIN;
   new_step_bound = otp_step_bound * (1 + INFINITY_MARGIN + MACRO_MARGIN);
   af_driver_tune->region_params[0].code_per_step =
    new_step_bound / (float)total_steps * qvalue;
  }
  else{
   /* if AF data is invalid, only boost code_per_step */
   af_driver_tune->region_params[0].code_per_step = qvalue;
  }

  SLOW("initial code %d, adjusted code_per_step: %d, qvalue: %d",
    af_driver_tune->initial_code,
    af_driver_tune->region_params[0].code_per_step,
    qvalue);

  return TRUE;
}
static eeprom_lib_func_t dw9761b_eeprom_lib_func_ptr = {
  .get_calibration_items = dw9761b_eeprom_get_calibration_items,
  .format_calibration_data = dw9761b_eeprom_format_calibration_data,
  .do_af_calibration = dw9761b_autofocus_calibration,
  .do_wbc_calibration = eeprom_whitebalance_calibration,
  .do_lsc_calibration = eeprom_lensshading_calibration,


  .do_dpc_calibration = NULL,
  .get_dpc_calibration_info = NULL,
  .get_raw_data = NULL,
};

/** dw9761b_eeprom_eeprom_open_lib:
 *
 * Get the funtion pointer of this lib.
 *
 * This function executes in eeprom module context.
 *
 * Return: eeprom_lib_func_t point to the function pointer.
 **/
void* dw9761b_eeprom_open_lib(void) {
  return &dw9761b_eeprom_lib_func_ptr;
}
