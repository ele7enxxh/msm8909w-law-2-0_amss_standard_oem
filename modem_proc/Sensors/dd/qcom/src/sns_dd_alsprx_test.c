/*==============================================================================

    S E N S O R S   A L S P R X     D R I V E R    T E S T

DESCRIPTION

   Basic sanity test for the alsprx driver 

Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_alsprx_test.c#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
10/12/10     ag      initial version
==============================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include "fixed_point.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_attrib.h"
#include "sns_dd_alsprx_priv.h"

extern sns_ddf_driver_if_s sns_alsprx_driver_fn_list;
sns_ddf_sensor_data_s *sensor_data;
sns_ddf_sensor_sample_s data_sample[10];
sns_ddf_memhandler_s memhandler;

void test_alsprx_set_all_attr(sns_ddf_handle_t dd_hndl)
{
   sns_ddf_powerstate_e pwr;
   uint8_t res;
   sns_ddf_status_e stat;

   // set ALS pwr mode
   pwr = SNS_DDF_POWERSTATE_LOWPOWER;
   stat = sns_alsprx_driver_fn_list.set_attrib(dd_hndl,
                                             SNS_DDF_SENSOR_AMBIENT,
                                             SNS_DDF_ATTRIB_POWER_STATE,
                                             &pwr);
   printf("ALSPRX: set power mode for als to %d with status %d\n", pwr, stat);

   pwr = SNS_DDF_POWERSTATE_ACTIVE;
   stat = sns_alsprx_driver_fn_list.set_attrib(dd_hndl,
                                             SNS_DDF_SENSOR_AMBIENT,
                                             SNS_DDF_ATTRIB_POWER_STATE,
                                             &pwr);
   printf("ALSPRX: set power mode for als to %d with status %d\n", pwr, stat);

   // set PRX pwr mode
   pwr = SNS_DDF_POWERSTATE_LOWPOWER;
   stat = sns_alsprx_driver_fn_list.set_attrib(dd_hndl,
                                             SNS_DDF_SENSOR_PROXIMITY,
                                             SNS_DDF_ATTRIB_POWER_STATE,
                                             &pwr);
   printf("ALSPRX: set power mode for prx to %d with status %d\n", pwr, stat);

   pwr = SNS_DDF_POWERSTATE_ACTIVE;
   stat = sns_alsprx_driver_fn_list.set_attrib(dd_hndl,
                                             SNS_DDF_SENSOR_PROXIMITY,
                                             SNS_DDF_ATTRIB_POWER_STATE,
                                             &pwr);
   printf("ALSPRX: set power mode for prx to %d with status %d\n", pwr, stat);

   //set ALS resolution
   res = SNS_DD_ALSPRX_RES_8_BIT;
   stat = sns_alsprx_driver_fn_list.set_attrib(dd_hndl,
                                             SNS_DDF_SENSOR_AMBIENT,
                                             SNS_DDF_ATTRIB_RESOLUTION,
                                             &res);
   printf("ALSPRX: set resolution for als to %d with status %d\n", res, stat);

   //set PRX resolution
   res = SNS_DD_ALSPRX_RES_16_BIT;
   stat = sns_alsprx_driver_fn_list.set_attrib(dd_hndl,
                                             SNS_DDF_SENSOR_AMBIENT,
                                             SNS_DDF_ATTRIB_RESOLUTION,
                                             &res);
   printf("ALSPRX: set resolution for prx to %d with status %d\n", res, stat);

   //set ALS resolution
   res = SNS_DD_ALSPRX_RES_8_BIT;
   stat = sns_alsprx_driver_fn_list.set_attrib(dd_hndl,
                                             SNS_DDF_SENSOR_PROXIMITY,
                                             SNS_DDF_ATTRIB_RESOLUTION,
                                             &res);
   printf("ALSPRX: set resolution for als to %d with status %d\n", res, stat);

   //set PRX resolution
   res = SNS_DD_ALSPRX_RES_16_BIT;
   stat = sns_alsprx_driver_fn_list.set_attrib(dd_hndl,
                                             SNS_DDF_SENSOR_PROXIMITY,
                                             SNS_DDF_ATTRIB_RESOLUTION,
                                             &res);
   printf("ALSPRX: set resolution for prx to %d with status %d\n", res, stat);
}

void test_alsprx_get_all_attr(sns_ddf_handle_t dd_hndl) 
{
   void* data;
   sns_ddf_driver_info_s* drv;
   sns_ddf_device_info_s* dev;
   sns_ddf_status_e stat;
   uint32_t len;

   // get resolution for ALS
   stat = sns_alsprx_driver_fn_list.get_attrib(dd_hndl,
                                             SNS_DDF_SENSOR_AMBIENT,
                                             SNS_DDF_ATTRIB_RESOLUTION,
                                             &memhandler,
                                             &data,
                                             &len);
   printf("ALSPRX: ALS resolution = %d\n", *(uint16_t *)data);

   // get resolution for PRX
   stat = sns_alsprx_driver_fn_list.get_attrib(dd_hndl,
                                             SNS_DDF_SENSOR_PROXIMITY,
                                             SNS_DDF_ATTRIB_RESOLUTION,
                                             &memhandler,
                                             &data,
                                             &len);
   printf("ALSPRX: PRX resolution = %d\n", *(uint16_t *)data);
   
   // get power for ALS
   stat = sns_alsprx_driver_fn_list.get_attrib(dd_hndl,
                                             SNS_DDF_SENSOR_AMBIENT,
                                             SNS_DDF_ATTRIB_POWER_INFO,
                                             &memhandler,
                                             &data,
                                             &len);
   printf("ALSPRX: ALS power = %d\n", *(uint32_t *)data);

   // get power for PRX
   stat = sns_alsprx_driver_fn_list.get_attrib(dd_hndl,
                                             SNS_DDF_SENSOR_PROXIMITY,
                                             SNS_DDF_ATTRIB_POWER_INFO,
                                             &memhandler,
                                             &data,
                                             &len);
   printf("ALSPRX: PRX power = %d\n", *(uint32_t *)data);

   // get accuracy for ALS
   stat = sns_alsprx_driver_fn_list.get_attrib(dd_hndl,
                                             SNS_DDF_SENSOR_AMBIENT,
                                             SNS_DDF_ATTRIB_ACCURACY,
                                             &memhandler,
                                             &data,
                                             &len);
   printf("ALSPRX: ALS accuracy = %d\n", *(uint16_t *)data);

   // get accuracy for PRX
   stat = sns_alsprx_driver_fn_list.get_attrib(dd_hndl,
                                             SNS_DDF_SENSOR_PROXIMITY,
                                             SNS_DDF_ATTRIB_ACCURACY,
                                             &memhandler,
                                             &data,
                                             &len);
   printf("ALSPRX: PRX accuracy = %d\n", *(uint16_t *)data);

   // get threshold for PRX
   stat = sns_alsprx_driver_fn_list.get_attrib(dd_hndl,
                                             SNS_DDF_SENSOR_PROXIMITY,
                                             SNS_DDF_ATTRIB_THRESHOLD,
                                             &memhandler,
                                             &data,
                                             &len);
   printf("ALSPRX: PRX threshold = %d\n", *(uint16_t *)data);

    //get driver info
   stat = sns_alsprx_driver_fn_list.get_attrib(dd_hndl,
                                             SNS_DDF_SENSOR_PROXIMITY,
                                             SNS_DDF_ATTRIB_DRIVER_INFO,
                                             &memhandler,
                                             &data,
                                             &len);
   drv = (sns_ddf_driver_info_s*)data;
   printf("ALSPRX: PRX driver info:\n");
   printf("ALSPRX:   name %s, version: %s\n", drv->name, drv->version);

   // get device info
   stat = sns_alsprx_driver_fn_list.get_attrib(dd_hndl,
	                                         SNS_DDF_SENSOR_PROXIMITY,
                                             SNS_DDF_ATTRIB_DEVICE_INFO,
                                             &memhandler,
                                             &data,
                                             &len);
   dev = (sns_ddf_device_info_s*)data;
   printf("ALSPRX: PRX device info:\n");
   printf("ALSPRX:   name %s, model %s, version: %s\n", 
      dev->name, dev->model, dev->version);

   stat = sns_alsprx_driver_fn_list.get_attrib(dd_hndl,
                                             SNS_DDF_SENSOR_AMBIENT,
                                             SNS_DDF_ATTRIB_DRIVER_INFO,
                                             &memhandler,
                                             &data,
                                             &len);
   drv = (sns_ddf_driver_info_s*)data;
   printf("ALSPRX: ALS driver info:\n");
   printf("ALSPRX:   name %s, version: %s\n", drv->name, drv->version);
}

void sns_dd_alsprx_test() 
{
   sns_ddf_sensor_e* sensors;
   uint32_t num_sensors;
   sns_ddf_memhandler_s memhandler;
   sns_ddf_handle_t dd_hndl;
   sns_ddf_status_e stat;
   sns_ddf_device_access_s dev;
   sns_ddf_i2c_config_s i2c_config;
   uint32_t i, handl=1;

   dev.device_select = 0;
   dev.port_config.bus = SNS_DDF_BUS_I2C;
   dev.port_config.bus_config.i2c = &i2c_config;

   i2c_config.addr_type = SNS_DDF_I2C_ADDR_7BIT;
   i2c_config.bus_acq_timeout = -1;
   i2c_config.xfer_timeout = -1;
   i2c_config.bus_freq = 100;
   i2c_config.dev_type = SNS_DDF_I2C_DEVICE_REGADDR;
   i2c_config.read_opt = SNS_DDF_I2C_START_BEFORE_RD;
   i2c_config.slave_addr = 0x44;

   //init memory handler
   sns_ddf_memhandler_init(&memhandler);

   //init alsprx driver
   printf("\nInitializing ALSPRX...");
   stat = sns_alsprx_driver_fn_list.init(&dd_hndl,
										 &handl,        //smgr_handle  
										 NULL,          //nv_params    
										 &dev,          //device_info[]
										 1,             //num_devices  
										 &memhandler,
										 &sensors,
										 &num_sensors);

   if (stat != SNS_DDF_SUCCESS)
   {
      printf("error initializing\n", stat);
      return;
   }

   printf("Done\n");
   printf("ALSPRX: Sensor count = %d\n", num_sensors);
   for (i = 0; i < num_sensors; i++)
   {
      printf("ALSPRX: Sensor type is %d\n", sensors[i]);
   }

   printf("ALSPRX: Getting sensor data\n");
   stat = sns_alsprx_driver_fn_list.get_data(dd_hndl,
											 sensors,
											 num_sensors,
											 &memhandler,
											 &sensor_data);

   printf("ALSPRX: get_data returned %d\n", stat);

   /* get all attr */
   test_alsprx_get_all_attr(dd_hndl);

   /* set all attr */
   test_alsprx_set_all_attr(dd_hndl);

   /* simulate timer callback for ALS */
   printf("ALSPRX: Trigger timer callback\n");
   sns_alsprx_driver_fn_list.handle_timer(dd_hndl,
										  &i);

   printf("ALSPRX: get_data returned %d\n", stat);

   /* simulate timer callback for PRX */
   printf("ALSPRX: Trigger timer callback\n");
   sns_alsprx_driver_fn_list.handle_timer(dd_hndl,
										  &i);

   printf("ALSPRX: get_data returned %d\n", stat);

#if 0
   /* get all attr */
   test_alsprx_get_all_attr(dd_hndl);
#endif
   return;
}
