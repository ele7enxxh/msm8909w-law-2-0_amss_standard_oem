/*==============================================================================

    S E N S O R S   M A G N E T O M E T E R AKM8975    D R I V E R    T E S T

DESCRIPTION

   Impelements the driver for the AKM8975 magnetometer driver 

Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dd/qcom/src/sns_dd_mag_akm8975_test.c#1 $


when         who     what, where, why
----------   ---     -----------------------------------------------------------
01/18/10     OF      Initial version
==============================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include "fixed_point.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_attrib.h"
#include "sns_dd_mag_akm8975_priv.h"

extern sns_ddf_driver_if_s sns_mag_akm8975_driver_fn_list;
sns_ddf_sensor_data_s *sensor_data;
sns_ddf_sensor_sample_s data_sample[3];
sns_ddf_memhandler_s memhandler;

sns_ddf_status_e test_mag_akm8975_set_all_attr(sns_ddf_handle_t dd_hndl)
{
   sns_ddf_status_e        status;
   sns_ddf_powerstate_e    pwr;

   // set pwr mode
   pwr = SNS_DDF_POWERSTATE_ACTIVE;
   status = sns_mag_akm8975_driver_fn_list.set_attrib(dd_hndl,
                                               SNS_DDF_SENSOR_MAG,
                                               SNS_DDF_ATTRIB_POWER_STATE,
                                               &pwr);

   if (status != SNS_DDF_SUCCESS)
   {
       return status;
   }

   pwr = SNS_DDF_POWERSTATE_LOWPOWER;
   status = sns_mag_akm8975_driver_fn_list.set_attrib(dd_hndl,
											   SNS_DDF_SENSOR_MAG,
											   SNS_DDF_ATTRIB_POWER_STATE,
											   &pwr);

   if (status != SNS_DDF_SUCCESS)
   {
	   return status;
   }

   return SNS_DDF_SUCCESS;
}

sns_ddf_status_e test_mag_akm8975_get_all_attr(sns_ddf_handle_t dd_hndl) 
{
   sns_ddf_status_e        status;
   void*                   data;
   sns_ddf_power_info_s*   power_attrib;
   uint32_t                num_elems;
   sns_ddf_range_s*        range;
   sns_ddf_resolution_t*   device_res;
   sns_ddf_delays_s*       device_delay;
   sns_ddf_driver_info_s*  driver_info;
   sns_ddf_device_info_s*  device_info;

   // get power info
   status = sns_mag_akm8975_driver_fn_list.get_attrib(dd_hndl,
                                               SNS_DDF_SENSOR_MAG,
                                               SNS_DDF_ATTRIB_POWER_INFO,
                                               &memhandler,
                                               &data,
                                               &num_elems);

   if (status != SNS_DDF_SUCCESS)
   {
       return status;
   }

   if (num_elems != 1)
   {
       return SNS_DDF_EDATA_BOUND;
   }
   power_attrib = (sns_ddf_power_info_s*)data;
   (void)power_attrib; // Prevent compiler warning

   // get range info
   status = sns_mag_akm8975_driver_fn_list.get_attrib(dd_hndl,
                                               SNS_DDF_SENSOR_MAG,
                                               SNS_DDF_ATTRIB_RANGE,
                                               &memhandler,
                                               &data,
                                               &num_elems);

   if (status != SNS_DDF_SUCCESS)
   {
       return status;
   }

   if (num_elems != 1)
   {
       return SNS_DDF_EDATA_BOUND;
   }
   range = (sns_ddf_range_s*)data;
   (void)range; // Prevent compiler warning

   // get resolution info
   status = sns_mag_akm8975_driver_fn_list.get_attrib(dd_hndl,
                                               SNS_DDF_SENSOR_MAG,
                                               SNS_DDF_ATTRIB_RESOLUTION,
                                               &memhandler,
                                               &data,
                                               &num_elems);

   if (status != SNS_DDF_SUCCESS)
   {
       return status;
   }

   if (num_elems != 1)
   {
       return SNS_DDF_EDATA_BOUND;
   }
   device_res = (sns_ddf_resolution_t*)data;
   (void)device_res; // Prevent compiler warning

   // get delay info
   status = sns_mag_akm8975_driver_fn_list.get_attrib(dd_hndl,
                                               SNS_DDF_SENSOR_MAG,
                                               SNS_DDF_ATTRIB_DELAYS,
                                               &memhandler,
                                               &data,
                                               &num_elems);

   if (status != SNS_DDF_SUCCESS)
   {
       return status;
   }

   if (num_elems != 1)
   {
       return SNS_DDF_EDATA_BOUND;
   }
   device_delay = (sns_ddf_delays_s*)data;
   (void)device_delay; // Prevent compiler warning

   // Get driver info
   status = sns_mag_akm8975_driver_fn_list.get_attrib(dd_hndl,
                                               SNS_DDF_SENSOR_MAG,
                                               SNS_DDF_ATTRIB_DRIVER_INFO,
                                               &memhandler,
                                               &data,
                                               &num_elems);

   if (status != SNS_DDF_SUCCESS)
   {
       return status;
   }

   if (num_elems != 1)
   {
       return SNS_DDF_EDATA_BOUND;
   }
   driver_info = (sns_ddf_driver_info_s*)data;
   (void)driver_info; // Prevent compiler warning

   // Get device info
   status = sns_mag_akm8975_driver_fn_list.get_attrib(dd_hndl,
                                               SNS_DDF_SENSOR_MAG,
                                               SNS_DDF_ATTRIB_DEVICE_INFO,
                                               &memhandler,
                                               &data,
                                               &num_elems);

   if (status != SNS_DDF_SUCCESS)
   {
       return status;
   }

   if (num_elems != 1)
   {
       return SNS_DDF_EDATA_BOUND;
   }
   device_info = (sns_ddf_device_info_s*)data;
   (void)device_info; // Prevent compiler warning

   return SNS_DDF_SUCCESS;
}

void sns_dd_mag_akm8975_test() 
{
   sns_ddf_sensor_e* 		sensors;
   uint32_t 				   num_sensors;
   sns_ddf_handle_t 		   dd_hndl;
   sns_ddf_status_e 		   status;
   sns_ddf_device_access_s dev;
   sns_ddf_i2c_config_s 	i2c_config;
   int                     i;
   sns_ddf_sensor_data_s*  data;
   static uint32_t         Ix=2;    // Mag number in sns_smgr_Constants array
   /* Device Info */
   dev.device_select = 				0;
   dev.port_config.bus = 			SNS_DDF_BUS_I2C;
   dev.port_config.bus_config.i2c = &i2c_config;

   /* I2C Info */
   i2c_config.addr_type = 		SNS_DDF_I2C_ADDR_7BIT;
   i2c_config.bus_acq_timeout = -1;
   i2c_config.xfer_timeout = 	-1;
   i2c_config.bus_freq = 		400;
   i2c_config.dev_type = 		SNS_DDF_I2C_DEVICE_REGADDR;
   i2c_config.read_opt = 		SNS_DDF_I2C_START_BEFORE_RD;
   i2c_config.slave_addr = 		AKM8975_MAG_I2C_ADDR1;

   //init memory handler
   sns_ddf_memhandler_init(&memhandler);

   //init akm8975 mag driver
   status = sns_mag_akm8975_driver_fn_list.init(&dd_hndl,
                                         &Ix,          //smgr_handle  
                                         NULL,          //nv_params    
                                         &dev,          //device_info[]
                                         1,             //num_devices  
                                         &memhandler,
                                         &sensors,
                                         &num_sensors);

   if (status != SNS_DDF_SUCCESS)
   {
      return;
   }

   // Run the get data for 3 times just to see the values are changing
   for (i = 0; i < 3 ; i++)
   {
       status = sns_mag_akm8975_driver_fn_list.get_data(
                                                         dd_hndl,
                                                         sensors,
                                                         num_sensors,
                                                         &memhandler,
                                                         &data);
	   if (status != SNS_DDF_SUCCESS)
	   {
		   goto errorCleanUp;
	   }
   }

   status = test_mag_akm8975_get_all_attr(dd_hndl);

   if (status != SNS_DDF_SUCCESS)
   {
	   goto errorCleanUp;
   }

   status = test_mag_akm8975_set_all_attr(dd_hndl);

   if (status != SNS_DDF_SUCCESS)
   {
	   goto errorCleanUp;
   }

   sns_ddf_memhandler_free(&memhandler);

   while (1)
   {
   }

   errorCleanUp:
	   sns_ddf_memhandler_free(&memhandler);

   while (1)
   {
   }
}
