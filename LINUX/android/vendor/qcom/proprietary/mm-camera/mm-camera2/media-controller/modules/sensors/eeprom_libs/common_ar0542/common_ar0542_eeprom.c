/*============================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.	3
  All Rights Reserved.	4
  Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/
#include <stdio.h>
#include "chromatix.h"
#include "eeprom.h"
#include "sensor_common.h"


#define MODULE_INFO_SIZE  4
#define MODULE_INFO_OFFSET 0


#define LSC_REG_SIZE_1  20
#define LSC_OFFSET_1  (MODULE_INFO_OFFSET+MODULE_INFO_SIZE*2)

#define LSC_REG_SIZE_2  20
#define LSC_OFFSET_2  (LSC_OFFSET_1+LSC_REG_SIZE_1*2)

#define LSC_REG_SIZE_3  20
#define LSC_OFFSET_3  (LSC_OFFSET_2+LSC_REG_SIZE_2*2)

#define LSC_REG_SIZE_4  20
#define LSC_OFFSET_4  (LSC_OFFSET_3+LSC_REG_SIZE_3*2)

#define LSC_REG_SIZE_5  20
#define LSC_OFFSET_5  (LSC_OFFSET_4+LSC_REG_SIZE_4*2)

#define LSC_REG_SIZE_6  2
#define LSC_OFFSET_6  (LSC_OFFSET_5+LSC_REG_SIZE_5*2)

#define LSC_REG_SIZE_7  4
#define LSC_OFFSET_7  (LSC_OFFSET_6+LSC_REG_SIZE_6*2)


#define LSC_REG_SIZE  (LSC_REG_SIZE_1+LSC_REG_SIZE_2+LSC_REG_SIZE_3+LSC_REG_SIZE_4+LSC_REG_SIZE_5+LSC_REG_SIZE_6+LSC_REG_SIZE_7)

#define AWB_REG_SIZE  2
#define WB_OFFSET  (LSC_OFFSET_1+LSC_REG_SIZE*2)

#define QVALUE 128.0

struct msm_camera_i2c_reg_array g_reg_array[LSC_REG_SIZE];
struct msm_camera_i2c_reg_setting g_reg_setting;

int Format_Calibration_Flag = 0;


void common_ar0542_get_calibration_items( void *e_ctrl )
{
  sensor_eeprom_data_t *ectrl = (sensor_eeprom_data_t *)e_ctrl;
  eeprom_calib_items_t *e_items = &(ectrl->eeprom_data.items);
  e_items->is_insensor = TRUE;
  e_items->is_afc = FALSE;
  e_items->is_wbc = TRUE;
  e_items->is_lsc = FALSE;
  e_items->is_dpc = FALSE;

}

static void common_ar0542_format_moduledata(sensor_eeprom_data_t *ectrl)
{
  uint8_t *buf = ectrl->eeprom_params.buffer;
  uint16_t model_id ,vcm_vendor,driverIC_vendor,lens_id;

  model_id =  (buf[MODULE_INFO_OFFSET] << 8)
               |(buf[MODULE_INFO_OFFSET+ 1]);
  vcm_vendor = (buf[MODULE_INFO_OFFSET+2] << 8)
               |(buf[MODULE_INFO_OFFSET+ 3]);
  driverIC_vendor = (buf[MODULE_INFO_OFFSET +4] << 8)
               |(buf[MODULE_INFO_OFFSET+ 5]);
  lens_id = (buf[MODULE_INFO_OFFSET+6] << 8)
               |(buf[MODULE_INFO_OFFSET+ 7]);

  SERR("model_id id: 0x%x", model_id);
  SERR("vcm_vendor: 0x%x", vcm_vendor);
  SERR("driverIC_vendor: 0x%x", driverIC_vendor);
  SERR("lens_id: 0x%x", lens_id);

}

static void common_ar0542_format_wbdata(sensor_eeprom_data_t *e_ctrl)
{

  int index;
  uint16_t awb_r_over_gr, awb_b_over_gr;

  SERR("Enter common_ar0542_format_wbdata");

  awb_r_over_gr =(e_ctrl->eeprom_params.buffer[WB_OFFSET] << 8) |
                 (e_ctrl->eeprom_params.buffer[WB_OFFSET+ 1]);

  awb_b_over_gr =(e_ctrl->eeprom_params.buffer[WB_OFFSET+2] << 8) |
                 (e_ctrl->eeprom_params.buffer[WB_OFFSET+ 3]);

  SERR("awb_r_over_gr: 0x%x", awb_r_over_gr);
  SERR("awb_b_over_gr: 0x%x", awb_b_over_gr);

  if((awb_r_over_gr>=128)||(awb_b_over_gr>=128)
     ||(awb_r_over_gr<=0)||(awb_b_over_gr<=0))
  {
     awb_r_over_gr = 84;
     awb_b_over_gr = 74;
  }

  for ( index = 0; index < AGW_AWB_MAX_LIGHT; index++ ) {

    e_ctrl->eeprom_data.wbc.r_over_g[index] =
      (float) (awb_r_over_gr)/(QVALUE);

    e_ctrl->eeprom_data.wbc.b_over_g[index] =
      (float) (awb_b_over_gr)/(QVALUE);

  }

 e_ctrl->eeprom_data.wbc.gr_over_gb = 1.0f;


  SERR("Exit");
}

static void common_ar0542_format_lensshading (sensor_eeprom_data_t *e_ctrl)
{
  int  j = 0, group_offset = 0;
  uint8_t mid;

  SERR("Enter");

   for (j = 0; j < LSC_REG_SIZE_1; j++) {
        g_reg_array[g_reg_setting.size].reg_addr = 0x3600 + j*2;
        g_reg_array[g_reg_setting.size].reg_data =
          (e_ctrl->eeprom_params.buffer[LSC_OFFSET_1 + j*2 ] << 8
          |e_ctrl->eeprom_params.buffer[LSC_OFFSET_1 + j*2 +1]);
        g_reg_setting.size++;
    }
    for (j = 0; j < LSC_REG_SIZE_2; j++) {
        g_reg_array[g_reg_setting.size].reg_addr = 0x3640 + j*2;
        g_reg_array[g_reg_setting.size].reg_data =
          (e_ctrl->eeprom_params.buffer[LSC_OFFSET_2 + j*2 ] << 8
          |e_ctrl->eeprom_params.buffer[LSC_OFFSET_2 + j*2 +1]);
        g_reg_setting.size++;
    }
    for (j = 0; j < LSC_REG_SIZE_3; j++) {
        g_reg_array[g_reg_setting.size].reg_addr = 0x3680 + j*2;
        g_reg_array[g_reg_setting.size].reg_data =
          (e_ctrl->eeprom_params.buffer[LSC_OFFSET_3 + j*2 ] << 8
          |e_ctrl->eeprom_params.buffer[LSC_OFFSET_3 + j*2 +1]);
        g_reg_setting.size++;
    }
    for (j = 0; j < LSC_REG_SIZE_4; j++) {
        g_reg_array[g_reg_setting.size].reg_addr = 0x36c0 + j*2;
        g_reg_array[g_reg_setting.size].reg_data =
          (e_ctrl->eeprom_params.buffer[LSC_OFFSET_4 + j*2 ] << 8
          |e_ctrl->eeprom_params.buffer[LSC_OFFSET_4 + j*2 +1]);
        g_reg_setting.size++;
    }
    for (j = 0; j < LSC_REG_SIZE_5; j++) {
        g_reg_array[g_reg_setting.size].reg_addr = 0x3700 + j*2;
        g_reg_array[g_reg_setting.size].reg_data =
          (e_ctrl->eeprom_params.buffer[LSC_OFFSET_5 + j*2 ] << 8
          |e_ctrl->eeprom_params.buffer[LSC_OFFSET_5 + j*2 +1]);
        g_reg_setting.size++;
    }

    for (j = 0; j < LSC_REG_SIZE_6; j++) {
        g_reg_array[g_reg_setting.size].reg_addr = 0x3782 + j*2;
        g_reg_array[g_reg_setting.size].reg_data =
          (e_ctrl->eeprom_params.buffer[LSC_OFFSET_6 + j*2 ] << 8
          |e_ctrl->eeprom_params.buffer[LSC_OFFSET_6 + j*2 +1]);
        g_reg_setting.size++;
    }

    for (j = 0; j < LSC_REG_SIZE_7; j++) {
        g_reg_array[g_reg_setting.size].reg_addr = 0x37C0 + j*2;
        g_reg_array[g_reg_setting.size].reg_data =
          (e_ctrl->eeprom_params.buffer[LSC_OFFSET_7 + j*2 ] << 8
          |e_ctrl->eeprom_params.buffer[LSC_OFFSET_7 + j*2 +1]);
        g_reg_setting.size++;
    }
    for(j = 0;j <  g_reg_setting.size;j++)
    {
        SLOW(":%d:reg[0x%x] = 0x%x",j,g_reg_array[j].reg_addr,g_reg_array[j].reg_data);
        }
  SERR("Exit");
}


void common_ar0542_format_calibration_data(void *e_ctrl)
{
  SERR("Enter");
  sensor_eeprom_data_t *ectrl = (sensor_eeprom_data_t *)e_ctrl;
  uint8_t *data = ectrl->eeprom_params.buffer;

  if(Format_Calibration_Flag){
        SERR("had been formated");
        return;
  }

  g_reg_setting.addr_type = MSM_CAMERA_I2C_WORD_ADDR;
  g_reg_setting.data_type = MSM_CAMERA_I2C_WORD_DATA;
  g_reg_setting.reg_setting = &g_reg_array[0];
  g_reg_setting.size = 0;
  g_reg_setting.delay = 0;
  common_ar0542_format_moduledata(ectrl);
  common_ar0542_format_wbdata(ectrl);
  common_ar0542_format_lensshading(ectrl);

  Format_Calibration_Flag = 1;

  SERR("Exit");
}

int32_t common_ar0542_get_raw_data(void *e_ctrl, void *data)
{
  if (e_ctrl && data)
    memcpy(data, &g_reg_setting, sizeof(g_reg_setting));
  else
    SERR("failed Null pointer");
  return 0;
}


static eeprom_lib_func_t common_ar0542_lib_func_ptr = {
    .get_calibration_items = common_ar0542_get_calibration_items,
    .format_calibration_data = common_ar0542_format_calibration_data,
    .do_af_calibration = NULL,
    .do_wbc_calibration = eeprom_whitebalance_calibration,
    .do_lsc_calibration = NULL,
    .do_dpc_calibration = NULL,
    .get_dpc_calibration_info = NULL,
    .get_raw_data = common_ar0542_get_raw_data,
};


void* common_ar0542_eeprom_open_lib(void) {
  return &common_ar0542_lib_func_ptr;
}

