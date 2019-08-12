/*==============================================================================

    S E N S O R S   A C C E L E R O M E T E R    D R I V E R    T E S T E R

DESCRIPTION

   Impelements the driver for the accelerometer driver 

Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_test_accel.c#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
07/07/10     jww     Creation 
08/27/10     RK      Updated the file to the latest DDF interface 
08/31/10     RK      Cleaned up the set attrib functions and other 
                     subroutines 
==============================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include "sns_ddf_driver_if.h"
#include "sns_ddf_attrib.h"
#include "fixed_point.h"
#include "sns_memmgr.h"

extern sns_ddf_driver_if_s sns_accel_driver_fn_list;
sns_ddf_sensor_data_s notify_data[10];
sns_ddf_sensor_sample_s data_sample[10];

void test_acc_get_all_attr(sns_ddf_handle_t dd_hndl) 
{
  sns_ddf_range_s range[10];
  sns_ddf_ranges_s supp_range;
  sns_ddf_res_levels_s supp_res_lev;
  sns_ddf_res_level_s res_lev[10];
  sns_ddf_driver_info_s drv;
  sns_ddf_device_info_s dev;
  uint16_t len;
  sns_ddf_status_e stat;
  int i, j, k;
  char name[50], name2[50], model[50];
  uint32_t freq_list[10];
  sns_ddf_lowpass_s freq_set;

  supp_range.ranges = range;
  supp_res_lev.res_levels = res_lev;
  freq_set.lowpass_freqs = freq_list;
   
  drv.name = name;
  dev.name = name2;
  dev.model = model;

  // get pwr cap - todo
  // get supported rang
  len = sizeof(sns_ddf_ranges_s) + 10 * sizeof(sns_ddf_range_s);
  stat = sns_accel_driver_fn_list.get_attrib(
          dd_hndl,
          SNS_DDF_SENSOR_ACCEL,
          SNS_DDF_ATTRIB_RANGE,
          &supp_range,
          &len);
  printf("supported range for accel are:\n");
  printf("length of the ranges = %d\n", len);
  for (i = 0; i < supp_range.num_ranges; i++ ) 
  {
    printf("min = %d, max = %d\n", supp_range.ranges[i].min, supp_range.ranges[i].max);
  }

  len = sizeof(sns_ddf_ranges_s) + 10 * sizeof(sns_ddf_range_s);
  stat = sns_accel_driver_fn_list.get_attrib(
          dd_hndl,
          SNS_DDF_SENSOR_TEMP,
          SNS_DDF_ATTRIB_RANGE,
          &supp_range,
          &len);
  printf("supported range for temp are:\n");
  printf("length of the ranges = %d\n", len);
  for (i = 0; i < supp_range.num_ranges; i++ ) 
  {
    printf("min = %d, max = %d\n", supp_range.ranges[i].min, supp_range.ranges[i].max);
  }

  // get supported res level
  len = sizeof(sns_ddf_res_levels_s) + 10 * sizeof(sns_ddf_res_levels_s);
  stat = sns_accel_driver_fn_list.get_attrib(
         dd_hndl,
         SNS_DDF_SENSOR_ACCEL,
         SNS_DDF_ATTRIB_RES_LEVEL,
         &supp_res_lev,
         &len);
  printf("supported res level for accel are:\n");
  printf("length of the ranges = %d\n", len);
  for (i = 0; i < supp_res_lev.num_levels; i++ )
  {
    printf("bit len = %d, time_us = %d\n", 
           supp_res_lev.res_levels[i].bit_length, supp_res_lev.res_levels[i].sample_convert_time);
  }

  len = sizeof(sns_ddf_res_levels_s) + 10 * sizeof(sns_ddf_res_levels_s);
  stat = sns_accel_driver_fn_list.get_attrib(
         dd_hndl,
         SNS_DDF_SENSOR_TEMP,
         SNS_DDF_ATTRIB_RES_LEVEL,
         &supp_res_lev,
         &len);
  printf("supported res level for temp are:\n");
  for (i = 0; i < supp_res_lev.num_levels; i++ ) 
  {
    printf("bit len = %d, time_us = %d\n", 
           supp_res_lev.res_levels[i].bit_length, supp_res_lev.res_levels[i].sample_convert_time);
  }

  len =  sizeof(sns_ddf_lowpass_s) + 10 * sizeof(freq_list);
  stat = sns_accel_driver_fn_list.get_attrib(
         dd_hndl,
         SNS_DDF_SENSOR_ACCEL,
         SNS_DDF_ATTRIB_LOWPASS,
         &freq_set,
         &len);
  printf("supported res level for temp are:\n");
  for (i = 0; i < freq_set.num_freqs; i++) 
  {
    printf("freq supported = %d\n", 
           freq_set.lowpass_freqs[i]);
  }

  // get drv inf
  len = sizeof(sns_ddf_driver_info_s) + 50;
  stat = sns_accel_driver_fn_list.get_attrib(
         dd_hndl,
         SNS_DDF_SENSOR_ACCEL,
         SNS_DDF_ATTRIB_DRIVER_INFO,
         &drv,
         &len);
  printf("Accel drv info:\n");
  printf("name %s, version: %d\n", drv.name, drv.version);

  len = sizeof(sns_ddf_driver_info_s) + 50;
  stat = sns_accel_driver_fn_list.get_attrib(
         dd_hndl,
         SNS_DDF_SENSOR_TEMP,
         SNS_DDF_ATTRIB_DRIVER_INFO,
         &drv,
         &len);
  printf("Temp drv info:\n");
  printf("name %s, version: %d\n", drv.name, drv.version);

   // get dev inf
  len = sizeof(sns_ddf_device_info_s) + 50 + 50;
  stat = sns_accel_driver_fn_list.get_attrib(
         dd_hndl,
         SNS_DDF_SENSOR_ACCEL,
         SNS_DDF_ATTRIB_DEVICE_INFO,
         &dev,
         &len);
  printf("Accel dev info:\n");
  printf("name %s, model %s, version: %d\n", dev.name, dev.model, dev.version);
}


void test_acc_set_all_attr(sns_ddf_handle_t dd_hndl) {
  sns_ddf_powerstate_e pwr;
  uint8_t range;
  uint8_t lowpass_index;
  sns_ddf_status_e stat;

  // set pwr mode
  printf("setting power mode for accel\n");
  pwr = SNS_DDF_POWERSTATE_LOWPOWER;

  stat = sns_accel_driver_fn_list.set_attrib(
     dd_hndl,
     SNS_DDF_SENSOR_ACCEL,
     SNS_DDF_ATTRIB_POWER_STATE,
     &pwr,
     sizeof(sns_ddf_powerstate_e) );

  printf("setting power mode for temp\n");

  pwr = SNS_DDF_POWERSTATE_STANDBY;

  stat = sns_accel_driver_fn_list.set_attrib(
     dd_hndl,
     SNS_DDF_SENSOR_TEMP,
     SNS_DDF_ATTRIB_POWER_STATE,
     &pwr,
     sizeof(sns_ddf_powerstate_e) );

  // set range
  printf("setting range for accel\n");
  range = 2;
  stat = sns_accel_driver_fn_list.set_attrib(
     dd_hndl,
     SNS_DDF_SENSOR_ACCEL,
     SNS_DDF_ATTRIB_RANGE,
     &range,
     sizeof(range) );

  //set lowpass freq
  printf("setting lowpass freq for temp\n");
  lowpass_index = 3;
  stat = sns_accel_driver_fn_list.set_attrib(
     dd_hndl,
     SNS_DDF_SENSOR_ACCEL,
     SNS_DDF_ATTRIB_LOWPASS,
     &lowpass_index,
     sizeof(lowpass_index));
}

void test_acc() 
{
  int sddi = 0;
  sns_ddf_device_access_s  dev;
  sns_ddf_i2c_config_s         i2c_config;
  int type_cnt;
  sns_ddf_sensor_e sns_type[10];
  sns_ddf_handle_t dd_hndl;
  sns_ddf_status_e stat;
  int i, j, k;

  dev.device_select = 0; // <pg> Don't know how this is used ..
  dev.port_config.bus             = SNS_DDF_BUS_I2C;
  dev.port_config.bus_config.i2c  = &i2c_config;
  i2c_config.addr_type                          = SNS_DDF_I2C_ADDR_7BIT;
  i2c_config.bus_acq_timeout                    = -1;
  i2c_config.xfer_timeout                       = -1;
  i2c_config.bus_freq                           = 400;
  i2c_config.dev_type                           = SNS_DDF_I2C_DEVICE_REGADDR;
  i2c_config.read_opt                           = SNS_DDF_I2C_START_BEFORE_RD;
  i2c_config.slave_addr                         = 0x38;
  

  // init the bma150 driver
  stat = sns_accel_driver_fn_list.init(&dd_hndl,
                      0,
                      NULL,
                      &dev,
                      1,
                      sns_type,
                      &type_cnt);

  if (stat != SNS_DDF_SUCCESS) {
     printf("error initializing\n", stat);
     return;
  }

  // print out supported type
  printf("type cnt = %d\n", type_cnt);
  for (i = 0; i < type_cnt; i++) 
  {
     printf("type is %d\n", sns_type[i]);
     notify_data[i].data = &data_sample[0];
  }

  printf("getting data: %d\n", i);
  j = type_cnt;
  stat = sns_accel_driver_fn_list.get_data(
           dd_hndl,
           sns_type,
           j,
           notify_data,
           &k);

  if (stat != SNS_DDF_SUCCESS) 
  {
    printf("error getting data: %d\n", stat);
  }
  else 
  {
    for (k = 0; k < j; k++ ) 
    {
      if (notify_data[k].sensor == SNS_DDF_SENSOR_ACCEL) 
      {
         printf("accel: %f %f %f\n", 
            FX_FIXTOFLT_Q16(notify_data[k].data[0].sample), 
            FX_FIXTOFLT_Q16(notify_data[k].data[1].sample), 
            FX_FIXTOFLT_Q16(notify_data[k].data[2].sample) );
      } else 
      {
         printf("temp: %f\n", FX_FIXTOFLT_Q16(notify_data[k].data[0].sample) );
      }
    }
  }

  /* get all attr */
  test_acc_get_all_attr(dd_hndl);

  /* try setting attr */
  test_acc_set_all_attr(dd_hndl);

  /* get all attr */
  //test_acc_get_all_attr(dd_hndl);

  return;
}
