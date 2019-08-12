/*==============================================================================

    S E N S O R S   G Y R O S C O P E    D R I V E R    T E S T

DESCRIPTION

   Impelements the driver for the gyroscope driver 

Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_gyro_test.c#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
10/03/10     ad      initial version
==============================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include "fixed_point.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_attrib.h"
#include "sns_dd_gyro_priv.h"

extern sns_ddf_driver_if_s sns_gyro_driver_fn_list;
sns_ddf_sensor_data_s *sensor_data;
sns_ddf_sensor_sample_s data_sample[10];
sns_ddf_memhandler_s memhandler;

void test_gyro_set_all_attr(sns_ddf_handle_t dd_hndl)
{
   sns_ddf_powerstate_e pwr;
   uint8_t range;
   uint8_t lowpass_index;
   sns_ddf_status_e status;

   // set pwr mode
   pwr = SNS_DDF_POWERSTATE_LOWPOWER;
   status = sns_gyro_driver_fn_list.set_attrib(dd_hndl,
                                               SNS_DDF_SENSOR_GYRO,
                                               SNS_DDF_ATTRIB_POWER_STATE,
                                               &pwr);
   printf("set power mode for gyro to %d with status %d\n", pwr, status);

   pwr = SNS_DDF_POWERSTATE_ACTIVE;
   status = sns_gyro_driver_fn_list.set_attrib(dd_hndl,
                                               SNS_DDF_SENSOR_TEMP,
                                               SNS_DDF_ATTRIB_POWER_STATE,
                                               &pwr);
   printf("set power mode for gyro_temp to %d with status %d\n", pwr, status);

   // set range
   range = GYRO_3050_DEF_RANGE_INDEX-1;
   status = sns_gyro_driver_fn_list.set_attrib(dd_hndl,
                                               SNS_DDF_SENSOR_GYRO,
                                               SNS_DDF_ATTRIB_RANGE,
                                               &range);
   printf("set range for gyro to %d with status %d\n", range, status);

   //set lowpass freq
   lowpass_index = GYRO_3050_DEF_DLPF_BW_INDEX-1;
   status = sns_gyro_driver_fn_list.set_attrib(dd_hndl,
                                               SNS_DDF_SENSOR_GYRO,
                                               SNS_DDF_ATTRIB_LOWPASS,
                                               &lowpass_index);
   printf("set lowpass freq for gyro to %d with status %d\n", lowpass_index, status);
}

void test_gyro_get_all_attr(sns_ddf_handle_t dd_hndl) 
{
   void* data;
   sns_ddf_range_s* range;
   sns_ddf_lowpass_freq_t *freq_set;
   sns_ddf_driver_info_s* drv;
   sns_ddf_device_info_s* dev;
   sns_ddf_status_e status;
   uint32_t i, len;

   // get gyro range
   status = sns_gyro_driver_fn_list.get_attrib(dd_hndl,
                                               SNS_DDF_SENSOR_GYRO,
                                               SNS_DDF_ATTRIB_RANGE,
                                               &memhandler,
                                               &data,
                                               &len);
   range = (sns_ddf_range_s*)data;
   printf("supported range for gyro are:\n");
   printf("length of the ranges = %d\n", len);
   for (i = 0; i < len; i++ )
   {
      printf("gyro: min = %d, max = %d\n", 
             (range[i].min >> 16), (range[i].max >> 16));
   }

   // get temperature range
   status = sns_gyro_driver_fn_list.get_attrib(dd_hndl,
                                               SNS_DDF_SENSOR_TEMP,
                                               SNS_DDF_ATTRIB_RANGE,
                                               &memhandler,
                                               &data,
                                               &len);
   range = (sns_ddf_range_s*)data;
   printf("supported range for temp is:\n");
   printf("length of the ranges = %d\n", len);
   for (i = 0; i < len; i++ )
   {
      printf("temp: min = %d, max = %d\n", 
             (range[i].min >> 16), (range[i].max >> 16));
   }

   //get filter bandwidths
   status = sns_gyro_driver_fn_list.get_attrib(dd_hndl,
                                               SNS_DDF_SENSOR_GYRO,
                                               SNS_DDF_ATTRIB_LOWPASS,
                                               &memhandler,
                                               &data,
                                               &len);
   freq_set = (sns_ddf_lowpass_freq_t*)data;
   printf("supported filter bandwidths are:\n");
   printf("length of the filter bandwidths = %d\n", len);
   for (i = 0; i < len; i++)
   {
      printf("freq supported = %d\n", freq_set[i]);
   }

   //get driver info
   status = sns_gyro_driver_fn_list.get_attrib(dd_hndl,
                                               SNS_DDF_SENSOR_GYRO,
                                               SNS_DDF_ATTRIB_DRIVER_INFO,
                                               &memhandler,
                                               &data,
                                               &len);
   drv = (sns_ddf_driver_info_s*)data;
   printf("gyro driver info:\n");
   printf("name %s, version: %s\n", drv->name, drv->version);

   status = sns_gyro_driver_fn_list.get_attrib(dd_hndl,
                                               SNS_DDF_SENSOR_TEMP,
                                               SNS_DDF_ATTRIB_DRIVER_INFO,
                                               &memhandler,
                                               &data,
                                               &len);
   drv = (sns_ddf_driver_info_s*)data;
   printf("temp driver info:\n");
   printf("name %s, version: %s\n", drv->name, drv->version);

   // get device info
   status = sns_gyro_driver_fn_list.get_attrib(dd_hndl,
                                               SNS_DDF_SENSOR_GYRO,
                                               SNS_DDF_ATTRIB_DEVICE_INFO,
                                               &memhandler,
                                               &data,
                                               &len);
   dev = (sns_ddf_device_info_s*)data;
   printf("gyro device info:\n");
   printf("name %s, model %s, version: %s\n", 
      dev->name, dev->model, dev->version);
}

void sns_dd_gyro_test() 
{
   sns_ddf_sensor_e* sensors;
   uint32_t num_sensors;
   sns_ddf_memhandler_s memhandler;
   sns_ddf_handle_t dd_hndl;
   sns_ddf_status_e status;
   sns_ddf_device_access_s dev;
   sns_ddf_i2c_config_s i2c_config;
   sns_ddf_powerstate_e pwr;

   uint32_t i;

   dev.device_select = 0;
   dev.port_config.bus = SNS_DDF_BUS_I2C;
   dev.port_config.bus_config.i2c = &i2c_config;

   i2c_config.addr_type = SNS_DDF_I2C_ADDR_7BIT;
   i2c_config.bus_acq_timeout = -1;
   i2c_config.xfer_timeout = -1;
   i2c_config.bus_freq = 100;
   i2c_config.dev_type = SNS_DDF_I2C_DEVICE_REGADDR;
   i2c_config.read_opt = SNS_DDF_I2C_START_BEFORE_RD;
   i2c_config.slave_addr = GYRO_3050_I2C_ADDR;

   //init memory handler
   sns_ddf_memhandler_init(&memhandler);

   //init gyro driver
   status = sns_gyro_driver_fn_list.init(&dd_hndl,
                                         NULL,          //smgr_handle  
                                         NULL,          //nv_params    
                                         &dev,          //device_info[]
                                         1,             //num_devices  
                                         &memhandler,
                                         &sensors,
                                         &num_sensors);

   if (status != SNS_DDF_SUCCESS)
   {
      printf("error initializing\n", status);
      return;
   }

   printf("sensor count = %d\n", num_sensors);
   for (i = 0; i < num_sensors; i++)
   {
      printf("sensor type is %d\n", sensors[i]);
   }

   pwr = SNS_DDF_POWERSTATE_ACTIVE;
   status = sns_gyro_driver_fn_list.set_attrib(dd_hndl,
                                               SNS_DDF_SENSOR_TEMP,
                                               SNS_DDF_ATTRIB_POWER_STATE,
                                               &pwr);
   printf("set power mode for gyro_temp to %d with status %d\n", 
          pwr, status);

   if (status != SNS_DDF_SUCCESS)
   {
      printf("error activating device\n", status);
      return;
   }

   printf("getting sensor data\n");
   status = sns_gyro_driver_fn_list.get_data(dd_hndl,
                                             sensors,
                                             num_sensors,
                                             &memhandler,
                                             &sensor_data);

   for (i = 0; i < num_sensors; i++)
   {
      if (sensor_data[i].sensor == SNS_DDF_SENSOR_GYRO)
      {
         int k;
         for (k = 0; k < SDD_GYRO_NUM_AXES; k++)
         {
            printf("gyro sample data: %f status: %d\n", 
                   FX_FIXTOFLT_Q16(sensor_data[i].samples[k].sample), 
                   sensor_data[i].samples[k].status);
         }
      } else
      {
         printf("temp sample data: %f status: %d\n", 
            FX_FIXTOFLT_Q16(sensor_data[i].samples[0].sample),
            sensor_data[i].samples[0].status);
      }
   }

   /* get all attr */
   test_gyro_get_all_attr(dd_hndl);

   /* set all attr */
   test_gyro_set_all_attr(dd_hndl);

   /* get all attr */
   test_gyro_get_all_attr(dd_hndl);

   return;
}
